#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "../include/core/FNames.h"
#include "../include/elf/Fix.h"
#include "../include/mem/Mem.h"
#include "../include/proc/Proc.h"
#include "../include/utils/Config.h"

using namespace std;

string pkg("com.tencent.ace.match2024");
string outputpath("/data/local/tmp/match2024");
static const char *lib_name = "libUE4.so";
bool isStrDump = false;
bool isVerbose = false;

kaddr getHexAddr(const char *addr)
{
    return (kaddr)strtoul(addr, nullptr, 16);
}

int main(int argc, char *argv[])
{
    // check output path
    struct stat st;
    if (stat(outputpath.c_str(), &st) != 0)
    {
        if (mkdir(outputpath.c_str(), 0755) != 0)
        {
            cout << "Can't create directory: " << outputpath << endl;
            return -1;
        }
    }

    // get pid
    target_pid = get_target_pid(pkg.c_str());
    if (target_pid == -1)
    {
        cout << "Can't find the process" << endl;
        return -1;
    }
    cout << "Process name: " << pkg.c_str() << ", Pid: " << target_pid << endl;

    // get module range
    lib_range = get_module_range(target_pid, lib_name);
    if (lib_range.base == 0)
    {
        cout << "Can't find Library: " << lib_name << endl;
        return -1;
    }
    cout << lib_name << ": base_addr: 0x" << std::hex << lib_range.base << ", end_addr: 0x" << lib_range.end
         << ", lib_size: 0x" << lib_range.size << std::dec << endl;

    // init offsets
    Offsets::initOffsets();

    // get strings dump
    isStrDump = true;
    Offsets::GName = getHexAddr("0x0B171CC0");
    if (isStrDump)
    {
        DumpStrings(outputpath);
    }
    return 0;
}