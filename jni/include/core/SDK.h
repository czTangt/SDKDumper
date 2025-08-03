#ifndef SDK_H
#define SDK_H

#include "Offsets.h"
#include "Tools.h"

void DumpStrings(std::string outputpath);

void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSize);

std::string GetFNameFromID(uint32 index);

void DumpActors(std::string outputpath);

kaddr GetUObjectFromID(uint32 index);

void DumpObjects(std::string outputpath);

int32 GetObjectCount();

void DumpSDK(std::string outputpath);

std::string resolveProp(std::list<kaddr> &recurrce, kaddr prop);

void writeStruct(std::ofstream &sdk, kaddr clazz);

std::list<kaddr> writeStructChild(std::ofstream &sdk, kaddr childprop);

#endif // SDK_H