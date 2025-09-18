#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

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

struct ModuleRange
{
    kaddr base;
    kaddr end;
    size_t size;
};

#endif // TYPES_H