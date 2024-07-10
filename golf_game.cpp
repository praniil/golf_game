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

    //sand texture
    sand_image.loadFromFile("sand_texture.jpg");
    sand_texture.loadFromImage(sand_image);
    sand_sprite.setTexture(sand_texture);
    sand_texture_size = sand_texture.getSize();
    sand_desired_height = 150.0f;
    sand_desired_width = 250.0f;
    sand_scale_x = sand_desired_width / sand_texture_size.x;
    sand_scale_y = sand_desired_height / sand_texture_size.y;
    sand_sprite.setScale(sand_scale_x, sand_scale_y);
    sand_damping = 0.8f;

    //water texture
    water_image.loadFromFile("water_texture.png");
    water_texture.loadFromImage(water_image);
    water_sprite.setTexture(water_texture);
    water_texture_size = water_texture.getSize();
    water_desired_height = 150.0f;
    water_desired_width = 250.0f;
    water_scale_x = water_desired_width / water_texture.getSize().x;
    water_scale_y = water_desired_height / water_texture.getSize().y;
    water_sprite.setScale(water_scale_x, water_scale_y);

    //tree texture
    wind_image.loadFromFile("wind.jpg");
    wind_texture.loadFromImage(wind_image);
    wind_sprite.setTexture(wind_texture);
    wind_texture_size = wind_texture.getSize();
    wind_desired_height = 75.0f;
    wind_desired_width = 125.0f;
    wind_scale_x = wind_desired_width / wind_texture.getSize().x;
    wind_scale_y = wind_desired_height / wind_texture.getSize().y;
    wind_sprite.setScale(wind_scale_x, wind_scale_y);

    //shot count text
    if (!shot_count_font.loadFromFile("/home/pranil/cppProjects/games_dev/golf_game/Arial.ttf")) {
        std::cout << "GG" << std::endl;
    }
    shot_count_text.setFont(shot_count_font);
    shot_count_text.setString("shot count: " + std::to_string(shot_count));
    shot_count_text.setCharacterSize(24);
    shot_count_text.setFillColor(sf::Color::Black);
    shot_count_text.setPosition(0, 0);

    //read least shout count file
    std::ifstream least_count_file("least_shot_count.txt");
    if (least_count_file.is_open()) {
        least_count_file >> least_shots_count;
    }

    least_shot_count_text.setFont(shot_count_font);
    least_shot_count_text.setString("least shot count: " + std::to_string(least_shots_count));
    least_shot_count_text.setCharacterSize(24);
    least_shot_count_text.setFillColor(sf::Color::Black);
    least_shot_count_text.setPosition(500, 0);

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
        golf_ball.setFillColor(sf::Color::White);
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
                                // std::cout << "golf ball vel x: " << golf_ball_velocity.x << std::endl;                     
                                isDragging = false;
                                shot_count = shot_count + 1;
                                update_score();
                                std::cout << shot_count << std::endl;
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
    percentage = (abs(drag_vector.x) / 100.0f) + (abs(drag_vector.y) / 100.0f);
    if (percentage > 1.0f) {
        percentage = 1.0f;
    }
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
            update_least_shot_count();
            std::ofstream least_count_file("least_shot_count.txt");

            if(least_count_file.is_open()) {
                least_count_file << least_shots_count;
            }
        }
        game_window.close();
    }
}

void Game:: update_least_shot_count() {
    least_shot_count_text.setString("shot count: " + std::to_string(least_shots_count));
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

void Game::sand_collision() {
    if(sand_sprite.getGlobalBounds().intersects(golf_ball.getGlobalBounds())) {
       golf_ball_velocity.x = golf_ball_velocity.x * sand_damping;
       golf_ball_velocity.y = golf_ball_velocity.y * sand_damping;
    }
}

void Game::water_collision() {
    if(water_sprite.getGlobalBounds().intersects(golf_ball.getGlobalBounds())) {
        golf_ball_velocity.x = golf_ball_velocity.x * 0.8;
        golf_ball_velocity.y = golf_ball_velocity.y * 0.8;
        golf_ball.setFillColor(sf::Color(0, 36, 86));
        if (abs(golf_ball_velocity.x) <= 0.01f && abs(golf_ball_velocity.y) <= 0.01f) {
            golf_ball_velocity.x = 0.0f;
            golf_ball_velocity.y = 0.0f;
            float dis_right = water_sprite.getPosition().x + water_sprite.getGlobalBounds().width - golf_ball.getPosition().x;
            float dis_left = golf_ball.getPosition().x - water_sprite.getPosition().x;
            float dis_top = golf_ball.getPosition().y - water_sprite.getPosition().y;
            float dis_bottom = water_sprite.getPosition().y + water_sprite.getGlobalBounds().height - golf_ball.getPosition().y;
            if(dis_right < dis_left) {
                golf_ball.setFillColor(sf::Color::White);
                if(dis_right < dis_bottom && dis_right < dis_top) {
                    golf_ball.setPosition(golf_ball.getPosition().x + dis_right + 3 * golf_ball.getRadius(), golf_ball.getPosition().y);
                } else if(dis_bottom > dis_top) {
                    golf_ball.setPosition(golf_ball.getPosition().x , golf_ball.getPosition().y - dis_top - 3 * golf_ball.getRadius());
                } else {
                    golf_ball.setPosition(golf_ball.getPosition().x , golf_ball.getPosition().y + dis_bottom + 3 *  golf_ball.getRadius());
                }
            } else {
                golf_ball.setFillColor(sf::Color::White);
                if(dis_left < dis_bottom && dis_left < dis_top) {
                    golf_ball.setPosition(golf_ball.getPosition().x - dis_left - 3 * golf_ball.getRadius(), golf_ball.getPosition().y);
                } else if(dis_bottom > dis_top) {
                    golf_ball.setPosition(golf_ball.getPosition().x , golf_ball.getPosition().y - dis_top - 3 * golf_ball.getRadius());
                } else {
                    golf_ball.setPosition(golf_ball.getPosition().x , golf_ball.getPosition().y + dis_bottom + 3 * golf_ball.getRadius());
                }
            }
            //shot count increases by 1 as a penalty
            shot_count = shot_count + 1;
            update_score();
            // std::cout << shot_count << std::endl;
        }    
    }
}

void Game::update_score() {
    shot_count_text.setString("shot count: " + std::to_string(shot_count));
}

void Game::wind_collision() {
    if(wind_sprite.getGlobalBounds().intersects(golf_ball.getGlobalBounds())) {
       int random_number;
       random_number = rand() % 8;
    //    std::cout << "random number" << random_number << std::endl; 
       switch (random_number)
       {
        case 0:
            golf_ball_velocity.x = golf_ball_velocity.x + 2.8f; 
        break;

        case 1:
            golf_ball_velocity.x = golf_ball_velocity.x - 2.8f;
        break;

        case 2: 
            golf_ball_velocity.y = golf_ball_velocity.y + 2.8f;
        break;

        case 3:
            golf_ball_velocity.y = golf_ball_velocity.y - 2.8f;
        break;

        case 4:
            golf_ball_velocity.x = golf_ball_velocity.x + 4.8f; 
        break;

        case 5:
            golf_ball_velocity.x = golf_ball_velocity.x - 4.8f;
        break;

        case 6: 
            golf_ball_velocity.y = golf_ball_velocity.y + 4.8f;
        break;

        case 7:
            golf_ball_velocity.y = golf_ball_velocity.y - 4.8f;
        break;
        
        default:
            std::cout << "wrong data" << std::endl;
        break;
       }
    }
}

void Game::render() {
    game_window.clear(sf::Color::Green);
    game_window.draw(golf_hole);
    
    //sand texture 1
    sand_sprite.setPosition(655.0f, 455.0f);
    sand_collision();
    game_window.draw(sand_sprite);
    //sand texture 2
    sand_sprite.setPosition(780.0f, 15.0f);
    sand_collision();
    game_window.draw(sand_sprite);

    //sand texture 3
    sand_sprite.setPosition(1180.0f, 200.0f);
    sand_collision();
    game_window.draw(sand_sprite);

    //water texture 1
    water_sprite.setPosition(100.0f, 75.0f);
    water_collision();
    game_window.draw(water_sprite);

    //water texture 2
    water_sprite.setPosition(300.0f, 700.0f);
    water_collision();
    game_window.draw(water_sprite);

    //water texture 3
    water_sprite.setPosition(870.0f, 700.0f);
    water_collision();
    game_window.draw(water_sprite);

    //wind_texture 1
    wind_sprite.setPosition(450.0f, 300.0f);
    wind_collision();
    game_window.draw(wind_sprite);

    //wind_texture 2
    wind_sprite.setPosition(850.0f, 350.0f);
    wind_collision();
    game_window.draw(wind_sprite);

    //wind_texture 3
    wind_sprite.setPosition(400.0f, 50.0f);
    wind_collision();
    game_window.draw(wind_sprite);

    //golf ball
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
    game_window.draw(shot_count_text);
    game_window.draw(least_shot_count_text);

    if(isDragging) {
        game_window.draw(power_meter);
        game_window.draw(power_meter_foreground);
        game_window.draw(arrow);
    }
    game_window.display();
}