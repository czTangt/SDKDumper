#include "../include/mem/Mem.h"

pid_t get_target_pid(const char *process_name)
{
    if (!process_name)
    {
        return -1;
    }

    DIR *dir = opendir("/proc");
    if (!dir)
    {
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            int id = atoi(entry->d_name);
            if (id > 0)
            {
                char path[256];
                std::snprintf(path, sizeof(path), "/proc/%d/cmdline", id);
                std::ifstream cmdline_file(path, std::ios::in | std::ios::binary);
                if (cmdline_file)
                {
                    std::string cmdline;
                    std::getline(cmdline_file, cmdline, '\0'); // 读取到第一个'\0'
                    if (std::strcmp(process_name, cmdline.c_str()) == 0)
                    {
                        closedir(dir);
                        return id;
                    }
                }
            }
        }
    }

    closedir(dir);
    return -1;
}

ModuleRange lib_range{0, 0, 0};
ModuleRange get_module_range(pid_t pid, const char *module_name)
{
    if (!module_name || !*module_name)
        return lib_range;

    char path[256];
    std::snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    std::ifstream maps_file(path);
    if (!maps_file)
        return lib_range;

    std::string line;
    bool found = false;
    while (std::getline(maps_file, line))
    {
        if (line.find(module_name) != std::string::npos)
        {
            size_t dash_pos = line.find('-');
            size_t end_pos = line.find(' ', dash_pos + 1);
            if (dash_pos != std::string::npos && end_pos != std::string::npos)
            {
                std::string start_str = line.substr(0, dash_pos);
                std::string end_str = line.substr(dash_pos + 1, end_pos - dash_pos - 1);
                kaddr start = std::stoull(start_str, nullptr, 16);
                kaddr end = std::stoull(end_str, nullptr, 16);
                if (!found || start < lib_range.start)
                    lib_range.start = start;
                if (!found || end > lib_range.end)
                    lib_range.end = end;
                lib_range.size = lib_range.end - lib_range.start;
                found = true;
            }
        }
    }
    return lib_range;
}