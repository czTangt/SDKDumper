#include "../../include/proc/Proc.h"
#include <array>

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