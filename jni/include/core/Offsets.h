#ifndef OFFSETS_H
#define OFFSETS_H

#include "Tools.h"

namespace Offsets
{

// Global Offsets
namespace Global
{
inline kaddr GWorld;
inline kaddr GName;
inline kaddr GUObjectArray;
inline kaddr PointerSize;
} // namespace Global

//---------SDK-----------//
// UnrealNames.cpp
static constexpr kaddr FNameBlockOffsetBits = 16;
static constexpr kaddr FNameBlockOffsets = 1 << FNameBlockOffsetBits;

// UObjectArray.h
static constexpr kaddr NumElementsPerChunk = 64 * 1024; // 64K

namespace FName
{
inline kaddr ComparisonIndex;
} // namespace FName

namespace FNamePool
{
inline kaddr Entries;
} // namespace FNamePool

namespace FNameEntryAllocator
{
inline kaddr Stride;
inline kaddr BlockSizeBytes;
inline kaddr Lock;
inline kaddr CurrentBlock;
inline kaddr CurrentByteCursor;
inline kaddr Blocks;
} // namespace FNameEntryAllocator

namespace FNameEntry
{
inline kaddr Header;
inline kaddr StringName;
inline kaddr StringLenBit;
} // namespace FNameEntry

namespace FNameEntryHeader
{
inline kaddr bIsWide;
inline kaddr StringLenBit;
} // namespace FNameEntryHeader

namespace UWorld
{
inline kaddr PersistentLevel;
} // namespace UWorld

namespace ULevel
{
inline kaddr AActors;
inline kaddr ActorsCount;
} // namespace ULevel

namespace FUObjectArray
{
inline kaddr ObjObjects;
}

namespace TUObjectArray
{
inline kaddr Objects;
inline kaddr NumElements;
inline kaddr NumChunks;
} // namespace TUObjectArray

namespace FUObjectItem
{
inline kaddr Object;
inline kaddr Size;
} // namespace FUObjectItem

namespace UObjectBase
{
inline kaddr ClassPrivate;
inline kaddr NamePrivate;
} // namespace UObjectBase

namespace UStruct
{
inline kaddr SuperStruct;
inline kaddr Children;
inline kaddr ChildProperties;
} // namespace UStruct

namespace FField
{
inline kaddr ClassPrivate;
inline kaddr Next;
inline kaddr NamePrivate;
} // namespace FField

namespace FProperty
{
inline kaddr ElementSize;
inline kaddr PropertyFlags;
inline kaddr Offset_Internal;
} // namespace FProperty

namespace FByteProperty
{
inline kaddr Enum;
} // namespace FByteProperty

namespace UEnum
{
inline kaddr Names;
inline kaddr enumItemSize; // UEnum -> TArray 指向的 TPair 大小
inline kaddr ArrayNum;     // UEnum -> TArray 中 ArrayNum 成员偏移
} // namespace UEnum

// UEnum 中 TArray<TPair<FName, int64>> Names; 的 TPair<FName, int64> 结构偏移
namespace TPair
{
inline kaddr Key;
inline kaddr Value;
} // namespace TPair

namespace FBoolProperty
{
inline kaddr FieldSize;
inline kaddr ByteOffset;
inline kaddr ByteMask;
inline kaddr FieldMask;
} // namespace FBoolProperty

namespace FObjectProperty
{
inline kaddr PropertyClass;
} // namespace FObjectProperty

namespace FClassProperty
{
inline kaddr MetaClass;
}

namespace FInterfaceProperty
{
inline kaddr InterfaceClass;
}

namespace FStructProperty
{
inline kaddr Struct;
}

namespace FArrayProperty
{
inline kaddr Inner;
}

namespace FMapProperty
{
inline kaddr KeyProp;
inline kaddr ValueProp;
} // namespace FMapProperty

namespace FSetProperty
{
inline kaddr ElementProp;
}

namespace FEnumProperty
{
inline kaddr Enum;
}

namespace UFunction
{
inline kaddr FunctionFlags;
inline kaddr Func;
} // namespace UFunction

namespace UField
{
inline kaddr Next;
}

inline void initOffsets()
{
    // Global Offsets
    Global::PointerSize = 0x8;

    //---------SDK-----------//
    // NameTypes.h
    FName::ComparisonIndex = 0x0; // 0x4

    // UnrealNames.cpp
    FNamePool::Entries = 0x0;

    FNameEntryAllocator::Stride = 0x2;
    FNameEntryAllocator::BlockSizeBytes = FNameEntryAllocator::Stride * FNameBlockOffsets;
    FNameEntryAllocator::Lock = 0x0;               // 0x38
    FNameEntryAllocator::CurrentBlock = 0x38;      // 0x4
    FNameEntryAllocator::CurrentByteCursor = 0x3C; // 0x4
    FNameEntryAllocator::Blocks = 0x40;            // 0x10000

    FNameEntry::Header = 0x0;     // 0x2
    FNameEntry::StringName = 0x2; // 0x400

    FNameEntryHeader::bIsWide = 1;
    FNameEntryHeader::StringLenBit = 6;

    // World
    UWorld::PersistentLevel = 0x30; // 0x8

    ULevel::AActors = 0x98;     // 0x8
    ULevel::ActorsCount = 0xA0; // 0x4

    // UObjectArray.h
    FUObjectArray::ObjObjects = 0x10;  // 0x8
    TUObjectArray::Objects = 0x0;      // 0x8
    TUObjectArray::NumElements = 0x14; // 0x4
    TUObjectArray::NumChunks = 0x1C;   // 0x4

    FUObjectItem::Object = 0x0; // 0x8
    FUObjectItem::Size = 0x18;

    // UObjectBase.h
    UObjectBase::ClassPrivate = 0x10; // 0x8
    UObjectBase::NamePrivate = 0x18;  // 0x8

    // Class.h
    UStruct::SuperStruct = 0x40;     // 0x8
    UStruct::Children = 0x48;        // 0x8
    UStruct::ChildProperties = 0x50; // 0x8

    UFunction::FunctionFlags = 0xB0; // 0x8
    UFunction::Func = 0xD8;          // 0x8

    UField::Next = 0x28; // 0x8

    // FField.h
    FField::ClassPrivate = 0x8; // 0x8
    FField::Next = 0x20;        // 0x8
    FField::NamePrivate = 0x28; // 0x8

    FProperty::ElementSize = 0x38;     // 0x4
    FProperty::PropertyFlags = 0x40;   // 0x8
    FProperty::Offset_Internal = 0x4C; // 0x4

    // UnrealType.h
    FByteProperty::Enum = 0x78;                // 0x8
    FBoolProperty::FieldSize = 0x78;           // 0x1
    FBoolProperty::ByteOffset = 0x79;          // 0x1
    FBoolProperty::ByteMask = 0x7A;            // 0x1
    FBoolProperty::FieldMask = 0x7B;           // 0x1
    FObjectProperty::PropertyClass = 0x78;     // 0x8
    FClassProperty::MetaClass = 0x80;          // 0x8
    FInterfaceProperty::InterfaceClass = 0x80; // 0x8
    FStructProperty::Struct = 0x78;            // 0x8
    FArrayProperty::Inner = 0x78;              // 0x8
    FMapProperty::KeyProp = 0x78;              // 0x8
    FMapProperty::ValueProp = 0x80;            // 0x8
    FSetProperty::ElementProp = 0x78;          // 0x8
    FEnumProperty::Enum = 0x80;                // 0x8

    UEnum::Names = 0x40;    // 0x16
    UEnum::ArrayNum = 0x48; // 0x4
    UEnum::enumItemSize = 0x10;

    // Other
    TPair::Key = 0x0;   // 0x8
    TPair::Value = 0x8; // 0x8
} // namespace inlinevoid initOffsets()

} // namespace Offsets

#endif // OFFSETS_H