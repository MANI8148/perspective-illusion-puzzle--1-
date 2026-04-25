#pragma once
#include "ui_renderer.h"
#include "levels.h"
#include <functional>

enum class AppState { HOME, LEVEL_SELECT, PLAYING, COMPLETE, INFO, DEAD };
enum class CameraMode { ORBIT, FIRST_PERSON };
// Season: 0=summer 1=spring 2=rainy 3=autumn 4=winter
// (assigned automatically per level group)

struct ScreenContext {
    UIRenderer& ui;
    int& currentLevel;
    AppState& state;
    std::vector<Level>& levels;
    double mouseX=0, mouseY=0;
    bool mouseClick=false;
    float time=0.f;
    bool* levelDone; // array[12]
    int playerHealth=3;
    int maxHealth=3;
    int season=0;
    CameraMode camMode=CameraMode::ORBIT;
};

void drawHome(ScreenContext& ctx);
void drawLevelSelect(ScreenContext& ctx);
void drawHUD(ScreenContext& ctx, bool illActive, bool isMoving);
void drawComplete(ScreenContext& ctx);
void drawInfo(ScreenContext& ctx);
void drawDead(ScreenContext& ctx);
