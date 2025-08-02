#ifndef STRUCTS_H
#define STRUCTS_H

// 宽字符处理工具
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

// UObject 工具函数
struct UObject
{
    static kaddr getClass(kaddr object)
    { // UClass*
        return Tools::getPtr(object + Offsets::UObjectBase::ClassPrivate);
    }

    static uint32 getNameID(kaddr object)
    {
        return Tools::Read<uint32>(object + Offsets::UObjectBase::NamePrivate + Offsets::FName::ComparisonIndex);
    }

    static bool isValid(kaddr object)
    {
        return (object > 0 && getNameID(object) > 0 && getClass(object) > 0);
    }

    static std::string getName(kaddr object)
    {
        return GetFNameFromID(getNameID(object));
    }

    static std::string getClassName(kaddr object)
    {
        return getName(getClass(object));
    }
};

// UStruct 工具函数
struct UStruct
{
    static kaddr getSuperClass(kaddr structz)
    { // UStruct* -> 该结构体的父类
        return Tools::getPtr(structz + Offsets::UStruct::SuperStruct);
    }

    static kaddr getChildren(kaddr structz)
    { // UField* -> 该结构体的方法
        return Tools::getPtr(structz + Offsets::UStruct::Children);
    }

    static kaddr getChildProperties(kaddr structz)
    { // FField* -> 该结构体的属性
        return Tools::getPtr(structz + Offsets::UStruct::ChildProperties);
    }

    static std::string getClassName(kaddr clazz)
    {
        return UObject::getName(clazz);
    }

    static std::string getClassPath(kaddr object)
    { // 获取当前对象的完全类限定符
        kaddr clazz = UObject::getClass(object);
        std::string classname = UObject::getName(clazz);

        kaddr superclass = getSuperClass(clazz);
        while (superclass)
        {
            classname += ".";
            classname += UObject::getName(superclass);

            superclass = getSuperClass(superclass);
        }

        return classname;
    }

    static std::string getStructClassPath(kaddr clazz)
    { // 获取当前类的完全类限定符
        std::string classname = UObject::getName(clazz);

        kaddr superclass = getSuperClass(clazz);
        while (superclass)
        {
            classname += ".";
            classname += UObject::getName(superclass);

            superclass = getSuperClass(superclass);
        }

        return classname;
    }
};

// FField 工具函数
struct FField
{
    static std::string getName(kaddr fField)
    { // FName -> 该字段的名称
        return GetFNameFromID(Tools::Read<uint32>(fField + Offsets::FField::NamePrivate));
    }

    static std::string getClassName(kaddr fField)
    { // FFieldClass* -> 该字段的类型名称
        return GetFNameFromID(Tools::Read<uint32>(Tools::getPtr(fField + Offsets::FField::ClassPrivate)));
    }

    static kaddr getNext(kaddr fField)
    { // FField* -> 下一个 FField* 对象
        return Tools::getPtr(fField + Offsets::FField::Next);
    }
};

// FProperty 工具函数
struct FProperty
{
    static int32 getElementSize(kaddr prop)
    {
        return Tools::Read<int32>(prop + Offsets::FProperty::ElementSize);
    }

    static uint64 getPropertyFlags(kaddr prop)
    {
        return Tools::Read<uint64>(prop + Offsets::FProperty::PropertyFlags);
    }

    static int32 getOffset(kaddr prop)
    {
        return Tools::Read<int32>(prop + Offsets::FProperty::Offset_Internal);
    }
};

// FObjectProperty 工具函数
struct FObjectProperty
{
    static kaddr getPropertyClass(kaddr prop)
    { // class UClass*
        return Tools::getPtr(prop + Offsets::FObjectProperty::PropertyClass);
    }
};

#endif // STRUCTS_H