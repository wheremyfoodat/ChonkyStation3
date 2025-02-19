#pragma once

#include <common.hpp>

#include <string>
#include <format>

#include <SDL.h>
#include <PlayStation3.hpp>


class GameWindow {
public:
    GameWindow();
    void run(PlayStation3* ps3);
};