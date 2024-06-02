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
    golf_hole_pos_y = game_window.getSize().y / 10;
    golf_hole.setPosition(golf_hole_pos_x, golf_hole_pos_y);

    //dragging attribute
    isDragging = false;

    //arrow attributes
    arrow_width = 2.0f;
    arrow_height = 20.0f;
    arrow.setSize(sf::Vector2f(arrow_width, arrow_height));
    arrow.setFillColor(sf::Color::Blue);
    arrow.setOrigin(-0, arrow.getSize().y );
    arrow.setOutlineThickness(0);
}

float Game::distance_calculator(sf::Vector2f &p1, sf::Vector2f &p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

sf::Vector2f Game::normalize(sf::Vector2f& source) {
    //applying pythagoras theorem
    //calculating the unit vector
    float length = sqrt(pow(source.x, 2) + pow(source.y, 2));
    if (length != 0) {
        return sf::Vector2f(source.x / length, source.y/ length);
    } else {
        return source;
    }
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
                
                //for getting the start position of ball
                if(event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                        if (golf_ball.getGlobalBounds().contains(mousePos)) {
                            isDragging = true;
                            dragStartPos = mousePos;
                        }
                    }
                }

                if (isDragging) {
                    update_arrow();
                }
                
                //for getting the end position of the ball after drag
                if (event.type == sf::Event::MouseButtonReleased) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (isDragging) {
                            dragEndPos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                            sf::Vector2f distance_drag = dragEndPos - dragStartPos;
                            golf_ball_velocity = -distance_drag * dragscale;                         
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

void Game::update_arrow() {
    arrow.setFillColor(sf::Color::Black);
    arrow.setPosition(golf_ball.getPosition().x + golf_ball.getRadius() -1, golf_ball.getPosition().y + golf_ball.getRadius() -1);
    std::cout << "golf ball pos x: " << golf_ball.getPosition().x << std::endl;
    std::cout << "arrow posn x: " << arrow.getPosition().x << std::endl;
    sf::Vector2f currentMousePosition = game_window.mapPixelToCoords(sf::Mouse::getPosition(game_window));
    sf::Vector2f dragVector = dragStartPos - currentMousePosition;
    sf::Vector2f normalizedDragVector = normalize(dragVector);
    float angle = std::atan2(normalizedDragVector.y, normalizedDragVector.x) * 180 / M_PI;
    arrow.setRotation(angle + 90);
    
}

void Game::update() {

    if(!isDragging) {
        golf_ball.move(golf_ball_velocity);
        golf_ball_velocity *= damping;
        handle_wall_collision();
        handle_collision();
    }
}

void Game::handle_collision () {
    sf::Vector2f golf_ball_center = golf_ball.getPosition() + sf::Vector2f(golf_ball.getRadius(), golf_ball.getRadius());
    sf::Vector2f golf_hole_center = golf_hole.getPosition() + sf::Vector2f(golf_hole.getRadius(), golf_hole.getRadius());
    float distance = distance_calculator(golf_ball_center, golf_hole_center);
    if (distance < golf_hole.getRadius() - golf_ball.getRadius() / 4) {
        if(golf_ball_velocity.x < 2.0f) {
            sf::Vector2f direction = golf_hole_center - golf_ball_center;
            direction /= distance;
            golf_ball_velocity = direction * 2.0f;
            golf_ball.setPosition(golf_hole.getPosition() + sf::Vector2f(golf_ball.getRadius() - 3, golf_ball.getRadius() - 3));
            golf_ball.setFillColor(sf::Color::Black);
        } else {
            sf::Vector2f direction = golf_hole_center - golf_ball_center;
            direction /= distance;
            golf_ball_velocity = direction * 2.0f;
        }
    }
}

void Game::handle_wall_collision() {
    if (golf_ball.getPosition().x <= 0 || golf_ball.getPosition().x >= game_window.getSize().x - golf_ball_diameter) {
        golf_ball_velocity.x = -golf_ball_velocity.x;
    } 
    if (golf_ball.getPosition().y <= 0 || golf_ball.getPosition().y >= game_window.getSize().y - golf_ball_diameter) {
        golf_ball_velocity.y = -golf_ball_velocity.y;
    }
}

void Game::render() {
    game_window.clear(sf::Color::Green);
    game_window.draw(golf_ball);
    if(isDragging) {
        std :: cout << "true" << std::endl;
        game_window.draw(arrow);
    }
    game_window.draw(golf_hole);
    game_window.display();
}