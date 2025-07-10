#include "../include/core/FNames.h"

void DumpStrings(std::string outputpath)
{
    uint32 count = 0;
    std::ofstream gname(outputpath + "/Strings.txt", std::ofstream::out);
    std::cout << "dumpstrings  " << outputpath;
}