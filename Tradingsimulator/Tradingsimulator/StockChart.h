#pragma once
// StockChart.h
#ifndef STOCKCHART_H
#define STOCKCHART_H

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

class StockChart {
public:
    StockChart();
    void draw(sf::RenderWindow& window, const nlohmann::json& data);

private:
    void parseData(const nlohmann::json& data);
    std::vector<sf::Vertex> lineVertices;
    float maxPrice, minPrice;
};

#endif // STOCKCHART_H
