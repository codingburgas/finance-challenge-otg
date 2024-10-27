// StockChart.cpp
#include "StockChart.h"
#include <limits>

StockChart::StockChart() : maxPrice(0), minPrice(0) {}

void StockChart::parseData(const nlohmann::json& data) {
    lineVertices.clear();
    maxPrice = 0;
    minPrice = std::numeric_limits<float>::max();

    if (data.contains("Time Series (5min)")) {
        for (const auto& point : data["Time Series (5min)"].items()) {
            if (point.value().contains("1. open")) {
                float price = std::stof(point.value()["1. open"].get<std::string>());
                lineVertices.emplace_back(sf::Vector2f(0, price), sf::Color::Green);
                maxPrice = std::max(maxPrice, price);
                minPrice = std::min(minPrice, price);
            }
        }
    }
}

void StockChart::draw(sf::RenderWindow& window, const nlohmann::json& data) {
    parseData(data);
    float width = window.getSize().x, height = window.getSize().y / 2;

    for (size_t i = 1; i < lineVertices.size(); ++i) {
        lineVertices[i].position.x = (width / lineVertices.size()) * i;
        lineVertices[i].position.y = height - (height * (lineVertices[i].position.y - minPrice) / (maxPrice - minPrice));
    }
    window.draw(&lineVertices[0], lineVertices.size(), sf::LinesStrip);
}