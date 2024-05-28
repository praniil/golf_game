#pragma once
#include<SFML/Graphics.hpp>

class Game{
    public:
        Game();
        bool isRunning() const;
        void run();
        void render();
    
    private:
        //window
        sf::RenderWindow game_window;
        //golf ball attributes
        sf::CircleShape golf_ball;
        int golf_ball_pos_x, golf_ball_pos_y;
        //golf hole attribute
        sf::CircleShape golf_hole;
        int golf_hole_pos_x, golf_hole_pos_y;
};