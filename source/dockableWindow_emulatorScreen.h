#pragma once

#include "dockableWindow.h"
#include "uiItem_enumButton.h"

class DockableWindow_EmulatorScreen : public DockableWindow
{
public:
    DockableWindow_EmulatorScreen(const string& title) : DockableWindow(title) {}

    int GetContentHeight();
    int GetContentWidth();

    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

    void OnMouseButtonDown(int button, int x, int y);
    void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);

    int m_zoomLevel = 2;

    void OnCapturedKeyInput(bool lostCapture, bool keyDown, u32 sym, u32 mod);
    void OnCapturedTextInput(bool lostCapture, const string& text) {}

protected:
    void OnColdPress();
    void OnResetPress();
    void OnPlayPress();
    void OnSingleCyclePress();
    void OnSingleInstructionPress();
    void OnSingleRowPress();
    void OnSingleFramePress();
    void OnZoomChanged(int option);

    int GetScreenRenderWidth();
    int GetScreenRenderHeight();
    bool m_capturingInput;
    UIItem_EnumButton* m_zoomButton;
};

