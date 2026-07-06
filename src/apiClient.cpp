#include "apiClient.h"

#include <curl/curl.h>
#include <stdexcept>


ApiClient::ApiClient(const std::string& apiKey)
    : m_apiKey(apiKey)
{

}
static size_t WriteCallback(void* contents,
    size_t size,
    size_t nmemb,
    void* userp)
{
    size_t totalSize = size * nmemb;

    std::string* response =
        static_cast<std::string*>(userp);

    response->append(static_cast<char*>(contents), totalSize);

    return totalSize;
}

std::string ApiClient::Get(const std::string& endpoint)
{
    CURL* curl = curl_easy_init();

    if (!curl)
        throw std::runtime_error("Failed to initialize curl.");


	curl_slist* headers = nullptr;
	std::string apiHeader = "X-Auth-Token: " + m_apiKey;
    headers = curl_slist_append(headers, apiHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string response;
    std::string url = BASE_URL + endpoint;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl,
        CURLOPT_WRITEFUNCTION,
        WriteCallback);

    curl_easy_setopt(curl,
        CURLOPT_WRITEDATA,
        &response);

    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK)
    {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error(curl_easy_strerror(result));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return response;    
}