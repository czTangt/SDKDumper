#ifndef MEMORY_H
#define MEMORY_H

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <string>

#include "../proc/Proc.h"

// Unsigned base types.
typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef unsigned short UTF16;

// Signed base types.
typedef signed char int8;
typedef signed short int int16;
typedef signed int int32;
typedef signed long long int64;

// Unsigned pointer type.
typedef uintptr_t kaddr;

// Function to get the PID of a target process by its name.
pid_t get_target_pid(const char *process_name);

extern struct ModuleRange
{
    kaddr start;
    kaddr end;
    size_t size;
} lib_range;
// Function to get the base address of a module by its name.
ModuleRange get_module_range(pid_t pid, const char *module_name);

#endif // MEMORY_H