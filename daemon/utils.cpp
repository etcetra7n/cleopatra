#include <fstream>
#include <sstream>
#include <cstdio>
#include <windows.h>
#include "daemon.h"

std::string getBaseDirectory(){
    char c_path[MAX_PATH];
    GetModuleFileNameA(NULL, c_path, MAX_PATH);
    std::string exe_path(c_path);
    std::string BASE_DIR = exe_path.substr(0, exe_path.length()-10);
    CreateDirectoryA((BASE_DIR+"\\cache").c_str(), NULL);
    return BASE_DIR;
}

std::string getMachineID(std::string BASE_DIR){
    std::ifstream file(BASE_DIR+"\\mid.csr");
    if (!file) {
        logError("Could not open MID_CSR file. Using UIM");
        return "UIM";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string machine_id = buffer.str();
    file.close();
    return machine_id;
}
std::string getDaemonKey(std::string BASE_DIR){
    std::ifstream file(BASE_DIR+"\\hkey.dat");
    if (!file) {
        logError("Could not open HKEY file");
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string DAEMON_KEY = buffer.str();
    file.close();
    return DAEMON_KEY;
}

int saveFile(std::string blob, std::string path){
    std::ofstream outFile(path);
    if (outFile.is_open()) {
        outFile << blob;
        outFile.close();
    } else {
        return 1;
    }
    return 0;
}

std::string escapeChars(std::string str){
    std::string result="";
    for (char c:str){
        if(c==10) //newline
          result += "<br>";
        else if(c=='\t')
          result += "<t>";
        else if(c=='"')
          result += "\\\"";
        else if(c=='\\')
          result += "\\\\";
        else if(1<=c&&c<=31)
          continue;
        else
          result += c;
    }
    return result;
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/*CURLcode ssl_ctx_callback(CURL* curl, void* ssl_ctx, void* userptr)
{
    //SSL_CTX *ctx = (SSL_CTX *)ssl_ctx;
    SSL_CTX* ctx = static_cast<SSL_CTX*>(ssl_ctx);
    const char* ca_path = static_cast<const char*>(userptr);

    if (SSL_CTX_load_verify_locations(ctx, ca_path, nullptr) != 1) {
        return CURLE_SSL_CACERT_BADFILE;
    }

    // Optionally, disable system certificates here
    // Example: Disable default system certificate store (if you want)
    //SSL_CTX_set_default_verify_paths(ctx);

    return CURLE_OK;
}*/