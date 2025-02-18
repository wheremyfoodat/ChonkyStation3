#include "MainWindow.hpp"

MainWindow::MainWindow() : QMainWindow() {
    ps3 = new PlayStation3();

    Ui::Main ui;
    ui.setupUi(this);

    // Setup menubar buttons
    connect(ui.actionOpen_ELF, &QAction::triggered, this, &MainWindow::launchELF);

    show();
    resize(640, 480);
    setWindowTitle("ChonkyStation3");
}

int MainWindow::run(int argc, char** argv) {
    
    return 0;
}

void MainWindow::launchELF() {
    const char* filters[1] = { "*.elf" };
    const char* path;
    if (path = tinyfd_openFileDialog("Select a Playstation 3 ELF", "", 1, filters, "ELF File", false)) {
        ps3->elf_path = path;
        ps3->init();
    }

    game_thread = std::thread(&GameWindow::run, &game_window, ps3);
}