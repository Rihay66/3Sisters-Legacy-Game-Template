#pragma once

#ifndef WINDOW_APP_HPP
#define WINDOW_APP_HPP

// include SDL windowing
#include <window/sisters_sdl_window.hpp>

class WindowApp : public SDL::Window {
    private:
        // private variables go here
    public:
        // window paradigm

        void init() override;
        void stepUpdate(double ts) override;
        void update() override;
        void render(double alpha) override;
};

#endif
