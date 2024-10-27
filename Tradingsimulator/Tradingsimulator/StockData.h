#pragma once
// StockData.h
#ifndef STOCKDATA_H
#define STOCKDATA_H

#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <iostream>

class StockData {
public:
    StockData(const std::string& apiKey);
    nlohmann::json fetchStockData(const std::string& symbol);

private:
    std::string apiKey;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
};

#endif // STOCKDATA_H