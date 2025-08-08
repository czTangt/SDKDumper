#ifndef TENCENT_2024_H
#define TENCENT_2024_H

#include "Offsets.h"

struct _2024Tencent : public _Offsets
{
    _2024Tencent()
    {
        // 设置全局偏移
        Global.GWorld = 0xB32D8A8;
        Global.GName = 0xB171CC0;
        Global.GUObjectArray = 0xB1B5F98;
        Global.PointerSize = 0x8;
    }
};

inline _2024Tencent Offsets;

#endif // TENCENT_2024_H