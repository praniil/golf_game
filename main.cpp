#include<SFML/Graphics.hpp>
#include<iostream>
#include "golf_game.h"   

int main (int argc, char *argv[]) {
    Game game;
    while(game.isRunning()) {
        game.run();
    }
}


