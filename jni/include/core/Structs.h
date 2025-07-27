#ifndef STRUCTS_H
#define STRUCTS_H

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

    static std::string readString(kaddr StrPtr, int StrLength)
    {
        std::wstring str = w_str(StrPtr, StrLength);

        std::string result(MAX_SIZE, '\0');

        wcstombs((char *)result.data(), str.c_str(), MAX_SIZE);

        return result;
    }
};

// struct UObject
// {
//     static kaddr getClass(kaddr object)
//     { // UClass*
//         return getPtr(object + Offsets::UObjectToClassPrivate);
//     }
//     static uint32 getNameID(kaddr object)
//     {
//         return Read<uint32>(object + Offsets::UObjectToFNameIndex);
//     }
//     static bool isValid(kaddr object)
//     {
//         return (object > 0 && getNameID(object) > 0 && getClass(object) > 0);
//     }
// }

#endif // STRUCTS_H