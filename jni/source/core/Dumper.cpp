#include "Dumper.h"
#include "Structs.h"

void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSizeBytes)
{
    kaddr It = Tools::getPtr(block + (blockIdx * Offsets.Global.PointerSize));
    kaddr End = It + blockSizeBytes - Offsets.FNameEntry.StringName;
    uint16 Offset = 0;
    while (It < End)
    {
        kaddr FNameEntry = It;
        int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);
        int StrLength = FNameEntryHeader >> Offsets.FNameEntryHeader.StringLenBit;
        if (StrLength)
        {
            bool wide = FNameEntryHeader & 1;
            if (StrLength > 0 && StrLength < 250) // 字符串长度限制
            {
                std::string str;
                uint32 key = (blockIdx << 16 | Offset);
                kaddr StrPtr = FNameEntry + Offsets.FNameEntry.StringName;

                if (wide)
                    str = Tools::WideStr::readString(StrPtr, StrLength);
                else
                    str = Tools::readString(StrPtr, StrLength);

                gname << "[0x" << std::setfill('0') << std::setw(6) << std::hex << key << "]\t" << std::setw(4)
                      << (wide ? "Wide\t" : "Ansi\t") << "{0x" << std::setfill('0') << std::setw(3) << StrLength
                      << "}\t" << str << std::endl;
            }
            else
            {
                StrLength = -StrLength;
            }

            // 遍历下一个
            uint16 totalBytes = Offsets.FNameEntry.StringName + StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            uint32 alignedBytes =
                (totalBytes + Offsets.FNameEntryAllocator.Stride - 1u) & ~(Offsets.FNameEntryAllocator.Stride - 1u);

            It += alignedBytes;
            // BlockSizeBytes = Stride * FNameBlockOffsets 为 2 * 0x10000，因此除以 Stride 保证 offset 在一个 block 内部
            // 真实的块内偏移为 Offset * 2
            Offset += alignedBytes / Offsets.FNameEntryAllocator.Stride;
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

        kaddr FNameEntryAllocator = Tools::getRealOffset(Offsets.Global.GName) + Offsets.FNamePool.Entries;
        uint32 CurrentBlock = Tools::Read<uint32>(FNameEntryAllocator + Offsets.FNameEntryAllocator.CurrentBlock);
        uint32 CurrentByteCursor =
            Tools::Read<uint32>(FNameEntryAllocator + Offsets.FNameEntryAllocator.CurrentByteCursor);
        kaddr Block = FNameEntryAllocator + Offsets.FNameEntryAllocator.Blocks;

        std::cout << "[1] Dumping Strings ---" << std::endl;
        std::cout << "Total Blocks: " << std::dec << (CurrentBlock + 1) << std::endl;
        std::cout << "Strings Format: [Key]\tType\t{Length}\tString" << std::endl;

        // All Blocks Except Current
        for (uint32 BlockIdx = 0; BlockIdx < CurrentBlock; ++BlockIdx)
        {
            DumpBlocks(gname, Block, BlockIdx, Offsets.FNameEntryAllocator.BlockSizeBytes);
        }
        // Last Block
        DumpBlocks(gname, Block, CurrentBlock, CurrentByteCursor);
    }
}

std::string GetFNameFromID(uint32 index)
{
    uint32 Block = index >> Offsets.FNameBlockOffsetBits;
    uint16 Offset = index & (Offsets.FNameBlockOffsets - 1);

    kaddr FNameEntryAllocator = Tools::getRealOffset(Offsets.Global.GName) + Offsets.FNamePool.Entries;
    kaddr Blocks = FNameEntryAllocator + Offsets.FNameEntryAllocator.Blocks;
    kaddr BlockInstance = Tools::getPtr(Blocks + (Block * Offsets.Global.PointerSize));

    kaddr FNameEntry = BlockInstance + (Offsets.FNameEntryAllocator.Stride * Offset);
    int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);

    kaddr StrPtr = FNameEntry + Offsets.FNameEntry.StringName;
    int StrLength = FNameEntryHeader >> Offsets.FNameEntryHeader.StringLenBit;
    std::string Name;
    if (StrLength > 0)
    {
        bool wide = FNameEntryHeader & Offsets.FNameEntryHeader.bIsWide;
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

void DumpActors(std::string outputpath)
{
    std::ofstream actorlist(outputpath + "/Actors.txt", std::ofstream::out);
    if (actorlist.is_open())
    {
        kaddr word = Tools::getPtr(Tools::getRealOffset(Offsets.Global.GWorld));
        kaddr level = Tools::getPtr(word + Offsets.UWorld.PersistentLevel);

        kaddr actorsArray = Tools::getPtr(level + Offsets.ULevel.AActors);
        uint32 actorsCount = Tools::Read<uint32>(level + Offsets.ULevel.ActorsCount);

        std::cout << "[2] Dumping Actors ---" << std::endl;
        std::cout << "Total Actors: " << actorsCount << std::endl;

        for (int i = 0; i < actorsCount; i++)
        {
            kaddr actor = Tools::getPtr(actorsArray + (i * Offsets.Global.PointerSize));
            if (UObject::isValid(actor))
            {
                actorlist << "Id: " << std::setw(3) << std::right << std::setbase(10) << i << ", Addr: 0x" << std::hex
                          << actor << ", ActorName: " << UStruct::getCPPName(actor) << std::endl;
            }
        }
    }
}

kaddr GetUObjectFromID(uint32 index)
{
    kaddr TUObjectArray =
        Tools::getPtr(Tools::getRealOffset(Offsets.Global.GUObjectArray) + Offsets.FUObjectArray.ObjObjects);
    kaddr Chunk = Tools::getPtr(TUObjectArray + ((index / Offsets.NumElementsPerChunk) * Offsets.Global.PointerSize));
    return Tools::getPtr(Chunk + ((index % Offsets.NumElementsPerChunk) * Offsets.FUObjectItem.Size));
}

int32 GetObjectCount()
{
    return Tools::Read<int32>(Tools::getRealOffset(Offsets.Global.GUObjectArray) + Offsets.FUObjectArray.ObjObjects +
                              Offsets.TUObjectArray.NumElements);
}

std::string GetOuterFullName(kaddr uobj)
{
    std::string fullname = UObject::getName(uobj);
    kaddr Outer = UObject::getOuter(uobj);
    while (Outer)
    {
        fullname = UObject::getName(Outer) + '.' + fullname;
        Outer = UObject::getOuter(Outer);
    }
    return fullname;
}
// 添加分类函数
ObjectType ClassifyObject(kaddr classPtr)
{
    // 遍历继承链
    kaddr currentClass = classPtr;
    while (currentClass)
    {
        if (currentClass == objectFullName.EngineActor)
        {
            return ObjectType::ACTOR;
        }
        else if (currentClass == objectFullName.CoreEnum)
        {
            return ObjectType::ENUM;
        }
        else if (currentClass == objectFullName.CoreClass)
        {
            return ObjectType::CLASS;
        }
        else if (currentClass == objectFullName.CoreFunction)
        {
            return ObjectType::FUNCTION;
        }
        else if (currentClass == objectFullName.CoreScriptStruct)
        {
            return ObjectType::STRUCT;
        }

        // 继续向上查找父类
        currentClass = UStruct::getSuperClass(currentClass);
    }

    return ObjectType::OTHER;
}

void DumpObjects(std::string outputpath)
{
    std::ofstream obj(outputpath + "/Objects.txt", std::ofstream::out);
    if (obj.is_open())
    {
        int32 count = GetObjectCount();
        std::cout << "[3] Dumping Objects ---" << std::endl;
        std::cout << "Total Objects: " << count << std::endl;

        obj << "============ Core Object ==========\n" << std::endl;
        // 遍历查找核心对象
        for (int32 i = 0; i < count; i++)
        {
            kaddr uobj = GetUObjectFromID(i);
            if (UObject::isValid(uobj))
            {
                std::string outerFullName = GetOuterFullName(uobj);

                // 查找并保存核心对象地址
                if (Tools::isEqual(outerFullName, "CoreUObject.Object"))
                {
                    objectFullName.CoreObject = uobj;
                    obj << "CoreUObject.Object: 0x" << std::hex << uobj << std::endl;
                }
                else if (Tools::isEqual(outerFullName, "Engine.Actor"))
                {
                    objectFullName.EngineActor = uobj;
                    obj << "Engine.Actor: 0x" << std::hex << uobj << std::endl;
                }
                else if (Tools::isEqual(outerFullName, "CoreUObject.Enum"))
                {
                    objectFullName.CoreEnum = uobj;
                    obj << "CoreUObject.Enum: 0x" << std::hex << uobj << std::endl;
                }
                else if (Tools::isEqual(outerFullName, "CoreUObject.Class"))
                {
                    objectFullName.CoreClass = uobj;
                    obj << "CoreUObject.Class: 0x" << std::hex << uobj << std::endl;
                }
                else if (Tools::isEqual(outerFullName, "CoreUObject.Function"))
                {
                    objectFullName.CoreFunction = uobj;
                    obj << "CoreUObject.Function: 0x" << std::hex << uobj << std::endl;
                }
                else if (Tools::isEqual(outerFullName, "CoreUObject.ScriptStruct"))
                {
                    objectFullName.CoreScriptStruct = uobj;
                    obj << "CoreUObject.ScriptStruct: 0x" << std::hex << uobj << std::endl;
                }
            }
        }

        // 基于核心对象分类处理所有对象
        obj << "\n====== Object Classification ======\n" << std::endl;
        std::vector<kaddr> enumObjects;
        std::vector<kaddr> classObjects;
        std::vector<kaddr> functionObjects;

        // 统计计数器
        int32 actorCount = 0, enumCount = 0, classCount = 0, functionCount = 0, structCount = 0, otherCount = 0;

        for (int32 i = 0; i < count; i++)
        {
            kaddr uobj = GetUObjectFromID(i);
            if (UObject::isValid(uobj))
            {
                kaddr classPtr = UObject::getClass(uobj);
                std::string className = UObject::getClassName(uobj);
                std::string objectName = UObject::getName(uobj);

                // 构建继承链并判断对象类型
                ObjectType objType = ClassifyObject(classPtr);

                obj << "[0x" << std::setfill('0') << std::setw(5) << std::hex << i << "] " << "Ptr: 0x" << std::hex
                    << uobj << " ";

                switch (objType)
                {
                case ObjectType::ACTOR:
                    obj << "[ACTOR] " << objectName << " (" << className << ")" << std::endl;
                    actorCount++;
                    break;

                case ObjectType::ENUM:
                    obj << "[ENUM] " << objectName << std::endl;
                    enumObjects.push_back(uobj); // 收集枚举对象
                    enumCount++;
                    break;

                case ObjectType::CLASS:
                    obj << "[CLASS] " << objectName << std::endl;
                    classObjects.push_back(uobj); // 收集类对象
                    classCount++;
                    break;

                case ObjectType::FUNCTION:
                    obj << "[FUNCTION] " << objectName << " in " << className << std::endl;
                    functionObjects.push_back(uobj);
                    functionCount++;
                    break;

                case ObjectType::STRUCT:
                    obj << "[STRUCT] " << objectName << std::endl;
                    structCount++;
                    break;

                case ObjectType::OTHER:
                default:
                    obj << "[OBJECT] " << objectName << " (" << className << ")" << std::endl;
                    otherCount++;
                    break;
                }
            }
        }

        if (!enumObjects.empty())
        {
            std::cout << "[3.1] Processing " << enumObjects.size() << " enums..." << std::endl;
            ProcessAllEnums(outputpath, enumObjects);
        }

        if (!classObjects.empty())
        {
            std::cout << "[3.2] Processing " << classObjects.size() << " classes..." << std::endl;
            ProcessAllClasses(outputpath, classObjects);
        }

        if (!functionObjects.empty())
        {
            std::cout << "[3.3] Processing " << functionObjects.size() << " functions..." << std::endl;
            ProcessAllFunctions(outputpath, functionObjects);
        }

        // 输出统计信息
        obj << "\n====== Statistics ======" << std::endl;
        obj << "Actors: " << std::dec << actorCount << std::endl;
        obj << "Enums: " << enumCount << std::endl;
        obj << "Classes: " << classCount << std::endl;
        obj << "Functions: " << functionCount << std::endl;
        obj << "Structs: " << structCount << std::endl;
        obj << "Others: " << otherCount << std::endl;
        obj << "Total: " << (actorCount + enumCount + classCount + functionCount + structCount + otherCount)
            << std::endl;

        std::cout << std::dec << "Actors: " << actorCount << ", Enums: " << enumCount << ", Classes: " << classCount
                  << ", Functions: " << functionCount << ", Structs: " << structCount << ", Others: " << otherCount
                  << std::endl;
    }
}

void ProcessAllEnums(std::string outputpath, const std::vector<kaddr> &enumObjects)
{
    std::ofstream enumFile(outputpath + "/Enum.hpp", std::ofstream::out);
    if (!enumFile.is_open())
    {
        std::cerr << "Failed to open Enums.hpp for writing!" << std::endl;
        return;
    }

    enumFile << "/*\n"
             << " * Generated by UE SDK Dumper\n"
             << " * Generated on: " << Tools::getCurrentTimeString() << "\n"
             << " */\n";

    for (kaddr enumObj : enumObjects)
    {
        std::string enumName = UObject::getName(enumObj);
        uint32_t enumCount = UEnum::getCount(enumObj);
        kaddr enumNamesArray = UEnum::getNameArray(enumObj);
        uint32 maxCount = 0;

        // 预扫描获取最大值
        for (uint32 i = 0; i < enumCount; i++)
        {
            uint32 enum_num = Tools::Read<uint32>(enumNamesArray + i * Offsets.TPair.Size + Offsets.TPair.Value);
            if (enum_num > maxCount)
                maxCount = enum_num;
        }
        std::string Type = (maxCount > 255) ? "uint32" : "uint8";

        enumFile << "\n// " << UObject::getClassName(enumObj) << " " << GetOuterFullName(enumObj) << std::endl
                 << "enum class " << enumName << " : " << Type << "\n{" << std::endl;

        // 遍历枚举值
        for (uint32 i = 0; i < enumCount; i++)
        {
            uint32 index = Tools::Read<uint32>(enumNamesArray + i * Offsets.TPair.Size + Offsets.TPair.Key);
            uint32 enum_num = Tools::Read<uint32>(enumNamesArray + i * Offsets.TPair.Size + Offsets.TPair.Value);

            std::string enumValueName = GetFNameFromID(index);
            std::string prefix = enumName + "::";
            if (enumValueName.find(prefix) == 0)
            {
                enumValueName = enumValueName.substr(prefix.length());
            }

            enumFile << "\t" << std::left << std::setw(40) << enumValueName << " = " << enum_num << ",\n";
        }
        enumFile << "};\n";
    }
}

std::string FormatPropertyType(kaddr prop)
{
    std::string cname = FField::getClassName(prop);
    if (Tools::isEqual(cname, "NameProperty"))
    {
        return "FName";
    }
    else if (Tools::isEqual(cname, "StrProperty"))
    {
        return "FString";
    }
    else if (Tools::isEqual(cname, "TextProperty"))
    {
        return "FText";
    }
    else if (Tools::isEqual(cname, "UInt16Property"))
    {
        return "uint16";
    }
    else if (Tools::isEqual(cname, "UInt32Property"))
    {
        return "uint32";
    }
    else if (Tools::isEqual(cname, "UInt64Property"))
    {
        return "uint64";
    }
    else if (Tools::isEqual(cname, "Int8Property"))
    {
        return "int8";
    }
    else if (Tools::isEqual(cname, "Int16Property"))
    {
        return "int16";
    }
    else if (Tools::isEqual(cname, "IntProperty"))
    {
        return "int32";
    }
    else if (Tools::isEqual(cname, "Int64Property"))
    {
        return "int64";
    }
    else if (Tools::isEqual(cname, "FloatProperty"))
    {
        return "float";
    }
    else if (Tools::isEqual(cname, "DelegateProperty"))
    {
        return "FDelegate";
    }
    else if (Tools::isEqual(cname, "SoftClassProperty"))
    {
        return "TSoftClassPtr<UObject>";
    }
    else if (Tools::isEqual(cname, "MulticastDelegateProperty"))
    {
        return "FMulticastDelegate";
    }
    else if (Tools::isEqual(cname, "MulticastSparseDelegateProperty"))
    {
        return "FMulticastSparseDelegate";
    }
    else if (Tools::isEqual(cname, "MulticastInlineDelegateProperty"))
    {
        return "FMulticastInlineDelegate";
    }

    // 特殊处理
    if (Tools::isEqual(cname, "MapProperty"))
    {
        std::string keyType = FormatPropertyType(FMapProperty::getKeyProp(prop));
        std::string valueType = FormatPropertyType(FMapProperty::getValueProp(prop));
        return "TMap<" + keyType + ", " + valueType + ">";
    }
    else if (Tools::isEqual(cname, "SetProperty"))
    {
        return "TSet<" + FormatPropertyType(FSetProperty::getElementProp(prop)) + ">";
    }
    else if (Tools::isEqual(cname, "EnumProperty"))
    {
        return "enum class " + UObject::getName(FEnumProperty::getEnum(prop));
    }
    else if (Tools::isEqual(cname, "BoolProperty"))
    {
        if (FBoolProperty::getFieldMask(prop) == 0xFF)
        {
            return "bool"; // 如果是单个布尔值
        }
        return "char";
    }
    else if (Tools::isEqual(cname, "ByteProperty"))
    {
        return "enum class " + UObject::getName(FByteProperty::getEnum(prop));
    }
    else if (Tools::isEqual(cname, "ClassProperty"))
    {
        return UStruct::getCPPName(FClassProperty::getMetaClass(prop)) + "*";
    }
    else if (Tools::isEqual(cname, "StructProperty"))
    {
        return FProperty::getPropCPPName(prop); // 返回结构体指针类型
    }
    else if (Tools::isEqual(cname, "InterfaceProperty"))
    {
        return "TScriptInterface<I" + UObject::getName(FInterfaceProperty::getInterfaceClass(prop)) + ">";
    }
    else if (Tools::isEqual(cname, "ObjectPropertyBase"))
    {
        return FProperty::getPropCPPName(prop);
    }
    else if (Tools::isEqual(cname, "ArrayProperty"))
    {
        return "TArray<" + FormatPropertyType(FArrayProperty::getInner(prop)) + ">";
    }
    else if (Tools::isEqual(cname, "WeakObjectProperty"))
    {
        return "TWeakObjectPtr<" + UStruct::getCPPName(FClassProperty::getMetaClass(prop)) + ">";
    }
    else if (Tools::isEqual(cname, "SoftObjectProperty"))
    {
        return "TSoftObjectPtr<" + UStruct::getCPPName(FClassProperty::getMetaClass(prop)) + ">";
    }

    return FProperty::getPropCPPName(prop) + "*"; // 默认返回指针类型
}

void ProcessAllClasses(std::string outputpath, const std::vector<kaddr> &classObjects)
{
    std::ofstream classFile(outputpath + "/Class.hpp", std::ofstream::out);
    if (!classFile.is_open())
    {
        std::cerr << "Failed to open Enums.hpp for writing!" << std::endl;
        return;
    }

    classFile << "/*\n"
              << " * Generated by UE SDK Dumper\n"
              << " * Generated on: " << Tools::getCurrentTimeString() << "\n"
              << " */\n";

    for (kaddr classObj : classObjects)
    {
        int pos = 0;
        std::string className = UStruct::getCPPName(classObj);
        uint32 classSize = UStruct::getPropertiesSize(classObj);
        kaddr superClass = UStruct::getSuperClass(classObj);
        if (superClass)
        {
            pos = UStruct::getPropertiesSize(superClass);
        }

        classFile << "\n// " << UObject::getClassName(classObj) << " " << GetOuterFullName(classObj) << std::endl
                  << "// Class Size: 0x" << std::hex << classSize << std::endl;
        classFile << "class " << className << " : public " << UStruct::getCPPName(superClass) << std::endl
                  << "{" << std::endl;

        for (kaddr prop = UStruct::getChildProperties(classObj); prop; prop = FField::getNext(prop))
        {
            std::string oname = FField::getName(prop);      // 成员变量名称
            std::string cname = FField::getClassName(prop); // 成员变量类型名称
            uint32 offset = FProperty::getOffset(prop);     // 成员变量偏移
            uint32 size = FProperty::getElementSize(prop);  // 成员变量大小
            std::string formattedType = FormatPropertyType(prop);

            if (pos < size)
            {
                int diff = offset - pos;
                char paddingName[64];
                sprintf(paddingName, "padding[0x%04X];", diff);

                // 输出填充数组
                std::ostringstream paddingLine;
                paddingLine << "\t" << std::left << std::setw(50) << "char" << std::left << std::setw(50) << paddingName
                            << "// 0x" << std::right << std::setfill('0') << std::setw(4) << std::hex << std::uppercase
                            << pos << "(0x" << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << diff
                            << ")";

                classFile << paddingLine.str() << std::endl;
            }

            std::ostringstream line;
            line << "\t" << std::left << std::setw(50) << formattedType << std::left << std::setw(50) << (oname + ";")
                 << "// 0x" << std::right << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << offset
                 << "(0x" << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << size << ")";
            classFile << line.str() << std::endl;

            pos = offset + size;
        }

        if (pos < classSize)
        {
            int diff = classSize - pos;

            // 生成末尾填充数组
            char paddingName[64];
            sprintf(paddingName, "padding[0x%04X];", diff);

            std::ostringstream paddingLine;
            paddingLine << "\t" << std::left << std::setw(50) << "char" << std::left << std::setw(50) << paddingName
                        << "// 0x" << std::right << std::setfill('0') << std::setw(4) << std::hex << std::uppercase
                        << pos << "(0x" << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << diff
                        << ")";

            classFile << paddingLine.str() << std::endl;
        }
        classFile << "};\n";
    }
}

void ProcessAllFunctions(std::string outputpath, const std::vector<kaddr> &functionObjects)
{
    std::ofstream functionFile(outputpath + "/Function.hpp", std::ofstream::out);
    if (!functionFile.is_open())
    {
        std::cerr << "Failed to open Function.hpp for writing!" << std::endl;
        return;
    }

    functionFile << "/*\n"
                 << " * Generated by UE SDK Dumper\n"
                 << " * Generated on: " << Tools::getCurrentTimeString() << "\n"
                 << " */\n";

    std::map<std::string, std::vector<kaddr>> classFunctionMap;
    for (kaddr funcObj : functionObjects)
    {
        std::string className = UStruct::getCPPName(UObject::getOuter(funcObj));
        std::string functionName = UObject::getName(funcObj);

        if (functionName.find("__Delegate") != std::string::npos || functionName.find("Default__") != std::string::npos)
            continue;

        classFunctionMap[className].push_back(funcObj);
    }

    for (const auto &[className, functions] : classFunctionMap)
    {
        if (functions.empty())
            continue;

        functionFile << "\nclass " << className << "\n{\n";

        for (kaddr funcObj : functions)
        {
            std::string functionName = UObject::getName(funcObj);
            std::string returnVal = "void";
            std::string params = "";

            for (kaddr funcParam = UStruct::getChildProperties(funcObj); funcParam;
                 funcParam = FField::getNext(funcParam))
            {
                uint64 PropertyFlags = FProperty::getPropertyFlags(funcParam);

                if (PropertyFlags & 0x0000000000000400)
                {
                    returnVal = FormatPropertyType(funcParam);
                }
                else
                {
                    std::string paramStr = "";

                    if (PropertyFlags & 0x0000000000000100)
                        paramStr += "out ";
                    if (PropertyFlags & 0x0000000000000002)
                        paramStr += "const ";

                    paramStr += FormatPropertyType(funcParam) + " " + FField::getName(funcParam);

                    if (!params.empty())
                        params += ", ";
                    params += paramStr;
                }
            }

            if (UFunction::getFunctionFlags(funcObj) & 0x00002000)
                functionFile << "\tstatic ";
            else
                functionFile << "\t";

            functionFile << returnVal << " " << functionName << "(" << params << ")"
                         << " // 0x" << std::hex << UFunction::getFunc(funcObj) - Tools::lib_range.base << "\n";
        }
        functionFile << "};\n\n";
    }
}

std::vector<uint32> structIDMap;

bool isScanned(uint32 id)
{
    for (int i = 0; i < structIDMap.size(); i++)
    {
        if (structIDMap[i] == id)
        {
            return true;
        }
    }
    return false;
}

std::string resolveProp(std::list<kaddr> &recurrce, kaddr prop)
{
    if (prop)
    {
        std::string cname = FField::getClassName(prop);

        if (Tools::isEqual(cname, "ByteProperty"))
        {
            kaddr enumObj = FByteProperty::getEnum(prop);
            std::string enumName = UObject::getName(enumObj);
            // std::cout << "\tenum " << enumName << " " << cname << " : byte"
            //           << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
            //           << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;

            // if (UObject::isValid(enumObj))
            // {
            //     std::cout << "\t{" << std::endl;
            //     kaddr enumNamesArray = UEnum::getNameArray(enumObj);
            //     for (uint32 i = 0; i < UEnum::getCount(enumObj); i++)
            //     {
            //         uint32 index =
            //             Tools::Read<uint32>(enumNamesArray + i * Offsets.UEnum.enumItemSize +
            // Offsets.TPair.Key);
            //         uint32 enum_num =
            //             Tools::Read<uint32>(enumNamesArray + i * Offsets.UEnum.enumItemSize +
            //             Offsets.TPair.Value);
            //         std::string enumValueName = GetFNameFromID(index);
            //         std::string prefix = enumName + "::";
            //         if (enumValueName.find(prefix) == 0)
            //         {
            //             enumValueName = enumValueName.substr(prefix.length());
            //         }

            //         std::cout << "\t\t" << enumValueName << " = " << enum_num << ";" << std::endl;
            //     }
            //     std::cout << "\t}" << std::endl;
            // }
            return "byte";
        }
        else if (Tools::isEqual(cname, "UInt16Property"))
        {
            return "uint16";
        }
        else if (Tools::isEqual(cname, "UInt32Property"))
        {
            return "uint32";
        }
        else if (Tools::isEqual(cname, "UInt64Property"))
        {
            return "uint64";
        }
        else if (Tools::isEqual(cname, "Int8Property"))
        {
            return "int8";
        }
        else if (Tools::isEqual(cname, "Int16Property"))
        {
            return "int16";
        }
        else if (Tools::isEqual(cname, "IntProperty"))
        {
            return "int32";
        }
        else if (Tools::isEqual(cname, "Int64Property"))
        {
            return "int64";
        }
        else if (Tools::isEqual(cname, "BoolProperty"))
        {
            return "bool";
        }
        else if (Tools::isEqual(cname, "FloatProperty"))
        {
            return "float";
        }
        else if (Tools::isEqual(cname, "DoubleProperty"))
        {
            return "double";
        }
        else if (Tools::isEqual(cname, "ObjectProperty") || Tools::isEqual(cname, "WeakObjectProperty") ||
                 Tools::isEqual(cname, "LazyObjectProperty") || Tools::isEqual(cname, "AssetObjectProperty") ||
                 Tools::isEqual(cname, "SoftObjectProperty"))
        {
            kaddr propertyClass = FObjectProperty::getPropertyClass(prop);
            recurrce.push_back(propertyClass);
            return UObject::getName(propertyClass) + "*";
        }
        else if (Tools::isEqual(cname, "ClassProperty") || Tools::isEqual(cname, "AssetClassProperty") ||
                 Tools::isEqual(cname, "SoftClassProperty"))
        {
            kaddr metaClass = FClassProperty::getMetaClass(prop);
            recurrce.push_back(metaClass);
            return "class " + UObject::getName(metaClass);
        }
        else if (Tools::isEqual(cname, "InterfaceProperty"))
        {
            kaddr interfaceClass = FInterfaceProperty::getInterfaceClass(prop);
            recurrce.push_back(interfaceClass);
            return "interface class " + UObject::getName(interfaceClass);
        }
        else if (Tools::isEqual(cname, "NameProperty"))
        {
            return "FName";
        }
        else if (Tools::isEqual(cname, "StructProperty"))
        {
            kaddr Struct = FStructProperty::getStruct(prop);
            recurrce.push_back(Struct);
            return UObject::getName(Struct);
        }
        else if (Tools::isEqual(cname, "StrProperty"))
        {
            return "FString";
        }
        else if (Tools::isEqual(cname, "TextProperty"))
        {
            return "FText";
        }
        else if (Tools::isEqual(cname, "ArrayProperty"))
        {
            return resolveProp(recurrce, FArrayProperty::getInner(prop)) + "[]";
        }
        else if (Tools::isEqual(cname, "DelegateProperty") || Tools::isEqual(cname, "MulticastDelegateProperty"))
        {
            return "delegate";
        }
        else if (Tools::isEqual(cname, "MapProperty"))
        {
            return "<" + resolveProp(recurrce, FMapProperty::getKeyProp(prop)) + "," +
                   resolveProp(recurrce, FMapProperty::getValueProp(prop)) + ">";
        }
        else if (Tools::isEqual(cname, "SetProperty"))
        {
            return "<" + resolveProp(recurrce, FSetProperty::getElementProp(prop)) + ">";
        }
        else if (Tools::isEqual(cname, "EnumProperty"))
        {
            return "enum";
        }
        else
        {
            return FField::getName(prop) + "(" + cname + ")";
        }
    }
    return "NULL";
}

std::list<kaddr> writeStructChild(std::ofstream &sdk, kaddr childprop)
{
    std::list<kaddr> recurrce;
    kaddr child = childprop;
    while (child)
    {
        kaddr prop = child;
        std::string oname = FField::getName(prop);      // 成员变量名称
        std::string cname = FField::getClassName(prop); // 成员变量类型名称

        if (Tools::isEqual(cname, "ByteProperty"))
        {
            kaddr enumObj = FByteProperty::getEnum(prop);
            std::string enumName = UObject::getName(enumObj);
            sdk << "\tenum " << enumName << " " << oname << " : byte"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;

            if (UObject::isValid(enumObj))
            {
                sdk << "\t{" << std::endl;
                kaddr enumNamesArray = UEnum::getNameArray(enumObj);
                for (uint32 i = 0; i < UEnum::getCount(enumObj); i++)
                {
                    uint32 index = Tools::Read<uint32>(enumNamesArray + i * Offsets.TPair.Size + Offsets.TPair.Key);
                    uint32 enum_num =
                        Tools::Read<uint32>(enumNamesArray + i * Offsets.TPair.Size + Offsets.TPair.Value);
                    std::string enumValueName = GetFNameFromID(index);
                    std::string prefix = enumName + "::";
                    if (enumValueName.find(prefix) == 0)
                    {
                        enumValueName = enumValueName.substr(prefix.length());
                    }

                    sdk << "\t\t" << enumValueName << " = " << enum_num << ";" << std::endl;
                }
                sdk << "\t}" << std::endl;
            }
        }
        else if (Tools::isEqual(cname, "UInt16Property"))
        {
            sdk << "\tuint16 " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "UInt32Property"))
        {
            sdk << "\tuint32 " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "UInt64Property"))
        {
            sdk << "\tuint64 " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "Int8Property"))
        {
            sdk << "\tint8 " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "Int16Property"))
        {
            sdk << "\tint16 " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "IntProperty"))
        {
            sdk << "\tint " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "Int64Property"))
        {
            sdk << "\tint64 " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "BoolProperty"))
        {
            sdk << "\tbool " << oname << ";" << std::setbase(10)
                << "//(ByteOffset: " << (int)FBoolProperty::getByteOffset(prop)
                << ", ByteMask: " << (int)FBoolProperty::getByteMask(prop)
                << ", FieldMask: " << (int)FBoolProperty::getFieldMask(prop) << ")"
                << "[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "FloatProperty"))
        {
            sdk << "\tfloat " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "DoubleProperty"))
        {
            sdk << "\tdouble " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "ObjectProperty") || Tools::isEqual(cname, "WeakObjectProperty") ||
                 Tools::isEqual(cname, "LazyObjectProperty") || Tools::isEqual(cname, "AssetObjectProperty") ||
                 Tools::isEqual(cname, "SoftObjectProperty"))
        {
            kaddr propertyClass = FObjectProperty::getPropertyClass(prop);

            sdk << "\t" << UObject::getName(propertyClass) << "* " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;

            recurrce.push_back(propertyClass);
        }
        else if (Tools::isEqual(cname, "ClassProperty") || Tools::isEqual(cname, "AssetClassProperty") ||
                 Tools::isEqual(cname, "SoftClassProperty"))
        {
            kaddr metaClass = FClassProperty::getMetaClass(prop);

            sdk << "\tclass " << UObject::getName(metaClass) << "* " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;

            recurrce.push_back(metaClass);
        }
        else if (Tools::isEqual(cname, "InterfaceProperty"))
        {
            kaddr interfaceClass = FInterfaceProperty::getInterfaceClass(prop);

            sdk << "\tinterface class " << UObject::getName(interfaceClass) << "* " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "NameProperty"))
        {
            sdk << "\tFName " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "StructProperty"))
        {
            kaddr Struct = FStructProperty::getStruct(prop);

            sdk << "\t" << UObject::getName(Struct) << " " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;

            recurrce.push_back(Struct);
        }
        else if (Tools::isEqual(cname, "StrProperty"))
        {
            sdk << "\tFString " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "TextProperty"))
        {
            sdk << "\tFText " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "ArrayProperty"))
        {
            sdk << "\t" << resolveProp(recurrce, FArrayProperty::getInner(prop)) << "[] " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "DelegateProperty") || Tools::isEqual(cname, "MulticastDelegateProperty") ||
                 Tools::isEqual(cname, "MulticastInlineDelegateProperty") ||
                 Tools::isEqual(cname, "MulticastSparseDelegateProperty"))
        {
            sdk << "\tdelegate " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "MapProperty"))
        {
            sdk << "\t<" << resolveProp(recurrce, FMapProperty::getKeyProp(prop)) << ","
                << resolveProp(recurrce, FMapProperty::getValueProp(prop)) << "> " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "SetProperty"))
        {
            sdk << "\t<" << resolveProp(recurrce, FSetProperty::getElementProp(prop)) << "> " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else if (Tools::isEqual(cname, "EnumProperty"))
        {
            kaddr enumObj = FEnumProperty::getEnum(prop);
            std::string enumName = UObject::getName(enumObj);
            sdk << "\tenum " << enumName << " " << oname << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop)
                << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;

            if (UObject::isValid(enumObj))
            {
                sdk << "\t{" << std::endl;
                kaddr enumNamesArray = UEnum::getNameArray(enumObj);
                for (uint32 i = 0; i < UEnum::getCount(enumObj); i++)
                {
                    uint32 index = Tools::Read<uint32>(enumNamesArray + i * Offsets.TPair.Size + Offsets.TPair.Key);
                    uint32 enum_num =
                        Tools::Read<uint32>(enumNamesArray + i * Offsets.TPair.Size + Offsets.TPair.Value);
                    std::string enumValueName = GetFNameFromID(index);
                    std::string prefix = enumName + "::";
                    if (enumValueName.find(prefix) == 0)
                    {
                        enumValueName = enumValueName.substr(prefix.length());
                    }
                    sdk << "\t\t" << enumValueName << " = " << enum_num << ";" << std::endl;
                }
                sdk << "\t}" << std::endl;
            }
        }
        else if (Tools::isEqual(cname, "XigPtrProperty"))
        {
            sdk << "\tXigPtrProperty " << oname << ";"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        else
        {
            sdk << "\t" << cname << " " << oname << ";"
                << "//[Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }
        child = FField::getNext(child);
    }
    return recurrce;
}

std::list<kaddr> writeStructChild_Func(std::ofstream &sdk, kaddr childprop)
{
    std::list<kaddr> recurrce;
    kaddr child = childprop;
    while (child)
    {
        kaddr prop = child;
        std::string oname = UObject::getName(prop);
        std::string cname = UObject::getClassName(prop);

        if (Tools::isStartWith(cname, "Function") || Tools::isEqual(cname, "DelegateFunction"))
        {
            std::string returnVal = "void";
            std::string params = "";

            kaddr funcParam = UStruct::getChildProperties(prop);
            while (funcParam)
            {
                uint64 PropertyFlags = FProperty::getPropertyFlags(funcParam);

                if ((PropertyFlags & 0x0000000000000400) == 0x0000000000000400)
                {
                    returnVal = resolveProp(recurrce, funcParam);
                }
                else
                {
                    if ((PropertyFlags & 0x0000000000000100) == 0x0000000000000100)
                    {
                        params += "out ";
                    }
                    if ((PropertyFlags & 0x0000000000000002) == 0x0000000000000002)
                    {
                        params += "const ";
                    }
                    params += resolveProp(recurrce, funcParam);
                    params += " ";
                    params += FField::getName(funcParam);
                    params += ", ";
                }

                funcParam = FField::getNext(funcParam);
            }

            if (!params.empty())
            {
                params.pop_back();
                params.pop_back();
            }

            sdk << "\t";

            int32 FunctionFlags = UFunction::getFunctionFlags(prop);

            if ((FunctionFlags & 0x00002000) == 0x00002000)
            {
                sdk << "static" << " ";
            }

            sdk << returnVal << " " << oname << "(" << params << ");"
                << "// 0x" << std::hex << (UFunction::getFunc(prop) - Tools::lib_range.base) << std::endl;
        }
        else
        {
            sdk << "\t" << cname << " " << oname << ";"
                << "//[Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
        }

        child = UField::getNext(child);
    }
    return recurrce;
}

void writeStruct(std::ofstream &sdk, kaddr clazz)
{
    // std::list<kaddr> recurrce;

    // kaddr currStruct = clazz;
    // while (UObject::isValid(currStruct))
    // {
    //     std::string name = UObject::getName(currStruct);
    //     if (Tools::isEqual(name, "None") || Tools::isContain(name, "/Game/") || Tools::isContain(name, "_png") ||
    //         name.empty())
    //     {
    //         break;
    //     }

    //     uint32 NameID = UObject::getNameID(currStruct);
    //     if (!isScanned(NameID))
    //     {
    //         structIDMap.push_back(NameID);
    //         sdk << "Class: " << UStruct::getStructClassPath(currStruct) << std::endl;
    //         recurrce.merge(writeStructChild(sdk, UStruct::getChildProperties(currStruct)));
    //         recurrce.merge(writeStructChild_Func(sdk, UStruct::getChildren(currStruct)));
    //         sdk << "\n------------------------------------" << std::endl;
    //     }
    //     currStruct = UStruct::getSuperClass(currStruct);
    // }

    // for (auto it = recurrce.begin(); it != recurrce.end(); ++it)
    //     writeStruct(sdk, *it);
}

void DumpSDK(std::string outputpath)
{
    std::ofstream sdk(outputpath + "/SDK.txt", std::ofstream::out);
    if (sdk.is_open())
    {
        std::cout << "[4] Dumping SDK ---" << std::endl;
        int32 count = GetObjectCount();
        if (count < 10 || count > 999999)
        {
            count = 300000;
        }
        for (int32 i = 0; i < count; i++)
        {
            kaddr uobj = GetUObjectFromID(i);
            if (UObject::isValid(uobj))
            {
                writeStruct(sdk, UObject::getClass(uobj));
            }
        }
    }
}