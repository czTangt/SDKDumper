#ifndef OFFSETS_H
#define OFFSETS_H

#include "../utils/Tools.h"

namespace Offsets
{
// Global Offsets
extern kaddr GWorld;
extern kaddr GName;
extern kaddr GUObjectArray;
extern kaddr PointerSize;

//---------SDK-----------//
// Class: FNamePool
extern kaddr FNameStride;
extern kaddr GNamesToFNamePool; // NamePoolData, alignas(FNamePool)
extern kaddr FNamePoolToCurrentBlock;
extern kaddr FNamePoolToCurrentByteCursor;
extern kaddr FNamePoolToBlocks;
// Class: FNameEntry
extern kaddr FNameEntryToLenBit;
extern kaddr FNameEntryToString;

void initOffsets();

} // namespace Offsets

#endif // OFFSETS_H