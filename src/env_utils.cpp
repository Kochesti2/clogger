// env_utils.cpp
#include "env_utils.h"
#include <fstream>
#include <string>

std::string getEnvVar(const std::string& key, const std::string& envPath) {
    std::ifstream file(envPath);
    if (!file.is_open()) {
        return "";
    }

    std::string line;
    while (std::getline(file, line)) {
        // salta righe vuote o commenti tipo "# ..."
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string k = line.substr(0, pos);
        std::string v = line.substr(pos + 1);

        if (k == key) {
            return v;
        }
    }
    return "";
}

