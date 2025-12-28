#include "../inc/window_app.hpp"

int main(int argc, char* argv[]){
    // instantiate window app object
    WindowApp app;

    // create initial window settings
    app.initializeWindow(1280, 720, "My Game");

    // run the app
    app.runtime();

    // app reaches here when the window is told to close
    return 0;
}