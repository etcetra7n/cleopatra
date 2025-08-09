#ifndef DAEMON_H 
#define DAEMON_H
#endif

struct Result {
    int exitcode;
    std::string output;
};

std::string getBaseDirectory();
std::string getMachineID(std::string BASE_DIR);
int saveFile(std::string blob, std::string path);
std::string getDaemonKey(std::string BASE_DIR);

Result exec(std::string rawcmd, std::string BASE_DIR);
Result executeCommand(const char* command);
int SendResultsToWorker(int jobId, Result res, std::string DAEMON_KEY); //const char* CERT_PATH
int logError(std::string errMsg);

std::string escapeChars(std::string str);
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
