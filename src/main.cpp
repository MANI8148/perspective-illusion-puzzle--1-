// Perspective Illusion Puzzle — main.cpp (fixed visuals)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include "shader.h"
#include "illusion.h"

const int W = 1280, H = 720;

// ── Camera ────────────────────────────────────────────────────────────────────
float camYaw = -135.f, camPitch = 38.f, camRadius = 16.f;
float lastX = W/2.f, lastY = H/2.f;
bool firstMouse = true;
const glm::vec3 PIVOT(2.5f, 2.0f, -4.5f);

// ── Globals ───────────────────────────────────────────────────────────────────
static int  gID = 0;
float dt = 0, lastFrame = 0;

inline void uMat4(const char* n, const glm::mat4& m){ glUniformMatrix4fv(glGetUniformLocation(gID,n),1,GL_FALSE,glm::value_ptr(m)); }
inline void uVec3(const char* n, const glm::vec3& v){ glUniform3fv(glGetUniformLocation(gID,n),1,glm::value_ptr(v)); }
inline void uBool(const char* n, bool b)            { glUniform1i(glGetUniformLocation(gID,n), b?1:0); }
inline void uFloat(const char* n, float f)          { glUniform1f(glGetUniformLocation(gID,n), f); }

// ── Colours ───────────────────────────────────────────────────────────────────
const glm::vec3 C_PLAT (0.20f, 0.42f, 0.92f);
const glm::vec3 C_GOAL (0.10f, 0.92f, 0.48f);
const glm::vec3 C_ILL  (1.00f, 0.78f, 0.08f);
const glm::vec3 C_PLAY (0.98f, 0.25f, 0.22f);
const glm::vec3 C_DARK (0.06f, 0.08f, 0.18f);
const glm::vec3 C_SHAD (0.03f, 0.04f, 0.10f);
const glm::vec3 C_STRT (0.30f, 0.85f, 0.95f);
const glm::vec3 C_GRID (0.12f, 0.16f, 0.30f);

// ── Platform / Level ──────────────────────────────────────────────────────────
struct Platform { glm::vec3 pos; bool isGoal; bool isStart; };
struct Level {
    std::vector<Platform> plats;
    glm::vec3 pivot;
    float initYaw, initPitch, initRadius;
    std::string hint;
};

std::vector<Level> levels = {
  {
    {
      {{0,0, 0},false,true },
      {{1,0, 0},false,false},
      {{2,0, 0},false,false},
      {{3,0, 0},false,false},
      {{3,0,-1},false,false},
      {{3,4,-7},false,false},
      {{4,4,-7},false,false},
      {{5,4,-7},false,false},
      {{5,4,-8},false,false},
      {{5,4,-9},true, false},
    },
    {2.5f,2.0f,-4.5f}, -135.f, 38.f, 16.f,
    "Orbit until two platforms GLOW GOLD — then press UP to cross the illusion!"
  },
  {
    {
      {{ 0,0, 0},false,true },
      {{ 1,0, 0},false,false},
      {{ 2,0, 0},false,false},
      {{ 2,0,-1},false,false},
      {{ 2,4,-6},false,false},
      {{ 3,4,-6},false,false},
      {{ 4,4,-6},false,false},
      {{ 4,0,-1},false,false},
      {{ 5,0,-1},false,false},
      {{ 6,0,-1},false,false},
      {{ 6,0, 0},false,false},
      {{ 7,0, 0},true, false},
    },
    {3.5f,2.0f,-3.0f}, -120.f, 35.f, 18.f,
    "Two illusions to cross — first UP to the ledge, then DOWN to reach the goal!"
  },
  {
    {
      {{ 0,0, 0},false,true },
      {{ 1,0, 0},false,false},
      {{ 2,0, 0},false,false},
      {{ 2,0,-1},false,false},
      {{ 2,6,-8},false,false},
      {{ 3,6,-8},false,false},
      {{ 4,6,-8},false,false},
      {{ 4,6,-9},false,false},
      {{ 4,6,-10},false,false},
      {{ 5,6,-10},false,false},
      {{ 5,6,-11},false,false},
      {{ 5,0,-4},false,false},
      {{ 6,0,-4},false,false},
      {{ 7,0,-4},false,false},
      {{ 7,0,-3},false,false},
      {{ 7,0,-2},true, false},
    },
    {3.5f,3.0f,-5.5f}, -125.f, 36.f, 20.f,
    "Scale the tower via illusion, then descend through a second illusion to reach the goal!"
  }
};

int  currentLevel = 0;
bool levelComplete = false;
float completeTimer = 0.f;

// ── Player ────────────────────────────────────────────────────────────────────
glm::vec3 playerPos, prevPos, targetPos;
float moveT = 1.f;
bool  isMoving = false, keyHeld = false;
const float MOVE_DUR = 0.18f;

float easeSin(float t){ return t*t*(3.f-2.f*t); }

void loadLevel(int idx){
    currentLevel  = idx;
    levelComplete = false;
    completeTimer = 0.f;
    const Level& lv = levels[idx];
    camYaw    = lv.initYaw;
    camPitch  = lv.initPitch;
    camRadius = lv.initRadius;
    playerPos = lv.plats[0].pos + glm::vec3(0,0.75f,0);
    prevPos = targetPos = playerPos;
    moveT = 1.f; isMoving = false; keyHeld = false;
    std::cout << "\n=== LEVEL " << idx+1 << " ===\n" << lv.hint << "\n\n";
}

// ── Platform lookup — checks X, Y, Z ─────────────────────────────────────────
int platAt(glm::vec3 p){
    const auto& pv = levels[currentLevel].plats;
    for(int i=0;i<(int)pv.size();++i){
        if(std::abs(p.x-pv[i].pos.x)<0.55f &&
           std::abs(p.z-pv[i].pos.z)<0.55f &&
           std::abs((p.y-0.75f)-pv[i].pos.y)<1.2f) return i;
    }
    return -1;
}

// ── Callbacks ─────────────────────────────────────────────────────────────────
void mouseCB(GLFWwindow* w, double x, double y){
    if(glfwGetMouseButton(w,GLFW_MOUSE_BUTTON_LEFT)!=GLFW_PRESS){firstMouse=true;return;}
    if(firstMouse){lastX=(float)x;lastY=(float)y;firstMouse=false;}
    camYaw   += (float)(x-lastX)*0.28f;
    camPitch += (float)(lastY-y)*0.28f;
    camPitch  = glm::clamp(camPitch,5.f,85.f);
    lastX=(float)x; lastY=(float)y;
}
void scrollCB(GLFWwindow*,double,double dy){
    camRadius -= (float)dy*0.8f;
    camRadius  = glm::clamp(camRadius,4.f,32.f);
}

// ── Draw helpers ──────────────────────────────────────────────────────────────
unsigned VAO, VAO_GRID;
int gridLineCount = 0;

void drawSolid(glm::vec3 pos, glm::vec3 sc, glm::vec3 col, glm::mat4 VP, bool glow=false){
    glm::mat4 M = glm::scale(glm::translate(glm::mat4(1),pos),sc);
    uMat4("MVP",VP*M); uMat4("model",M);
    uVec3("objectColor",col); uBool("isGlow",glow); uBool("isWireframe",false);
    glBindVertexArray(VAO); glDrawArrays(GL_TRIANGLES,0,36);
}

void drawOutline(glm::vec3 pos, glm::vec3 sc, glm::mat4 VP, float th=0.028f){
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glLineWidth(1.8f);
    glm::vec3 s=sc+glm::vec3(th);
    glm::mat4 M=glm::scale(glm::translate(glm::mat4(1),pos),s);
    uMat4("MVP",VP*M); uMat4("model",M);
    uVec3("objectColor",C_DARK); uBool("isGlow",false); uBool("isWireframe",true);
    glBindVertexArray(VAO); glDrawArrays(GL_TRIANGLES,0,36);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

// ── Input ─────────────────────────────────────────────────────────────────────
void processInput(GLFWwindow* win, glm::mat4 VP){
    if(glfwGetKey(win,GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(win,true);
    if(glfwGetKey(win,GLFW_KEY_R)==GLFW_PRESS){
        loadLevel(currentLevel); return;
    }
    if(levelComplete){
        if(glfwGetKey(win,GLFW_KEY_N)==GLFW_PRESS){
            if(currentLevel+1<(int)levels.size()) loadLevel(currentLevel+1);
        }
        return;
    }
    if(isMoving) return;

    const int    KEYS[4]={GLFW_KEY_RIGHT,GLFW_KEY_LEFT,GLFW_KEY_UP,GLFW_KEY_DOWN};
    const glm::vec2 DIRS[4]={{1,0},{-1,0},{0,-1},{0,1}};

    bool any=false;
    for(int k:KEYS) any|=(glfwGetKey(win,k)==GLFW_PRESS);
    if(!any){keyHeld=false;return;}
    if(keyHeld) return;

    int which=-1;
    for(int k=0;k<4;++k) if(glfwGetKey(win,KEYS[k])==GLFW_PRESS){which=k;break;}
    if(which<0){keyHeld=true;return;}

    glm::vec2 dir=DIRS[which];
    int ci=platAt(playerPos);
    if(ci<0){keyHeld=true;return;}
    glm::vec3 cp=levels[currentLevel].plats[ci].pos;
    const auto& pv=levels[currentLevel].plats;

    // Real neighbour (1-unit XZ, any Y)
    int best=-1; float bestD=0.35f;
    for(int i=0;i<(int)pv.size();++i){
        if(i==ci) continue;
        glm::vec2 d2(pv[i].pos.x-cp.x, pv[i].pos.z-cp.z);
        float len=glm::length(d2);
        if(len<0.01f||len>1.55f) continue;
        float dot=glm::dot(d2/len,dir);
        if(dot>bestD){bestD=dot;best=i;}
    }
    if(best>=0){
        prevPos=playerPos;
        targetPos=pv[best].pos+glm::vec3(0,0.75f,0);
        moveT=0;isMoving=true;keyHeld=true;return;
    }

    // Illusion neighbour
    int bIll=-1; float bID=0.35f;
    for(int i=0;i<(int)pv.size();++i){
        if(i==ci) continue;
        if(!checkAlignment(cp,pv[i].pos,VP,W,H)) continue;
        glm::vec2 d2(pv[i].pos.x-cp.x, pv[i].pos.z-cp.z);
        float len=glm::length(d2);
        if(len<0.01f) continue;
        float dot=glm::dot(d2/len,dir);
        if(dot>bID){bID=dot;bIll=i;}
    }
    if(bIll>=0){
        prevPos=playerPos;
        targetPos=pv[bIll].pos+glm::vec3(0,0.75f,0);
        moveT=0;isMoving=true;keyHeld=true;
        std::cout<<"✨ Illusion crossed to platform "<<bIll<<"!\n";
        return;
    }
    keyHeld=true;
}

// ── MAIN ─────────────────────────────────────────────────────────────────────
int main(){
    if(!glfwInit()){std::cerr<<"glfwInit failed\n";return -1;}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES,8);

    GLFWwindow* win=glfwCreateWindow(W,H,"Perspective Illusion Puzzle",NULL,NULL);
    if(!win){std::cerr<<"Window failed\n";glfwTerminate();return -1;}
    glfwMakeContextCurrent(win);
    glfwSetCursorPosCallback(win,mouseCB);
    glfwSetScrollCallback(win,scrollCB);
    glfwSwapInterval(1);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){std::cerr<<"GLAD failed\n";return -1;}

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    // NOTE: GL_CULL_FACE intentionally disabled — we need ALL faces visible
    //       from every camera angle so blocks never disappear when orbiting.
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glClearColor(0.04f,0.06f,0.14f,1);

    Shader sh("shaders/vertex.glsl","shaders/fragment.glsl");
    if(!sh.ID){std::cerr<<"Shader failed\n";return -1;}
    gID=sh.ID;

    // Cube: pos(3)+normal(3)
    float verts[]={
        -0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f, 0.5f,-0.5f, 0,0,-1,
         0.5f, 0.5f,-0.5f, 0,0,-1, -0.5f, 0.5f,-0.5f, 0,0,-1, -0.5f,-0.5f,-0.5f, 0,0,-1,
        -0.5f,-0.5f, 0.5f, 0,0, 1,  0.5f,-0.5f, 0.5f, 0,0, 1,  0.5f, 0.5f, 0.5f, 0,0, 1,
         0.5f, 0.5f, 0.5f, 0,0, 1, -0.5f, 0.5f, 0.5f, 0,0, 1, -0.5f,-0.5f, 0.5f, 0,0, 1,
        -0.5f, 0.5f, 0.5f,-1,0, 0, -0.5f, 0.5f,-0.5f,-1,0, 0, -0.5f,-0.5f,-0.5f,-1,0, 0,
        -0.5f,-0.5f,-0.5f,-1,0, 0, -0.5f,-0.5f, 0.5f,-1,0, 0, -0.5f, 0.5f, 0.5f,-1,0, 0,
         0.5f, 0.5f, 0.5f, 1,0, 0,  0.5f, 0.5f,-0.5f, 1,0, 0,  0.5f,-0.5f,-0.5f, 1,0, 0,
         0.5f,-0.5f,-0.5f, 1,0, 0,  0.5f,-0.5f, 0.5f, 1,0, 0,  0.5f, 0.5f, 0.5f, 1,0, 0,
        -0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f, 0.5f, 0,-1,0,
         0.5f,-0.5f, 0.5f, 0,-1,0, -0.5f,-0.5f, 0.5f, 0,-1,0, -0.5f,-0.5f,-0.5f, 0,-1,0,
        -0.5f, 0.5f,-0.5f, 0, 1,0,  0.5f, 0.5f,-0.5f, 0, 1,0,  0.5f, 0.5f, 0.5f, 0, 1,0,
         0.5f, 0.5f, 0.5f, 0, 1,0, -0.5f, 0.5f, 0.5f, 0, 1,0, -0.5f, 0.5f,-0.5f, 0, 1,0,
    };
    unsigned VBO;
    glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // ── Ground grid ──────────────────────────────────────────────────────────
    std::vector<float> gridVerts;
    float gMin=-4.f, gMax=14.f, gY=-0.45f;
    for(float x=gMin;x<=gMax;x+=1.f){
        gridVerts.insert(gridVerts.end(),{x,gY,gMin, 0,1,0});
        gridVerts.insert(gridVerts.end(),{x,gY,gMax, 0,1,0});
    }
    for(float z=gMin;z<=gMax;z+=1.f){
        gridVerts.insert(gridVerts.end(),{gMin,gY,z, 0,1,0});
        gridVerts.insert(gridVerts.end(),{gMax,gY,z, 0,1,0});
    }
    gridLineCount=(int)(gridVerts.size()/6);
    unsigned VAO_G, VBO_G;
    glGenVertexArrays(1,&VAO_G); glGenBuffers(1,&VBO_G);
    glBindVertexArray(VAO_G);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_G);
    glBufferData(GL_ARRAY_BUFFER,gridVerts.size()*sizeof(float),gridVerts.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    VAO_GRID=VAO_G;

    loadLevel(0);

    const glm::vec3 PLAT_SC(1.0f, 0.70f, 1.0f);  // taller slabs — more visible
    const glm::vec3 PLAY_SC(0.36f, 0.68f, 0.36f);

    while(!glfwWindowShouldClose(win)){
        float now=(float)glfwGetTime();
        dt=now-lastFrame; lastFrame=now;

        // Camera
        glm::vec3 pivot=levels[currentLevel].pivot;
        glm::vec3 camOff(
            camRadius*cosf(glm::radians(camYaw))*cosf(glm::radians(camPitch)),
            camRadius*sinf(glm::radians(camPitch)),
            camRadius*sinf(glm::radians(camYaw))*cosf(glm::radians(camPitch))
        );
        glm::vec3 camPos=pivot+camOff;
        glm::mat4 view=glm::lookAt(camPos,pivot,glm::vec3(0,1,0));
        glm::mat4 proj=glm::perspective(glm::radians(44.f),(float)W/H,0.1f,120.f);
        glm::mat4 VP=proj*view;

        processInput(win,VP);

        // Smooth movement + hop arc
        if(isMoving){
            moveT+=dt/MOVE_DUR;
            if(moveT>=1.f){
                moveT=1.f; isMoving=false; playerPos=targetPos;
                int gi=platAt(playerPos);
                if(gi>=0 && levels[currentLevel].plats[gi].isGoal && !levelComplete){
                    levelComplete=true;
                    std::cout<<"🎉  LEVEL "<<currentLevel+1<<" COMPLETE!\n";
                    if(currentLevel+1<(int)levels.size())
                        std::cout<<"Press N for next level, R to retry.\n\n";
                    else
                        std::cout<<"All levels done! Press R to replay.\n\n";
                }
            } else {
                float et=easeSin(moveT);
                playerPos=glm::mix(prevPos,targetPos,et);
                playerPos.y+=sinf(et*glm::pi<float>())*0.30f; // hop
            }
        }

        // Illusion glow detection
        const auto& pv=levels[currentLevel].plats;
        std::vector<bool> glow(pv.size(),false);
        bool anyIll=false;
        for(int i=0;i<(int)pv.size();++i)
            for(int j=i+1;j<(int)pv.size();++j){
                float xzD=glm::length(glm::vec2(pv[j].pos.x-pv[i].pos.x,pv[j].pos.z-pv[i].pos.z));
                if(xzD<1.6f) continue;
                if(checkAlignment(pv[i].pos,pv[j].pos,VP,W,H)){
                    glow[i]=glow[j]=true; anyIll=true;
                }
            }

        // ── Render ────────────────────────────────────────────────────────────
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        sh.use();
        uFloat("time",now);
        uVec3("viewPos",camPos);

        // Ground grid
        glLineWidth(1.0f);
        glm::mat4 gridM=glm::mat4(1);
        uMat4("MVP",VP*gridM); uMat4("model",gridM);
        uVec3("objectColor",C_GRID); uBool("isGlow",false); uBool("isWireframe",true);
        glBindVertexArray(VAO_GRID);
        glDrawArrays(GL_LINES,0,gridLineCount);

        // Platforms (solid + outline — no shadow slab, grid replaces it)
        glPolygonOffset(1.f,1.f);
        for(int i=0;i<(int)pv.size();++i){
            glm::vec3 col = pv[i].isGoal  ? C_GOAL
                           : pv[i].isStart ? C_STRT
                           : glow[i]       ? C_ILL
                                           : C_PLAT;
            bool g = pv[i].isGoal || glow[i];
            drawSolid(pv[i].pos, PLAT_SC, col, VP, g);
            drawOutline(pv[i].pos, PLAT_SC, VP);
        }
        glPolygonOffset(0,0);

        // Illusion beacon pillars above aligned platforms
        for(int i=0;i<(int)pv.size();++i){
            if(!glow[i]) continue;
            float pulse = 0.5f + 0.5f*sinf(now*4.0f);
            glm::vec3 pillarPos = pv[i].pos + glm::vec3(0, 1.1f + pulse*0.25f, 0);
            glm::vec3 pillarSc(0.14f, 0.9f + pulse*0.35f, 0.14f);
            drawSolid(pillarPos, pillarSc, C_ILL, VP, true);
            glm::vec3 capPos = pv[i].pos + glm::vec3(0, 2.0f + pulse*0.45f, 0);
            drawSolid(capPos, glm::vec3(0.28f), C_ILL, VP, true);
        }

        // Player
        glPolygonOffset(-2.f,-2.f);
        drawSolid(playerPos,PLAY_SC,C_PLAY,VP,levelComplete);
        drawOutline(playerPos,PLAY_SC,VP,0.022f);
        glPolygonOffset(0,0);

        // Window title
        std::string st;
        if(levelComplete)
            st = (currentLevel+1<(int)levels.size())
               ? " | COMPLETE! N=Next  R=Retry"
               : " | ALL LEVELS DONE!  R=Replay";
        else if(anyIll)
            st = " | ILLUSION ACTIVE — press arrow key to cross!";
        else
            st = " | LMB=Orbit  Scroll=Zoom  Arrows=Move  R=Restart";

        glfwSetWindowTitle(win,("Perspective Illusion  L"+std::to_string(currentLevel+1)+
                                "/"+std::to_string(levels.size())+
                                "  FPS:"+std::to_string((int)(1.f/std::max(dt,0.001f)))+st).c_str());

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO_G);
    glDeleteBuffers(1,&VBO_G);
    glfwTerminate();
    return 0;
}
