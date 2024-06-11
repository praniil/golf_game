#pragma once
// #include <string>
#include<SFML/Graphics.hpp>
class Game{
    public:
        Game();
        bool isRunning() const;
        void run();
        void render();
        void update();
        void handle_collision();
        void handle_wall_collision();
        float distance_calculator(sf::Vector2f &p1, sf::Vector2f &p2);
        sf::Vector2f normalize(sf::Vector2f &source);
        void update_arrow();
        void update_power_meter(sf::Vector2f drag_vector);
    private:
        //window
        sf::RenderWindow game_window;

        //golf ball attributes
        sf::CircleShape golf_ball;
        int golf_ball_pos_x, golf_ball_pos_y;

        //golf hole attribute
        sf::CircleShape golf_hole;
        int golf_hole_pos_x, golf_hole_pos_y;

        //arrow
        sf::RectangleShape arrow;
        float arrow_width, arrow_height;

        //dragging
        bool isDragging;
        sf::Vector2f dragStartPos;
        sf::Vector2f dragEndPos;
        sf::Vector2f golf_ball_velocity;

        bool show_arrow;
        bool inHole;

        //shot count
        int shot_count;

        //least shots count
        int least_shots_count;

        //power meter
        sf::RectangleShape power_meter;
        sf::RectangleShape power_meter_foreground;
        float power_meter_width, power_meter_height;
};

class Rectangle{
    public:
        sf::Vector2f size;
        Rectangle(sf::Vector2f size) {
            this -> size = size;
        }

        virtual sf::RectangleShape set_size() {
            sf::RectangleShape rectange;
            rectange.setSize(size);
            return rectange;
        }

        void manage_collision_effect(sf::RectangleShape &rectangle, sf::CircleShape &ball, sf::Vector2f &velocity);
};