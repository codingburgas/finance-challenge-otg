// StockData.cpp
#include "StockData.h"

StockData::StockData(const std::string& apiKey) : apiKey(apiKey) {}

size_t StockData::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

nlohmann::json StockData::fetchStockData(const std::string& symbol) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string url = "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol="
        + symbol + "&interval=5min&apikey=" + apiKey;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    // Parse the JSON response
    nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);
    return jsonResponse;
}
