#ifndef FNAMES_H
#define FNAMES_H

#include <iostream>
#include <string>

#include "Offsets.h"
#include "Tools.h"

extern uint32 GNameLimit;

void DumpStrings(std::string outputpath);

std::string GetFNameFromID(uint32 index);

void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSize);

struct WideStr
{
    static constexpr size_t MAX_SIZE = 100;

    static int is_surrogate(UTF16 uc)
    {
        return (uc - 0xd800u) < 2048u;
    }

    static int is_high_surrogate(UTF16 uc)
    {
        return (uc & 0xfffffc00) == 0xd800;
    }

    static int is_low_surrogate(UTF16 uc)
    {
        return (uc & 0xfffffc00) == 0xdc00;
    }

    static wchar_t surrogate_to_utf32(UTF16 high, UTF16 low)
    {
        return (high << 10) + low - 0x35fdc00;
    }

    static std::wstring w_str(kaddr str, size_t len)
    {
        auto source = Tools::ReadArr<UTF16>(str, len);
        std::wstring output(len, L'\0');

        for (size_t i = 0; i < len; i++)
        {
            const UTF16 uc = source[i];
            if (!is_surrogate(uc))
            {
                output[i] = uc;
            }
            else
            {
                if (is_high_surrogate(uc) && is_low_surrogate(source[i]))
                    output[i] = surrogate_to_utf32(uc, source[i]);
                else
                    output[i] = L'?';
            }
        }
        return output;
    }

    static std::string getString(kaddr StrPtr, int StrLength)
    {
        std::wstring str = w_str(StrPtr, StrLength);

        std::string result(MAX_SIZE, '\0');

        wcstombs((char *)result.data(), str.c_str(), MAX_SIZE);

        return result;
    }
};
#endif // FNAMES_H