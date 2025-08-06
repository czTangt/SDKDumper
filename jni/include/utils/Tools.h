#ifndef TOOLS_H
#define TOOLS_H

#include <array>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <unistd.h>
#include <vector>

#include "Types.h"

namespace Tools
{
extern pid_t target_pid;
extern ModuleRange lib_range;

/*
 * https://man7.org/linux/man-pages/man2/process_vm_readv.2.html
 * Syscall Implementation of process_vm_readv & process_vm_writev
 */
bool pvm(void *address, void *buffer, size_t size, bool iswrite);

// 处理器虚拟内存读取
bool vm_readv(void *address, void *buffer, size_t size);

// 处理器虚拟内存写入
bool vm_writev(void *address, void *buffer, size_t size);

// 通过进程名获取目标进程的PID
pid_t getTargetPid(const char *process_name);

// 通过进程PID和模块名获取模块范围
ModuleRange getModuleRange(pid_t pid, const char *module_name);

// 读取指定地址和大小的字符串
std::string readString(kaddr address, unsigned int size);

// 转换 kaddr 偏移到实际内存地址
kaddr getRealOffset(kaddr offset);
// 根据地址读取指针值
kaddr getPtr(kaddr address);

// 检测字符串是否与给定的C风格字符串相等。
bool isEqual(std::string s1, const char *check);
// 检测字符串是否包含子字符串。
bool isContain(std::string str, std::string check);

// 读取指定地址的单个值
template <typename T> T Read(kaddr address)
{
    T data{};
    vm_readv(reinterpret_cast<void *>(address), &data, sizeof(T));
    return data;
}

// 写入单个值到指定地址
template <typename T> void Write(kaddr address, const T &data)
{
    vm_writev(reinterpret_cast<void *>(address), &data, sizeof(T));
}

// 读取指定地址的数组并返回 std::vector
template <typename T> std::vector<T> ReadArr(kaddr address, unsigned int size)
{
    std::vector<T> data(size);
    if (size > 0)
        vm_readv(reinterpret_cast<void *>(address), data.data(), sizeof(T) * size);
    return data;
}

// 宽字符串转换
struct WideStr
{
    static std::string readString(kaddr StrPtr, int StrLength)
    {
        auto source = Tools::ReadArr<UTF16>(StrPtr, StrLength);

        std::string result;
        result.reserve(StrLength * 3); // UTF-8 最多需要3字节 per 字符

        for (size_t i = 0; i < StrLength; i++)
        {
            UTF16 c = source[i];
            if (c == 0)
                break; // 遇到空字符停止

            // 转换为 UTF-8
            if (c < 0x80)
            {
                result.push_back(static_cast<char>(c));
            }
            else if (c < 0x800)
            {
                result.push_back(static_cast<char>(0xC0 | (c >> 6)));
                result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else if (c < 0xD800 || c > 0xDFFF)
            {
                // 不是代理对
                result.push_back(static_cast<char>(0xE0 | (c >> 12)));
                result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else if (c >= 0xD800 && c <= 0xDBFF && i + 1 < StrLength)
            {
                // 高代理对
                UTF16 low = source[i + 1];
                if (low >= 0xDC00 && low <= 0xDFFF)
                {
                    uint32_t codepoint = 0x10000 + ((c & 0x3FF) << 10) + (low & 0x3FF);
                    result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                    i++; // 跳过低代理
                }
            }
        }

        return result;
    }
};

// 获取当前时间字符串
std::string getCurrentTimeString();

} // namespace Tools

#endif // TOOLS_H