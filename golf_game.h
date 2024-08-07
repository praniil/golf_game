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
        void reset();
        void handle_collision();
        void handle_wall_collision();
        float distance_calculator(sf::Vector2f &p1, sf::Vector2f &p2);
        sf::Vector2f normalize(sf::Vector2f &source);
        void update_arrow();
        void update_power_meter(sf::Vector2f drag_vector);
        void sand_collision();
        void water_collision();
        void wind_collision();
        void update_score();
        void update_least_shot_count();
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

        //exit button
        sf::RectangleShape exit_button;
        sf::Text exit_button_text;

        //restart game button
        sf::RectangleShape restart_button;
        sf::Text restart_button_text;

        //dragging
        bool isDragging;
        sf::Vector2f dragStartPos;
        sf::Vector2f dragEndPos;
        sf::Vector2f golf_ball_velocity;

        bool show_arrow;
        bool inHole;
        bool inGameWindow;

        //shot count
        int shot_count;

        //least shots count
        int least_shots_count;

        //power meter
        sf::RectangleShape power_meter;
        sf::RectangleShape power_meter_foreground;
        float power_meter_width, power_meter_height;

        //for sand texture
        sf::Image sand_image;
        sf::Texture sand_texture;
        sf::Sprite sand_sprite;
        sf::Vector2u sand_texture_size;
        float sand_scale_x;
        float sand_scale_y;
        float sand_desired_width;
        float sand_desired_height;
        float sand_damping;

        //for water texture
        sf::Image water_image;
        sf::Texture water_texture;
        sf::Sprite water_sprite;
        sf::Vector2u water_texture_size;
        float water_scale_x;
        float water_scale_y;
        float water_desired_width;
        float water_desired_height;

        //for tree texture
        sf::Image wind_image;
        sf::Texture wind_texture;
        sf::Sprite wind_sprite;
        sf::Vector2u wind_texture_size;
        float wind_scale_x;
        float wind_scale_y;
        float wind_desired_width;
        float wind_desired_height;

        //draw shot_count text
        sf::Text shot_count_text;
        sf::Font shot_count_font;

        //draw least shout count
        sf::Text least_shot_count_text;
        //draw game over text
        sf::Text game_over_text;
        //draw best score text
        sf::Text best_score_text;
        //if scores high score
        sf::Text congratulation_text;

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