#ifndef SDK_H
#define SDK_H

#include "Offsets.h"
#include "Tools.h"

void DumpStrings(std::string outputpath);

void DumpBlocks(std::ofstream &gname, kaddr block, uint32 blockIdx, uint32 blockSize);

std::string GetFNameFromID(uint32 index);

void DumpActors(std::string outputpath);

#endif // DUMP_H