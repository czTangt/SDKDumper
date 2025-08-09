#ifndef TENCENT_2024_FINAL_H
#define TENCENT_2024_FINAL_H

#include "Offsets.h"

struct _2024TencentFinal : public _Offsets
{
    _2024TencentFinal()
    {
        Global.GWorld = 0x4F5C0D0;
        Global.GName = 0x4E2EC00;
        Global.GUObjectArray = 0x4E533AC;
        Global.PointerSize = 0x4;

        FNameEntryAllocator.CurrentBlock = 0x28;
        FNameEntryAllocator.CurrentByteCursor = 0x2C;
        FNameEntryAllocator.Blocks = 0x30;

        TUObjectArray.NumElements = 0xC;
        FUObjectItem.Padd = 0x4;
        FUObjectItem.Size = 0x14;
        UObject.ClassPrivate = 0x14;
        UObject.NamePrivate = 0x18;
        UObject.OuterPrivate = 0x20;

        UStruct.ChildProperties = 0x44;
        UStruct.SuperStruct = 0x40;
        UStruct.Children = 0x6C;
        UStruct.PropertiesSize = 0x48; // 比对多个 Class 看出来的，虽然大小和 2024 sdk 有差距，但是看着像

        UFunction.FunctionFlags = 0x84;
        UFunction.Func = 0xA4;

        UField.Next = 0x2C;

        FField.ClassPrivate = 0x8;
        FField.Next = 0x10;
        FField.NamePrivate = 0x14;

        FClassProperty.MetaClass = 0x50;
    }
};

inline _2024TencentFinal Offsets;

#endif // TENCENT_2024_FINAL_H