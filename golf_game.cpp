#include<SFML/Graphics.hpp>
#include "golf_game.h"
#include<stdio.h>
#include<iostream>
#include<cmath>
#include<fstream>

//golbal variable
const int golf_ball_diameter = 10;
const float dragscale = 0.2f;
const float damping = 0.99f;
int drag_angle;

//small rectange
Rectangle rec_small(sf::Vector2f(35.f, 150.f));
sf::RectangleShape rect_small = rec_small.set_size();

//medium rectange
Rectangle rec_medium(sf::Vector2f(35.f, 200.f));
sf::RectangleShape rect_medium = rec_medium.set_size();

//large rectange
Rectangle rec_large(sf::Vector2f(40.f, 350.f));
sf::RectangleShape rect_large = rec_large.set_size();

Game :: Game() {
    //game window config
    sf::VideoMode desktop_size = sf::VideoMode::getDesktopMode();
    game_window.create(desktop_size, "GOLF_GAME", sf::Style::Fullscreen);
    game_window.setFramerateLimit(60);
    //golf ball attribute
    golf_ball.setRadius(golf_ball_diameter);
    golf_ball.setFillColor(sf::Color::White);
    golf_ball.setPointCount(30);

    //golf ball position
    golf_ball_pos_x = 20 * golf_ball_diameter;
    golf_ball_pos_y = game_window.getSize().y - 20 * golf_ball_diameter;
    golf_ball.setPosition(golf_ball_pos_x, golf_ball_pos_y);

    //golf hole attribute
    golf_hole.setRadius(golf_ball_diameter * 1.52);
    golf_hole.setFillColor(sf::Color::Black);
    golf_hole.setPointCount(30);
    //golf hole position
    golf_hole_pos_x = game_window.getSize().x - 10 * golf_ball_diameter;
    golf_hole_pos_y = game_window.getSize().y / 10;
    golf_hole.setPosition(golf_hole_pos_x, golf_hole_pos_y);

    //dragging attribute
    isDragging = false;

    //arrow attributes
    arrow_width = 2.0f;
    arrow_height = 30.0f;
    arrow.setSize(sf::Vector2f(arrow_width, arrow_height));
    arrow.setFillColor(sf::Color::Blue);
    arrow.setOrigin(-0, arrow.getSize().y );
    arrow.setOutlineThickness(0);

    inHole = false;

    shot_count = 0;

    power_meter_width = 100.0f;
    power_meter_height = 22.0f;
    power_meter.setSize(sf::Vector2f(power_meter_width + 6, power_meter_height));
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

void Rectangle::manage_collision_effect(sf::RectangleShape &rectangle, sf::CircleShape &ball, sf::Vector2f &velocity) {
    if(rectangle.getGlobalBounds().intersects(ball.getGlobalBounds())) {
        // Check the side of collision
        sf::FloatRect ballBounds = ball.getGlobalBounds();
        sf::FloatRect rectBounds = rectangle.getGlobalBounds();

        // Determine the overlap amounts on each axis
        float overlapLeft = ballBounds.left + ballBounds.width - rectBounds.left;
        float overlapRight = rectBounds.left + rectBounds.width - ballBounds.left;
        float overlapTop = ballBounds.top + ballBounds.height - rectBounds.top;
        float overlapBottom = rectBounds.top + rectBounds.height - ballBounds.top;

        // Find the minimum overlap
        bool ballFromLeft = (overlapLeft < overlapRight);
        bool ballFromTop = (overlapTop < overlapBottom);

        float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
        float minOverlapY = ballFromTop ? overlapTop : overlapBottom;

        // Resolve collision on the axis with the smallest overlap
        if(minOverlapX < minOverlapY) {
            // Collide with left or right side
            ball.move(-velocity.x, -velocity.y);
            velocity.x *= -1;
        } else {
            // Collide with top or bottom side
            ball.move(-velocity.x, -velocity.y);
            velocity.y *= -1;
        }
    }
}

void Game::run() {
    while(game_window.isOpen()) {
        sf::Event event;
        while(game_window.pollEvent(event)) {
                if(golf_ball_velocity.x >= 0.01f || golf_ball_velocity.y >= 0.01f) {
                    if (event.type == sf::Event::MouseButtonPressed){
                            continue;
                    }
                }
                if(event.type == sf::Event::KeyPressed ) {
                    if(event.key.code == sf::Keyboard::Escape) {
                        game_window.close();
                    }
                }
                
                if(inHole == false) {
                    //for getting the start position of ball
                    
                    if(event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                            if (golf_ball.getGlobalBounds().contains(mousePos)) {
                                isDragging = true;
                                dragStartPos = mousePos;
                            }  
                            dragEndPos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
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
                                    if(distance_drag.x < -100.0f) {
                                    distance_drag.x = -100.0f;
                                } else if(distance_drag.x > 100.0f) {
                                    distance_drag.x = 100.0f;
                                }

                                if (distance_drag.y > 100.0f) {
                                    distance_drag.y = 100.0f;
                                } else if (distance_drag.y < -100.0f) {
                                    distance_drag.y = -100.0f;
                                }
                                golf_ball_velocity = -distance_drag * dragscale;                       
                                isDragging = false;
                                shot_count++;
                            }
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
    sf::Vector2f currentMousePosition = game_window.mapPixelToCoords(sf::Mouse::getPosition(game_window));
    sf::Vector2f dragVector = dragStartPos - currentMousePosition;
    sf::Vector2f normalizedDragVector = normalize(dragVector);
    float angle = std::atan2(normalizedDragVector.y, normalizedDragVector.x) * 180 / M_PI;
    arrow.setRotation(angle + 90);
    if(dragVector.x < -100.0f) {
        dragVector.x = -100.0f;
    } else if(dragVector.x > 100.0f) {
        dragVector.x = 100.0f;
    }
    if (dragVector.y > 100.0f) {
        dragVector.y = 100.0f;
    } else if (dragVector.y < -100.0f) {
        dragVector.y = -100.0f;
    }
    update_power_meter(dragVector);
}

void Game::update_power_meter(sf::Vector2f drag_vector) {
    //if close to both bottom and right wall
    if(golf_ball.getPosition().y + golf_ball.getRadius() * 6 >= game_window.getSize().y && golf_ball.getPosition().x + golf_ball.getRadius() * 6 >= game_window.getSize().x) {
        power_meter_foreground.setPosition(golf_ball.getPosition().x - power_meter_width, golf_ball.getPosition().y + 2.5 + golf_ball.getRadius() * 3 - power_meter_height - golf_ball.getRadius() * 4);
        power_meter.setPosition(golf_ball.getPosition().x - power_meter_width, golf_ball.getPosition().y + golf_ball.getRadius() * 3 - power_meter_height - golf_ball.getRadius() * 4);
    } 
    //if ball is much closer to right wall
    else if(golf_ball.getPosition().x + golf_ball.getRadius() * 8 >= game_window.getSize().x) {
        power_meter.setPosition(golf_ball.getPosition().x - power_meter_width, golf_ball.getPosition().y + golf_ball.getRadius() * 3);
        power_meter_foreground.setPosition(golf_ball.getPosition().x + 3 - power_meter_width, golf_ball.getPosition().y + 2.5 + golf_ball.getRadius() * 3);
    }
    //if ball is much closer to the bottom
    else if (golf_ball.getPosition().y + golf_ball.getRadius() * 6 >= game_window.getSize().y) {
        power_meter_foreground.setPosition(golf_ball.getPosition().x + 3, golf_ball.getPosition().y + 2.5 + golf_ball.getRadius() * 3 - power_meter_height - golf_ball.getRadius() * 4);
        power_meter.setPosition(golf_ball.getPosition().x, golf_ball.getPosition().y + golf_ball.getRadius() * 3 - power_meter_height - golf_ball.getRadius() * 4);
    }
    //all other cases
    else {
        power_meter_foreground.setPosition(golf_ball.getPosition().x + 3, golf_ball.getPosition().y + 2.5 + golf_ball.getRadius() * 3);
        power_meter.setPosition(golf_ball.getPosition().x, golf_ball.getPosition().y + golf_ball.getRadius() * 3);
    }
    power_meter_foreground.setFillColor(sf::Color(235, 14, 55, 150));
    // power_meter.setFillColor(sf::Color::White);
    // power_meter_foreground.setFillColor(sf::Color::Black);
    power_meter.setFillColor(sf::Color(14, 194, 235, 150));

    float percentage;
    percentage = (abs(drag_vector.x) / 200.0f) + (abs(drag_vector.y) / 200.0f);
    power_meter_foreground.setSize(sf::Vector2f((percentage * power_meter_width), power_meter_height - 5));
}

void Game::update() {
    if(!isDragging) {
        golf_ball.move(golf_ball_velocity);
        golf_ball_velocity *= damping;
        handle_wall_collision();
        handle_collision();
    }

    if(inHole == true) {
        std::ifstream least_count_file("least_shot_count.txt");
        if (least_count_file.is_open()) {
            least_count_file >> least_shots_count;
        }
        if (shot_count < least_shots_count) {
            least_shots_count = shot_count;
            std::ofstream least_count_file("least_shot_count.txt");

            if(least_count_file.is_open()) {
                least_count_file << least_shots_count;
            }
        }
        game_window.close();
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
            inHole = true;
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
    game_window.draw(golf_hole);
    game_window.draw(golf_ball);

    // Small obstacles
    rect_small.setPosition(400.0f, 220.0f);
    rect_small.setFillColor(sf::Color::Black);
    rec_small.manage_collision_effect(rect_small, golf_ball, golf_ball_velocity);
    game_window.draw(rect_small);

    rect_small.setPosition(600.0f, 40.0f); // Slightly shifted to create a gap
    rect_small.setFillColor(sf::Color::Black);
    rec_small.manage_collision_effect(rect_small, golf_ball, golf_ball_velocity);
    game_window.draw(rect_small);

    // Medium obstacles
    rect_medium.setPosition(600.0f, 500.0f); // Positioned lower
    rect_medium.setFillColor(sf::Color::Red);
    rec_medium.manage_collision_effect(rect_medium, golf_ball, golf_ball_velocity);
    game_window.draw(rect_medium);

    rect_medium.setPosition(750.0f, 230.0f); // Creating a gap between medium obstacles
    rect_medium.setFillColor(sf::Color::Red);
    rec_medium.manage_collision_effect(rect_medium, golf_ball, golf_ball_velocity);
    game_window.draw(rect_medium);

    // Large obstacles
    rect_large.setPosition(1175.0f, 400.0f); // Positioned centrally but higher
    rect_large.setFillColor(sf::Color::Cyan);
    rec_large.manage_collision_effect(rect_large, golf_ball, golf_ball_velocity);
    game_window.draw(rect_large);

    rect_large.setPosition(1050.0f, 50.0f); // Creating a narrow path
    rect_large.setFillColor(sf::Color::Cyan);
    rec_large.manage_collision_effect(rect_large, golf_ball, golf_ball_velocity);
    game_window.draw(rect_large);

    if(isDragging) {
        game_window.draw(power_meter);
        game_window.draw(power_meter_foreground);
        game_window.draw(arrow);
    }
    game_window.display();
}