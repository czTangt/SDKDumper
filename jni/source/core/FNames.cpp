#include "FNames.h"
#include "Config.h"

uint32 GNameLimit = 170000;

// std::string GetFNameFromID(uint32 index)
// {
//     uint32 Block = index >> 16;
//     uint16 Offset = index & 65535;

//     kaddr FNamePool = Tools::getRealOffset(Offsets::GName) + Offsets::GNamesToFNamePool;

//     kaddr NamePoolChunk = Tools::getPtr(FNamePool + Offsets::FNamePoolToBlocks + (Block * Offsets::PointerSize));
//     kaddr FNameEntry = NamePoolChunk + (Offsets::FNameStride * Offset);

//     int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);
//     kaddr StrPtr = FNameEntry + Offsets::FNameEntryToString;
//     int StrLength = FNameEntryHeader >> Offsets::FNameEntryToLenBit;

//     /// Unicode Dumping Not Supported Yet
//     if (StrLength > 0 && StrLength < 250)
//     {
//         bool wide = FNameEntryHeader & 1;
//         if (wide)
//         {
//             return WideStr::getString(StrPtr, StrLength);
//         }
//         else
//         {
//             return Tools::readFixedString(StrPtr, StrLength);
//         }
//     }
//     else
//     {
//         return "None";
//     }
// }

void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSizeBytes)
{
    kaddr It = Tools::getPtr(block + (blockIdx * Offsets::Global::PointerSize));
    kaddr End = It + blockSizeBytes - Offsets::FNameEntry::StringName;
    uint32 Block = blockIdx;
    uint16 Offset = 0;
    while (It < End)
    {
        kaddr FNameEntry = It;
        int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);
        int StrLength = FNameEntryHeader >> Offsets::FNameEntry::StringLenBit;
        if (StrLength)
        {
            bool wide = FNameEntryHeader & 1;

            /// Unicode Dumping Not Supported
            if (StrLength > 0)
            {
                // String Length Limit
                if (StrLength < 250)
                {
                    std::string str;
                    uint32 key = (Block << 16 | Offset);
                    kaddr StrPtr = FNameEntry + Offsets::FNameEntry::StringName;

                    if (wide)
                    {
                        str = WideStr::getString(StrPtr, StrLength);
                    }
                    else
                    {
                        str = Tools::readFixedString(StrPtr, StrLength);
                    }

                    if (isVerbose)
                    {
                        std::cout << (wide ? "Wide" : "") << std::dec << "{" << StrLength << "} " << std::hex << "["
                                  << key << "]: " << str << std::endl;
                    }

                    gname << (wide ? "Wide" : "") << std::dec << "{" << StrLength << "} " << std::hex << "[" << key
                          << "]: " << str << std::endl;
                }
            }
            else
            {
                StrLength = -StrLength; // Negative lengths are for Unicode Characters
            }

            // Next
            Offset += StrLength / Offsets::FNameEntryAllocator::Stride;
            uint16 bytes = Offsets::FNameEntry::StringName + StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            It += (bytes + Offsets::FNameEntryAllocator::Stride - 1u) & ~(Offsets::FNameEntryAllocator::Stride - 1u);
        }
        else
        { // Null-terminator entry found
            break;
        }
    }
}

void DumpStrings(std::string outputpath)
{
    uint32 count = 0;
    std::ofstream gname(outputpath + "/Strings.txt", std::ofstream::out);
    if (gname.is_open())
    {
        std::cout << "[1] Dumping Strings ---" << std::endl;
        kaddr fNameEntryAllocator = Tools::getRealOffset(Offsets::Global::GName) + Offsets::FNamePool::Entries;
        uint32 currentBlock = Tools::Read<uint32>(fNameEntryAllocator + Offsets::FNameEntryAllocator::CurrentBlock);
        uint32 currentByteCursor =
            Tools::Read<uint32>(fNameEntryAllocator + Offsets::FNameEntryAllocator::CurrentByteCursor);
        kaddr block = fNameEntryAllocator + Offsets::FNameEntryAllocator::Blocks;

        // All Blocks Except Current
        for (uint32 BlockIdx = 0; BlockIdx < currentBlock; ++BlockIdx)
        {
            DumpBlocks(gname, block, BlockIdx, Offsets::FNameEntryAllocator::BlockSizeBytes);
        }
        gname << "last_block: " << "CurrentBlock: " << currentBlock << " CurrentByteCursor: " << currentByteCursor
              << std::endl;
        // Last Block
        DumpBlocks(gname, block, currentBlock, currentByteCursor);
    }
}