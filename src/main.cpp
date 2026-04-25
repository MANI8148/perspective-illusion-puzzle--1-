
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

int W=1280, H=720; // set from monitor at runtime
float camYaw=-135.f, camPitch=38.f, camRadius=16.f;
// Smooth camera lerp targets
float tgtYaw=-135.f, tgtPitch=38.f, tgtRadius=16.f;
float lastX=W/2.f, lastY=H/2.f;
bool firstMouse=true, dragging=false;

AppState appState = AppState::HOME;
CameraMode camMode = CameraMode::ORBIT;
static int gID=0, skyID=0;
float dt=0, lastFrame=0;

// Health
const int MAX_HEALTH = 3;
int playerHealth = MAX_HEALTH;
float lastDamageTime = -10.f; // invincibility timer

// Season (derived from level)
static inline int getSeason(int lv) { return (lv / 2) % 5; }

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

// Draw a thin vertical beacon pillar above a node
void drawBeacon(int id, glm::vec3 base, glm::vec3 col, float now, const glm::mat4& VP, float height=1.8f) {
    float pulse = 0.5f+0.5f*sinf(now*3.f);
    for(int i=0;i<4;i++) {
        float fy = (float)i/3.f;
        float alpha_scale = 1.f-fy*0.6f;
        glm::vec3 c = col * (0.8f + pulse*0.5f) * alpha_scale;
        glm::vec3 p = base + glm::vec3(0, 0.55f + fy*height, 0);
        drawCube(id, p, glm::vec3(0.06f, height/4.f*0.7f, 0.06f), c, VP, true, 5);
    }
    float hs = 0.18f + 0.06f*pulse;
    drawCube(id, base + glm::vec3(0, 0.55f+height+0.1f, 0), glm::vec3(hs, 0.04f, hs), col*(1.f+pulse), VP, true, 5);
}

void drawManFigure(int id, glm::vec3 pos, float time, const glm::mat4& VP) {
    // pos = node world pos = block top surface (block.pos.y + 0.5)
    // Draw the figure so feet sit exactly on pos (y=0 relative)
    float bob = sinf(time * 2.5f) * 0.04f;
    float br  = 1.0f + sinf(time * 1.8f) * 0.02f;
    glm::vec3 body  = glm::vec3(0.92f, 0.35f, 0.35f); // red body
    glm::vec3 head  = glm::vec3(1.00f, 0.87f, 0.78f); // skin head
    glm::vec3 leg   = glm::vec3(0.25f, 0.35f, 0.75f); // blue legs
    // Legs (feet at pos.y)
    drawCube(id, pos + glm::vec3(-0.08f, 0.12f+bob, 0), glm::vec3(0.10f, 0.24f, 0.10f), leg, VP, false, 3);
    drawCube(id, pos + glm::vec3( 0.08f, 0.12f+bob, 0), glm::vec3(0.10f, 0.24f, 0.10f), leg, VP, false, 3);
    // Body (torso sits above legs)
    drawCube(id, pos + glm::vec3(0, 0.40f+bob, 0), glm::vec3(0.26f, 0.30f, 0.14f)*br, body, VP, false, 3);
    // Arms
    drawCube(id, pos + glm::vec3(-0.22f, 0.38f+bob, 0), glm::vec3(0.08f, 0.22f, 0.08f), body, VP, false, 3);
    drawCube(id, pos + glm::vec3( 0.22f, 0.38f+bob, 0), glm::vec3(0.08f, 0.22f, 0.08f), body, VP, false, 3);
    // Head
    drawCube(id, pos + glm::vec3(0, 0.65f+bob, 0), glm::vec3(0.20f)*br, head, VP, false, 3);
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
    currentLevel = idx;
    const Level& lv = levels[idx];
    camYaw = lv.initYaw; camPitch = lv.initPitch; camRadius = lv.initRadius;
    tgtYaw = camYaw; tgtPitch = camPitch; tgtRadius = camRadius;
    for(auto& b : lv.blocks)
        for(auto& n : b.nodes)
            if(n.id == lv.startNodeId) {
                // Node offset is {0, 0.5, 0} — this IS the block top surface
                player.pos = b.pos + n.offset;
                player.currentNodeId = n.id;
            }
    player.movePath.clear(); player.moveT = 0.f; particles.clear();
    playerHealth = MAX_HEALTH;
    lastDamageTime = -10.f;
    camMode = CameraMode::ORBIT;
}

int main() {
    if(!glfwInit()) return -1;
    // Detect primary monitor size and use 90% of it
    {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        if(mon) {
            const GLFWvidmode* vm = glfwGetVideoMode(mon);
            if(vm) { W = (int)(vm->width * 0.90f); H = (int)(vm->height * 0.90f); }
        }
    }
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
        bool illActive = false;
        int season = getSeason(currentLevel);

        // ── Camera: orbit or first-person ──────────────────────────────────
        glm::mat4 view;
        float fovDeg = 44.f;
        if(camMode == CameraMode::FIRST_PERSON) {
            fovDeg = 70.f;
            glm::vec3 eye = player.pos + glm::vec3(0, 0.65f, 0);
            float yawR   = glm::radians(camYaw);
            float pitchR = glm::radians(glm::clamp(camPitch,-80.f,80.f));
            glm::vec3 front(
                cosf(pitchR)*cosf(yawR+glm::radians(90.f)),
                sinf(pitchR),
                cosf(pitchR)*sinf(yawR+glm::radians(90.f)));
            view = glm::lookAt(eye, eye + front, glm::vec3(0,1,0));
        } else {
            view = glm::lookAt(pivot + glm::vec3(
                camRadius*cosf(glm::radians(camYaw))*cosf(glm::radians(camPitch)),
                camRadius*sinf(glm::radians(camPitch)),
                camRadius*sinf(glm::radians(camYaw))*cosf(glm::radians(camPitch))),
                pivot, glm::vec3(0,1,0));
        }
        glm::mat4 proj = glm::perspective(glm::radians(fovDeg), (float)W/H, 0.1f, 150.f);
        glm::mat4 VP = proj * view;

        // 1. Skybox
        glDepthMask(GL_FALSE); skySh.use();
        uMat4(skyID, "view", view); uMat4(skyID, "projection", proj);
        uFloat(skyID, "time", now); uInt(skyID, "levelIndex", currentLevel);
        uInt(skyID, "season", season);
        drawCube(skyID, glm::vec3(0), glm::vec3(120.f), glm::vec3(1), proj*glm::mat4(glm::mat3(view)), false, 0);
        glDepthMask(GL_TRUE);

        if(appState == AppState::PLAYING || appState == AppState::DEAD) {
            sh.use(); uFloat(gID, "time", now); uVec3(gID, "viewPos", pivot);
            uInt(gID, "season", season);
            // Logic (only when playing, not dead)
            if(appState == AppState::PLAYING) {
            if(!player.movePath.empty()) {
                glm::vec3 target; for(auto& b : lv.blocks) for(auto& n : b.nodes) if(n.id == player.movePath[0]) target = getNodeWorldPos(b, n, now);
                player.moveT += dt / player.MOVE_DUR;
                if(player.moveT >= 1.f) { player.pos = target; player.currentNodeId = player.movePath[0]; player.movePath.erase(player.movePath.begin()); player.moveT = 0.f; if(player.currentNodeId == lv.goalNodeId) { levelDone[currentLevel]=true; appState = AppState::COMPLETE; } }
                else {
                    float et = player.moveT*player.moveT*(3-2*player.moveT);
                    player.pos = glm::mix(player.pos, target, et);
                    player.pos.y += sinf(et*M_PI)*0.25f;
                    // Trail sparkles
                    if(fmodf(now, 0.05f) < dt*2.f)
                        spawnPart(player.pos+glm::vec3((float)(rand()%100-50)*0.005f,0.3f,(float)(rand()%100-50)*0.005f),
                                  glm::vec3(0,0.8f+((float)(rand()%100)/100.f)*1.2f,0),
                                  glm::vec3(0.5f,1.f,0.8f), 0.4f, 0.05f, PartType::TRAIL);
                }
            } else if(mouseClick) {
                glm::vec2 mNDC((mx/W)*2-1, -((my/H)*2-1)); float bestD = 0.1f; std::string bId = "";
                for(auto& b : lv.blocks) for(auto& n : b.nodes) { 
                    glm::vec4 sp = VP * glm::vec4(getNodeWorldPos(b, n, now), 1.f); 
                    if (sp.w > 0.0f) {
                        float d = glm::distance(mNDC, glm::vec2(sp.x/sp.w, sp.y/sp.w)); 
                        if(d < bestD) { bestD = d; bId = n.id; } 
                    }
                }
                if(bId != "") player.movePath = findPath(player.currentNodeId, bId, lv, now, VP);
            }
            } // end PLAYING-only logic
        // Draw Platforms
            illActive = false;
            std::set<std::string> ill; 
            std::map<std::string, glm::vec3> id2P; 
            std::map<std::string, glm::vec2> id2Screen;
            std::map<std::string, bool> id2Vis;
            for(auto& b : lv.blocks) {
                for(auto& n : b.nodes) {
                    glm::vec3 p = getNodeWorldPos(b, n, now);
                    id2P[n.id] = p;
                    glm::vec4 sp = VP * glm::vec4(p, 1.f);
                    if(sp.w > 0.0f) {
                        sp /= sp.w;
                        if(sp.z <= 1.0f && sp.z >= -1.0f) {
                            id2Screen[n.id] = glm::vec2((sp.x * 0.5f + 0.5f) * W, (sp.y * 0.5f + 0.5f) * H);
                            id2Vis[n.id] = true;
                        } else id2Vis[n.id] = false;
                    } else id2Vis[n.id] = false;
                }
            }
            const float THRESH = 38.0f;
            for(auto const& [idA, posA] : id2P) {
                if(!id2Vis[idA]) continue;
                for(auto const& [idB, posB] : id2P) {
                    if(idA < idB && id2Vis[idB] && glm::distance(posA, posB) >= 1.6f) {
                        glm::vec2 sA = id2Screen[idA];
                        glm::vec2 sB = id2Screen[idB];
                        if(std::abs(sA.x - sB.x) < THRESH && std::abs(sA.y - sB.y) < THRESH) {
                            ill.insert(idA); ill.insert(idB);
                        }
                    }
                }
            }
            illActive = !ill.empty();
            for(auto& b : lv.blocks) {
                // Obstacle block: render as lava, skip normal logic
                if(b.type == BlockType::OBSTACLE) {
                    float pulse = 0.5f+0.5f*sinf(now*2.5f);
                    drawCube(gID, b.pos, b.scale, b.color, VP, true, 6, 0.f);
                    continue;
                }
                float rY = 0.f; bool isG = false; int mt = 0;
                bool isStart = false;
                if(b.type == BlockType::ROTATING) { float c=4.f; float p=fmodf(now,c)/c; rY=floorf(now/c)*(M_PI/2); if(p>0.75f) rY+=(pow((p-0.75f)*4,2)*(3-2*(p-0.75f)*4))*(M_PI/2); }
                for(auto& n : b.nodes) {
                    if(ill.count(n.id)) isG = true;
                    if(n.id == lv.goalNodeId)  mt = 1;
                    if(n.id == lv.startNodeId) isStart = true;
                }
                if(isG && mt==0) mt = 4;
                // Start platform: bright cyan (Minecraft diamond-block blue)
                glm::vec3 drawCol = b.color;
                if(isStart && mt == 0) {
                    drawCol = glm::vec3(0.0f, 0.85f, 1.0f);
                    mt = 0;
                }
                drawCube(gID, b.pos, b.scale, drawCol, VP, isG, mt, rY);
                drawOutline(gID, b.pos, b.scale, VP, 0.03f, rY);
                // Start beacon (blue)
                for(auto& n : b.nodes) if(n.id == lv.startNodeId)
                    drawBeacon(gID, getNodeWorldPos(b,n,now), glm::vec3(0.4f,0.7f,1.f), now, VP);
                // Goal beacon (green)
                for(auto& n : b.nodes) if(n.id == lv.goalNodeId)
                    drawBeacon(gID, getNodeWorldPos(b,n,now), glm::vec3(0.2f,1.f,0.45f), now, VP, 2.2f);
            }
            drawManFigure(gID, player.pos, now, VP);

            // ── Obstacle collision / damage ───────────────────────────────────
            if(appState == AppState::PLAYING) {
                for(auto& b : lv.blocks) {
                    if(!b.isDamaging) continue;
                    glm::vec3 dangerCtr = b.pos + glm::vec3(0, 0.5f, 0);
                    if(glm::distance(player.pos, dangerCtr) < 0.70f) {
                        if(now - lastDamageTime > 1.0f) {
                            playerHealth--;
                            lastDamageTime = now;
                            // Red damage burst
                            for(int k=0;k<8;k++)
                                spawnPart(player.pos+glm::vec3(0,0.4f,0),
                                    glm::vec3((rand()%100-50)*0.04f,0.8f+rand()%100*0.01f,(rand()%100-50)*0.04f),
                                    glm::vec3(1.f,0.2f,0.0f), 0.5f, 0.07f, PartType::SPARK);
                            if(playerHealth <= 0) {
                                appState = AppState::DEAD;
                            }
                        }
                    }
                }
            }

            // ── Season ambient particles ──────────────────────────────────────
            if(appState == AppState::PLAYING) {
                if(season == 2) { // Rainy: blue-gray streaks
                    for(int k=0;k<4;k++) {
                        glm::vec3 rp = player.pos + glm::vec3((rand()%40-20),6.f+rand()%4,(rand()%40-20));
                        spawnPart(rp,glm::vec3(0,-7.f,0),glm::vec3(0.5f,0.65f,1.f),0.28f,0.02f,PartType::AMBIENT);
                    }
                } else if(season == 4) { // Winter: slow white snow
                    for(int k=0;k<2;k++) {
                        glm::vec3 sp2 = player.pos + glm::vec3((rand()%40-20),7.f+rand()%4,(rand()%40-20));
                        spawnPart(sp2,glm::vec3((rand()%100-50)*0.01f,-0.7f,0),glm::vec3(1.f,1.f,1.f),2.5f,0.07f,PartType::AMBIENT);
                    }
                }
            }

            // Render Particles
            for(int i=0; i<(int)particles.size(); i++) {
                auto& p = particles[i]; p.pos += p.vel * dt; p.life -= dt;
                if(p.life < 0) { particles.erase(particles.begin()+i); i--; continue; }
                drawCube(gID, p.pos, glm::vec3(p.sz), p.col, VP, true, 5);
            }
        }
        ui.time = now;
        ScreenContext ctx{ui, currentLevel, appState, (std::vector<Level>&)levels,
                          mx, my, mouseClick, now, levelDone,
                          playerHealth, MAX_HEALTH, season, camMode};
        ui.beginFrame();
        if(appState == AppState::HOME)         drawHome(ctx);
        else if(appState == AppState::LEVEL_SELECT) drawLevelSelect(ctx);
        else if(appState == AppState::PLAYING)  drawHUD(ctx, illActive, !player.movePath.empty());
        else if(appState == AppState::COMPLETE) drawComplete(ctx);
        else if(appState == AppState::INFO)     drawInfo(ctx);
        else if(appState == AppState::DEAD) {
            drawHUD(ctx, false, false); // keep world visible
            drawDead(ctx);
            if(appState == AppState::PLAYING) { // respawn pressed
                loadLevel(currentLevel);
            }
        }
        ui.endFrame();
        if(appState == AppState::PLAYING && glfwGetKey(win, 82) == 1) loadLevel(currentLevel);
        if(appState == AppState::PLAYING && glfwGetKey(win, 256) == 1) appState = AppState::HOME;
        // V key (key=86) - toggle camera mode
        static bool vWasDown = false;
        bool vDown = (appState == AppState::PLAYING) && glfwGetKey(win, 86) == 1;
        if(vDown && !vWasDown)
            camMode = (camMode == CameraMode::ORBIT) ? CameraMode::FIRST_PERSON : CameraMode::ORBIT;
        vWasDown = vDown;
        mouseClick = false; glfwSwapBuffers(win); glfwPollEvents();
    }
    glfwTerminate(); return 0;
}
