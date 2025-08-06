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

std::string GetFNameFromID(uint32 index);
kaddr GetUObjectFromID(uint32 index);
int32 GetObjectCount();
ObjectType ClassifyObject(kaddr classPtr);
std::string GetOuterFullName(kaddr uobj);
std::string FormatPropertyType(kaddr prop);

void DumpStrings(std::string outputpath);
void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSize);
void DumpActors(std::string outputpath);
void DumpSDK(std::string outputpath);

void ProcessAllEnums(std::string outputpath, const std::vector<kaddr> &enumObjects);
void ProcessAllClasses(std::string outputpath, const std::vector<kaddr> &classObjects);
void ProcessAllFunctions(std::string outputpath, const std::vector<kaddr> &functionObjects);
void ProcessAllStructs(std::string outputpath, const std::vector<kaddr> &structObjects);

#endif // DUMPER_H