#include <string>
#include <fstream>
#include <ctime>
#include <cstring>
#include <windows.h>
#include "daemon.h"

int logError(std::string errMsg){
    std::time_t now = std::time(nullptr);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", std::localtime(&now));
    std::string timestamp(buffer);
    
    char c_path[MAX_PATH];
    GetModuleFileName(NULL, c_path, MAX_PATH);
    c_path[strlen(c_path) - 9] = '\0';
    std::string path(c_path);
    path += "logs";
    
    std::ofstream outFile(path, std::ios::app);
    if (outFile.is_open()) {
        outFile << "[" + timestamp + "] " + errMsg + '\n';
        outFile.close();
    } else {
        return 1;
    }
    return 0;
}