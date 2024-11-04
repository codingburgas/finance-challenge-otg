#include "LoginPage.h"
#include <iostream>
#include <fstream>

using namespace std;

LoginUI::LoginUI(sf::RenderWindow& window)
    : window(window), usernameFocus(false), passwordFocus(false), registerMode(false) {

    // Load font
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Error loading font." << std::endl;
    }

    // Load background texture and set it to the sprite
    if (!backgroundTexture.loadFromFile("C:/Users/milko/source/repos/finance-challenge-otg/images/Background.png")) {
        std::cerr << "Error loading background image." << std::endl;
    }
    // Get the size of the window
    sf::Vector2u windowSize = window.getSize();

    // Get the size of the background image
    sf::Vector2u textureSize = backgroundTexture.getSize();

    // Scale the background to fit the window size
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    backgroundSprite.setScale(scaleX, scaleY);
    backgroundSprite.setTexture(backgroundTexture);

    // Username field setup
    usernameField.setSize(sf::Vector2f(300, 50));
    usernameField.setPosition((windowSize.x - 300) / 2, windowSize.y / 2 - 75); // Centered horizontally
    usernameField.setFillColor(sf::Color::White);
    usernameField.setOutlineThickness(3);
    usernameField.setOutlineColor(sf::Color(200, 200, 200, 180));

    usernameText.setFont(font);
    usernameText.setCharacterSize(26);
    usernameText.setFillColor(sf::Color(150, 150, 150)); // Light color for placeholder
    usernameText.setString("Username");  // Placeholder text
    usernameText.setPosition((windowSize.x - 300) / 2 + 5, windowSize.y / 2 - 70);

    // Password field setup
    passwordField.setSize(sf::Vector2f(300, 50));
    passwordField.setPosition((windowSize.x - 300) / 2, windowSize.y / 2); // Centered horizontally
    passwordField.setFillColor(sf::Color::White);
    passwordField.setOutlineThickness(3);
    passwordField.setOutlineColor(sf::Color(200, 200, 200, 180));

    passwordText.setFont(font);
    passwordText.setCharacterSize(26);
    passwordText.setFillColor(sf::Color(150, 150, 150)); // Light color for placeholder
    passwordText.setString("Password");  // Placeholder text
    passwordText.setPosition((windowSize.x - 300) / 2 + 5, windowSize.y / 2 + 5);

    // Login button setup
    loginButton.setSize(sf::Vector2f(150, 50));
    loginButton.setPosition((windowSize.x - 100) / 2 - 110, windowSize.y / 2 + 100);
    loginButton.setFillColor(sf::Color(70, 130, 180));
    loginButtonText.setFont(font);
    loginButtonText.setString("Login");
    loginButtonText.setCharacterSize(24);
    loginButtonText.setFillColor(sf::Color::White);
    loginButtonText.setPosition((windowSize.x - 100) / 2 - 105, windowSize.y / 2 + 110);

    // Register button setup
    registerButton.setSize(sf::Vector2f(150, 50));
    registerButton.setPosition((windowSize.x - 100) / 2 +110, windowSize.y / 2 + 100);
    registerButton.setFillColor(sf::Color(70, 130, 180));
    registerButtonText.setFont(font);
    registerButtonText.setString("Register");
    registerButtonText.setCharacterSize(24);
    registerButtonText.setFillColor(sf::Color::White);
    registerButtonText.setPosition((windowSize.x - 100) / 2 + 115, windowSize.y / 2 + 110);
}

void LoginUI::handleEvent(sf::Event& event, bool& isLoggedIn, bool& isRegistered) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Username focus handling
        usernameFocus = usernameField.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
        if (usernameFocus && usernameInput.empty()) {
            usernameText.setString(""); // Clear placeholder text
        }
        else if (!usernameFocus && usernameInput.empty()) {
            usernameText.setString("Username");
            usernameText.setFillColor(sf::Color(150, 150, 150)); // Reset to light color
        }

        // Password focus handling
        passwordFocus = passwordField.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
        if (passwordFocus && passwordInput.empty()) {
            passwordText.setString(""); // Clear placeholder text
        }
        else if (!passwordFocus && passwordInput.empty()) {
            passwordText.setString("Password");
            passwordText.setFillColor(sf::Color(150, 150, 150)); // Reset to light color
        }

        // Handle Login and Register button clicks...
        if (loginButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            if (!usernameInput.empty() && !passwordInput.empty()) {
                LOGINDETAILS loginUserInfo{ usernameInput, passwordInput };
                isLoggedIn = login(loginUserInfo);
            }
        }
        else if (registerButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            if (registerMode && !usernameInput.empty() && !passwordInput.empty()) {
                USER newUser{ usernameInput, passwordInput };
                isRegistered = registerUser(newUser);
            }
            registerMode = !registerMode;
            registerButtonText.setString(registerMode ? "Back" : "Register");
            loginButtonText.setString(registerMode ? "Submit" : "Login");
        }
    }

    if (event.type == sf::Event::TextEntered) {
        if (usernameFocus) {
            if (event.text.unicode == '\b') {
                if (!usernameInput.empty()) usernameInput.pop_back();
            }
            else if (event.text.unicode < 128) {
                usernameInput += static_cast<char>(event.text.unicode);
                usernameText.setFillColor(sf::Color::Black); // Change color to active
            }
            usernameText.setString(usernameInput.empty() ? "Username" : usernameInput);
        }
        else if (passwordFocus) {
            if (event.text.unicode == '\b') {
                if (!passwordInput.empty()) passwordInput.pop_back();
            }
            else if (event.text.unicode < 128) {
                passwordInput += static_cast<char>(event.text.unicode);
                passwordText.setFillColor(sf::Color::Black); // Change color to active
            }
            passwordText.setString(passwordInput.empty() ? "Password" : std::string(passwordInput.size(), '*'));
        }
    }
}

void LoginUI::draw() {
    window.draw(backgroundSprite);         // Draw background
    window.draw(usernameField);
    window.draw(passwordField);
    window.draw(usernameText);
    window.draw(passwordText);
    window.draw(loginButton);
    window.draw(loginButtonText);
    window.draw(registerButton);
    window.draw(registerButtonText);
}

bool LoginUI::login(const LOGINDETAILS& loginUserInfo) {
    ifstream accountsInfo("AccountInformation.txt");
    if (!accountsInfo.is_open()) {
        cerr << "Error: Unable to open file for login." << endl;
        return false;
    }

    string username, password;
    while (getline(accountsInfo, username) && getline(accountsInfo, password)) {
        if (username == loginUserInfo.loginUername && password == loginUserInfo.loginPassword) {
            return true;
        }
    }
    accountsInfo.close();
    return false;
}

bool LoginUI::registerUser(const USER& user) {
    ofstream accountsInfo("AccountInformation.txt", ios::app);
    if (!accountsInfo.is_open()) {
        cerr << "Error: Unable to open file for registration." << endl;
        return false;
    }

    accountsInfo << user.username << endl << user.password << endl;
    accountsInfo.close();
    return true;
}