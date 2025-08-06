#ifndef STRUCTS_H
#define STRUCTS_H

#include "Dumper.h"

struct UStruct;

struct UObject
{
    static kaddr getClass(kaddr object)
    { // UClass*
        return Tools::getPtr(object + Offsets.UObject.ClassPrivate);
    }

    static kaddr getOuter(kaddr object)
    { // UObject*
        return Tools::getPtr(object + Offsets.UObject.OuterPrivate);
    }

    static uint32 getNameID(kaddr object)
    {
        return Tools::Read<uint32>(object + Offsets.UObject.NamePrivate + Offsets.FName.ComparisonIndex);
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

struct UStruct
{
    static kaddr getSuperClass(kaddr structz)
    { // UStruct* -> 该结构体的父类
        return Tools::getPtr(structz + Offsets.UStruct.SuperStruct);
    }

    static kaddr getChildProperties(kaddr structz)
    { // FField* -> 该结构体的属性
        return Tools::getPtr(structz + Offsets.UStruct.ChildProperties);
    }

    static uint32 getPropertiesSize(kaddr structz)
    {
        return Tools::Read<uint32>(structz + Offsets.UStruct.PropertiesSize);
    }

    static std::string getCPPName(kaddr object)
    {
        std::string name = UObject::getName(object);

        ObjectType objType = ClassifyObject(UObject::getClass(object));

        switch (objType)
        {
        case ObjectType::ACTOR:
            return "A" + name;

        case ObjectType::CLASS:
            return "U" + name; // UClass 类型

        case ObjectType::ENUM:
            return "E" + name; // 枚举类型

        case ObjectType::FUNCTION:
            return "U" + name; // UFunction 类型

        case ObjectType::STRUCT:
            return "F" + name; // 结构体类型

        case ObjectType::OTHER:
            return "F" + name; // 默认为结构体
        }
    }
};

struct FField
{
    static std::string getName(kaddr fField)
    { // FName -> 该字段的名称
        return GetFNameFromID(Tools::Read<uint32>(fField + Offsets.FField.NamePrivate));
    }

    static std::string getClassName(kaddr fField)
    { // FFieldClass* -> 该字段的类型名称
        return GetFNameFromID(Tools::Read<uint32>(Tools::getPtr(fField + Offsets.FField.ClassPrivate)));
    }

    static kaddr getNext(kaddr fField)
    { // FField* -> 下一个 FField* 对象
        return Tools::getPtr(fField + Offsets.FField.Next);
    }
};

struct FProperty
{
    static uint32 getElementSize(kaddr prop)
    {
        return Tools::Read<uint32>(prop + Offsets.FProperty.ElementSize);
    }

    static uint64 getPropertyFlags(kaddr prop)
    {
        return Tools::Read<uint64>(prop + Offsets.FProperty.PropertyFlags);
    }

    static uint32 getOffset(kaddr prop)
    {
        return Tools::Read<uint32>(prop + Offsets.FProperty.Offset_Internal);
    }

    static std::string getPropCPPName(kaddr prop)
    {
        kaddr propObject = Tools::getPtr(prop + Offsets.FProperty.Size);
        return UStruct::getCPPName(propObject);
    }
};

struct FByteProperty
{
    static kaddr getEnum(kaddr prop)
    { // class UEnum*
        return Tools::getPtr(prop + Offsets.FByteProperty.Enum);
    }
};

struct UEnum
{
    static kaddr getNameArray(kaddr en)
    {
        return Tools::getPtr(en + Offsets.UEnum.Names);
    }

    static uint32 getCount(kaddr en)
    {
        return Tools::Read<uint32>(en + Offsets.UEnum.ArrayNum);
    }
};

struct FBoolProperty
{
    static uint8 getFieldSize(kaddr prop)
    {
        return Tools::Read<uint8>(prop + Offsets.FBoolProperty.FieldSize);
    }

    static uint8 getByteOffset(kaddr prop)
    {
        return Tools::Read<uint8>(prop + Offsets.FBoolProperty.ByteOffset);
    }

    static uint8 getByteMask(kaddr prop)
    {
        return Tools::Read<uint8>(prop + Offsets.FBoolProperty.ByteMask);
    }

    static uint8 getFieldMask(kaddr prop)
    {
        return Tools::Read<uint8>(prop + Offsets.FBoolProperty.FieldMask);
    }
};

struct FObjectPropertyBase
{
    static kaddr getPropertyClass(kaddr prop)
    { // class UClass*
        return Tools::getPtr(prop + Offsets.FObjectProperty.PropertyClass);
    }
};

struct FClassProperty
{
    static kaddr getMetaClass(kaddr prop)
    { // class UClass*
        return Tools::getPtr(prop + Offsets.FClassProperty.MetaClass);
    }
};

struct FInterfaceProperty
{
    static kaddr getInterfaceClass(kaddr prop)
    { // class UClass*
        return Tools::getPtr(prop + Offsets.FInterfaceProperty.InterfaceClass);
    }
};

struct FStructProperty
{
    static kaddr getStruct(kaddr prop)
    { // UStruct*
        return Tools::getPtr(prop + Offsets.FStructProperty.Struct);
    }
};

struct FArrayProperty
{
    static kaddr getInner(kaddr prop)
    { // UProperty*
        return Tools::getPtr(prop + Offsets.FArrayProperty.Inner);
    }
};

struct FMapProperty
{
    static kaddr getKeyProp(kaddr prop)
    { // UProperty*
        return Tools::getPtr(prop + Offsets.FMapProperty.KeyProp);
    }

    static kaddr getValueProp(kaddr prop)
    { // UProperty*
        return Tools::getPtr(prop + Offsets.FMapProperty.ValueProp);
    }
};

struct FSetProperty
{
    static kaddr getElementProp(kaddr prop)
    { // UProperty*
        return Tools::getPtr(prop + Offsets.FSetProperty.ElementProp);
    }
};

struct FEnumProperty
{
    static kaddr getEnum(kaddr prop)
    { // class UEnum*
        return Tools::getPtr(prop + Offsets.FEnumProperty.Enum);
    }
};

struct UFunction
{
    static int32 getFunctionFlags(kaddr func)
    {
        return Tools::Read<int32>(func + Offsets.UFunction.FunctionFlags);
    }

    static kaddr getFunc(kaddr func)
    {
        return Tools::getPtr(func + Offsets.UFunction.Func);
    }
};

struct UField
{
    static kaddr getNext(kaddr field)
    { // UField*
        return Tools::getPtr(field + Offsets.UField.Next);
    }
};

#endif // STRUCTS_H