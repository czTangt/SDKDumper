#include "SDK.h"
#include "Structs.h"

void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSizeBytes)
{
    kaddr It = Tools::getPtr(block + (blockIdx * Offsets::Global::PointerSize));
    kaddr End = It + blockSizeBytes - Offsets::FNameEntry::StringName;
    uint16 Offset = 0;
    while (It < End)
    {
        kaddr FNameEntry = It;
        int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);
        int StrLength = FNameEntryHeader >> Offsets::FNameEntryHeader::StringLenBit;
        if (StrLength)
        {
            bool wide = FNameEntryHeader & 1;

            if (StrLength > 0)
            {
                // 字符串长度限制
                if (StrLength < 256)
                {
                    std::string str;
                    uint32 key = (blockIdx << 16 | Offset);
                    kaddr StrPtr = FNameEntry + Offsets::FNameEntry::StringName;

                    if (wide)
                        str = WideStr::readString(StrPtr, StrLength);
                    else
                        str = Tools::readString(StrPtr, StrLength);

                    gname << "[0x" << std::setfill('0') << std::setw(6) << std::hex << key << "]\t" << std::setw(4)
                          << (wide ? "Wide\t" : "Ansi\t") << "{0x" << std::setfill('0') << std::setw(3) << StrLength
                          << "}\t" << str << std::endl;
                }
            }
            else
            {
                StrLength = -StrLength;
            }

            // 遍历下一个
            uint16 totalBytes = Offsets::FNameEntry::StringName + StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            uint32 alignedBytes =
                (totalBytes + Offsets::FNameEntryAllocator::Stride - 1u) & ~(Offsets::FNameEntryAllocator::Stride - 1u);

            It += alignedBytes;
            // BlockSizeBytes = Stride * FNameBlockOffsets 为 2 * 0x10000，因此除以 Stride 保证 offset 在一个 block 内部
            // 真实的块内偏移为 Offset * 2
            Offset += alignedBytes / Offsets::FNameEntryAllocator::Stride;
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
        kaddr FNameEntryAllocator = Tools::getRealOffset(Offsets::Global::GName) + Offsets::FNamePool::Entries;
        uint32 CurrentBlock = Tools::Read<uint32>(FNameEntryAllocator + Offsets::FNameEntryAllocator::CurrentBlock);
        uint32 CurrentByteCursor =
            Tools::Read<uint32>(FNameEntryAllocator + Offsets::FNameEntryAllocator::CurrentByteCursor);
        kaddr Block = FNameEntryAllocator + Offsets::FNameEntryAllocator::Blocks;

        std::cout << "[1] Dumping Strings ---" << std::endl;
        std::cout << "Total Blocks: " << std::dec << (CurrentBlock + 1) << std::endl;
        std::cout << "Strings Format: [Key]\tType\t{Length}\tString" << std::endl;

        // All Blocks Except Current
        for (uint32 BlockIdx = 0; BlockIdx < CurrentBlock; ++BlockIdx)
        {
            DumpBlocks(gname, Block, BlockIdx, Offsets::FNameEntryAllocator::BlockSizeBytes);
        }
        // Last Block
        DumpBlocks(gname, Block, CurrentBlock, CurrentByteCursor);
    }
}

std::string GetFNameFromID(uint32 index)
{
    uint32 Block = index >> Offsets::FNameBlockOffsetBits;
    uint16 Offset = index & (Offsets::FNameBlockOffsets - 1);

    kaddr FNameEntryAllocator = Tools::getRealOffset(Offsets::Global::GName) + Offsets::FNamePool::Entries;
    kaddr Blocks = FNameEntryAllocator + Offsets::FNameEntryAllocator::Blocks;
    kaddr BlockInstance = Tools::getPtr(Blocks + (Block * Offsets::Global::PointerSize));

    kaddr FNameEntry = BlockInstance + (Offsets::FNameEntryAllocator::Stride * Offset);
    int16 FNameEntryHeader = Tools::Read<int16>(FNameEntry);

    kaddr StrPtr = FNameEntry + Offsets::FNameEntry::StringName;
    int StrLength = FNameEntryHeader >> Offsets::FNameEntryHeader::StringLenBit;
    if (StrLength > 0 && StrLength < 256)
    {
        bool wide = FNameEntryHeader & Offsets::FNameEntryHeader::bIsWide;
        if (wide)
        {
            return WideStr::readString(StrPtr, StrLength);
        }
        else
        {
            return Tools::readString(StrPtr, StrLength);
        }
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
        kaddr word = Tools::getPtr(Tools::getRealOffset(Offsets::Global::GWorld));
        kaddr level = Tools::getPtr(word + Offsets::UWorld::PersistentLevel);

        kaddr actorsArray = Tools::getPtr(level + Offsets::ULevel::AActors);
        uint32 actorsCount = Tools::Read<uint32>(level + Offsets::ULevel::ActorsCount);

        std::cout << "[2] Dumping Actors ---" << std::endl;
        std::cout << "Total Actors: " << actorsCount << std::endl;

        for (int i = 0; i < actorsCount; i++)
        {
            kaddr actor = Tools::getPtr(actorsArray + (i * Offsets::Global::PointerSize));
            if (UObject::isValid(actor))
            {
                actorlist << "Id: " << std::setw(3) << std::right << std::setbase(10) << i << ", Addr: 0x" << std::hex
                          << actor << ", ActorName: " << UObject::getName(actor) << std::endl;
            }
        }
    }
}

kaddr GetUObjectFromID(uint32 index)
{
    kaddr TUObjectArray =
        Tools::getPtr(Tools::getRealOffset(Offsets::Global::GUObjectArray) + Offsets::FUObjectArray::ObjObjects);
    kaddr Chunk =
        Tools::getPtr(TUObjectArray + ((index / Offsets::NumElementsPerChunk) * Offsets::Global::PointerSize));
    return Tools::getPtr(Chunk + ((index % Offsets::NumElementsPerChunk) * Offsets::FUObjectItem::Size));
}

int32 GetObjectCount()
{
    return Tools::Read<int32>(Tools::getRealOffset(Offsets::Global::GUObjectArray) +
                              Offsets::FUObjectArray::ObjObjects + Offsets::TUObjectArray::NumElements);
}

void DumpObjects(std::string outputpath)
{
    std::ofstream obj(outputpath + "/Objects.txt", std::ofstream::out);
    if (obj.is_open())
    {
        int32 count = GetObjectCount();
        if (count < 10 || count > 999999)
        {
            count = 300000;
        }

        std::cout << "[3] Dumping Objects ---" << std::endl;
        std::cout << "Total Objects: " << count << std::endl;

        for (int32 i = 0; i < count; i++)
        {
            kaddr uobj = GetUObjectFromID(i);
            if (UObject::isValid(uobj))
            {
                obj << "[0x" << std::setfill('0') << std::setw(5) << std::hex << i << "]:" << std::endl;
                obj << "Name:  " << UObject::getName(uobj).c_str() << std::endl;
                obj << "Class: " << UObject::getClassName(uobj).c_str() << std::endl;
                obj << "ObjectPtr: 0x" << std::hex << uobj << std::endl;
                obj << "ClassPtr:  0x" << std::hex << UObject::getClass(uobj) << std::endl;
                obj << std::endl;
            }
        }
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
            return "interface class" + UObject::getName(interfaceClass);
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
            sdk << "\tbyte " << oname << " : enum"
                << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
            kaddr enumObj = FByteProperty::getEnum(prop);
            if (UObject::isValid(enumObj))
            {
                sdk << "\t{" << std::endl;
                kaddr enumNamesArray = UEnum::getNameArray(enumObj);
                for (uint32 i = 0; i < UEnum::getCount(enumObj); i++)
                {
                    uint32 index =
                        Tools::Read<uint32>(enumNamesArray + i * Offsets::UEnum::enumItemSize + Offsets::TPair::Key);
                    uint32 enum_num =
                        Tools::Read<uint32>(enumNamesArray + i * Offsets::UEnum::enumItemSize + Offsets::TPair::Value);
                    sdk << "\t\t" << GetFNameFromID(index) << " = " << enum_num << ";" << std::endl;
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
            sdk << "\tenum " << oname << "//[Offset: 0x" << std::hex << FProperty::getOffset(prop) << ", "
                << "Size: 0x" << std::hex << FProperty::getElementSize(prop) << "]" << std::endl;
            sdk << "\t{" << std::endl;
            kaddr enumObj = FEnumProperty::getEnum(prop);
            if (UObject::isValid(enumObj))
            {
                kaddr enumNamesArray = UEnum::getNameArray(enumObj);
                for (uint32 i = 0; i < UEnum::getCount(enumObj); i++)
                {
                    uint32 index =
                        Tools::Read<uint32>(enumNamesArray + i * Offsets::UEnum::enumItemSize + Offsets::TPair::Key);
                    uint32 enum_num =
                        Tools::Read<uint32>(enumNamesArray + i * Offsets::UEnum::enumItemSize + Offsets::TPair::Value);
                    sdk << "\t\t" << GetFNameFromID(index) << " = " << enum_num << ";" << std::endl;
                }
            }
            sdk << "\t}" << std::endl;
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
    std::list<kaddr> recurrce;

    kaddr currStruct = clazz;
    while (UObject::isValid(currStruct))
    {
        std::string name = UObject::getName(currStruct);
        if (Tools::isEqual(name, "None") || Tools::isContain(name, "/Game/") || Tools::isContain(name, "_png") ||
            name.empty())
        {
            break;
        }

        uint32 NameID = UObject::getNameID(currStruct);
        if (!isScanned(NameID))
        {
            structIDMap.push_back(NameID);
            sdk << "Class: " << UStruct::getStructClassPath(currStruct) << std::endl;
            recurrce.merge(writeStructChild(sdk, UStruct::getChildProperties(currStruct)));
            recurrce.merge(writeStructChild_Func(sdk, UStruct::getChildren(currStruct)));
            sdk << "\n------------------------------------" << std::endl;
        }
        currStruct = UStruct::getSuperClass(currStruct);
    }

    for (auto it = recurrce.begin(); it != recurrce.end(); ++it)
        writeStruct(sdk, *it);
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