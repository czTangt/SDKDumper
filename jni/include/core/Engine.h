#ifndef ENGINE_H
#define ENGINE_H

#include "Offsets.h"
#include "Types.h"

struct TArray
{
  public:
    kaddr *AllocatorInstance;
    uint32 ArrayNum;
    uint32 ArrayMax;
};

class UClass;
class FField;

class UObject
{
  public:
    UClass *GetClass()
    {
        return Tools::Read<UClass *>(this + Offsets.UObject.ClassPrivate);
    }
    UObject *GetOuter()
    {
        return Tools::Read<UObject *>(this + Offsets.UObject.Outer);
    }
    std::string GetName()
    {
        uint32 FNameEntryId = Tools::Read<uint32>(this + Offsets.UObject.NamePrivate);
        return GetFNameFromID(FNameEntryId);
    }
    std::string GetCppName()
    {
        return GetName();
    }
    std::string GetFullName()
    {
        return GetName();
    }

    // static UObject *StaticClass();

    // template <typename T> bool IsA() const;

    template <typename T> T *SafeCast()
    {
        return IsA<T>() ? static_cast<T *>(this) : nullptr;
    }
};

class FNamePool
{
  public:
    std::string GetName(uint32_t FNameEntryId)
    {
        uint32 Block = FNameEntryId >> Offsets.FNameBlockOffsetBits;
        uint16 Offset = FNameEntryId & (Offsets.FNameBlockOffsets - 1);

        kaddr FNameEntryAllocator = Tools::getRealOffset(Offsets.Global.GName) + Offsets.FNamePool.Entries;
        kaddr Blocks = FNameEntryAllocator + Offsets.FNameEntryAllocator.Blocks;
        kaddr BlockInstance = Tools::getPtr(Blocks + (Block * Offsets.Global.PointerSize));

        kaddr FNameEntry = BlockInstance + (Offsets.FNameEntryAllocator.Stride * Offset);
        int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);

        kaddr StrPtr = FNameEntry + Offsets.FNameEntry.StringName;
        int StrLength = FNameEntryHeader >> Offsets.FNameEntryHeader.StringLenBit;
        if (StrLength > 0 && StrLength < 250)
        {
            bool wide = FNameEntryHeader & Offsets.FNameEntryHeader.bIsWide;
            std::string Name;
            if (wide)
            {
                Name = Tools::WideStr::readString(StrPtr, StrLength);
            }
            else
            {
                Name = Tools::readString(StrPtr, StrLength);
            }

            size_t Pos = Name.rfind('/');
            if (Pos != std::string::npos)
            {
                Name = Name.substr(Pos + 1);
            }
            return Name;
        }
        else
        {
            return "None";
        }
    }
};

// class AActor : public UObject
// {
//   public:
//     static UObject *StaticClass();
// }

#endif // ENGINE_H