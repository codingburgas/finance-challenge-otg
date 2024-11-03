// main.cpp
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <curl/curl.h>
#include "LoginPage.h"
#include "UserDetails.h"
#include "ChartDisplay.h"

enum class PageState { Login, StartMenu, TradePage };

bool isMouseOver(const sf::RectangleShape& box, const sf::Vector2i& mousePos) {
    return box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

// Helper function for handling data from libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    }
    catch (std::bad_alloc& e) {
        return 0;
    }
    return newLength;
}

// Fetch real-time price from CoinGecko using libcurl
float getCurrentPrice(const std::string& crypto) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    std::string url = "https://api.coingecko.com/api/v3/simple/price?ids=" + crypto + "&vs_currencies=usd";

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

    if (res != CURLE_OK) {
        std::cerr << "Error: Unable to fetch price from CoinGecko. Falling back to default price.\n";
        return 100.0f;  // Default price if fetching fails
    }

    // Parse the JSON response to extract price
    size_t pos = readBuffer.find("\"usd\":");
    if (pos != std::string::npos) {
        std::string priceStr = readBuffer.substr(pos + 6);
        priceStr = priceStr.substr(0, priceStr.find_first_of("}"));
        return std::stof(priceStr);
    }
    return 100.0f; // Default price if parsing fails
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1400, 900), "Crypto Trading Simulator");
    window.setFramerateLimit(60);

    // Initialize Login UI and page states
    LoginUI loginUI(window);
    PageState currentPage = PageState::Login;
    bool isLoggedIn = false;
    bool isRegistered = false;

    // Create ChartDisplay instances for each cryptocurrency
    ChartDisplay bitcoinChart(980, 600);
    bitcoinChart.setPosition(400, 100); // Position for Bitcoin
    bitcoinChart.updateData({ 30000, 30500, 29500, 31000, 32000, 31500, 32500 }); // Bitcoin data

    ChartDisplay ethereumChart(980, 600);
    ethereumChart.setPosition(400, 320); // Position for Ethereum
    ethereumChart.updateData({ 2000, 2100, 1950, 2200, 2250, 2150, 2300 }); // Ethereum data

    ChartDisplay binanceCoinChart(980, 600);
    binanceCoinChart.setPosition(400, 540); // Position for Binance Coin
    binanceCoinChart.updateData({ 300, 310, 290, 320, 330, 325, 340 }); // Binance Coin data

    ChartDisplay cardanoChart(980, 600);
    cardanoChart.setPosition(800, 100); // Position for Cardano
    cardanoChart.updateData({ 0.50, 0.52, 0.48, 0.55, 0.56, 0.54, 0.58 }); // Cardano data

    ChartDisplay solanaChart(980, 600);
    solanaChart.setPosition(800, 320); // Position for Solana
    solanaChart.updateData({ 30, 32, 28, 35, 37, 34, 38 }); // Solana data

    int currentChartIndex = 0; // 0 = Bitcoin, 1 = Ethereum, 2 = Binance Coin, 3 = Cardano, 4 = Solana

    float balance = 1000.0f;
    PageState simulatorPage = PageState::StartMenu;
    std::vector<std::string> cryptocurrencies = { "bitcoin", "ethereum", "binancecoin", "cardano", "solana" };
    std::vector<float> marketCaps = { 500, 300, 80, 40, 20 }; // in billions
    std::vector<float> dailyChanges = { -2.3, 1.4, 0.8, -1.2, 0.5 };
    std::string selectedCrypto = "";

    // To hold open positions
    struct Position {
        std::string crypto;
        float amount;
        float entryPrice;
        float leverage;
        float profitLoss;
    };
    std::vector<Position> openPositions;

    // Load font for UI elements
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cout << "Error: Unable to load font file 'Arial.ttf'." << std::endl;
        return -1;
    }

    // Header and Balance Display
    sf::RectangleShape header(sf::Vector2f(1380, 70));
    header.setFillColor(sf::Color(30, 30, 30));
    header.setPosition(10, 10);

    sf::Text balanceText("Balance: $" + std::to_string(balance), font, 26);
    balanceText.setFillColor(sf::Color::White);
    balanceText.setPosition(20, 20);

    // Restart Button
    sf::RectangleShape restartButton(sf::Vector2f(120, 40));
    restartButton.setFillColor(sf::Color(80, 80, 80));
    restartButton.setPosition(1250, 20);

    sf::Text restartText("Restart", font, 22);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(1270, 25);

    // Crypto Table Display
    sf::RectangleShape cryptoTable(sf::Vector2f(1380, 600));
    cryptoTable.setFillColor(sf::Color(40, 40, 40));
    cryptoTable.setPosition(10, 100);

    sf::Text cryptoHeaderText("Crypto     Market Cap (B)     Daily Change (%)", font, 28);
    cryptoHeaderText.setFillColor(sf::Color::White);
    cryptoHeaderText.setPosition(20, 120);

    std::vector<sf::Text> cryptoTexts;
    for (size_t i = 0; i < cryptocurrencies.size(); ++i) {
        sf::Text cryptoText("", font, 24);
        cryptoText.setFillColor(dailyChanges[i] >= 0 ? sf::Color::Green : sf::Color::Red);
        cryptoText.setPosition(20, 180 + i * 50);
        cryptoTexts.push_back(cryptoText);
    }

    // Trading Panel Variables
    sf::RectangleShape tradePanel(sf::Vector2f(380, 600));
    tradePanel.setFillColor(sf::Color(60, 60, 60));
    tradePanel.setPosition(10, 100);

    sf::Text tradePanelText("Trade " + selectedCrypto, font, 20);
    tradePanelText.setFillColor(sf::Color::White);
    tradePanelText.setPosition(20, 130);

    sf::Text moneyInputText("Amount ($):", font, 20);
    moneyInputText.setFillColor(sf::Color::White);
    moneyInputText.setPosition(20, 180);
    sf::RectangleShape moneyInputBox(sf::Vector2f(200, 30));
    moneyInputBox.setFillColor(sf::Color::White);
    moneyInputBox.setPosition(20, 210);
    std::string moneyInput;

    sf::Text leverageInputText("Leverage (1-100):", font, 20);
    leverageInputText.setFillColor(sf::Color::White);
    leverageInputText.setPosition(20, 250);
    sf::RectangleShape leverageInputBox(sf::Vector2f(200, 30));
    leverageInputBox.setFillColor(sf::Color::White);
    leverageInputBox.setPosition(20, 280);
    std::string leverageInput;

    // Buy/Sell Buttons
    sf::RectangleShape buyButton(sf::Vector2f(80, 40));
    buyButton.setFillColor(sf::Color(70, 180, 70));
    buyButton.setPosition(20, 320);
    sf::Text buyButtonText("Long", font, 20);
    buyButtonText.setFillColor(sf::Color::White);
    buyButtonText.setPosition(30, 325);

    sf::RectangleShape sellButton(sf::Vector2f(80, 40));
    sellButton.setFillColor(sf::Color(180, 70, 70));
    sellButton.setPosition(120, 320);
    sf::Text sellButtonText("Short", font, 20);
    sellButtonText.setFillColor(sf::Color::White);
    sellButtonText.setPosition(130, 325);

    sf::RectangleShape backButton(sf::Vector2f(80, 40));
    backButton.setFillColor(sf::Color(80, 80, 80));
    backButton.setPosition(1300, 20);  // Position it at the top right corner
    sf::Text backButtonText("Back", font, 20);
    backButtonText.setFillColor(sf::Color::White);
    backButtonText.setPosition(1310, 25);

    // Position Summary at Bottom with Close Button
    sf::RectangleShape positionSummaryBox(sf::Vector2f(1380, 150));
    positionSummaryBox.setFillColor(sf::Color(30, 30, 30));
    positionSummaryBox.setPosition(10, 710);

    sf::Text positionSummaryText("Open Positions:", font, 20);
    positionSummaryText.setFillColor(sf::Color::White);
    positionSummaryText.setPosition(20, 720);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (currentPage == PageState::Login) {
                loginUI.handleEvent(event, isLoggedIn, isRegistered);
                if (isLoggedIn) {
                    currentPage = PageState::StartMenu;
                }
            }
            else if (currentPage == PageState::StartMenu) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (isMouseOver(restartButton, mousePos)) {
                        balance = 1000.0f;
                        openPositions.clear();
                        currentPage = PageState::StartMenu;
                    }
                    for (size_t i = 0; i < cryptoTexts.size(); ++i) {
                        if (cryptoTexts[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            selectedCrypto = cryptocurrencies[i];
                            currentPage = PageState::TradePage;
                            break;
                        }
                    }
                }
            }
            else if (currentPage == PageState::TradePage) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Right) {
                        currentChartIndex = (currentChartIndex + 1) % 5; // Cycle through 0-4
                    }
                    else if (event.key.code == sf::Keyboard::Left) {
                        currentChartIndex = (currentChartIndex + 4) % 5; // Cycle backwards
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Check for the Back button click
                    if (isMouseOver(backButton, mousePos)) {
                        currentPage = PageState::StartMenu; // Return to the main page
                    }
                }
                if (event.type == sf::Event::TextEntered) {
                    if (moneyInputBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
                        if (event.text.unicode == '\b') {
                            if (!moneyInput.empty()) moneyInput.pop_back();
                        }
                        else if (event.text.unicode < 128) {
                            moneyInput += static_cast<char>(event.text.unicode);
                        }
                    }
                    else if (leverageInputBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
                        if (event.text.unicode == '\b') {
                            if (!leverageInput.empty()) leverageInput.pop_back();
                        }
                        else if (event.text.unicode < 128) {
                            leverageInput += static_cast<char>(event.text.unicode);
                        }
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (isMouseOver(buyButton, mousePos)) {
                        if (!moneyInput.empty() && !leverageInput.empty()) {
                            float money = std::stof(moneyInput);
                            float leverage = std::stof(leverageInput);
                            if (money <= balance) {
                                float entryPrice = getCurrentPrice(selectedCrypto);
                                openPositions.push_back({ selectedCrypto, money, entryPrice, leverage, 0.0f });
                                balance -= money;
                            }
                        }
                    }
                    else if (isMouseOver(sellButton, mousePos)) {
                        if (!moneyInput.empty() && !leverageInput.empty()) {
                            float money = std::stof(moneyInput);
                            float leverage = std::stof(leverageInput);
                            if (money <= balance) {
                                float entryPrice = getCurrentPrice(selectedCrypto);
                                openPositions.push_back({ selectedCrypto, -money, entryPrice, leverage, 0.0f });
                                balance += money;
                            }
                        }
                    }

                    for (size_t i = 0; i < openPositions.size(); ++i) {
                        sf::RectangleShape closeButton(sf::Vector2f(60, 30));
                        closeButton.setPosition(20 + i * 160, 800);
                        if (isMouseOver(closeButton, mousePos)) {
                            float currentPrice = getCurrentPrice(openPositions[i].crypto);
                            if (openPositions[i].amount > 0) { // Long Position
                                openPositions[i].profitLoss = (currentPrice - openPositions[i].entryPrice) * (openPositions[i].amount / openPositions[i].leverage);
                            }
                            else { // Short Position
                                openPositions[i].profitLoss = (openPositions[i].entryPrice - currentPrice) * (-openPositions[i].amount / openPositions[i].leverage);
                            }
                            balance += openPositions[i].profitLoss + openPositions[i].amount;
                            openPositions.erase(openPositions.begin() + i);
                            break;
                        }
                    }
                }
            }
        }

        balanceText.setString("Balance: $" + std::to_string(balance));

        std::stringstream positionSummary;
        positionSummary << "Open Positions:\n";
        for (size_t i = 0; i < openPositions.size(); ++i) {
            positionSummary << openPositions[i].crypto << ": $" << std::fixed << std::setprecision(2)
                << openPositions[i].amount << " (Lev: " << openPositions[i].leverage << ") "
                << "P/L: $" << openPositions[i].profitLoss << "\n";
        }
        positionSummaryText.setString(positionSummary.str());

        // Update Crypto Table
        for (size_t i = 0; i < cryptocurrencies.size(); ++i) {
            std::stringstream cryptoRow;
            cryptoRow << cryptocurrencies[i] << "     $" << std::fixed << std::setprecision(1) << marketCaps[i] << "B"
                << "       " << std::fixed << std::setprecision(2) << dailyChanges[i] << "%";
            cryptoTexts[i].setString(cryptoRow.str());
            cryptoTexts[i].setFillColor(dailyChanges[i] >= 0 ? sf::Color::Green : sf::Color::Red);
        }

        window.clear(sf::Color::Black);

        if (currentPage == PageState::Login) {
            loginUI.draw();
        }
        else if (currentPage == PageState::StartMenu) {
            window.draw(header);
            window.draw(balanceText);
            window.draw(restartButton);
            window.draw(restartText);
            window.draw(cryptoTable);
            window.draw(cryptoHeaderText);

            for (const auto& cryptoText : cryptoTexts) {
                window.draw(cryptoText);
            }
        }
        else if (currentPage == PageState::TradePage) {
            tradePanelText.setString("Trade " + selectedCrypto);
            window.draw(tradePanel);
            window.draw(tradePanelText);
            window.draw(balanceText);
            window.draw(moneyInputText);
            window.draw(moneyInputBox);
            window.draw(leverageInputText);
            window.draw(leverageInputBox);
            window.draw(buyButton);
            window.draw(buyButtonText);
            window.draw(sellButton);
            window.draw(sellButtonText);
            window.draw(positionSummaryBox);
            window.draw(positionSummaryText);
            window.draw(backButton);
            window.draw(backButtonText);
            switch (currentChartIndex) {
            case 0: bitcoinChart.draw(window); break;  // Bitcoin
            case 1: ethereumChart.draw(window); break; // Ethereum
            case 2: binanceCoinChart.draw(window); break; // Binance Coin
            case 3: cardanoChart.draw(window); break; // Cardano
            case 4: solanaChart.draw(window); break; // Solana
            }

            sf::Text moneyInputDisplay(moneyInput, font, 20);
            moneyInputDisplay.setFillColor(sf::Color::Black);
            moneyInputDisplay.setPosition(25, 215);
            window.draw(moneyInputDisplay);

            sf::Text leverageInputDisplay(leverageInput, font, 20);
            leverageInputDisplay.setFillColor(sf::Color::Black);
            leverageInputDisplay.setPosition(25, 285);
            window.draw(leverageInputDisplay);

            for (size_t i = 0; i < openPositions.size(); ++i) {
                sf::RectangleShape closeButton(sf::Vector2f(60, 30));
                closeButton.setFillColor(sf::Color(200, 0, 0));
                closeButton.setPosition(20 + i * 160, 800);
                window.draw(closeButton);

                sf::Text closeText("Close", font, 20);
                closeText.setFillColor(sf::Color::White);
                closeText.setPosition(25 + i * 160, 805);
                window.draw(closeText);
            }
        }

        window.display();
    }

    return 0;
}