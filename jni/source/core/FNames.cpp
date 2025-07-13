#include "FNames.h"
#include "Config.h"

void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSizeBytes)
{
    kaddr It = Tools::getPtr(block + (blockIdx * Offsets::Global::PointerSize));
    kaddr End = It + blockSizeBytes - Offsets::FNameEntry::StringName;
    uint16 Offset = 0;
    while (It < End)
    {
        kaddr FNameEntry = It;
        int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);
        int StrLength = FNameEntryHeader >> Offsets::FNameEntryHeader::StringLenBit;
        if (StrLength)
        {
            bool wide = FNameEntryHeader & 1;

            /// Unicode Dumping Not Supported
            if (StrLength > 0)
            {
                // String Length Limit
                if (StrLength < 256)
                {
                    std::string str;
                    uint32 key = (blockIdx << 16 | Offset);
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

                    gname << "[" << std::setw(6) << std::right << std::hex << key << "]\t" << std::setw(4) << std::left
                          << (wide ? "Wide\t" : "Ansi\t") << "{" << std::setw(2) << std::right << StrLength << "}\t"
                          << str << std::endl;
                }
            }
            else
            {
                StrLength = -StrLength;
            }

            // Next
            uint16 totalBytes = Offsets::FNameEntry::StringName + StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            uint32 alignedBytes =
                (totalBytes + Offsets::FNameEntryAllocator::Stride - 1u) & ~(Offsets::FNameEntryAllocator::Stride - 1u);

            It += alignedBytes;
            // BlockSizeBytes = Stride * FNameBlockOffsets 为 2 * 0x10000，因此除以 Stride 保证 offset 在一个 block 内部
            // 真实的块内偏移为 Offset * 2
            Offset += alignedBytes / Offsets::FNameEntryAllocator::Stride;
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
        // Last Block
        DumpBlocks(gname, block, currentBlock, currentByteCursor);
    }
}