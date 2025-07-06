#include "PPUDebuggerWidget.hpp"


static int getLinesInViewport(QListWidget* list_widget) {
    int viewport_height = list_widget->viewport()->height();
    QFontMetrics fm = QFontMetrics(list_widget->font());
    int line_height = fm.height();

    int visible_lines = viewport_height / line_height;
    return visible_lines;
}

static std::pair<int, int> getVisibleLineRange(QListWidget* list_widget, QScrollBar* scroll_bar) {
    int first_line = scroll_bar->value();
    int visible_lines = getLinesInViewport(list_widget);
    
    return {first_line, visible_lines};
}

PPUDebuggerWidget::PPUDebuggerWidget(PlayStation3* ps3, QWidget* parent) : QWidget(parent, Qt::Window), ps3(ps3) {
    ui.setupUi(this);

    // Setup disabled widget overlay
    disabled_overlay = new DisabledWidgetOverlay(this, "Pause the emulator to use the PPU Debugger");
    disabled_overlay->resize(size());   // Fill the whole screen
    disabled_overlay->raise();
    disabled_overlay->hide();
    
    // Monospace font
    QFont mono_font = QFont("Courier New");
    mono_font.setStyleHint(QFont::Monospace);
    ui.disasmListWidget->setFont(mono_font);
    ui.registerTextEdit->setFont(mono_font);
    
    // To forward scrolling from the list widget to the external scrollbar
    ui.disasmListWidget->installEventFilter(this);
    
    // Setup scroll bar
    ui.verticalScrollBar->setRange(0, INT32_MAX);
    ui.verticalScrollBar->setPageStep(getLinesInViewport(ui.disasmListWidget));
    ui.verticalScrollBar->hide();
    connect(ui.verticalScrollBar, &QScrollBar::valueChanged, this, &PPUDebuggerWidget::updateDisasm);
    
    ui.registerTextEdit->setReadOnly(true);
    
    disabled_overlay->show();
    setWindowTitle("PPU Debugger");
    hide();
}

void PPUDebuggerWidget::enable() {
    disabled_overlay->hide();
    ui.verticalScrollBar->setValue(ps3->ppu->state.pc);
    updateDisasm();
    updateRegisters();
}

void PPUDebuggerWidget::disable() {
    disabled_overlay->show();
}

void PPUDebuggerWidget::updateDisasm() {
    auto [first, amount] = getVisibleLineRange(ui.disasmListWidget, ui.verticalScrollBar);
    ui.disasmListWidget->clear();
    first = (first + 3) & ~3;   // Align to 4 bytes
    amount *= 4;    // 1 instr == 4 bytes
    for (u32 addr = first; addr < first + amount; addr += 4) {
        if (ps3->mem.isMapped(addr).first) {
            const Instruction instr = { .raw = ps3->mem.read<u32>(addr) };
            auto disasm = PPUDisassembler::disasm(ps3->ppu->state, instr, &ps3->mem, addr);
            
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(disasm));
            if (ps3->ppu->state.pc == addr)
                item->setBackground(Qt::darkGreen);
            
            ui.disasmListWidget->addItem(item);
        } else ui.disasmListWidget->addItem("???");
    }
}

void PPUDebuggerWidget::updateRegisters() {
    auto& state = ps3->ppu->state;
    auto curr_thread = ps3->thread_manager.getCurrentThread();
    std::string t;
    t.reserve(2048);
    
    // Current thread
    t += std::format("Current thread: {:d} \"{:s}\"\n\n", curr_thread->id, curr_thread->name);
    
    t += std::format("pc:   0x{:08x}\n", state.pc);
    t += std::format("lr:   0x{:08x}\n", state.lr);
    t += std::format("ctr:  0x{:08x}\n", state.ctr);
    t += std::format("cr:   0x{:08x}\n", state.cr.raw);
    
    t += "\nGPRs\n";
    for (int i = 0; i < 10; i++)
        t += std::format("r{:01d}:     0x{:016x}\n", i, state.gprs[i]);
    for (int i = 10; i < 32; i++)
        t += std::format("r{:02d}:    0x{:016x}\n", i, state.gprs[i]);
    
    t += "\nFPRs\n";
    for (int i = 0; i < 10; i++)
        t += std::format("f{:01d}:     {:f}\n", i, state.fprs[i]);
    for (int i = 10; i < 32; i++)
        t += std::format("f{:02d}:    {:f}\n", i, state.fprs[i]);

    t += "\nVRs\n";
    for (int i = 0; i < 10; i++)
        t += std::format("v{:01d}:   {{0x{:08x}, 0x{:08x}, 0x{:08x}, 0x{:08x}}}\n      ({:f}, {:f}, {:f}, {:f})\n", i, state.vrs[i].w[3], state.vrs[i].w[2], state.vrs[i].w[1], state.vrs[i].w[0], *(float*)&state.vrs[i].w[3], *(float*)&state.vrs[i].w[2], *(float*)&state.vrs[i].w[1], *(float*)&state.vrs[i].w[0]);
    for (int i = 10; i < 32; i++)
        t += std::format("v{:02d}:  {{0x{:08x}, 0x{:08x}, 0x{:08x}, 0x{:08x}}}\n      ({:f}, {:f}, {:f}, {:f})\n", i, state.vrs[i].w[3], state.vrs[i].w[2], state.vrs[i].w[1], state.vrs[i].w[0], *(float*)&state.vrs[i].w[3], *(float*)&state.vrs[i].w[2], *(float*)&state.vrs[i].w[1], *(float*)&state.vrs[i].w[0]);
    
    ui.registerTextEdit->setPlainText(QString::fromStdString(t));
}

bool PPUDebuggerWidget::eventFilter(QObject* obj, QEvent* event) {
    // Forward scroll events from the list widget to the external scrollbar
    if (obj == ui.disasmListWidget && event->type() == QEvent::Wheel) {
        QWheelEvent* wheel_event = (QWheelEvent*)event;
        int wheel_steps = wheel_event->angleDelta().y() / 60;
        int new_scroll_value = ui.verticalScrollBar->value() - wheel_steps;
        new_scroll_value = qBound(ui.verticalScrollBar->minimum(), new_scroll_value, ui.verticalScrollBar->maximum());
        ui.verticalScrollBar->setValue(new_scroll_value);
        return true;
    }
    
    return QWidget::eventFilter(obj, event);
}

void PPUDebuggerWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    
    enable();
}
 
void PPUDebuggerWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    disabled_overlay->resize(event->size());
    updateDisasm();
}
