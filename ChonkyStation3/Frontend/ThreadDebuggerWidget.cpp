#include "ThreadDebuggerWidget.hpp"


ThreadDebuggerWidget::ThreadDebuggerWidget(PlayStation3* ps3, QWidget* parent) : QWidget(parent, Qt::Window), ps3(ps3) {
    ui.setupUi(this);
    
    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, &ThreadDebuggerWidget::update);
    
    setWindowTitle("Thread Debugger");
    hide();
}

void ThreadDebuggerWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    
    timer.start();
}

void ThreadDebuggerWidget::closeEvent(QCloseEvent* event) {
    QWidget::closeEvent(event);
    
    timer.stop();
}

void ThreadDebuggerWidget::update() {
    // ***** PPU *****
    // Clear table
    ui.ppuTable->clear();
    
    ui.ppuTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui.ppuTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui.ppuTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    ui.ppuTable->setRowCount(ps3->thread_manager.threads.size());
    ui.ppuTable->setColumnCount(4);
    for (int i = 0; i < ui.ppuTable->rowCount(); i++)
        ui.ppuTable->setRowHeight(i, 20);
    ui.ppuTable->setColumnWidth(0, 250);
    ui.ppuTable->setColumnWidth(1, 100);
    ui.ppuTable->setColumnWidth(2, 80);
    ui.ppuTable->setColumnWidth(3, 80);
    //ui.ppuTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.ppuTable->setHorizontalHeaderLabels(QStringList({ "Name", "Status", "ID", "pc"}));
    
    for (int i = 0; i < ps3->thread_manager.threads.size(); i++) {
        auto& thread = ps3->thread_manager.threads[i];
        setListItem(ui.ppuTable, i, 0, thread.name);
        setListItem(ui.ppuTable, i, 1, Thread::threadStatusToString(thread.status));
        setListItem(ui.ppuTable, i, 2, std::format("{:d}", thread.id));
        setListItem(ui.ppuTable, i, 3, std::format("{:08x}", thread.state.pc));
    }
    
    // ***** SPU *****
    ui.spuTable->clear();
    
    ui.spuTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui.spuTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui.spuTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    ui.spuTable->setRowCount(ps3->spu_thread_manager.threads.size());
    ui.spuTable->setColumnCount(4);
    for (int i = 0; i < ui.spuTable->rowCount(); i++)
        ui.spuTable->setRowHeight(i, 20);
    ui.spuTable->setColumnWidth(0, 250);
    ui.spuTable->setColumnWidth(1, 100);
    ui.spuTable->setColumnWidth(2, 80);
    ui.spuTable->setColumnWidth(3, 80);
    //ui.spuTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.spuTable->setHorizontalHeaderLabels(QStringList({ "Name", "Status", "ID", "pc"}));
    
    for (int i = 0; i < ps3->spu_thread_manager.threads.size(); i++) {
        auto& thread = ps3->spu_thread_manager.threads[i];
        setListItem(ui.spuTable, i, 0, thread.name);
        setListItem(ui.spuTable, i, 1, SPUThread::threadStatusToString(thread.status));
        setListItem(ui.spuTable, i, 2, std::format("{:d}", thread.id));
        setListItem(ui.spuTable, i, 3, std::format("{:08x}", thread.state.pc));
    }
}

void ThreadDebuggerWidget::setListItem(QTableWidget* table, int row, int column, std::string str) {
    QTableWidgetItem* item = new QTableWidgetItem();
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    QLabel* label = new QLabel(widget);

    layout->setAlignment(Qt::AlignVCenter);
    layout->setContentsMargins(5, 0, 5, 0);
    label->setStyleSheet("color: white; font-size: 12px;");

    label->setText(QString::fromStdString(str));
    layout->addWidget(label);
    widget->setLayout(layout);

    table->setItem(row, column, item);
    table->setCellWidget(row, column, widget);
}
