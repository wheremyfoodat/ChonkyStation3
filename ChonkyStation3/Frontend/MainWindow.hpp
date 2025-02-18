#pragma once

#include <common.hpp>

#include "PlayStation3.hpp"

#include <thread>

#include <tinyfiledialogs.h>
#include <Frontend/GameWindow.hpp>
#include "Frontend/UI/ui_mainwindow.h"
#include <QtWidgets>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    int run(int argc, char** argv);

    void launchELF();

    PlayStation3* ps3;
    GameWindow game_window;
    std::thread game_thread;
};