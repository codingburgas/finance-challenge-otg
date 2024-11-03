#pragma once
#ifndef CHARTDISPLAY_H
#define CHARTDISPLAY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class ChartDisplay {
public:
    ChartDisplay(float width, float height);
    void updateData(const std::vector<float>& prices);
    void draw(sf::RenderWindow& window);
    void setPosition(float x, float y); // Make setPosition public

private:
    std::vector<float> historicalPrices; // Store historical price data
    sf::VertexArray line; // To draw the price line
    sf::RectangleShape chartArea; // The area where the chart is drawn
    float chartWidth;
    float chartHeight;

    void setupChartArea(float width, float height);
    void updateLine();
};

#endif // CHARTDISPLAY_H
