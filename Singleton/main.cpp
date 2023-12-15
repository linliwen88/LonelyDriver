// Singleton.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Window.h"

class App
{
public:
    App();
    void DoSomething();
private:
    Window* m_window;
};

App::App()
{
    m_window = new Window(600, 600);
}

void App::DoSomething()
{
    Window::Init(800, 600);
}

int main()
{
    App* app = new App();
}

