// TradeWindow.cpp
#include "TradeWindow.h"
#include <iostream>

TradeWindow::TradeWindow(const std::string& stockSymbol)
    : stockSymbol(stockSymbol), balance(1000.0f), position(0.0f), positionOpen(false) {
    font.loadFromFile("arial.ttf"); // Make sure to have an arial font file or use a system font
    balanceText.setFont(font);
    stockSymbolText.setFont(font);
    positionText.setFont(font);
}

void TradeWindow::draw(sf::RenderWindow& window) {
    // Display balance
    balanceText.setString("Balance: $" + std::to_string(balance));
    balanceText.setPosition(10, 10);
    window.draw(balanceText);

    // Display stock symbol
    stockSymbolText.setString("Stock: " + stockSymbol);
    stockSymbolText.setPosition(10, 50);
    window.draw(stockSymbolText);

    // Display position
    if (positionOpen) {
        positionText.setString("Position: $" + std::to_string(position));
        positionText.setPosition(10, 90);
        window.draw(positionText);
    }
}

void TradeWindow::updateBalance(float change) {
    balance += change;
}

void TradeWindow::setPosition(float price) {
    positionOpen = true;
    position = price;
}
