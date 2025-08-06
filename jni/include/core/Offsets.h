#ifndef OFFSETS_H
#define OFFSETS_H

#include "Tools.h"

struct _Offsets
{
    static constexpr kaddr FNameBlockOffsetBits = 16;
    static constexpr kaddr FNameBlockOffsets = 1 << FNameBlockOffsetBits;
    static constexpr kaddr NumElementsPerChunk = 64 * 1024; // 64K

    struct
    {
        kaddr GWorld = 0x0B32D8A8;
        kaddr GName = 0x0B171CC0;
        kaddr GUObjectArray = 0xB1B5F98;
        kaddr PointerSize = 0x8;
    } Global;

    struct
    {
        kaddr ComparisonIndex = 0x0; // 0x4
    } FName;

    struct
    {
        kaddr Entries = 0x0; // 0x48
    } FNamePool;

    struct
    {
        kaddr Stride = 0x2;
        kaddr BlockSizeBytes = Stride * FNameBlockOffsets;
        kaddr Lock = 0x0;               // 0x38
        kaddr CurrentBlock = 0x38;      // 0x4
        kaddr CurrentByteCursor = 0x3C; // 0x4
        kaddr Blocks = 0x40;            // 0x10000
    } FNameEntryAllocator;

    struct
    {
        kaddr Header = 0x0;     // 0x2
        kaddr StringName = 0x2; // 0x400
        kaddr StringLenBit = 6;
    } FNameEntry;

    struct
    {
        kaddr bIsWide = 1;
        kaddr StringLenBit = 6;
    } FNameEntryHeader;

    struct
    {
        kaddr PersistentLevel = 0x30; // 0x8
    } UWorld;

    struct
    {
        kaddr AActors = 0x98;     // 0x8
        kaddr ActorsCount = 0xA0; // 0x4
    } ULevel;

    struct
    {
        kaddr ObjObjects = 0x10; // 0x8
    } FUObjectArray;

    struct
    {
        kaddr Objects = 0x0;      // 0x8
        kaddr NumElements = 0x14; // 0x4
        kaddr NumChunks = 0x1C;   // 0x4
    } TUObjectArray;

    struct
    {
        kaddr Object = 0x0; // 0x8
        kaddr Size = 0x18;
    } FUObjectItem;

    struct
    {
        kaddr ClassPrivate = 0x10; // 0x8
        kaddr NamePrivate = 0x18;  // 0x8
        kaddr OuterPrivate = 0x20; // 0x8
    } UObject;

    struct
    {
        kaddr SuperStruct = 0x40;     // 0x8
        kaddr Children = 0x48;        // 0x8
        kaddr ChildProperties = 0x50; // 0x8
        kaddr PropertiesSize = 0x58;  // 0x4
    } UStruct;

    struct
    {
        kaddr FunctionFlags = 0xB0; // 0x8
        kaddr Func = 0xD8;          // 0x8
    } UFunction;

    struct
    {
        kaddr Next = 0x28; // 0x8
    } UField;

    struct
    {
        kaddr ClassPrivate = 0x8; // 0x8
        kaddr Next = 0x20;        // 0x8
        kaddr NamePrivate = 0x28; // 0x8
    } FField;

    struct
    {
        kaddr ElementSize = 0x38;     // 0x4
        kaddr PropertyFlags = 0x40;   // 0x8
        kaddr Offset_Internal = 0x4C; // 0x4
        kaddr Size = 0x78;
    } FProperty;

    struct
    {
        kaddr Enum = 0x78; // 0x8
    } FByteProperty;

    struct
    {
        kaddr Names = 0x40;    // 0x16
        kaddr ArrayNum = 0x48; // 0x4
    } UEnum;

    struct
    {
        kaddr Key = 0x0;   // 0x8
        kaddr Value = 0x8; // 0x8
        kaddr Size = 0x10;
    } TPair;

    struct
    {
        kaddr FieldSize = 0x78;  // 0x1
        kaddr ByteOffset = 0x79; // 0x1
        kaddr ByteMask = 0x7A;   // 0x1
        kaddr FieldMask = 0x7B;  // 0x1
    } FBoolProperty;

    struct
    {
        kaddr PropertyClass = 0x78; // 0x8
    } FObjectProperty;

    struct
    {
        kaddr MetaClass = 0x80; // 0x8
    } FClassProperty;

    struct
    {
        kaddr InterfaceClass = 0x78; // 0x8
    } FInterfaceProperty;

    struct
    {
        kaddr Struct = 0x78; // 0x8
    } FStructProperty;

    struct
    {
        kaddr Inner = 0x78; // 0x8
    } FArrayProperty;

    struct
    {
        kaddr KeyProp = 0x78;   // 0x8
        kaddr ValueProp = 0x80; // 0x8
    } FMapProperty;

    struct
    {
        kaddr ElementProp = 0x78; // 0x8
    } FSetProperty;

    struct
    {
        kaddr Enum = 0x80; // 0x8
    } FEnumProperty;
};

inline _Offsets Offsets;

#endif // OFFSETS_H
