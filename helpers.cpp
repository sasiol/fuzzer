#include "helpers.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string>

std::string findTargetBinary() {
    DIR* dir = opendir("./target");
    if (!dir) return "";

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        if (name == "." || name == "..") continue;

        // only allow files without extension (likely binaries)
        if (name.find('.') != std::string::npos) continue;

        std::string path = "./target/" + name;

        struct stat st;
        if (stat(path.c_str(), &st) == 0) {
            if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
                closedir(dir);
                return path;
            }
        }
    }

    closedir(dir);
    return "";
}