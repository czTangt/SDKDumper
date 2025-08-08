#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <getopt.h>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>

#include "Dumper.h"

using namespace std;

// 配置 app 信息
string pkg("com.tencent.ace.gamematch2024final");
string outputpath("/sdcard/Download/match2024_final");
static const char *lib_name = "libUE4.so";

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
    Tools::target_pid = Tools::getTargetPid(pkg.c_str());
    if (Tools::target_pid == -1)
    {
        cout << "Can't find the process" << endl;
        return -1;
    }
    cout << "Process name: " << pkg.c_str() << ", Pid: " << Tools::target_pid << endl;

    // get module range
    Tools::lib_range = Tools::getModuleRange(Tools::target_pid, lib_name);
    if (Tools::lib_range.base == 0)
    {
        cout << "Can't find Library: " << lib_name << endl;
        return -1;
    }
    cout << lib_name << ": base_addr: 0x" << std::hex << Tools::lib_range.base << ", end_addr: 0x"
         << Tools::lib_range.end << ", lib_size: 0x" << Tools::lib_range.size << std::dec << endl;

    // dump strings, actors, sdk
    // DumpStrings(outputpath);
    // DumpActors(outputpath);
    // DumpSDK(outputpath);

    // DumpObjects(outputpath);
    // std::cout << GetFNameFromID(0x6847b) << std::endl;
    std::cout << GetFNameFromID(451687) << std::endl;

    return 0;
}