#ifndef PROCESS_H
#define PROCESS_H

#include <sys/syscall.h>
#include <sys/uio.h>
#include <unistd.h>

extern pid_t target_pid;

/*
 * https://man7.org/linux/man-pages/man2/process_vm_readv.2.html
 * Syscall Implementation of process_vm_readv & process_vm_writev
 */
bool pvm(void *address, void *buffer, size_t size, bool iswrite);

// Process Virtual Memory Reader
bool vm_readv(void *address, void *buffer, size_t size);

// Process Virtual Memory Writer
bool vm_writev(void *address, void *buffer, size_t size);

#endif // PROCESS_H