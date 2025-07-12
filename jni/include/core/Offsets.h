#ifndef OFFSETS_H
#define OFFSETS_H

#include "Tools.h"

namespace Offsets
{
// Global Offsets
namespace Global
{
extern kaddr GWorld;
extern kaddr GName;
extern kaddr GUObjectArray;
extern kaddr PointerSize;
} // namespace Global

//---------SDK-----------//
// UnrealNames.cpp
static constexpr kaddr FNameBlockOffsetBits = 16;
static constexpr kaddr FNameBlockOffsets = 1 << FNameBlockOffsetBits;

namespace FNamePool
{
extern kaddr Entries;
} // namespace FNamePool

namespace FNameEntryAllocator
{
extern kaddr Stride;
extern kaddr BlockSizeBytes;
extern kaddr Lock;
extern kaddr CurrentBlock;
extern kaddr CurrentByteCursor;
extern kaddr Blocks;
} // namespace FNameEntryAllocator

namespace FNameEntry
{
extern kaddr FNameEntryHeader;
extern kaddr StringName;
extern kaddr StringLenBit;
} // namespace FNameEntry

void initOffsets();

} // namespace Offsets

#endif // OFFSETS_H