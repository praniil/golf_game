#include<SFML/Graphics.hpp>
#include<iostream>

int main (int argc, char *argv[]) {
    std::cout << "Number of command-line arguments: " << argc << std::endl;

    for (int i = 0; i < argc; i++) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }

    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[1], "GOLF GAME", sf::Style::Fullscreen);

    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
        window.clear();

        window.display();

        sf::sleep(sf::milliseconds(16));
    }
    
    return 0;
}


