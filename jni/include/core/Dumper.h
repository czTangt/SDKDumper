#ifndef DUMPER_H
#define DUMPER_H

#include "Offsets.h"
#include "Tools.h"
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

std::string GetFNameFromID(uint32 index);
kaddr GetUObjectFromID(uint32 index);
int32 GetObjectCount();
ObjectType ClassifyObject(kaddr classPtr);
std::string GetOuterFullName(kaddr uobj);

void DumpStrings(std::string outputpath);
void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSize);
void DumpActors(std::string outputpath);
void DumpObjects(std::string outputpath);

void ProcessAllEnums(std::string outputpath, const std::vector<kaddr> &enumObjects);
void ProcessAllClasses(std::string outputpath, const std::vector<kaddr> &classObjects);
std::string FormatPropertyType(kaddr prop);

void DumpSDK(std::string outputpath);

std::string resolveProp(std::list<kaddr> &recurrce, kaddr prop);

void writeStruct(std::ofstream &sdk, kaddr clazz);

std::list<kaddr> writeStructChild_Func(std::ofstream &sdk, kaddr childprop);

std::list<kaddr> writeStructChild(std::ofstream &sdk, kaddr childprop);

#endif // DUMPER_H