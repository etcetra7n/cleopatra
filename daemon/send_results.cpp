#include <string>
#include <algorithm>
#include <curl/curl.h>
#include "daemon.h"

int SendResultsToWorker(int jobId, Result res, std::string DAEMON_KEY){
    //std::cout << "Job ID: "+std::to_string(jobId) << std::endl;
    //std::cout << "exit code: "+std::to_string(res.exitcode) << std::endl;
    //std::cout << "output: "+ res.output << std::endl;
    CURL *curl;
    CURLcode curlRes;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, "https://<REDACTED>.workers.dev/api/send");

        //curl_easy_setopt(curl, CURLOPT_CAINFO, CERT_PATH);
          
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization:"+DAEMON_KEY).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string post = "{\"JID\":" + std::to_string(jobId) +","+
                            "\"output\":\"" + res.output +"\","+
                            "\"exitCode\":" + std::to_string(res.exitcode) + "}";
        //std::cout << "final JSON BEGIN<"<< post<<">final JSON OVER"<<std::endl;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
        curlRes = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        if (curlRes != CURLE_OK) {
            logError("dispatch failure" + std::to_string(curlRes));
        }
    }
    curl_easy_cleanup(curl);
    return 0;
}