#include "Offsets.h"

namespace Offsets
{
//-----Global Offsets-----//
namespace Global
{
kaddr GWorld = 0;
kaddr GName = 0;
kaddr GUObjectArray = 0;
kaddr PointerSize = 0;
} // namespace Global

//---------SDK-----------//
// UnrealNames.cpp
namespace FNamePool
{
kaddr Entries = 0;
} // namespace FNamePool

namespace FNameEntryAllocator
{
kaddr Stride = 0;
kaddr BlockSizeBytes = 0;
kaddr Lock = 0;
kaddr CurrentBlock = 0;
kaddr CurrentByteCursor = 0;
kaddr Blocks = 0;
} // namespace FNameEntryAllocator

namespace FNameEntry
{
kaddr FNameEntryHeader = 0;
kaddr StringName = 0;
kaddr StringLenBit = 0;
} // namespace FNameEntry

void initOffsets()
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

    FNameEntry::FNameEntryHeader = 0x0; // 0x2
    FNameEntry::StringName = 0x2;       // 0x400
    FNameEntry::StringLenBit = 0x6;
}
} // namespace Offsets