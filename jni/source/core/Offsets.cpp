#include "../../include/core/Offsets.h"

namespace Offsets
{
//-----Global Offsets-----//
kaddr GWorld = 0;
kaddr GName = 0;
kaddr GUObjectArray = 0;
kaddr PointerSize = 0;

//---------SDK-----------//
// Class: FNamePool
kaddr FNameStride = 0;
kaddr GNamesToFNamePool = 0; // NamePoolData, alignas(FNamePool)
kaddr FNamePoolToCurrentBlock = 0;
kaddr FNamePoolToCurrentByteCursor = 0;
kaddr FNamePoolToBlocks = 0;
// Class: FNameEntry
kaddr FNameEntryToLenBit = 0;
kaddr FNameEntryToString = 0;

void initOffsets()
{
    // Global Offsets
    PointerSize = 0x8;

    //---------SDK-----------//
    // Class: FNamePool
    FNameStride = 0x2;
    GNamesToFNamePool = 0x38;
    FNamePoolToCurrentBlock = 0x0;
    FNamePoolToCurrentByteCursor = 0x4;
    FNamePoolToBlocks = 0x8;
    // Class: FNameEntry
    FNameEntryToLenBit = 6;
    FNameEntryToString = 0x2;
}
} // namespace Offsets