
#include "screens.h"
#include <cmath>
#include <string>

// ── Colour Palette ────────────────────────────────────────────────────────────
static const glm::vec4 WHITE  (1.f, 1.f, 1.f, 1.f);
static const glm::vec4 GOLD   (1.f, 0.80f, 0.20f, 1.f);
static const glm::vec4 GOLD2  (1.f, 0.55f, 0.10f, 1.f);
static const glm::vec4 GREEN  (0.18f, 0.92f, 0.50f, 1.f);

// Per-level accent colours
static glm::vec4 lvlCol(int idx){
    static const glm::vec4 C[]={
        {0.38f,0.72f,1.0f,1}, {1.0f,0.45f,0.78f,1}, {0.32f,0.97f,0.62f,1},
        {1.0f,0.82f,0.22f,1}, {0.52f,0.55f,1.0f,1}, {1.0f,0.60f,0.92f,1},
        {0.22f,0.90f,0.75f,1},{0.98f,0.42f,0.60f,1},{0.0f,0.78f,0.90f,1},
        {0.95f,0.15f,0.42f,1},{0.50f,0.85f,0.22f,1},{1.0f,0.38f,0.12f,1},
    };
    return C[idx%12];
}

// Level metadata
static const char* lvlTips[]={
    "Click adjacent platforms to walk",
    "Rotate camera until platforms overlap",
    "Follow the spiral staircase upwards",
    "A leap of faith — align from afar",
    "Multi-floor palace — climb and bridge",
    "Sync with the clockwork rotation",
    "Two illusion jumps required",
    "Ascend stairs, cross the rotating bridge",
    "Chain three illusions to escape",
    "Crystal maze — mind the altitude gap",
    "Float across two garden terraces",
    "Ancient temple — find the hidden altar",
};
static const char* controls[]={
    "Left-drag  —  Orbit camera",
    "Scroll     —  Zoom in / out",
    "Click      —  Move player",
    "V          —  Toggle FP camera",
    "R          —  Restart level",
    "Esc        —  Back to menu",
};

// ── Button helper ─────────────────────────────────────────────────────────────
static bool button(ScreenContext& ctx, float x, float y, float w, float h,
                   const std::string& label, glm::vec4 col, float sz=20.f){
    bool hov = UIRenderer::hit((float)ctx.mouseX,(float)ctx.mouseY,x,y,w,h);
    glm::vec4 c = hov ? glm::vec4(glm::vec3(col)*1.15f, glm::min(col.a+0.15f,1.f)) : col;
    ctx.ui.rect(x, y, w, h, c, 0.07f, hov?3:2, glm::vec4(1.f,1.f,1.f,0.9f));
    ctx.ui.text(label, x+w*0.5f, y+h*0.5f, sz, WHITE, hov?0.6f:0.f, true, true);
    return hov && ctx.mouseClick;
}

// ── HOME ══════════════════════════════════════════════════════════════════════
void drawHome(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H, t=ctx.time;

    // Rich deep gradient background
    ctx.ui.rect(0,0,W,H, glm::vec4(0.04f,0.05f,0.16f,1.f), 0.f,1,
                glm::vec4(0.12f,0.06f,0.28f,1.f));

    // Floating bokeh orbs
    for(int i=0;i<18;i++){
        float bx = fmodf(i*173.f + t*10.f*(0.3f+(i%4)*0.15f), W+60.f)-30.f;
        float by = fmodf(i*113.f + t*6.f*(0.4f+(i%5)*0.12f), H+80.f)-40.f;
        float br = 18.f + sinf(t*1.2f+i)*10.f;
        glm::vec4 bc = lvlCol(i%12);
        ctx.ui.rect(bx-br, by-br, br*2.f, br*2.f,
                    glm::vec4(bc.r,bc.g,bc.b,0.07f+0.03f*sinf(t+i)), 0.5f, 0);
    }

    // Central frosted glass card
    float pw=660.f, ph=460.f, px=(W-pw)*0.5f, py=(H-ph)*0.5f-24.f;
    ctx.ui.rect(px,py,pw,ph, glm::vec4(1.f,1.f,1.f,0.09f),0.07f,2,
                glm::vec4(1.f,1.f,1.f,0.35f));

    // Glowing top accent bar
    float pulse = 0.5f+0.5f*sinf(t*1.6f);
    ctx.ui.rect(px+40,py+8,pw-80,3.f, glm::vec4(pulse,0.7f,1.f,0.7f+0.2f*pulse),0.f,0);

    // Title
    ctx.ui.text("PERSPECTIVE", W*0.5f, py+88.f, 58.f,
                glm::vec4(1.f,1.f,1.f,1.f), 0.5f+0.35f*pulse, true, true);
    ctx.ui.text("ILLUSION  PUZZLE", W*0.5f, py+142.f, 30.f,
                glm::vec4(0.78f,0.88f,1.f,0.95f), 0.2f, true, true);

    // Sub-tagline
    ctx.ui.text("Navigate impossible geometry through perspective",
                W*0.5f, py+185.f, 15.f, glm::vec4(1.f,1.f,1.f,0.55f), 0.f, true, true);

    // Thin divider
    ctx.ui.rect(px+80, py+212, pw-160, 1.5f, glm::vec4(1.f,1.f,1.f,0.18f), 0.f, 0);

    // Play + Level Select buttons
    float bw=210.f, bh=52.f, gap=24.f;
    float bx=(W-2*bw-gap)*0.5f, by=py+240.f;
    if(button(ctx,bx,by,bw,bh,"  PLAY",     glm::vec4(0.22f,0.55f,1.f,0.85f),19.f))
        { ctx.currentLevel=0; ctx.state=AppState::PLAYING; }
    if(button(ctx,bx+bw+gap,by,bw,bh,"  LEVELS", glm::vec4(0.55f,0.28f,0.85f,0.75f),19.f))
        ctx.state=AppState::LEVEL_SELECT;

    // Controls list
    float cy=py+325.f;
    ctx.ui.text("CONTROLS", W*0.5f, cy, 13.f, glm::vec4(1.f,1.f,1.f,0.45f), 0.f,true,true);
    cy+=22.f;
    for(auto& c:controls){
        ctx.ui.text(c, W*0.5f, cy, 13.f, glm::vec4(0.85f,0.92f,1.f,0.75f), 0.f,true,true);
        cy+=19.f;
    }

    // Footer
    ctx.ui.text("12 LEVELS  •  OpenGL 3.3  •  GLSL Shaders",
                W*0.5f, H-20.f, 12.f, glm::vec4(1.f,1.f,1.f,0.28f), 0.f,true,true);
}

// ── LEVEL SELECT ══════════════════════════════════════════════════════════════
void drawLevelSelect(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H, t=ctx.time;

    // Background
    ctx.ui.rect(0,0,W,H, glm::vec4(0.04f,0.05f,0.16f,1.f),0.f,1,
                glm::vec4(0.10f,0.05f,0.24f,1.f));

    // Header bar
    ctx.ui.rect(0,0,W,68.f, glm::vec4(1.f,1.f,1.f,0.08f),0.f,2,
                glm::vec4(1.f,1.f,1.f,0.30f));
    ctx.ui.text("SELECT LEVEL", W*0.5f, 34.f, 24.f, WHITE, 0.3f, true, true);

    if(button(ctx,16,14,88,40,"< Back", glm::vec4(1.f,1.f,1.f,0.15f),13.f))
        ctx.state=AppState::HOME;

    // Grid  4 col x 3 row
    int N=(int)ctx.levels.size();
    float CW=272.f, CH=162.f, GAP=16.f;
    float totalW=4*CW+3*GAP, gx=(W-totalW)*0.5f, gy=86.f;

    for(int i=0;i<N;i++){
        int col=i%4, row=i/4;
        float cx=gx+col*(CW+GAP), cy=gy+row*(CH+GAP);
        glm::vec4 ac=lvlCol(i);
        bool done=ctx.levelDone[i];
        bool hov=UIRenderer::hit((float)ctx.mouseX,(float)ctx.mouseY,cx,cy,CW,CH);

        // Card background
        float cardAlpha = hov ? 0.22f : 0.12f;
        ctx.ui.rect(cx,cy,CW,CH, glm::vec4(ac.r,ac.g,ac.b,cardAlpha),0.06f,2,ac);

        // Top accent strip
        ctx.ui.rect(cx+4,cy+4,CW-8,3.f, glm::vec4(ac.r,ac.g,ac.b,0.8f),0.f,0);

        // Level number badge
        ctx.ui.rect(cx+10,cy+14,46,46, glm::vec4(ac.r,ac.g,ac.b,0.3f),0.08f,4,ac);
        ctx.ui.text(std::to_string(i+1), cx+33, cy+37, 20.f, WHITE,0.5f,true,true);

        // Done checkmark
        if(done){
            ctx.ui.rect(cx+CW-34,cy+8,26,18, glm::vec4(0.15f,0.85f,0.40f,0.85f),0.04f,0);
            ctx.ui.text("✓", cx+CW-21, cy+17, 11.f, WHITE,0.f,true,true);
        }

        // Level name
        ctx.ui.text(ctx.levels[i].name, cx+10, cy+72, 13.5f,
                    glm::vec4(1.f,1.f,1.f,0.95f), 0.f);
        // Tip
        std::string tip=lvlTips[i];
        if(tip.size()>34) tip=tip.substr(0,31)+"...";
        ctx.ui.text(tip, cx+10, cy+92, 10.5f, glm::vec4(0.8f,0.88f,1.f,0.70f),0.f);

        // Play button
        float pb_x=cx+CW-76, pb_y=cy+CH-30, pb_w=68, pb_h=22;
        bool pHov=UIRenderer::hit((float)ctx.mouseX,(float)ctx.mouseY,pb_x,pb_y,pb_w,pb_h);
        ctx.ui.rect(pb_x,pb_y,pb_w,pb_h,
            glm::vec4(ac.r,ac.g,ac.b, pHov?0.85f:0.55f),0.04f,pHov?3:0,ac);
        ctx.ui.text("PLAY",pb_x+pb_w*0.5f,pb_y+11,11.f,WHITE,0.f,true,true);

        if(ctx.mouseClick && pHov){ ctx.currentLevel=i; ctx.state=AppState::PLAYING; }
    }
}

// ── HUD ═══════════════════════════════════════════════════════════════════════
void drawHUD(ScreenContext& ctx, bool illActive, bool isMoving){
    float W=ctx.ui.W, H=ctx.ui.H;
    int lv=ctx.currentLevel;
    glm::vec4 ac=lvlCol(lv);

    // Top bar — frosted
    ctx.ui.rect(0,0,W,48.f, glm::vec4(0.04f,0.05f,0.14f,0.80f),0.f,2,
                glm::vec4(ac.r,ac.g,ac.b,0.6f));

    // Level badge
    ctx.ui.rect(10,7,64,34, glm::vec4(ac.r,ac.g,ac.b,0.25f),0.05f,0);
    ctx.ui.text("L"+std::to_string(lv+1), 42, 24, 17.f, WHITE, 0.4f, true, true);

    // Level name
    ctx.ui.text(ctx.levels[lv].name, 88, 24, 16.f,
                glm::vec4(0.9f,0.95f,1.f,0.95f), 0.f, false, true);

    // Right buttons
    if(button(ctx,W-128,8,58,32,"Lvls", glm::vec4(1.f,1.f,1.f,0.12f),12.f))
        ctx.state=AppState::LEVEL_SELECT;
    if(button(ctx,W-64,8,56,32,"Info", glm::vec4(1.f,1.f,1.f,0.12f),12.f))
        ctx.state=AppState::INFO;

    // Controls cheat-sheet — bottom-left glass card
    float cw=240.f, ch=130.f, cx2=12.f, cy2=H-ch-12.f;
    ctx.ui.rect(cx2,cy2,cw,ch, glm::vec4(0.04f,0.05f,0.14f,0.72f),0.05f,2,
                glm::vec4(1.f,1.f,1.f,0.22f));
    ctx.ui.text("CONTROLS", cx2+12, cy2+14, 12.f,
                glm::vec4(ac.r,ac.g,ac.b,0.9f), 0.f);
    float lcy=cy2+32.f;
    for(auto& c:controls){
        ctx.ui.text(c, cx2+14, lcy, 11.f, glm::vec4(1.f,1.f,1.f,0.78f),0.f);
        lcy+=16.f;
    }

    // ── Health hearts — bottom-right ────────────────────────────────────
    {
        static const char* seasonName[]={
            "Summer","Spring","Rainy","Autumn","Winter"};
        const float HS=22.f, HG=6.f;
        float hx=W-14.f-(ctx.maxHealth*(HS+HG)), hy=H-38.f;
        ctx.ui.rect(hx-8,hy-8,(ctx.maxHealth*(HS+HG))+16,HS+16,
            glm::vec4(0.04f,0.05f,0.14f,0.70f),0.04f,0);
        for(int i=0;i<ctx.maxHealth;i++){
            float hbx=hx+i*(HS+HG);
            glm::vec4 hcol = (i < ctx.playerHealth)
                ? glm::vec4(0.95f,0.20f,0.20f,1.f)
                : glm::vec4(0.30f,0.10f,0.10f,0.7f);
            ctx.ui.rect(hbx, hy, HS, HS, hcol, 0.04f, 0);
            // inner pixel cross to mimic heart pixel art
            if(i < ctx.playerHealth) {
                ctx.ui.rect(hbx+4,hy+2,HS-8,4.f,glm::vec4(1,0.4f,0.4f,0.9f),0.f,0);
                ctx.ui.rect(hbx+2,hy+4,HS-4,HS-10,glm::vec4(1,0.4f,0.4f,0.9f),0.f,0);
            }
        }
        // Season label
        ctx.ui.text(seasonName[ctx.season], W-12.f, hy-18.f, 12.f,
            glm::vec4(ac.r,ac.g,ac.b,0.85f),0.f,false,true);
    }

    // Camera mode badge
    {
        const char* modeStr = (ctx.camMode==CameraMode::FIRST_PERSON) ? "FP VIEW" : "ORBIT";
        float bw2=80.f;
        ctx.ui.rect(W-bw2-8,58.f,bw2,22.f,glm::vec4(0.f,0.f,0.f,0.55f),0.03f,0);
        ctx.ui.text(modeStr,W-8.f-bw2*0.5f,69.f,11.f,glm::vec4(1.f,1.f,1.f,0.9f),0.f,true,true);
    }

    // Illusion banner — top-centre animated
    if(illActive){
        float bw=460.f, bh=40.f, bx=(W-bw)*0.5f;
        float p=0.5f+0.5f*sinf(ctx.time*5.f);
        ctx.ui.rect(bx,54.f,bw,bh,
            glm::vec4(1.f,0.85f,0.15f,0.20f+0.12f*p),0.05f,3,
            glm::vec4(1.f,0.85f,0.15f,1.f));
        ctx.ui.text("ILLUSION ALIGNED  —  Click the platform to cross!",
                    W*0.5f,74.f,13.f, WHITE, 0.7f, true,true);
    }
}

// ── LEVEL COMPLETE ════════════════════════════════════════════════════════════
void drawComplete(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H, t=ctx.time;

    // Dark vignette overlay
    ctx.ui.rect(0,0,W,H, glm::vec4(0.f,0.f,0.f,0.52f),0.f,0);

    // Panel
    float pw=520.f, ph=360.f, px=(W-pw)*0.5f, py=(H-ph)*0.5f;
    ctx.ui.rect(px,py,pw,ph, glm::vec4(0.06f,0.08f,0.20f,0.97f),0.08f,2,
                glm::vec4(0.3f,0.9f,0.45f,1.f));

    // Green top strip
    ctx.ui.rect(px,py,pw,6.f, glm::vec4(0.2f,0.95f,0.5f,0.8f),0.f,0);

    // Headline
    float gp = 0.5f+0.5f*sinf(t*2.2f);
    ctx.ui.text("LEVEL COMPLETE!", W*0.5f, py+68.f, 34.f,
                glm::vec4(0.22f,1.f,0.52f,1.f), 0.6f+0.3f*gp, true,true);
    ctx.ui.text(ctx.levels[ctx.currentLevel].name, W*0.5f, py+110.f, 19.f,
                glm::vec4(0.85f,0.95f,1.f,0.9f), 0.2f, true,true);

    // SDF Stars (mode 6)
    for(int i=0;i<3;i++){
        float sx = W*0.5f+(i-1)*78.f;
        float sy = py+178.f;
        float sc = 1.f+0.18f*sinf(t*2.8f+i*0.5f);
        float sz = 50.f*sc;
        ctx.ui.rect(sx-sz*0.5f, sy-sz*0.5f, sz,sz,
                    GOLD, 0.f, 6, GOLD2);
    }

    // Buttons
    float bw=190.f, bh=46.f, gap=18.f, by=py+246.f;
    float bx=(W-2*bw-gap)*0.5f;

    if(button(ctx,bx,by,bw,bh,"  Retry", glm::vec4(0.28f,0.38f,0.72f,0.85f),16.f))
        ctx.state=AppState::PLAYING;

    bool hasNext=(ctx.currentLevel+1<(int)ctx.levels.size());
    if(hasNext){
        if(button(ctx,bx+bw+gap,by,bw,bh,"  Next Level",
                  glm::vec4(0.15f,0.62f,0.28f,0.90f),16.f)){
            ctx.currentLevel++; ctx.state=AppState::PLAYING;
        }
    } else {
        if(button(ctx,bx+bw+gap,by,bw,bh,"  Home",
                  glm::vec4(0.55f,0.22f,0.72f,0.85f),16.f))
            ctx.state=AppState::HOME;
    }

    if(button(ctx,(W-bw)*0.5f, by+bh+16, bw, 38.f, "Level Select",
              glm::vec4(1.f,1.f,1.f,0.12f),14.f))
        ctx.state=AppState::LEVEL_SELECT;
}

// ── INFO ══════════════════════════════════════════════════════════════════════
void drawInfo(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H;
    int lv=ctx.currentLevel;
    glm::vec4 ac=lvlCol(lv);

    // Overlay
    ctx.ui.rect(0,0,W,H, glm::vec4(0.f,0.f,0.f,0.55f),0.f,0);

    float pw=560.f, ph=430.f, px=(W-pw)*0.5f, py=(H-ph)*0.5f;
    ctx.ui.rect(px,py,pw,ph, glm::vec4(0.05f,0.07f,0.20f,0.97f),0.07f,2,ac);

    // Header
    ctx.ui.rect(px,py,pw,58.f, glm::vec4(ac.r,ac.g,ac.b,0.22f),0.07f,0);
    ctx.ui.text("Level "+std::to_string(lv+1)+"  —  "+ctx.levels[lv].name,
                px+pw*0.5f, py+29.f, 19.f, WHITE, 0.5f, true,true);

    // Level number icon
    ctx.ui.rect(px+22,py+76,66,66, glm::vec4(ac.r,ac.g,ac.b,0.25f),0.10f,4,ac);
    ctx.ui.text(std::to_string(lv+1), px+55, py+109, 26.f, WHITE,0.7f,true,true);

    // Objective
    ctx.ui.text("OBJECTIVE", px+106, py+88, 13.f, ac, 0.4f);
    ctx.ui.text(ctx.levels[lv].hint, px+106, py+112, 14.f,
                glm::vec4(0.9f,0.95f,1.f,0.95f), 0.f);
    ctx.ui.text(lvlTips[lv], px+106, py+132, 12.f,
                glm::vec4(0.75f,0.85f,1.f,0.75f), 0.f);

    // Divider
    ctx.ui.rect(px+20,py+162,pw-40,1.5f, glm::vec4(1.f,1.f,1.f,0.18f),0.f,0);

    // Controls
    ctx.ui.text("HOW TO PLAY", px+pw*0.5f, py+184, 13.f, ac, 0.3f,true,true);
    float cy=py+206;
    for(auto& c:controls){
        ctx.ui.rect(px+32,cy-3,8,8, ac, 0.03f,0);
        ctx.ui.text(c, px+50, cy+3, 12.5f, glm::vec4(1.f,1.f,1.f,0.88f),0.f);
        cy+=22.f;
    }

    // Platform legend
    ctx.ui.rect(px+20,cy+10,pw-40,1.5f, glm::vec4(1.f,1.f,1.f,0.15f),0.f,0);
    cy+=22.f;
    ctx.ui.text("PLATFORM TYPES", px+pw*0.5f, cy+10, 13.f, ac, 0.25f,true,true);
    cy+=28.f;
    const char* pn[]={"Start (blue node)","Goal (green runes)","Aligned (gold glow)","Pillar (decorative)"};
    glm::vec4 pc[]={{0.4f,0.8f,1.f,1.f},{0.2f,0.95f,0.45f,1.f},{1.f,0.82f,0.18f,1.f},{0.78f,0.82f,0.90f,1.f}};
    for(int i=0;i<4;i++){
        ctx.ui.rect(px+32,cy,14,14,pc[i],0.03f,0);
        ctx.ui.text(pn[i], px+54, cy+10, 12.f, glm::vec4(0.88f,0.92f,1.f,0.85f),0.f,false,true);
        cy+=20.f;
    }

    // Buttons
    float bw=180.f, bh=44.f, bgap=18.f;
    float bx=(W-2*bw-bgap)*0.5f, bby=py+ph-58.f;
    if(button(ctx,bx,bby,bw,bh,"  Play Now", ac,15.f))
        ctx.state=AppState::PLAYING;
    if(button(ctx,bx+bw+bgap,bby,bw,bh,"< Back",
              glm::vec4(1.f,1.f,1.f,0.15f),15.f))
        ctx.state=AppState::LEVEL_SELECT;
}

// ── DEAD (YOU DIED) ═══════════════════════════════════════════════════════════
void drawDead(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H, t=ctx.time;

    // Dark red overlay
    ctx.ui.rect(0,0,W,H, glm::vec4(0.35f,0.01f,0.01f,0.60f),0.f,0);

    // Panel
    float pw=480.f, ph=260.f, px=(W-pw)*0.5f, py=(H-ph)*0.5f;
    ctx.ui.rect(px,py,pw,ph, glm::vec4(0.10f,0.02f,0.02f,0.97f),0.06f,2,
                glm::vec4(0.85f,0.12f,0.12f,1.f));
    ctx.ui.rect(px,py,pw,6.f, glm::vec4(0.90f,0.15f,0.10f,0.9f),0.f,0);

    // YOU DIED text
    float p2 = 0.5f+0.5f*sinf(t*1.8f);
    ctx.ui.text("YOU DIED", W*0.5f, py+76.f, 42.f,
                glm::vec4(0.95f,0.15f+0.1f*p2,0.10f,1.f), 0.4f+0.2f*p2, true, true);
    ctx.ui.text("Touched a lava obstacle!", W*0.5f, py+122.f, 15.f,
                glm::vec4(1.f,0.60f,0.40f,0.85f), 0.f, true, true);

    // Respawn button
    float bw=200.f, bh=48.f;
    if(button(ctx,(W-bw)*0.5f, py+166.f, bw, bh, "RESPAWN",
              glm::vec4(0.72f,0.08f,0.08f,0.90f), 17.f))
        ctx.state=AppState::PLAYING;
}
