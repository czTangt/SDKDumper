#ifndef MEMORY_H
#define MEMORY_H

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>

#include "./proc/Proc.h"
#include "./utils/Log.h"

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

// get the PID of a target process by its name.
pid_t get_target_pid(const char *process_name);

extern struct ModuleRange
{
    kaddr base;
    kaddr end;
    size_t size;
} lib_range;
// get the base address of a module by its name.
ModuleRange get_module_range(pid_t pid, const char *module_name);

// read a C-style string from a given address.
std::string readCString(kaddr address, unsigned int maxSize);
// read a fixed-length string from a given address.
std::string readFixedString(kaddr address, unsigned int size);

// convert a kaddr offset to a real memory address.
kaddr getRealOffset(kaddr offset);
// read a pointer value from a given address.
kaddr getPtr(kaddr address);
// read an int32 value from a given address.
int32 getInt32(kaddr address);
// read an uint8 value from a given address.
uint8 getUInt8(kaddr address);

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

#endif // MEMORY_H