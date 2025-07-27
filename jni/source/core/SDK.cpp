#include "SDK.h"
#include "Structs.h"

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

            if (StrLength > 0)
            {
                // 字符串长度限制
                if (StrLength < 256)
                {
                    std::string str;
                    uint32 key = (blockIdx << 16 | Offset);
                    kaddr StrPtr = FNameEntry + Offsets::FNameEntry::StringName;

                    if (wide)
                        str = WideStr::readString(StrPtr, StrLength);
                    else
                        str = Tools::readString(StrPtr, StrLength);

                    gname << "[" << std::setw(6) << std::right << std::hex << key << "]\t" << std::setw(4) << std::left
                          << (wide ? "Wide\t" : "Ansi\t") << "{" << std::setw(2) << std::right << StrLength << "}\t"
                          << str << std::endl;
                }
            }
            else
            {
                StrLength = -StrLength;
            }

            // 遍历下一个
            uint16 totalBytes = Offsets::FNameEntry::StringName + StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            uint32 alignedBytes =
                (totalBytes + Offsets::FNameEntryAllocator::Stride - 1u) & ~(Offsets::FNameEntryAllocator::Stride - 1u);

            It += alignedBytes;
            // BlockSizeBytes = Stride * FNameBlockOffsets 为 2 * 0x10000，因此除以 Stride 保证 offset 在一个 block 内部
            // 真实的块内偏移为 Offset * 2
            Offset += alignedBytes / Offsets::FNameEntryAllocator::Stride;
        }
        else
        {
            break;
        }
    }
}

void DumpStrings(std::string outputpath)
{
    std::ofstream gname(outputpath + "/Strings.txt", std::ofstream::out);
    if (gname.is_open())
    {
        std::cout << "[1] Dumping Strings ---" << std::endl;
        kaddr FNameEntryAllocator = Tools::getRealOffset(Offsets::Global::GName) + Offsets::FNamePool::Entries;
        uint32 CurrentBlock = Tools::Read<uint32>(FNameEntryAllocator + Offsets::FNameEntryAllocator::CurrentBlock);
        uint32 CurrentByteCursor =
            Tools::Read<uint32>(FNameEntryAllocator + Offsets::FNameEntryAllocator::CurrentByteCursor);
        kaddr Block = FNameEntryAllocator + Offsets::FNameEntryAllocator::Blocks;

        // All Blocks Except Current
        for (uint32 BlockIdx = 0; BlockIdx < CurrentBlock; ++BlockIdx)
        {
            DumpBlocks(gname, Block, BlockIdx, Offsets::FNameEntryAllocator::BlockSizeBytes);
        }
        // Last Block
        DumpBlocks(gname, Block, CurrentBlock, CurrentByteCursor);
    }
}

std::string GetFNameFromID(uint32 index)
{
    uint32 Block = index >> Offsets::FNameBlockOffsetBits;
    uint16 Offset = index & (Offsets::FNameBlockOffsets - 1);

    kaddr FNameEntryAllocator = Tools::getRealOffset(Offsets::Global::GName) + Offsets::FNamePool::Entries;
    kaddr Blocks = FNameEntryAllocator + Offsets::FNameEntryAllocator::Blocks;
    kaddr BlockInstance = Tools::getPtr(Blocks + (Block * Offsets::Global::PointerSize));

    kaddr FNameEntry = BlockInstance + (Offsets::FNameEntryAllocator::Stride * Offset);
    int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);

    kaddr StrPtr = FNameEntry + Offsets::FNameEntry::StringName;
    int StrLength = FNameEntryHeader >> Offsets::FNameBlockOffsetBits;
    if (StrLength > 0 && StrLength < 256)
    {
        bool wide = FNameEntryHeader & Offsets::FNameEntryHeader::bIsWide;
        if (wide)
        {
            return WideStr::readString(StrPtr, StrLength);
        }
        else
        {
            return Tools::readString(StrPtr, StrLength);
        }
    }
    else
    {
        return "None";
    }
}

// void DumpActors(std::string outputpath)
// {
//     std::ofstream gname(outputpath + "/Actors.txt", std::ofstream::out);
//     if (gname.is_open())
//     {
//         std::cout << "[2] Dumping Actors ---" << std::endl;
//         kaddr word = Tools::getPtr(Tools::getRealOffset(Offsets::Global::GWorld));
//         kaddr level = Tools::getPtr(word + Offsets::UWorld::PersistentLevel);

//         kaddr actorsArray = Tools::getPtr(level + Offsets::ULevel::AActors);
//         uint32 actorsCount = Tools::Read<uint32>(level + Offsets::ULevel::ActorsCount);

//         for (int i = 0; i < actorsCount; i++)
//         {
//             kaddr actor = Tools::getPtr(actorsArray + (i * Offsets::Global::PointerSize));
//         }
//     }
// }