#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>

enum class PageState { StartMenu, TradePage };
enum class Timeframe { FifteenMin, OneHour, OneDay };

bool isMouseOver(const sf::RectangleShape& box, const sf::Vector2i& mousePos) {
    return box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

int main() {
    float balance = 1000.0f;
    PageState currentPage = PageState::StartMenu;
    Timeframe currentTimeframe = Timeframe::FifteenMin;
    std::vector<std::string> stocks = { "AAPL", "MSFT", "GOOGL", "AMZN", "TSLA" };
    std::vector<float> marketCaps = { 2500, 2000, 1500, 1800, 1000 };
    std::vector<float> dailyChanges = { -1.25, 0.56, 1.8, -0.45, 0.9 };
    std::string selectedStock = "";

    sf::RenderWindow window(sf::VideoMode(1400, 900), "Trading Simulator");

    // --- Timer Setup ---
    auto lastUpdate = std::chrono::system_clock::now();

    // --- UI Enhancements ---
    sf::Font font;
    font.loadFromFile("Arial.ttf");

    // --- Top Header ---
    sf::RectangleShape header(sf::Vector2f(1380, 70));
    header.setFillColor(sf::Color(30, 30, 30));
    header.setPosition(10, 10);

    sf::Text balanceText("Balance: $" + std::to_string(balance), font, 26);
    balanceText.setFillColor(sf::Color::White);
    balanceText.setPosition(20, 20);

    sf::RectangleShape restartButton(sf::Vector2f(120, 40));
    restartButton.setFillColor(sf::Color(80, 80, 80));
    restartButton.setPosition(1250, 20);

    sf::Text restartText("Restart", font, 22);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(1270, 25);

    // --- Stock Table ---
    sf::RectangleShape stockTable(sf::Vector2f(1380, 600));
    stockTable.setFillColor(sf::Color(40, 40, 40));
    stockTable.setPosition(10, 100);

    sf::Text stockHeaderText("Stock     Market Cap (B)     Daily Change (%)", font, 28);
    stockHeaderText.setFillColor(sf::Color::White);
    stockHeaderText.setPosition(20, 120);

    std::vector<sf::Text> stockTexts;
    for (size_t i = 0; i < stocks.size(); ++i) {
        sf::Text stockText("", font, 24);
        stockText.setFillColor(dailyChanges[i] >= 0 ? sf::Color::Green : sf::Color::Red);
        stockText.setPosition(20, 180 + i * 50);
        stockTexts.push_back(stockText);
    }

    // --- Trade Panel ---
    sf::RectangleShape tradePanel(sf::Vector2f(380, 600));
    tradePanel.setFillColor(sf::Color(60, 60, 60));
    tradePanel.setPosition(10, 100);

    sf::Text tradePanelText("Trade Panel\nBuy/Long | Sell/Short\nLeverage (1-100)\nTP & SL", font, 20);
    tradePanelText.setFillColor(sf::Color::White);
    tradePanelText.setPosition(20, 130);

    // --- Chart Area with Timeframes ---
    sf::RectangleShape chartBox(sf::Vector2f(960, 600));
    chartBox.setFillColor(sf::Color(50, 50, 50));
    chartBox.setPosition(400, 100);

    sf::Text timeframeText("Timeframes:\n15 min | 1 hour | 1 day", font, 20);
    timeframeText.setFillColor(sf::Color::White);
    timeframeText.setPosition(410, 110);

    // --- Bottom Position Summary ---
    sf::RectangleShape positionSummaryBox(sf::Vector2f(1380, 150));
    positionSummaryBox.setFillColor(sf::Color(30, 30, 30));
    positionSummaryBox.setPosition(10, 710);

    sf::Text positionSummaryText("Position Summary:\n\nOpen Positions\nP&L", font, 20);
    positionSummaryText.setFillColor(sf::Color::White);
    positionSummaryText.setPosition(20, 720);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle Mouse Clicks
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (isMouseOver(restartButton, mousePos)) {
                    balance = 1000.0f;
                    currentPage = PageState::StartMenu;
                }

                if (currentPage == PageState::StartMenu) {
                    for (size_t i = 0; i < stockTexts.size(); ++i) {
                        if (stockTexts[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            selectedStock = stocks[i];
                            currentPage = PageState::TradePage;
                            break;
                        }
                    }
                }
            }
        }

        // Update Balance Text
        balanceText.setString("Balance: $" + std::to_string(balance));

        // --- Live Data Update Logic ---
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate);
        if (elapsed.count() >= 5) {  // Update every 5 seconds
            std::default_random_engine generator;
            std::uniform_real_distribution<float> distribution(-2.0f, 2.0f);
            for (auto& change : dailyChanges) {
                change += distribution(generator); // Randomly simulate % change
            }
            lastUpdate = now;
        }

        // Format and set stock data
        for (size_t i = 0; i < stocks.size(); ++i) {
            std::stringstream stockRow;
            stockRow << stocks[i] << "     $" << std::fixed << std::setprecision(1) << marketCaps[i] << "B"
                << "       " << std::fixed << std::setprecision(2) << dailyChanges[i] << "%";
            stockTexts[i].setString(stockRow.str());
            stockTexts[i].setFillColor(dailyChanges[i] >= 0 ? sf::Color::Green : sf::Color::Red);
        }

        // Clear window
        window.clear(sf::Color::Black);

        // Draw Elements
        window.draw(header);
        window.draw(balanceText);
        window.draw(restartButton);
        window.draw(restartText);

        if (currentPage == PageState::StartMenu) {
            window.draw(stockTable);
            window.draw(stockHeaderText);
            for (const auto& stockText : stockTexts) {
                window.draw(stockText);
            }
        }
        else if (currentPage == PageState::TradePage) {
            window.draw(tradePanel);
            window.draw(tradePanelText);
            window.draw(chartBox);
            window.draw(timeframeText);
            window.draw(positionSummaryBox);
            window.draw(positionSummaryText);
        }

        // Display all drawn elements
        window.display();
    }

    return 0;
}
