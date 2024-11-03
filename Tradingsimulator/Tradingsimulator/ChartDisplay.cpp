#include "ChartDisplay.h"
#include <algorithm> // Include for std::max_element and std::min_element

ChartDisplay::ChartDisplay(float width, float height)
    : chartWidth(width), chartHeight(height), line(sf::PrimitiveType::LineStrip) {
    setupChartArea(width, height);
}

void ChartDisplay::setupChartArea(float width, float height) {
    chartArea.setSize(sf::Vector2f(width, height));
    chartArea.setFillColor(sf::Color(50, 50, 50)); // Gray background for the chart
    chartArea.setPosition(10, 10); // Initial position, will be overridden by setPosition
}

void ChartDisplay::updateData(const std::vector<float>& prices) {
    historicalPrices = prices;
    updateLine();
}

void ChartDisplay::updateLine() {
    line.clear();
    if (historicalPrices.empty()) return;

    float maxPrice = *std::max_element(historicalPrices.begin(), historicalPrices.end());
    float minPrice = *std::min_element(historicalPrices.begin(), historicalPrices.end());

    // Get the position of the chartArea for offset
    sf::Vector2f chartPosition = chartArea.getPosition();

    for (size_t i = 0; i < historicalPrices.size(); ++i) {
        float x = chartPosition.x + chartWidth * (static_cast<float>(i) / (historicalPrices.size() - 1));
        float y = chartPosition.y + chartHeight - ((historicalPrices[i] - minPrice) / (maxPrice - minPrice) * chartHeight);
        line.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Cyan));
    }
}

void ChartDisplay::draw(sf::RenderWindow& window) {
    window.draw(chartArea);
    window.draw(line);
}

void ChartDisplay::setPosition(float x, float y) {
    chartArea.setPosition(x, y); // Update the position of the chart area
}