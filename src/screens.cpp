
#include "screens.h"
#include <cmath>
#include <string>

// ── palette ──────────────────────────────────────────────────────────────────
static const glm::vec4 BG     (0.04f,0.06f,0.14f,1.f);
static const glm::vec4 PANEL  (0.10f,0.14f,0.28f,0.92f);
static const glm::vec4 ACCENT (0.30f,0.65f,1.00f,1.f);
static const glm::vec4 GOLD   (1.00f,0.80f,0.20f,1.f);
static const glm::vec4 GREEN  (0.20f,0.90f,0.50f,1.f);
static const glm::vec4 WHITE  (1.f,1.f,1.f,1.f);
static const glm::vec4 DIM    (0.7f,0.7f,0.8f,0.8f);
static const glm::vec4 RED    (1.f,0.3f,0.3f,1.f);

// level accent colours (one per level, wraps)
static glm::vec4 lvlCol(int idx){
    static const glm::vec4 C[]={
        {0.40f,0.75f,1.0f,1},{1.0f,0.5f,0.8f,1},{0.4f,1.0f,0.65f,1},
        {1.0f,0.82f,0.3f,1},{0.55f,0.6f,1.0f,1},{1.0f,0.62f,0.95f,1},
        {0.33f,0.94f,0.77f,1},{0.98f,0.47f,0.66f,1},{0.0f,0.81f,0.79f,1},
        {0.91f,0.12f,0.39f,1},{0.55f,0.77f,0.29f,1},{1.0f,0.34f,0.13f,1},
    };
    return C[idx%12];
}

static const char* lvlIcons[]={
    "→","⋯","◎","⏏","⌂","⚙","◈","↑","∞","◇","❀","⛩"
};
static const char* lvlTips[]={
    "Move arrows to travel platforms",
    "Orbit camera until platforms align",
    "Follow the spiral staircase up",
    "Align the far ledge via illusion",
    "Multi-floor palace — find the bridge",
    "Clockwork arms — rotate to connect",
    "Two illusion jumps required",
    "Climb stairs, cross rotating bridge",
    "Three chained illusions to solve",
    "Crystal maze — mind the height gap",
    "Float across two garden terraces",
    "Ancient temple — hidden altar awaits",
};
static const char* controls[]={"LMB drag — Orbit camera","Scroll — Zoom in / out",
    "Arrow keys — Move player","R — Restart level","N — Next level (on complete)",
    "Escape — Quit"};

// ── button helper ─────────────────────────────────────────────────────────────
static bool button(ScreenContext& ctx,float x,float y,float w,float h,
                   const std::string& label,glm::vec4 col,float sz=22.f){
    bool hov=UIRenderer::hit((float)ctx.mouseX,(float)ctx.mouseY,x,y,w,h);
    glm::vec4 c=col; if(hov) c=glm::vec4(glm::vec3(col)*1.25f,col.a);
    ctx.ui.rect(x,y,w,h,c,0.06f,hov?3:2,
                glm::vec4(glm::vec3(col)*1.5f+glm::vec3(0.1f),1.f));
    ctx.ui.text(label,x+w/2,y+h/2,sz,WHITE,hov?0.8f:0.f,true,true);
    return hov && ctx.mouseClick;
}

// ═══════════════════════════════════════════════════════════════════════════════
void drawHome(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H;
    float t=ctx.time;

    // Full-screen gradient bg
    ctx.ui.rect(0,0,W,H,glm::vec4(0.04f,0.05f,0.13f,1.f),0.f,1,
                glm::vec4(0.06f,0.10f,0.22f,1.f));

    // Animated starfield dots (simple)
    for(int i=0;i<40;i++){
        float px=fmodf(i*137.5f,W), py=fmodf(i*97.3f+t*8.f*(0.3f+(i%5)*0.1f),H);
        float sz=1.5f+sinf(t*2.f+i)*0.8f;
        ctx.ui.rect(px,py,sz,sz,glm::vec4(0.6f,0.8f,1.f,0.4f+0.3f*sinf(t+i)),0.f,0);
    }

    // Central glass panel
    float pw=600,ph=420,px=(W-pw)/2,py=(H-ph)/2-20;
    ctx.ui.rect(px,py,pw,ph,glm::vec4(0.12f,0.18f,0.35f,0.88f),0.06f,2,
                glm::vec4(0.3f,0.55f,1.f,1.f));

    // Title
    float pulse=0.5f+0.5f*sinf(t*1.8f);
    glm::vec4 tc=glm::vec4(0.5f+0.5f*pulse,0.75f,1.f,1.f);
    ctx.ui.text("PERSPECTIVE",W/2,py+65,52.f,tc,0.9f,true,true);
    ctx.ui.text("ILLUSION  PUZZLE",W/2,py+118,34.f,glm::vec4(0.8f,0.9f,1.f,0.95f),0.6f,true,true);

    // Subtitle
    ctx.ui.text("A sci-fi perspective puzzle in OpenGL",W/2,py+165,16.f,
                glm::vec4(0.6f,0.7f,0.9f,0.7f),0.f,true,true);

    // Divider
    ctx.ui.rect(px+40,py+190,pw-80,2,glm::vec4(0.3f,0.55f,1.f,0.4f),0.f,0);

    // Buttons
    float bw=220,bh=50,gap=20;
    float bx=(W-2*bw-gap)/2, by=py+215;

    if(button(ctx,bx,by,bw,bh,"▶  PLAY  (Level 1)",
              glm::vec4(0.15f,0.5f,0.9f,1.f),20.f)){
        ctx.currentLevel=0; ctx.state=AppState::PLAYING;
    }
    if(button(ctx,bx+bw+gap,by,bw,bh,"≡  SELECT LEVEL",
              glm::vec4(0.25f,0.35f,0.6f,1.f),20.f)){
        ctx.state=AppState::LEVEL_SELECT;
    }

    // Controls summary
    ctx.ui.text("CONTROLS",W/2,py+296,15.f,glm::vec4(0.5f,0.7f,1.f,0.9f),0.f,true,true);
    float cy=py+318;
    for(auto& c:controls){
        ctx.ui.text(c,W/2,cy,13.f,glm::vec4(0.65f,0.75f,0.9f,0.7f),0.f,true,true);
        cy+=18;
    }

    // Footer
    ctx.ui.text("12 LEVELS  •  OpenGL 3.3 + GLSL",W/2,H-22,12.f,
                glm::vec4(0.4f,0.5f,0.7f,0.5f),0.f,true,true);
}

// ═══════════════════════════════════════════════════════════════════════════════
void drawLevelSelect(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H;
    float t=ctx.time;

    // BG
    ctx.ui.rect(0,0,W,H,glm::vec4(0.04f,0.05f,0.13f,1.f),0.f,1,
                glm::vec4(0.06f,0.10f,0.22f,1.f));

    // Header panel
    ctx.ui.rect(0,0,W,72,glm::vec4(0.08f,0.12f,0.26f,0.97f),0.f,2,
                glm::vec4(0.25f,0.5f,1.f,1.f));
    ctx.ui.text("SELECT  LEVEL",W/2,36,26.f,
                glm::vec4(0.7f,0.88f,1.f,1.f),0.5f,true,true);

    // Back button
    if(button(ctx,18,16,90,40,"← Back",glm::vec4(0.18f,0.25f,0.5f,1.f),14.f))
        ctx.state=AppState::HOME;

    // Grid: 4 cols × 3 rows
    int N=(int)ctx.levels.size();
    int COLS=4;
    float CW=260,CH=160,GAP=18;
    float totalW=COLS*CW+(COLS-1)*GAP;
    float gx=(W-totalW)/2, gy=95;

    for(int i=0;i<N;i++){
        int col=i%COLS, row=i/COLS;
        float cx=gx+col*(CW+GAP), cy=gy+row*(CH+GAP);

        bool done=ctx.levelDone[i];
        bool hov=UIRenderer::hit((float)ctx.mouseX,(float)ctx.mouseY,cx,cy,CW,CH);
        glm::vec4 ac=lvlCol(i);
        glm::vec4 bg=glm::vec4(glm::vec3(ac)*0.18f+glm::vec3(0.05f,0.07f,0.15f),0.93f);
        if(hov) bg=glm::vec4(glm::vec3(ac)*0.28f+glm::vec3(0.06f,0.09f,0.18f),0.97f);

        // Card BG
        ctx.ui.rect(cx,cy,CW,CH,bg,0.05f,2,ac);

        // Done badge
        if(done){
            ctx.ui.rect(cx+CW-36,cy+6,30,20,
                        glm::vec4(0.1f,0.7f,0.3f,0.9f),0.04f,0);
            ctx.ui.text("✓",cx+CW-21,cy+16,11.f,WHITE,0.f,true,true);
        }

        // Icon bg circle
        ctx.ui.rect(cx+10,cy+10,52,52,
                    glm::vec4(glm::vec3(ac)*0.3f,0.9f),0.12f,4,ac);
        ctx.ui.text(std::to_string(i+1),cx+36,cy+36,18.f,WHITE,0.6f,true,true);

        // Level name
        ctx.ui.text(ctx.levels[i].name,cx+8,cy+70,13.5f,
                    glm::vec4(0.9f,0.95f,1.f,1.f),0.f);
        // Tip
        std::string tip=lvlTips[i];
        if(tip.size()>32) tip=tip.substr(0,29)+"...";
        ctx.ui.text(tip,cx+8,cy+92,10.5f,glm::vec4(0.6f,0.7f,0.85f,0.8f),0.f);

        // Info button
        float ib_x=cx+8, ib_y=cy+CH-32, ib_w=60, ib_h=24;
        bool iHov=UIRenderer::hit((float)ctx.mouseX,(float)ctx.mouseY,ib_x,ib_y,ib_w,ib_h);
        ctx.ui.rect(ib_x,ib_y,ib_w,ib_h,
                    glm::vec4(0.15f,0.25f,0.5f,iHov?0.95f:0.75f),0.04f,0);
        ctx.ui.text("i  Info",ib_x+ib_w/2,ib_y+12,11.f,ACCENT,0.f,true,true);

        // Play button
        float pb_x=cx+CW-78, pb_y=cy+CH-32, pb_w=70, pb_h=24;
        bool pHov=UIRenderer::hit((float)ctx.mouseX,(float)ctx.mouseY,pb_x,pb_y,pb_w,pb_h);
        ctx.ui.rect(pb_x,pb_y,pb_w,pb_h,
                    glm::vec4(glm::vec3(ac)*(pHov?0.9f:0.7f),1.f),0.04f,pHov?3:0,ac);
        ctx.ui.text("▶ Play",pb_x+pb_w/2,pb_y+12,11.f,WHITE,0.f,true,true);

        if(ctx.mouseClick){
            if(iHov){ ctx.currentLevel=i; ctx.state=AppState::INFO; }
            if(pHov){ ctx.currentLevel=i; ctx.state=AppState::PLAYING; }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
void drawHUD(ScreenContext& ctx, bool illActive, bool isMoving){
    float W=ctx.ui.W;
    int  lv=ctx.currentLevel;
    glm::vec4 ac=lvlCol(lv);

    // Top bar
    ctx.ui.rect(0,0,W,44,glm::vec4(0.05f,0.08f,0.18f,0.88f),0.f,2,
                glm::vec4(glm::vec3(ac)*0.6f,1.f));

    ctx.ui.text("L"+std::to_string(lv+1)+"/12",12,22,16.f,ac,0.4f,false,true);
    ctx.ui.text(ctx.levels[lv].name,80,22,15.f,
                glm::vec4(0.85f,0.9f,1.f,0.95f),0.f,false,true);

    // Right side buttons
    float W2=W;
    if(button(ctx,W2-130,6,60,32,"≡ Lvls",glm::vec4(0.2f,0.28f,0.55f,0.9f),11.f))
        ctx.state=AppState::LEVEL_SELECT;
    if(button(ctx,W2-65,6,58,32,"i Info",glm::vec4(0.2f,0.28f,0.55f,0.9f),11.f))
        ctx.state=AppState::INFO;

    // Illusion alert banner
    if(illActive){
        float bw=420,bh=38,bx=(W-bw)/2;
        float pulse=0.5f+0.5f*sinf(ctx.time*4.f);
        ctx.ui.rect(bx,52,bw,bh,
                    glm::vec4(1.f,0.78f,0.08f,0.15f+0.1f*pulse),0.05f,3,
                    glm::vec4(1.f,0.78f,0.08f,1.f));
        ctx.ui.text("✨  ILLUSION ALIGNED — Press arrow to cross!",
                    W/2,71,13.f,GOLD,0.7f,true,true);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
void drawComplete(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H;
    float t=ctx.time;

    // Dark overlay
    ctx.ui.rect(0,0,W,H,glm::vec4(0.f,0.f,0.f,0.6f),0.f,0);

    float pw=480,ph=320,px=(W-pw)/2,py=(H-ph)/2;
    ctx.ui.rect(px,py,pw,ph,glm::vec4(0.08f,0.14f,0.28f,0.97f),0.07f,2,
                glm::vec4(0.3f,0.85f,0.4f,1.f));

    float pulse=0.5f+0.5f*sinf(t*2.f);
    ctx.ui.text("LEVEL COMPLETE!",W/2,py+65,28.f,
                glm::vec4(0.2f,1.f,0.5f,1.f),0.9f,true,true);
    ctx.ui.text(ctx.levels[ctx.currentLevel].name,W/2,py+105,17.f,
                glm::vec4(0.8f,0.9f,1.f,0.9f),0.3f,true,true);

    // Stars
    for(int i=0;i<3;i++){
        float sx=W/2+(i-1)*60.f, sy=py+148;
        float delay=i*0.4f;
        float sc=0.6f+0.4f*sinf(t*2.5f+delay);
        ctx.ui.rect(sx-14*sc,sy-14*sc,28*sc,28*sc,
                    GOLD,0.06f,0,glm::vec4(1.f,0.6f,0.1f,1.f));
    }

    float bw=180,bh=44,gap=16,by=py+195;
    float bx=(W-2*bw-gap)/2;

    bool hasNext=(ctx.currentLevel+1<(int)ctx.levels.size());
    if(button(ctx,bx,by,bw,bh,"↺  Retry",glm::vec4(0.25f,0.35f,0.65f,1.f),16.f))
        ctx.state=AppState::PLAYING; // caller resets level

    if(hasNext){
        if(button(ctx,bx+bw+gap,by,bw,bh,"▶  Next Level",
                  glm::vec4(0.15f,0.55f,0.25f,1.f),16.f)){
            ctx.currentLevel++; ctx.state=AppState::PLAYING;
        }
    } else {
        if(button(ctx,bx+bw+gap,by,bw,bh,"🏠  Home",
                  glm::vec4(0.35f,0.2f,0.6f,1.f),16.f))
            ctx.state=AppState::HOME;
    }

    if(button(ctx,(W-bw)/2,by+bh+18,bw,38,"≡  Level Select",
              glm::vec4(0.18f,0.25f,0.5f,0.9f),14.f))
        ctx.state=AppState::LEVEL_SELECT;
}

// ═══════════════════════════════════════════════════════════════════════════════
void drawInfo(ScreenContext& ctx){
    float W=ctx.ui.W, H=ctx.ui.H;
    int lv=ctx.currentLevel;
    glm::vec4 ac=lvlCol(lv);

    // Overlay
    ctx.ui.rect(0,0,W,H,glm::vec4(0.f,0.f,0.f,0.65f),0.f,0);

    float pw=520,ph=400,px=(W-pw)/2,py=(H-ph)/2;
    ctx.ui.rect(px,py,pw,ph,glm::vec4(0.07f,0.11f,0.24f,0.97f),0.07f,2,ac);

    // Header
    ctx.ui.rect(px,py,pw,56,glm::vec4(glm::vec3(ac)*0.35f,0.95f),0.07f,0);
    ctx.ui.text("Level "+std::to_string(lv+1)+" — "+ctx.levels[lv].name,
                px+pw/2,py+28,18.f,WHITE,0.5f,true,true);

    // Icon
    ctx.ui.rect(px+20,py+72,64,64,glm::vec4(glm::vec3(ac)*0.3f,0.9f),0.1f,4,ac);
    ctx.ui.text(std::to_string(lv+1),px+52,py+104,24.f,WHITE,0.7f,true,true);

    // Hint / tip
    ctx.ui.text("OBJECTIVE",px+100,py+85,12.f,ac,0.4f);
    ctx.ui.text(ctx.levels[lv].hint,px+100,py+110,13.f,
                glm::vec4(0.85f,0.92f,1.f,0.9f),0.f);
    ctx.ui.text(lvlTips[lv],px+100,py+122,11.5f,
                glm::vec4(0.6f,0.72f,0.88f,0.75f),0.f);

    // Divider
    ctx.ui.rect(px+20,py+155,pw-40,1.5f,glm::vec4(glm::vec3(ac)*0.5f,0.5f),0.f,0);

    // Controls
    ctx.ui.text("HOW TO PLAY",px+pw/2,py+175,13.f,ac,0.3f,true,true);
    float cy=py+196;
    for(auto& c:controls){
        ctx.ui.rect(px+30,cy-2,6,6,ac,0.03f,0);
        ctx.ui.text(c,px+44,cy+3,12.f,glm::vec4(0.75f,0.85f,1.f,0.85f),0.f);
        cy+=20;
    }

    // Platform legend
    ctx.ui.rect(px+20,cy+8,pw-40,1.f,glm::vec4(glm::vec3(ac)*0.4f,0.4f),0.f,0);
    cy+=16;
    ctx.ui.text("PLATFORM TYPES",px+pw/2,cy+8,12.f,ac,0.2f,true,true);
    cy+=26;
    const char* ptypes[]={"Start  (blue)","Goal  (green)","Illusion-aligned (gold)","Pillar (decor)"};
    glm::vec4 pcols[]={{0.3f,0.85f,0.95f,1},{0.1f,0.92f,0.48f,1},{1.f,0.78f,0.08f,1},{0.88f,0.9f,0.92f,1}};
    for(int i=0;i<4;i++){
        ctx.ui.rect(px+30,cy,12,12,pcols[i],0.02f,0);
        ctx.ui.text(ptypes[i],px+50,cy+8,11.f,glm::vec4(0.7f,0.8f,0.95f,0.8f),0.f,false,true);
        cy+=18;
    }

    // Buttons
    float bw=160,bh=40,gap=14;
    float bx=(W-2*bw-gap)/2, by=py+ph-56;
    if(button(ctx,bx,by,bw,bh,"▶ Play Now",ac,15.f)){
        ctx.state=AppState::PLAYING;
    }
    if(button(ctx,bx+bw+gap,by,bw,bh,"← Back",
              glm::vec4(0.2f,0.28f,0.55f,1.f),15.f)){
        ctx.state=AppState::LEVEL_SELECT;
    }
}
