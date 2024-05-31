#include<SFML/Graphics.hpp>
#include "golf_game.h"
#include<stdio.h>
#include<iostream>
#include<cmath>

//golbal variable
int golf_ball_diameter = 10;
const float dragscale = 0.2f;
const float damping = 0.99f;
int drag_angle;

Game :: Game() {
    //game window config
    sf::VideoMode desktop_size = sf::VideoMode::getDesktopMode();
    game_window.create(desktop_size, "GOLF_GAME", sf::Style::Fullscreen);
    game_window.setFramerateLimit(100);
    //golf ball attribute
    golf_ball.setRadius(golf_ball_diameter);
    golf_ball.setFillColor(sf::Color::White);
    golf_ball.setPointCount(30);

    //golf ball position
    golf_ball_pos_x = 5 * golf_ball_diameter;
    golf_ball_pos_y = game_window.getSize().y - 5 * golf_ball_diameter;
    golf_ball.setPosition(golf_ball_pos_x, golf_ball_pos_y);

    //golf hole attribute
    golf_hole.setRadius(golf_ball_diameter * 1.52);
    golf_hole.setFillColor(sf::Color::Black);
    golf_hole.setPointCount(30);

    //golf hole position
    golf_hole_pos_x = 20 * golf_ball_diameter;
    golf_ball_pos_y = game_window.getSize().y - 10 * golf_ball_diameter;
    golf_hole.setPosition(golf_hole_pos_x, golf_ball_pos_y);

    //arrow attribute
    arrow_width = 2.0;
    arrow_height = 20.0;
    arrow.setSize(sf::Vector2f(arrow_width, arrow_height));
    show_arrow = false;

    arrow.setFillColor(sf::Color::Black);
    // arrow.setPosition(golf_ball.getPosition().x + golf_ball_diameter / 2 + 3, golf_ball.getPosition().y - golf_ball_diameter - 12);
    //dragging attribute
    isDragging = false;

}

bool Game::isRunning() const{
    return game_window.isOpen();
}

void Game::run() {
    while(game_window.isOpen()) {
        if (show_arrow == true) {
            arrow.setFillColor(sf::Color::Black);
            arrow.setPosition(golf_ball.getPosition().x + golf_ball_diameter / 2 + 3, golf_ball.getPosition().y - golf_ball_diameter - 12);
        } else {
            arrow.setFillColor(sf::Color::Green);
        }

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
                
                //for getting the start position of ball
                if(event.type == sf::Event::MouseButtonPressed) {
                    show_arrow = true;
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                        printf("mouse pos x: %f", mousePos.x);
                        if (golf_ball.getGlobalBounds().contains(mousePos)) {
                            isDragging = true;
                            dragStartPos = mousePos;
                            printf("drag posn x: %f", dragStartPos.x);
                        }
                    }
                }
                
                //for getting the end position of the ball after drag
                if (event.type == sf::Event::MouseButtonReleased) {
                    show_arrow = false;
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (isDragging) {
                            dragEndPos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                            printf("drag end pos: %f", dragEndPos.x); 
                            sf::Vector2f distance_drag = dragEndPos - dragStartPos;
                            // float base = golf_ball.getPosition().x - distance_drag.x;
                            // drag_angle = acos(base / sqrt(pow(golf_ball.getPosition().x - distance_drag.x, 2) + pow(golf_ball.getPosition().y - distance_drag.y, 2)));
                            drag_angle = (atan2(distance_drag.y, distance_drag.x) * 180.f / 3.14f) - 90.f;
                            std::cout << "angle: " << drag_angle << std::endl;
                            golf_ball_velocity = -distance_drag * dragscale;
                            arrow.rotate(drag_angle);
                            isDragging = false;
                        }
                    }
                }
        } 
            //updating the position of the ball
            update();
            render();
    }
}

void Game::update() {
    if(!isDragging) {
        printf("1");
        golf_ball.move(golf_ball_velocity);
        printf("2");
        golf_ball_velocity *= damping;
        show_arrow = false;
        handle_collision();
    }
}

void Game::handle_collision () {
    if (golf_ball.getGlobalBounds().intersects(golf_hole.getGlobalBounds())) {
        sf::Vector2f direction = golf_hole.getPosition() - golf_ball.getPosition();
        float length = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
        direction /= length;
        golf_ball_velocity = direction * 2.0f;
        golf_ball.setPosition(golf_hole.getPosition());
        golf_ball.setRadius(0.04 * golf_ball.getRadius());
    }
}

void Game::render() {
    printf("3");
    game_window.clear(sf::Color::Green);
    game_window.draw(arrow);
    game_window.draw(golf_ball);
    game_window.draw(golf_hole);
    game_window.display();
}

