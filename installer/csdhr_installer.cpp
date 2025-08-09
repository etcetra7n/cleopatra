#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <minizip/unzip.h>
#include <cstdlib>
#include <argon2.h>
#include <windows.h>
#include <shlobj.h>
#include <stdexcept>
#include "data.h" 

std::string WideToUTF8(const wchar_t* wideStr);
int saveDaemonKey(std::string key, std::string INSTALL_DIR);
void DaemonScheduler(std::string INSTALL_DIR);
int saveDaemonID(std::string MACHINE_ID, std::string INSTALL_DIR);
void extractZip(const std::string& zipPath, const std::string& destDir);

int main() {
    try {
        wchar_t path[MAX_PATH];
        if (! SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
            std::cerr << "Failed to get AppData path" << std::endl;
            return 1;
        }
        const std::string INSTALL_DIR = WideToUTF8(path)+"\\csdhr";
        //const std::string INSTALL_DIR = "C:/Users/Admin/AppData/Local/Cleopatra";
        const std::string zip_file = INSTALL_DIR + "\\output.zip";
        std::cout << "Install path: " + INSTALL_DIR << "\n" << std::endl;
        
        std::filesystem::create_directories(std::filesystem::path(zip_file).parent_path());

        std::ofstream out(zip_file, std::ios::binary);
        out.write(reinterpret_cast<const char*>(data_zip), data_zip_len);
        out.close();

        extractZip(zip_file, INSTALL_DIR);
        std::remove(zip_file.c_str());

        std::string MACHINE_ID;
        std::cout << "Create MID: ";
        std::cin >> MACHINE_ID;
        
        std::string DAEMON_KEY;
        std::cout << "Enter Key: ";
        std::cin.ignore(64, '\n');
        std::getline(std::cin, DAEMON_KEY);
        
        saveDaemonID(MACHINE_ID, INSTALL_DIR);
        saveDaemonKey(DAEMON_KEY, INSTALL_DIR);
        DaemonScheduler(INSTALL_DIR);
        std::cout << "Installation complete.\n" << std::endl;
        system("pause");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n"<< std::endl;
        system("pause");
        return 0;
    }
    return 0;
}
std::string WideToUTF8(const wchar_t* wideStr) {
    if (!wideStr) return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (size_needed == 0) return "";

    std::string result(size_needed - 1, 0); // exclude null terminator
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &result[0], size_needed, NULL, NULL);

    return result;
}
int saveDaemonID(std::string MACHINE_ID, std::string INSTALL_DIR){
    std::ofstream outFile(INSTALL_DIR+"\\mid.csr");

    if (outFile) {
        outFile << MACHINE_ID;
        outFile.close();
    } else {
        std::cerr << "Failed to save mid" << std::endl;
        return 1;
    }
    return 0;
}
/*void selfDestruct(){
    // Self destructs the file on next reboot
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    MoveFileEx(path, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}*/
void DaemonScheduler(std::string INSTALL_DIR){
    std::string ps_script = "cd \""+INSTALL_DIR+"\";./sch.ps1;Start-Sleep -Milliseconds 200;";
    system(("powershell -Command "+ps_script).c_str());
    const std::string sch_ps1_path = INSTALL_DIR+"\\sch.ps1";
    //std::string cmd_str = "powershell.exe -ExecutionPolicy Bypass -File \""+sch_ps1_path+"\"";
    /*const char* command = cmd_str.c_str()
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        std::cout<<"failed to create pipe"<<std::endl;
        return;
    }

    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;

    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, const_cast<char*>(command), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hRead);
        CloseHandle(hWrite);
        std::cout<<"failed to run script"<<std::endl;
        return;
    }
    CloseHandle(hWrite);
    char buffer[128];
    DWORD bytesRead;
    std::string output;

    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }

    CloseHandle(hRead);
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        exitCode = GetLastError();
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    std::cout<<output<<std::endl;*/
    
    //const char* cmd = "powershell.exe";
    //const std::string params = "-ExecutionPolicy Bypass -FILE \""+INSTALL_DIR+"\\sch.ps1\"";
    //ShellExecuteA(NULL, "runas", cmd, params.c_str(), NULL, SW_HIDE);
    //Sleep(300);
    
    std::remove(sch_ps1_path.c_str());
    //std::remove((INSTALL_DIR+"\\sch.xml").c_str());
    
    //system("cmd /c del /F /Q out");
    //system("start /B powershell -Command Start-Sleep -Milliseconds 200; Remove-Item ./finish.exe -Force");
}
int saveDaemonKey(std::string key, std::string INSTALL_DIR){
    constexpr uint32_t t_cost = 4;
    constexpr uint32_t m_cost = 65536; //64 Mib
    constexpr uint32_t parallelism = 2;
    std::string salt = "B2UVP3o/HEIIZT8ZAz95CT8/Pz90GzA/Pzk/P18/cj8NCg==";
    char hash_output[32];
    constexpr uint32_t HASHLEN = 32;

    int result = argon2id_hash_raw(t_cost, m_cost, parallelism, 
                                  key.c_str(), key.length(), 
                                  salt.c_str(), salt.length(), 
                                  hash_output, HASHLEN);
    char hex_hash[HASHLEN * 2 + 1];
    for (int i = 0; i < HASHLEN; ++i) {
        sprintf(hex_hash + (i * 2), "%02x", (unsigned char)hash_output[i]);
    }
    std::string first_hash(hex_hash);
    
    std::ofstream outFile(INSTALL_DIR+"\\hkey.dat");
    if (outFile) {
        outFile << first_hash;
        outFile.close();
    } else {
        std::cerr << "Failed to save hkey" << std::endl;
        return 1;
    }
    return 0;
}

void extractZip(const std::string& zipPath, const std::string& destDir) {
    unzFile zipfile = unzOpen(zipPath.c_str());
    if (!zipfile) {
        std::cerr << "Cannot open zip file\n";
        return;
    }

    do {
        char filename[32];
        unz_file_info fileInfo;
        if (unzGetCurrentFileInfo(zipfile, &fileInfo, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK)
            break;

        std::string fullPath = destDir + "\\" + filename;

        // Handle directories
        if ((filename[strlen(filename) - 1] == '/')||(filename[strlen(filename) - 1] == '\\')) {
            std::filesystem::create_directories(fullPath);
            continue;
        }

        std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());

        if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            break;

        std::ofstream out(fullPath, std::ios::binary);
        std::vector<char> buffer(4096);
        int read = 0;
        while ((read = unzReadCurrentFile(zipfile, buffer.data(), buffer.size())) > 0)
            out.write(buffer.data(), read);

        out.close();
        unzCloseCurrentFile(zipfile);

    } while (unzGoToNextFile(zipfile) == UNZ_OK);

    unzClose(zipfile);
    return;
}
