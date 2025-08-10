#ifndef TENCENT_2024_H
#define TENCENT_2024_H

#include "Offsets.h"

struct _2024Tencent : public _Offsets64
{
    _2024Tencent()
    {
        Global.GWorld = 0xB32D8A8;
        Global.GName = 0xB171CC0;
        Global.GUObjectArray = 0xB1B5F98;
    }
};

inline _2024Tencent Offsets;

#endif // TENCENT_2024_H