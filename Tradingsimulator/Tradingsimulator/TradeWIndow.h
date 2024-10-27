#pragma once
// TradeWindow.h
#ifndef TRADEWINDOW_H
#define TRADEWINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

class TradeWindow {
public:
    TradeWindow(const std::string& stockSymbol);
    void draw(sf::RenderWindow& window);
    void updateBalance(float change);
    void setPosition(float price);

private:
    float balance;
    std::string stockSymbol;
    float position;
    sf::Text balanceText;
    sf::Text stockSymbolText;
    sf::Text positionText;
    sf::Font font;
    bool positionOpen;
};

#endif // TRADEWINDOW_H