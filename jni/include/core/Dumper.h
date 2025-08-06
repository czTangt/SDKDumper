#ifndef DUMPER_H
#define DUMPER_H

#include "Offsets.h"
#include "Tools.h"
#include <map>
#include <sstream>

inline struct ObjectFullName
{
    kaddr CoreObject = 0;
    kaddr EngineActor = 0;
    kaddr CoreEnum = 0;
    kaddr CoreClass = 0;
    kaddr CoreFunction = 0;
    kaddr CoreScriptStruct = 0;
} objectFullName;

enum class ObjectType
{
    ACTOR,
    ENUM,
    CLASS,
    FUNCTION,
    STRUCT,
    OTHER
};

// 根据 String ID 获取字符串
std::string GetFNameFromID(uint32 index);
// 根据 UObject ID 获取 UObject
kaddr GetUObjectFromID(uint32 index);
int32 GetObjectCount();
// 根据 UObject 的 Class 进行分类
ObjectType ClassifyObject(kaddr classPtr);
std::string GetOuterFullName(kaddr uobj);
// 解析 Property 类型
std::string FormatPropertyType(kaddr prop);

// dump
void DumpStrings(std::string outputpath);
void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSize);
void DumpActors(std::string outputpath);
void DumpSDK(std::string outputpath);

// 处理所有枚举、类、函数和结构体
void ProcessAllEnums(std::string outputpath, const std::vector<kaddr> &enumObjects);
void ProcessAllClasses(std::string outputpath, const std::vector<kaddr> &classObjects);
void ProcessAllFunctions(std::string outputpath, const std::vector<kaddr> &functionObjects);
void ProcessAllStructs(std::string outputpath, const std::vector<kaddr> &structObjects);

#endif // DUMPER_H