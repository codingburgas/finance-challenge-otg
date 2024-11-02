#include "LoginPage.h"
#include <iostream>
#include <fstream>

using namespace std;

LoginUI::LoginUI(sf::RenderWindow& window)
    : window(window), usernameFocus(false), passwordFocus(false), registerMode(false) {
    font.loadFromFile("Arial.ttf");

    // Username field setup
    usernameField.setSize(sf::Vector2f(300, 50));
    usernameField.setPosition(550, 300);
    usernameField.setFillColor(sf::Color::White);
    usernameText.setFont(font);
    usernameText.setCharacterSize(24);
    usernameText.setFillColor(sf::Color::Black);
    usernameText.setPosition(555, 305);

    // Password field setup
    passwordField.setSize(sf::Vector2f(300, 50));
    passwordField.setPosition(550, 400);
    passwordField.setFillColor(sf::Color::White);
    passwordText.setFont(font);
    passwordText.setCharacterSize(24);
    passwordText.setFillColor(sf::Color::Black);
    passwordText.setPosition(555, 405);

    // Login button setup
    loginButton.setSize(sf::Vector2f(100, 50));
    loginButton.setPosition(550, 500);
    loginButton.setFillColor(sf::Color(70, 130, 180));
    loginButtonText.setFont(font);
    loginButtonText.setString("Login");
    loginButtonText.setCharacterSize(24);
    loginButtonText.setFillColor(sf::Color::White);
    loginButtonText.setPosition(565, 510);

    // Register button setup
    registerButton.setSize(sf::Vector2f(100, 50));
    registerButton.setPosition(670, 500);
    registerButton.setFillColor(sf::Color(70, 130, 180));
    registerButtonText.setFont(font);
    registerButtonText.setString("Register");
    registerButtonText.setCharacterSize(24);
    registerButtonText.setFillColor(sf::Color::White);
    registerButtonText.setPosition(680, 510);
}

void LoginUI::handleEvent(sf::Event& event, bool& isLoggedIn, bool& isRegistered) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        usernameFocus = usernameField.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
        passwordFocus = passwordField.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));

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
            }
        }
        else if (passwordFocus) {
            if (event.text.unicode == '\b') {
                if (!passwordInput.empty()) passwordInput.pop_back();
            }
            else if (event.text.unicode < 128) {
                passwordInput += static_cast<char>(event.text.unicode);
            }
        }
    }

    usernameText.setString(usernameInput);
    passwordText.setString(std::string(passwordInput.size(), '*'));
}

void LoginUI::draw() {
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