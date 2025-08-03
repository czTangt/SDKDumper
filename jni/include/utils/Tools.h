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

// Process Virtual Memory Reader
bool vm_readv(void *address, void *buffer, size_t size);

// Process Virtual Memory Writer
bool vm_writev(void *address, void *buffer, size_t size);

// get the PID of a target process by its name.
pid_t getTargetPid(const char *process_name);

// get the base address of a module by its name.
ModuleRange getModuleRange(pid_t pid, const char *module_name);

// convert a kaddr offset to a real memory address.
kaddr getHexAddr(const char *addr);

// read a string from a given address with a specified size.
std::string readString(kaddr address, unsigned int size);

// convert a kaddr offset to a real memory address.
kaddr getRealOffset(kaddr offset);
// read a pointer value from a given address.
kaddr getPtr(kaddr address);
// read an int32 value from a given address.
int32 getInt32(kaddr address);
// read an uint8 value from a given address.
uint8 getUInt8(kaddr address);

// check if a string is equal to a given C-style string.
bool isEqual(std::string s1, const char *check);
// check if a string contains a substring.
bool isContain(std::string str, std::string check);
// check if a string starts with a given C-style string.
bool isStartWith(std::string str, const char *check);

// read single value
template <typename T> T Read(kaddr address)
{
    T data{};
    vm_readv(reinterpret_cast<void *>(address), &data, sizeof(T));
    return data;
}

// write single value
template <typename T> void Write(kaddr address, const T &data)
{
    vm_writev(reinterpret_cast<void *>(address), &data, sizeof(T));
}

// read array as std::vector
template <typename T> std::vector<T> ReadArr(kaddr address, unsigned int size)
{
    std::vector<T> data(size);
    if (size > 0)
        vm_readv(reinterpret_cast<void *>(address), data.data(), sizeof(T) * size);
    return data;
}
} // namespace Tools

#endif // TOOLS_H