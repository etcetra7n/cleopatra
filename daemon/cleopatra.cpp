#include <iostream>
#include <string>
#include <curl/curl.h>

// Callback function to handle the response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    ((std::string*)userp)->append((char*)contents, total_size); // Append response to the string
    return total_size;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string read_buffer;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set URL for GET request
        curl_easy_setopt(curl, CURLOPT_URL, "https://cleopatra.etcetra7n.workers.dev/api/fetch-daemon-job");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "GET request failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Response: " << std::endl;
            std::cout << read_buffer << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}
