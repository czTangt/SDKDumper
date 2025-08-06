#include "Tools.h"

namespace Tools
{
pid_t target_pid = -1;
constexpr int process_vm_readv_syscall = 270;
constexpr int process_vm_writev_syscall = 271;

bool pvm(void *address, void *buffer, size_t size, bool iswrite)
{
    std::array<iovec, 1> local{{buffer, size}};
    std::array<iovec, 1> remote{{address, size}};

    if (target_pid < 0)
        return false;

    ssize_t bytes = syscall(iswrite ? process_vm_writev_syscall : process_vm_readv_syscall, target_pid, local.data(), 1,
                            remote.data(), 1, 0);
    return bytes == static_cast<ssize_t>(size);
}

bool vm_readv(void *address, void *buffer, size_t size)
{
    return pvm(address, buffer, size, false);
}

bool vm_writev(void *address, void *buffer, size_t size)
{
    return pvm(address, buffer, size, true);
}

pid_t getTargetPid(const char *process_name)
{
    if (!process_name)
    {
        return -1;
    }

    DIR *dir = opendir("/proc");
    if (!dir)
    {
        return -1;
    }

    // 遍历 /proc 目录，查找匹配的进程
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            int id = atoi(entry->d_name);
            if (id > 0)
            {
                char path[256];
                std::snprintf(path, sizeof(path), "/proc/%d/cmdline", id);
                std::ifstream cmdline_file(path, std::ios::in | std::ios::binary);
                if (cmdline_file)
                {
                    std::string cmdline;
                    std::getline(cmdline_file, cmdline, '\0'); // 读取到第一个'\0'
                    if (std::strcmp(process_name, cmdline.c_str()) == 0)
                    {
                        closedir(dir);
                        return id;
                    }
                }
            }
        }
    }

    closedir(dir);
    return -1;
}

ModuleRange lib_range{0, 0, 0};
ModuleRange getModuleRange(pid_t pid, const char *module_name)
{
    if (!module_name || !*module_name)
        return lib_range;

    char path[256];
    std::snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    std::ifstream maps_file(path);
    if (!maps_file)
        return lib_range;

    // 读取 maps 文件，查找指定模块的地址范围
    std::string line;
    bool found = false;
    while (std::getline(maps_file, line))
    {
        if (line.find(module_name) != std::string::npos)
        {
            size_t dash_pos = line.find('-');
            size_t end_pos = line.find(' ', dash_pos + 1);
            if (dash_pos != std::string::npos && end_pos != std::string::npos)
            {
                std::string start_str = line.substr(0, dash_pos);
                std::string end_str = line.substr(dash_pos + 1, end_pos - dash_pos - 1);
                kaddr start = std::stoull(start_str, nullptr, 16);
                kaddr end = std::stoull(end_str, nullptr, 16);
                if (!found || start < lib_range.base)
                    lib_range.base = start;
                if (!found || end > lib_range.end)
                    lib_range.end = end;
                lib_range.size = lib_range.end - lib_range.base;
                found = true;
            }
        }
    }
    return lib_range;
}

kaddr getRealOffset(kaddr offset)
{
    return (lib_range.base + offset);
}

std::string readString(kaddr address, unsigned int size)
{
    std::string result(size, '\0');
    vm_readv(reinterpret_cast<void *>(address), result.data(), size);
    return result;
}

kaddr getPtr(kaddr address)
{
    return Read<kaddr>(address);
}

bool isEqual(std::string s1, const char *check)
{
    std::string s2(check);
    return (s1 == s2);
}

bool isContain(std::string str, std::string check)
{
    size_t found = str.find(check);
    return (found != std::string::npos);
}

std::string getCurrentTimeString()
{
    std::time_t now = std::time(nullptr);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

} // namespace Tools