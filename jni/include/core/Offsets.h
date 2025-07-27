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

inline void initOffsets()
{
    // Global Offsets
    Global::PointerSize = 0x8;

    //---------SDK-----------//
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
    ULevel::AActors = 0x98;         // 0x8
    ULevel::ActorsCount = 0xa0;     // 0x4
}

} // namespace Offsets

#endif // OFFSETS_H