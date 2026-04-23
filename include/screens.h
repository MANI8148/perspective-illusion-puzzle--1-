#pragma once
#include "ui_renderer.h"
#include "levels.h"
#include <functional>

enum class AppState { HOME, LEVEL_SELECT, PLAYING, COMPLETE, INFO };

struct ScreenContext {
    UIRenderer& ui;
    int& currentLevel;
    AppState& state;
    std::vector<Level>& levels;
    double mouseX=0, mouseY=0;
    bool mouseClick=false;
    float time=0.f;
    bool* levelDone; // array[12]
};

void drawHome(ScreenContext& ctx);
void drawLevelSelect(ScreenContext& ctx);
void drawHUD(ScreenContext& ctx, bool illActive, bool isMoving);
void drawComplete(ScreenContext& ctx);
void drawInfo(ScreenContext& ctx);
