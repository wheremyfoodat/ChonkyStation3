#pragma once

#include <common.hpp>

#include "Frontend/UI/ui_ppu_debugger.h"
#include "DisabledWidgetOverlay.hpp"
#include <QtWidgets>
#include <PlayStation3.hpp>


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
    
    bool eventFilter(QObject* obj, QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};
