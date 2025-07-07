#pragma once

#include <common.hpp>

#include <PlayStation3.hpp>
#include "Frontend/UI/ui_ppu_debugger.h"
#include <QtWidgets>


class DisabledWidgetOverlay;

class PPUDebuggerWidget : public QWidget {
    Q_OBJECT
    
public:
    PPUDebuggerWidget(PlayStation3* ps3, QWidget* parent = nullptr);
    PlayStation3* ps3;
    Ui::PPUDebugger ui;
    
    void enable();
    void disable();
    
private:
    DisabledWidgetOverlay* disabled_overlay;
    
    void updateDisasm();
    void updateRegisters();
    void scrollToAddress(u32 addr);
    void scrollToPC();
    
    bool eventFilter(QObject* obj, QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};
