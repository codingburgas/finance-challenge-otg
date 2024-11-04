#pragma once
#include <SFML/Graphics.hpp>
#include "UserDetails.h"
#include <string>
#include <fstream>

class LoginUI {
public:
    LoginUI(sf::RenderWindow& window);
    USER user;
    void handleEvent(sf::Event& event, bool& isLoggedIn, bool& isRegistered);
    void draw();

private:
    sf::RenderWindow& window;

    // Text fields, buttons, and text displays
    sf::RectangleShape usernameField;
    sf::RectangleShape passwordField;
    sf::Text usernameText;
    sf::Text passwordText;
    sf::Text loginButtonText;
    sf::Text registerButtonText;
    sf::RectangleShape loginButton;
    sf::RectangleShape registerButton;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Text titleText;

    // Variables to hold input data and state
    std::string usernameInput;
    std::string passwordInput;
    bool usernameFocus;
    bool passwordFocus;
    bool registerMode;

    // Font
    sf::Font font;

    bool login(const LOGINDETAILS& loginUserInfo);
    bool registerUser(const USER& user);
};