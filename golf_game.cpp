#include<SFML/Graphics.hpp>
#include "golf_game.h"

#include<iostream>

//golbal variable
int golf_ball_diameter = 10;

Game :: Game() {
    //game window config
    sf::VideoMode desktop_size = sf::VideoMode::getDesktopMode();
    game_window.create(desktop_size, "GOLF_GAME");
    //golf ball attribute
    golf_ball.setRadius(golf_ball_diameter);
    golf_ball.setFillColor(sf::Color::White);
    golf_ball.setPointCount(100000);

    //golf ball position
    golf_ball_pos_x = 5 * golf_ball_diameter;
    golf_ball_pos_y = game_window.getSize().y - 5 * golf_ball_diameter;
    golf_ball.setPosition(golf_ball_pos_x, golf_ball_pos_y);

    //golf hole attribute
    golf_hole.setRadius(golf_ball_diameter * 1.52);
    golf_hole.setFillColor(sf::Color::Black);
    golf_hole.setPointCount(100000);


    //golf hole position
    golf_hole_pos_x = game_window.getSize().x /2 + golf_ball_diameter;
    golf_ball_pos_y = 3 * golf_ball_diameter;
    golf_hole.setPosition(golf_hole_pos_x, golf_ball_pos_y);
}

bool Game::isRunning() const{
    return game_window.isOpen();
}

void Game::run() {
    while(game_window.isOpen()) {
        sf::Event event;
        while(game_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                game_window.close();
            }

            if(event.type == sf::Event::KeyPressed ) {
                if(event.key.code == sf::Keyboard::Escape) {
                    game_window.close();
                    std::cout<<"in event" << std::endl;
                }
            }
        } 
        render();
    }
}

void Game::render() {
    game_window.clear(sf::Color::Green);
    game_window.draw(golf_ball);
    game_window.draw(golf_hole);
    game_window.display();
}

