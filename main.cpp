// Inclure les bibliothèques nécessaires
#include "Mortar.hpp"


int main() {
    Mortar game;
    game.init();
    game.loadScene();
    game.gameCycle();
    game.exit();
    return 0;
}

