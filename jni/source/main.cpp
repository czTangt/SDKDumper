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
#include <unistd.h>
#include <vector>

#include "../include/core/FNames.h"
#include "../include/elf/Fix.h"
#include "../include/mem/Mem.h"
#include "../include/proc/Proc.h"
#include "../include/utils/Log.h"
#include "../include/utils/Offsets.h"

using namespace std;

string pkg("com.tencent.ace.match2024");
string outputpath("/data/local/tmp/match2024");
static const char *lib_name = "libUE4.so";
bool isStrDump = false;

kaddr getHexAddr(const char *addr)
{
    return (kaddr)strtoul(addr, nullptr, 16);
}

int main(int argc, char *argv[])
{
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
    if (lib_range.start == 0)
    {
        cout << "Can't find Library: " << lib_name << endl;
        return -1;
    }
    cout << lib_name << ": start_addr: 0x" << std::hex << lib_range.start << ", end_addr: 0x" << lib_range.end
         << ", lib_size: 0x" << lib_range.size << std::dec << endl;

    // get strings dump
    isStrDump = true;
    if (isStrDump)
    {
        Offsets::GName = getHexAddr("0x0B171CC0");
        DumpStrings(outputpath);
        cout << endl;
    }
    return 0;
}