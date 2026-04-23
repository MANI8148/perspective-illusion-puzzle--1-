
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include "shader.h"
#include "illusion.h"
#include "levels.h"
#include "ui_renderer.h"
#include "screens.h"

const int W=1280, H=720;
float camYaw=-135.f, camPitch=38.f, camRadius=16.f;
float lastX=W/2.f, lastY=H/2.f;
bool firstMouse=true, dragging=false;

AppState appState = AppState::HOME;
static int gID=0, skyID=0;
float dt=0, lastFrame=0;

std::vector<Level> levels = buildLevels();
int currentLevel = 0;
bool levelDone[12] = {};

struct Player {
    glm::vec3 pos;
    std::string currentNodeId;
    std::vector<std::string> movePath;
    float moveT = 0.f;
    const float MOVE_DUR = 0.18f;
} player;

enum class PartType { AMBIENT, SPARK, TRAIL, FOUNTAIN };
struct Particle { glm::vec3 pos, vel, col; float life, sz; PartType type; };
std::vector<Particle> particles;

void spawnPart(glm::vec3 p, glm::vec3 v, glm::vec3 c, float l, float s, PartType t) {
    particles.push_back({p, v, c, l, s, t});
}

inline void uMat4(int id, const char*n,const glm::mat4&m){glUniformMatrix4fv(glGetUniformLocation(id,n),1,GL_FALSE,glm::value_ptr(m));}
inline void uVec3(int id, const char*n,const glm::vec3&v){glUniform3fv(glGetUniformLocation(id,n),1,glm::value_ptr(v));}
inline void uBool(int id, const char*n,bool b){glUniform1i(glGetUniformLocation(id,n),b?1:0);}
inline void uFloat(int id, const char*n,float f){glUniform1f(glGetUniformLocation(id,n),f);}
inline void uInt(int id, const char*n,int i){glUniform1i(glGetUniformLocation(id,n),i);}

unsigned VAO;

void drawCube(int id, glm::vec3 pos, glm::vec3 sc, glm::vec3 col, const glm::mat4& VP, bool glow=false, int matT=0, float rotY=0.f) {
    glm::mat4 M = glm::translate(glm::mat4(1), pos);
    if(rotY != 0.f) M = glm::rotate(M, rotY, glm::vec3(0,1,0));
    M = glm::scale(M, sc);
    uMat4(id, "MVP", VP*M); uMat4(id, "model", M);
    uVec3(id, "objectColor", col); uBool(id, "isGlow", glow); uBool(id, "isWireframe", false); uInt(id, "matType", matT);
    glBindVertexArray(VAO); glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawOutline(int id, glm::vec3 pos, glm::vec3 sc, const glm::mat4& VP, float th=0.03f, float rotY=0.f) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); glLineWidth(1.8f);
    glm::mat4 M = glm::translate(glm::mat4(1), pos);
    if(rotY != 0.f) M = glm::rotate(M, rotY, glm::vec3(0,1,0));
    M = glm::scale(M, sc + glm::vec3(th));
    uMat4(id, "MVP", VP*M); uMat4(id, "model", M);
    uVec3(id, "objectColor", glm::vec3(0.05f)); uBool(id, "isGlow", false); uBool(id, "isWireframe", true); uInt(id, "matType", 0);
    glBindVertexArray(VAO); glDrawArrays(GL_TRIANGLES, 0, 36);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawManFigure(int id, glm::vec3 pos, float time, const glm::mat4& VP) {
    float bob = sinf(time * 2.5f) * 0.04f;
    float br  = 1.0f + sinf(time * 1.8f) * 0.02f;
    glm::vec3 c = glm::vec3(0.98f, 0.42f, 0.42f);
    drawCube(id, pos + glm::vec3(0, 0.25f+bob, 0), glm::vec3(0.28f, 0.45f, 0.15f)*br, c, VP, false, 3);
    drawCube(id, pos + glm::vec3(0, 0.55f+bob, 0), glm::vec3(0.22f)*br, glm::vec3(1, 0.85f, 0.8f), VP, false, 3);
    drawCube(id, pos + glm::vec3(-0.18f, 0.2f+bob, 0), glm::vec3(0.08f, 0.3f, 0.08f), c, VP, false, 3);
    drawCube(id, pos + glm::vec3( 0.18f, 0.2f+bob, 0), glm::vec3(0.08f, 0.3f, 0.08f), c, VP, false, 3);
}

glm::vec3 getNodeWorldPos(const Platform& b, const Node& n, float time) {
    if(b.type != BlockType::ROTATING) return b.pos + n.offset;
    float cycle = 4.f; float phase = fmodf(time, cycle) / cycle;
    float angle = floorf(time / cycle) * (M_PI / 2.f);
    if(phase > 0.75f) { float t = (phase - 0.75f) * 4.f; angle += (t * t * (3.f - 2.f * t)) * (M_PI / 2.f); }
    glm::vec4 off = glm::rotate(glm::mat4(1), angle, glm::vec3(0,1,0)) * glm::vec4(n.offset, 1.f);
    return b.pos + glm::vec3(off);
}

std::vector<std::string> findPath(const std::string& sId, const std::string& tId, const Level& lv, float t, const glm::mat4& VP) {
    if(sId == tId) return {};
    std::queue<std::pair<std::string, std::vector<std::string>>> q; q.push({sId, {}});
    std::set<std::string> seen;
    std::map<std::string, glm::vec3> id2P; std::map<std::string, std::vector<std::string>> adj;
    for(auto& b : lv.blocks) for(auto& n : b.nodes) { id2P[n.id] = getNodeWorldPos(b, n, t); for(auto& c : n.connections) adj[n.id].push_back(c); }
    while(!q.empty()){
        auto cur = q.front(); q.pop();
        if(cur.first == tId) return cur.second;
        if(seen.count(cur.first)) continue; seen.insert(cur.first);
        for(auto& nxt : adj[cur.first]) if(id2P.count(nxt) && glm::distance(id2P[cur.first], id2P[nxt]) < 1.6f) { auto p = cur.second; p.push_back(nxt); q.push({nxt, p}); }
        for(auto const& [id, pos] : id2P) if(id != cur.first && checkAlignment(id2P[cur.first], pos, VP, W, H)) { auto p = cur.second; p.push_back(id); q.push({id, p}); }
    }
    return {};
}

void loadLevel(int idx) {
    currentLevel = idx; const Level& lv = levels[idx];
    camYaw = lv.initYaw; camPitch = lv.initPitch; camRadius = lv.initRadius;
    for(auto& b : lv.blocks) for(auto& n : b.nodes) if(n.id == lv.startNodeId) { player.pos = b.pos + n.offset; player.currentNodeId = n.id; }
    player.movePath.clear(); player.moveT = 0.f; particles.clear();
}

int main() {
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); glfwWindowHint(GLFW_SAMPLES,8);
    GLFWwindow* win = glfwCreateWindow(W, H, "Perspective Illusion Puzzle", NULL, NULL);
    if(!win) return -1;
    glfwMakeContextCurrent(win); glfwSwapInterval(1);
    glfwSetCursorPosCallback(win, [](GLFWwindow*, double x, double y){
        if(appState != AppState::PLAYING) { firstMouse=true; return; }
        if(glfwGetMouseButton(glfwGetCurrentContext(), 0) != 1) { firstMouse=true; dragging=false; return; }
        if(firstMouse) { lastX=(float)x; lastY=(float)y; firstMouse=false; dragging=true; }
        if(dragging) { camYaw += (float)(x - lastX) * 0.25f; camPitch += (float)(lastY - y) * 0.25f; camPitch = glm::clamp(camPitch, 5.f, 85.f); }
        lastX=(float)x; lastY=(float)y;
    });
    glfwSetScrollCallback(win, [](GLFWwindow*, double, double dy){ if(appState==AppState::PLAYING) { camRadius -= (float)dy; camRadius=glm::clamp(camRadius, 5.f, 40.f); } });
    static bool mouseClick = false;
    glfwSetMouseButtonCallback(win, [](GLFWwindow*, int b, int a, int){ if(b==0 && a==1) mouseClick=true; });
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST); glEnable(GL_MULTISAMPLE); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Shader sh("shaders/vertex.glsl", "shaders/fragment.glsl"); gID = sh.ID;
    Shader skySh("shaders/sky_vert.glsl", "shaders/sky_frag.glsl"); skyID = skySh.ID;
    UIRenderer ui; ui.init(W, H);
    
    float verts[]={
        -.5f,-.5f,-.5f,0,0,-1, .5f,-.5f,-.5f,0,0,-1, .5f,.5f,-.5f,0,0,-1, .5f,.5f,-.5f,0,0,-1, -.5f,.5f,-.5f,0,0,-1, -.5f,-.5f,-.5f,0,0,-1,
        -.5f,-.5f,.5f,0,0,1, .5f,-.5f,.5f,0,0,1, .5f,.5f,.5f,0,0,1, .5f,.5f,.5f,0,0,1, -.5f,.5f,.5f,0,0,1, -.5f,-.5f,.5f,0,0,1,
        -.5f,.5f,.5f,-1,0,0, -.5f,.5f,-.5f,-1,0,0, -.5f,-.5f,-.5f,-1,0,0, -.5f,-.5f,-.5f,-1,0,0, -.5f,-.5f,.5f,-1,0,0, -.5f,.5f,.5f,-1,0,0,
        .5f,.5f,.5f,1,0,0, .5f,.5f,-.5f,1,0,0, .5f,-.5f,-.5f,1,0,0, .5f,-.5f,-.5f,1,0,0, .5f,-.5f,.5f,1,0,0, .5f,.5f,.5f,1,0,0,
        -.5f,-.5f,-.5f,0,-1,0, .5f,-.5f,-.5f,0,-1,0, .5f,-.5f,.5f,0,-1,0, .5f,-.5f,.5f,0,-1,0, -.5f,-.5f,.5f,0,-1,0, -.5f,-.5f,-.5f,0,-1,0,
        -.5f,.5f,-.5f,0,1,0, .5f,.5f,-.5f,0,1,0, .5f,.5f,.5f,0,1,0, .5f,.5f,.5f,0,1,0, -.5f,.5f,.5f,0,1,0, -.5f,.5f,-.5f,0,1,0,
    };
    unsigned VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,0,6*sizeof(float),0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,0,6*sizeof(float),(void*)(3*sizeof(float))); glEnableVertexAttribArray(1);

    loadLevel(0);
    while(!glfwWindowShouldClose(win)){
        float now = (float)glfwGetTime(); dt = now - lastFrame; lastFrame = now;
        double mx, my; glfwGetCursorPos(win, &mx, &my);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        const Level& lv = levels[currentLevel];
        glm::vec3 pivot = lv.pivot;
        glm::mat4 view = glm::lookAt(pivot + glm::vec3(camRadius*cosf(glm::radians(camYaw))*cosf(glm::radians(camPitch)), camRadius*sinf(glm::radians(camPitch)), camRadius*sinf(glm::radians(camYaw))*cosf(glm::radians(camPitch))), pivot, glm::vec3(0,1,0));
        glm::mat4 proj = glm::perspective(glm::radians(44.f), (float)W/H, 0.1f, 150.f);
        glm::mat4 VP = proj * view;

        // 1. Skybox
        glDepthMask(GL_FALSE); skySh.use();
        uMat4(skyID, "view", view); uMat4(skyID, "projection", proj); uFloat(skyID, "time", now);
        drawCube(skyID, glm::vec3(0), glm::vec3(120.f), glm::vec3(1), proj*glm::mat4(glm::mat3(view)), false, 0);
        glDepthMask(GL_TRUE);

        if(appState == AppState::PLAYING) {
            sh.use(); uFloat(gID, "time", now); uVec3(gID, "viewPos", pivot);
            // Logic
            if(!player.movePath.empty()) {
                glm::vec3 target; for(auto& b : lv.blocks) for(auto& n : b.nodes) if(n.id == player.movePath[0]) target = getNodeWorldPos(b, n, now);
                player.moveT += dt / player.MOVE_DUR;
                if(player.moveT >= 1.f) { player.pos = target; player.currentNodeId = player.movePath[0]; player.movePath.erase(player.movePath.begin()); player.moveT = 0.f; if(player.currentNodeId == lv.goalNodeId) { levelDone[currentLevel]=true; appState = AppState::COMPLETE; } }
                else { float et = player.moveT*player.moveT*(3-2*player.moveT); player.pos = glm::mix(player.pos, target, et); player.pos.y += sinf(et*M_PI)*0.25f; }
            } else if(mouseClick) {
                glm::vec2 mNDC((mx/W)*2-1, -((my/H)*2-1)); float bestD = 0.1f; std::string bId = "";
                for(auto& b : lv.blocks) for(auto& n : b.nodes) { glm::vec4 sp = VP * glm::vec4(getNodeWorldPos(b, n, now), 1.f); float d = glm::distance(mNDC, glm::vec2(sp.x/sp.w, sp.y/sp.w)); if(d < bestD) { bestD = d; bId = n.id; } }
                if(bId != "") player.movePath = findPath(player.currentNodeId, bId, lv, now, VP);
            }
            // Ambient Dust

            // Draw Platforms
            std::set<std::string> ill; std::map<std::string, glm::vec3> id2P; for(auto& b : lv.blocks) for(auto& n : b.nodes) id2P[n.id] = getNodeWorldPos(b, n, now);
            for(auto const& [id, pos] : id2P) if(id != player.currentNodeId && checkAlignment(id2P[player.currentNodeId], pos, VP, W, H)) ill.insert(id);

            for(auto& b : lv.blocks) {
                float rY = 0.f; bool isG = false; int mt = 0;
                if(b.type == BlockType::ROTATING) { float c=4.f; float p=fmodf(now,c)/c; rY=floorf(now/c)*(M_PI/2); if(p>0.75f) rY+=(pow((p-0.75f)*4,2)*(3-2*(p-0.75f)*4))*(M_PI/2); }
                for(auto& n : b.nodes) { if(ill.count(n.id)) isG = true; if(n.id == lv.goalNodeId) mt=1; }
                if(isG && mt==0) mt=4; 
                drawCube(gID, b.pos, b.scale, b.color, VP, isG, mt, rY); drawOutline(gID, b.pos, b.scale, VP, 0.03f, rY);
            }
            drawManFigure(gID, player.pos, now, VP);
            // Render Particles
            for(int i=0; i<(int)particles.size(); i++) {
                auto& p = particles[i]; p.pos += p.vel * dt; p.life -= dt;
                if(p.life < 0) { particles.erase(particles.begin()+i); i--; continue; }
                drawCube(gID, p.pos, glm::vec3(p.sz), p.col, VP, true, 5);
            }
        }
        ui.time = now; ScreenContext ctx{ui, currentLevel, appState, (std::vector<Level>&)levels, mx, my, mouseClick, now, levelDone};
        ui.beginFrame();
        if(appState == AppState::HOME) drawHome(ctx); else if(appState == AppState::LEVEL_SELECT) drawLevelSelect(ctx); else if(appState == AppState::PLAYING) drawHUD(ctx, false, !player.movePath.empty()); else if(appState == AppState::COMPLETE) drawComplete(ctx); else if(appState == AppState::INFO) drawInfo(ctx);
        ui.endFrame();
        if(appState == AppState::PLAYING && glfwGetKey(win, 82) == 1) loadLevel(currentLevel);
        if(appState == AppState::PLAYING && glfwGetKey(win, 256) == 1) appState = AppState::HOME;
        mouseClick = false; glfwSwapBuffers(win); glfwPollEvents();
    }
    glfwTerminate(); return 0;
}
