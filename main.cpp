bool debug = true;

//? I|O Operations
#include <iostream>

//? SFML
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

//* Window Variables
sf::RenderWindow windowSettings;
sf::RenderWindow windowKeyboard;
sf::RenderWindow windowMouse;

sf::RenderTexture rTextureSettings({280, 395});
sf::RenderTexture rTextureKeyboard({778, 395});
sf::RenderTexture rTextureMouse({325, 432});

enum class DRAG {
    NONE,
    wSettings,
    wKeyboard,
    wMouse
};

DRAG dragging = DRAG::NONE;
sf::Vector2i dragOffset;

//? File Management
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
std::string path = fs::current_path().string() + "/";

#include <nlohmann/json.hpp>
using json = nlohmann::json;
json highlightConfig;

//? Datatypes
#include <unordered_map>
//* Storrage for all textures
std::unordered_map<std::string, sf::Texture> textureMap;


#include <vector>
struct Highlight {
    private:
        sf::RenderTexture *h_window;

        std::string     h_TextureName;
        sf::Vector2f    h_position;
        int             h_lifeTime;
    
    public:
        Highlight(sf::RenderTexture *window, std::string textureName, std::vector<float> position, int lifeTime) {
            h_window        = window;
            h_TextureName   = textureName;
            h_position      = {position[0], position[1]};
            h_lifeTime      = lifeTime;
        }

        ~Highlight() {
            h_window = nullptr;
        }

        void draw(sf::Sprite &sprite) {
            if (h_window != nullptr) {
                h_window->draw(sprite);
            }
        }

        std::string getTextureName() {
            return h_TextureName;
        }

        sf::Vector2f getPosition() {
            return h_position;
        }

        int getLifeTime() {
            h_lifeTime -= 1;
            return h_lifeTime+1;
        }
};

std::vector<Highlight> highlightVectorKeyboard;
std::vector<Highlight> highlightVectorMouse;

#include "sfmlKeyMaps.hpp"


json loadJson() {
    std::ifstream inputFile(path + "data/config/highlightConfig.json");
    if (!inputFile.is_open())
    {
        std::cout << "Failed to open " + path + "data/config/highlightConfig.json\n";
        exit(1);
    }

    json j;
    inputFile >> j;
    return j;
}

void renderHighlight(Highlight highlight) {
    if (textureMap.find(highlight.getTextureName()) != textureMap.end()) {
        sf::Sprite tempSprite = sf::Sprite(textureMap[highlight.getTextureName()]);
        tempSprite.setPosition(highlight.getPosition());
        highlight.draw(tempSprite);
    } else {
        std::cout << "Missing Texture: " << highlight.getTextureName() << "\n";
    }
}

/*
* Loads All Textures into the textureMap.
*/
int loadTextures() {
    std::string textureFolderPath = path + "data/textures/";
    

    for (const auto& dir : fs::recursive_directory_iterator(textureFolderPath)) {
        if (dir.is_regular_file() && dir.path().extension() == ".png") {
            std::string tempPath = dir.path().string();
            std::string tempName = dir.path().stem().string();

            if (debug) {
                std::cout << "File: " << tempName << " in " << tempPath << "\n";
            }

            sf::Texture tempTexture;
            if (!tempTexture.loadFromFile(tempPath)) {
                std::cout << "Failed to load texture!\n";
                continue;
            }

            textureMap[tempName] = tempTexture;
            std::cout << "Loaded File : " << tempPath << " in " << tempName << "\n"; 
        }
    }
    return 0;
}

void handleDragging(sf::RenderWindow &windowRef, DRAG window, std::optional<sf::Event> event) {
    if (const sf::Event::MouseButtonPressed* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Mouse::Button btn = mouseButton->button;
        if (btn == sf::Mouse::Button::Left) {
            std::cout << "Started Dragging\n";
            dragging = window;
            dragOffset = sf::Mouse::getPosition() - windowRef.getPosition();
        }
    }
    else if (const sf::Event::MouseButtonReleased* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
        sf::Mouse::Button btn = mouseButton->button;
        if (btn == sf::Mouse::Button::Left) {
            std::cout << "Stopped Dragging\n";
            dragging = DRAG::NONE;
        }
    }
}

void renderRenderTexturesAndWindow() {
    sf::Texture tempText = textureMap.at("empty");
    sf::Sprite tempSprite(tempText);

    if (windowKeyboard.isOpen()) {
        //* Mouse Content
        tempText = textureMap.at("backgroundM");
        tempSprite.setTexture(tempText, true);
        rTextureMouse.draw(tempSprite);

        //* Rendering for mouse button highlight
        for (auto it = highlightVectorMouse.begin(); it != highlightVectorMouse.end(); ) {
            if (it->getLifeTime() > 0) {
                renderHighlight(*it);
                ++it;
            } else {
                it = highlightVectorMouse.erase(it);
            }
        }

        rTextureMouse.display();
        tempText = rTextureMouse.getTexture();
        tempSprite.setTexture(tempText, true);

        tempSprite.setScale(
            {static_cast<float>(windowMouse.getSize().x) / tempText.getSize().x,
            static_cast<float>(windowMouse.getSize().y) / tempText.getSize().y}
        );
        
        windowMouse.clear();
        windowMouse.draw(tempSprite);
        windowMouse.display();
    }
    
    if (windowKeyboard.isOpen()) {
        //* Keyboard Content
        tempText = textureMap.at("backgroundK");
        tempSprite.setTexture(tempText, true);
        rTextureKeyboard.draw(tempSprite);

        //* Rendering keyboard key Highlight
        for (auto it = highlightVectorKeyboard.begin(); it != highlightVectorKeyboard.end(); ) {
            if (it->getLifeTime() > 0) {
                renderHighlight(*it);
                ++it;
            } else {
                it = highlightVectorKeyboard.erase(it);
            }
        }

        rTextureKeyboard.display();
        tempText = rTextureKeyboard.getTexture();
        tempSprite.setTexture(tempText, true);

        tempSprite.setScale(
            {static_cast<float>(windowKeyboard.getSize().x) / tempText.getSize().x,
            static_cast<float>(windowKeyboard.getSize().y) / tempText.getSize().y}
        );

        windowKeyboard.clear();
        windowKeyboard.draw(tempSprite);
        windowKeyboard.display();
    }

    //* Settings Content
    rTextureSettings.display();
    tempText = rTextureSettings.getTexture();
    tempSprite.setTexture(tempText, true);

    tempSprite.setScale(
        {static_cast<float>(windowSettings.getSize().x) / tempText.getSize().x,
        static_cast<float>(windowSettings.getSize().y) / tempText.getSize().y}
    );

    windowSettings.clear();
    windowSettings.draw(tempSprite);
    windowSettings.display();
}

void checkKeys() {
    //* Keyboard:
    if (windowKeyboard.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            Highlight h{
                &rTextureKeyboard,
                highlightConfig["keyboard"]["keyMap"]["A"]["texture"].get<std::string>(),
                highlightConfig["keyboard"]["keyMap"]["A"]["position"].get<std::vector<float>>(),
                highlightConfig["DelayFrames"].get<int>()
            };
            highlightVectorKeyboard.push_back(h);
        }
    }

    //* Mouse:
    if (windowMouse.isOpen()) {
        for (std::string btnName : buttonNames) {
            if (sf::Mouse::isButtonPressed(buttonMap.at(btnName))) {
                Highlight h{
                    &rTextureMouse,
                    highlightConfig["mouse"]["keyMap"][btnName]["texture"].get<std::string>(),
                    highlightConfig["mouse"]["keyMap"][btnName]["position"].get<std::vector<float>>(),
                    highlightConfig["DelayFrames"].get<int>()
                };
                highlightVectorMouse.push_back(h);
            }
        }
    }
}

int main (int argc, char* argv[]) {
    loadTextures();

    highlightConfig = loadJson();

    //* Window Confings
    sf::VideoMode videoModeSettings(
        {highlightConfig["settings"]["windowSize"][0].get<unsigned int>(),
        highlightConfig["settings"]["windowSize"][1].get<unsigned int>()}
    );

    sf::VideoMode videoModeKeyboard(
        {highlightConfig["keyboard"]["windowSize"][0].get<unsigned int>(),
        highlightConfig["keyboard"]["windowSize"][1].get<unsigned int>()}
    );
    
    sf::VideoMode videoModeMouse(
        {highlightConfig["mouse"]["windowSize"][0].get<unsigned int>(),
        highlightConfig["mouse"]["windowSize"][1].get<unsigned int>()}
    );

    //* Settings Window
    windowSettings.create(videoModeSettings, "Key Visualizer ++", sf::Style::Default, sf::State::Windowed);
    windowSettings.setPosition({821, 138});

    //* Keyboard Window
    windowKeyboard.create(videoModeKeyboard, "Keyboard", sf::Style::None, sf::State::Windowed);
    windowKeyboard.setPosition({572, 594});

    //* Mouse Window Top
    windowMouse.create(videoModeMouse, "Mouse", sf::Style::None, sf::State::Windowed);
    windowMouse.setPosition({1465, 138});



    windowKeyboard.setVisible(highlightConfig["keyboard"]["show"].get<bool>());
    windowMouse.setVisible(highlightConfig["mouse"]["show"].get<bool>());

    rTextureMouse.setSmooth(true);
    rTextureKeyboard.setSmooth(true);
    rTextureSettings.setSmooth(true);

    bool running = true;

    while (running) {

        //* Handle Settings Window Events (Other Windows are just Render!)
        while (const std::optional event = windowSettings.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>()) {
                running = false;
                windowSettings.close();
            } else if (const sf::Event::KeyPressed* eventKey = event->getIf<sf::Event::KeyPressed>()) {
                
            } else if (const sf::Event::MouseButtonPressed* eventKey = event->getIf<sf::Event::MouseButtonPressed>()) {
                
            }
            handleDragging(windowSettings, DRAG::wSettings, event);
        }

        //* Clear Events For The Other Windows and enable Dragging
        while (const std::optional event = windowKeyboard.pollEvent() ) {
            handleDragging(windowKeyboard, DRAG::wKeyboard, event);
        }
        while (const std::optional event = windowMouse.pollEvent() ) {
            handleDragging(windowMouse, DRAG::wMouse, event);
        }

        checkKeys();

        rTextureMouse.clear(sf::Color(80, 80, 80, 255));
        rTextureSettings.clear(sf::Color(80, 80, 80, 255));
        rTextureKeyboard.clear(sf::Color(80, 80, 80, 255));

        //* Rendering für die Settings
        if (windowSettings.isOpen()) {

        }

        renderRenderTexturesAndWindow();

        //* Updating Position of the dragged window Based on Mouse Position
        if (dragging != DRAG::NONE) {
            switch (dragging) {
                case DRAG::wSettings : {
                    windowSettings.setPosition(sf::Mouse::getPosition() - dragOffset);
                    break;
                }
                case DRAG::wKeyboard : {
                    windowKeyboard.setPosition(sf::Mouse::getPosition() - dragOffset);
                    break;
                }
                case DRAG::wMouse : {
                    windowMouse.setPosition(sf::Mouse::getPosition() - dragOffset);
                    break;
                }
            }
        }
    }

    return 0;
}