#include "App.h"

// settings
const std::string  TITLE      = "Lonely Driver";
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

int main()
{
    static App* LonelyDriver = new App(SCR_WIDTH, SCR_HEIGHT, TITLE);
    return 0;
}