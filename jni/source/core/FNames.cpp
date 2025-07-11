#include "../../include/core/FNames.h"
#include "../../include/utils/Config.h"

uint32 MAX_SIZE = 100;
uint32 GNameLimit = 170000;

std::string GetFNameFromID(uint32 index)
{
    std::cout << "1";
    return "";
}

void DumpBlocks423(std::ofstream &gname, uint32 &count, kaddr FNamePool, uint32 blockId, uint32 blockSize)
{
    kaddr It = Tools::getPtr(FNamePool + Offsets::FNamePoolToBlocks + (blockId * Offsets::PointerSize));
    kaddr End = It + blockSize - Offsets::FNameEntryToString;
    uint32 Block = blockId;
    uint16 Offset = 0;
    while (It < End)
    {
        kaddr FNameEntry = It;
        int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);
        int StrLength = FNameEntryHeader >> Offsets::FNameEntryToLenBit;
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
                    kaddr StrPtr = FNameEntry + Offsets::FNameEntryToString;

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
                    count++;
                }
            }
            else
            {
                StrLength = -StrLength; // Negative lengths are for Unicode Characters
            }

            // Next
            Offset += StrLength / Offsets::FNameStride;
            uint16 bytes = Offsets::FNameEntryToString + StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            It += (bytes + Offsets::FNameStride - 1u) & ~(Offsets::FNameStride - 1u);
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
        kaddr FNamePool = Tools::getRealOffset(Offsets::GName) + Offsets::GNamesToFNamePool;

        uint32 BlockSize = Offsets::FNameStride * 65536;
        uint32 CurrentBlock = Tools::Read<uint32>(FNamePool + Offsets::FNamePoolToCurrentBlock);
        uint32 CurrentByteCursor = Tools::Read<uint32>(FNamePool + Offsets::FNamePoolToCurrentByteCursor);

        // All Blocks Except Current
        for (uint32 BlockIdx = 0; BlockIdx < CurrentBlock; ++BlockIdx)
        {
            DumpBlocks423(gname, count, FNamePool, BlockIdx, BlockSize);
        }

        // Last Block
        DumpBlocks423(gname, count, FNamePool, CurrentBlock, CurrentByteCursor);
        // for (uint32 i = 0; i < GNameLimit; i++)
        // {
        //     std::string s = GetFNameFromID(i);
        //     if (!s.empty())
        //     {
        //         // gname << "[" << i << "]: " << s << std::endl;
        //         std::cout << "[" << i << "]: " << s << std::endl;
        //         count++;
        //     }
        // }
    }
}