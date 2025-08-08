#ifndef TENCENT_2024_FINAL_H
#define TENCENT_2024_FINAL_H

#include "Offsets.h"

#ifdef kaddr
#undef kaddr
#endif
#define kaddr uint32

struct _2024TencentFinal : public _Offsets
{
    _2024TencentFinal()
    {
        // 设置全局偏移
        Global.GWorld = 0x4F5C0D0;
        Global.GName = 0x4E2EC00;
        Global.GUObjectArray = 0x4E533AC;
        Global.PointerSize = 0x4;

        // 只修改需要的特定字段
        FNameEntryAllocator.CurrentBlock = 0x28;
        FNameEntryAllocator.CurrentByteCursor = 0x2C;
        FNameEntryAllocator.Blocks = 0x30;
    }
};

inline _2024TencentFinal Offsets;

#endif // TENCENT_2024_FINAL_H