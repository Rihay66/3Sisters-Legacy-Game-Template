#include "../inc/window_app.hpp"

void WindowApp::init(){
    // initialize resources, players, ECS, etc...
}

void WindowApp::stepUpdate(double ts){
    // update in a time step manner like physics, movmement, etc...
}

void WindowApp::update(){
    // update non-time sensitive functionality like logic, input, etc...
}

void WindowApp::render(double alpha){
    // render players, text, items, etc...
    
    // render clear color for the background
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

}
