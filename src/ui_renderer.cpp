#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "ui_renderer.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

// ── UIShader ───────────────────────────────────────────────────────────────
static std::string readFile(const char* p){
    std::ifstream f(p);
    if(!f){std::cerr<<"[UIShader] cannot open "<<p<<"\n";return "";}
    std::stringstream ss; ss<<f.rdbuf(); return ss.str();
}
static unsigned compileShader(GLenum type, const std::string& src){
    unsigned s=glCreateShader(type);
    const char* c=src.c_str();
    glShaderSource(s,1,&c,nullptr);
    glCompileShader(s);
    int ok; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){char buf[512];glGetShaderInfoLog(s,512,nullptr,buf);std::cerr<<"[Shader] "<<buf<<"\n";}
    return s;
}

bool UIShader::load(const char* vs, const char* fs){
    std::string vsrc=readFile(vs), fsrc=readFile(fs);
    if(vsrc.empty()||fsrc.empty()) return false;
    unsigned v=compileShader(GL_VERTEX_SHADER,vsrc);
    unsigned f=compileShader(GL_FRAGMENT_SHADER,fsrc);
    ID=glCreateProgram();
    glAttachShader(ID,v); glAttachShader(ID,f);
    glLinkProgram(ID);
    int ok; glGetProgramiv(ID,GL_LINK_STATUS,&ok);
    if(!ok){char buf[512];glGetProgramInfoLog(ID,512,nullptr,buf);std::cerr<<"[Program] "<<buf<<"\n";}
    glDeleteShader(v); glDeleteShader(f);
    return ok;
}
void  UIShader::use()                                    const { glUseProgram(ID); }
void  UIShader::setMat4 (const char* n,const glm::mat4& m) const { glUniformMatrix4fv(glGetUniformLocation(ID,n),1,GL_FALSE,glm::value_ptr(m)); }
void  UIShader::setVec4 (const char* n,const glm::vec4& v) const { glUniform4fv(glGetUniformLocation(ID,n),1,glm::value_ptr(v)); }
void  UIShader::setFloat(const char* n,float f)            const { glUniform1f(glGetUniformLocation(ID,n),f); }
void  UIShader::setInt  (const char* n,int i)              const { glUniform1i(glGetUniformLocation(ID,n),i); }

// ── FontAtlas ──────────────────────────────────────────────────────────────
bool FontAtlas::init(const char* fontPath, float pixelHeight){
    // Load font file
    FILE* fp = fopen(fontPath,"rb");
    if(!fp){std::cerr<<"[Font] cannot open "<<fontPath<<"\n";return false;}
    fseek(fp,0,SEEK_END); long sz=ftell(fp); rewind(fp);
    std::vector<unsigned char> buf(sz);
    fread(buf.data(),1,sz,fp); fclose(fp);

    // Bake bitmap
    std::vector<unsigned char> bmp(bitmapW*bitmapH);
    stbtt_bakedchar chars[96];
    stbtt_BakeFontBitmap(buf.data(),0,pixelHeight,bmp.data(),bitmapW,bitmapH,32,96,chars);

    // Store glyph data
    stbtt_fontinfo info;
    stbtt_InitFont(&info,buf.data(),0);
    scale = stbtt_ScaleForPixelHeight(&info,pixelHeight);

    for(int i=0;i<96;++i){
        auto& bc   = chars[i];
        auto& g    = glyphs[32+i];
        g.xoff     = bc.xoff;
        g.yoff     = bc.yoff;
        g.x1       = (float)bc.x1;
        g.y1       = (float)bc.y1;
        g.sx0      = (float)bc.x0/bitmapW;
        g.sy0      = (float)bc.y0/bitmapH;
        g.sx1      = (float)bc.x1/bitmapW;
        g.sy1      = (float)bc.y1/bitmapH;
        g.advance  = bc.xadvance;
    }

    // Upload texture
    glGenTextures(1,&texID);
    glBindTexture(GL_TEXTURE_2D,texID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,bitmapW,bitmapH,0,GL_RED,GL_UNSIGNED_BYTE,bmp.data());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Allow single-channel
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    loaded = true;
    std::cout<<"[Font] loaded "<<fontPath<<" at "<<pixelHeight<<"px\n";
    return true;
}

float FontAtlas::textWidth(const std::string& s, float sz) const {
    if(!loaded) return 0.f;
    float ratio = sz / 48.f; // baked at 48px
    float w = 0;
    for(char c : s){
        if(c<32||c>127) continue;
        w += glyphs[(int)c].advance * ratio;
    }
    return w;
}

// ── UIRenderer ─────────────────────────────────────────────────────────────
bool UIRenderer::init(int sw, int sh){
    W=sw; H=sh;

    if(!uiSh.load("shaders/ui_vert.glsl","shaders/ui_frag.glsl"))   return false;
    if(!textSh.load("shaders/text_vert.glsl","shaders/text_frag.glsl")) return false;

    // Font (baked at 48px, scaled at draw time)
    const char* FONTS[]={
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
    };
    for(auto fp : FONTS) if(font.init(fp,48.f)) break;
    if(!font.loaded) std::cerr<<"[UI] WARNING: no font loaded, text invisible\n";

    // Quad VAO (dynamic, updated per draw)
    glGenVertexArrays(1,&quadVAO); glGenBuffers(1,&quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*24,nullptr,GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));

    // Text VAO (same layout)
    glGenVertexArrays(1,&textVAO); glGenBuffers(1,&textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER,textVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*24,nullptr,GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glBindVertexArray(0);

    resize(sw,sh);
    return true;
}

void UIRenderer::resize(int w, int h){
    W=w; H=h;
    proj = glm::ortho(0.f,(float)w,(float)h,0.f,-1.f,1.f);
}

void UIRenderer::beginFrame(){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void UIRenderer::endFrame(){
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void UIRenderer::drawQuad(float x, float y, float w, float h){
    // 6 vertices, each: px py u v
    float verts[24]={
        x,   y,   0,0,
        x+w, y,   1,0,
        x,   y+h, 0,1,
        x+w, y,   1,0,
        x+w, y+h, 1,1,
        x,   y+h, 0,1,
    };
    glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(verts),verts);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void UIRenderer::rect(float x, float y, float w, float h,
                       glm::vec4 col, float radius, int mode, glm::vec4 col2)
{
    uiSh.use();
    uiSh.setMat4("uProj", proj);
    uiSh.setVec4("uColor",  col);
    uiSh.setVec4("uColor2", col2);
    uiSh.setFloat("uRadius", radius);
    uiSh.setFloat("uTime",   time);
    uiSh.setInt("uMode", mode);
    drawQuad(x,y,w,h);
}

void UIRenderer::text(const std::string& s, float x, float y,
                       float sz, glm::vec4 col,
                       float glow, bool centreX, bool centreY)
{
    if(!font.loaded) return;
    float ratio = sz / 48.f;
    float tw    = font.textWidth(s, sz);
    if(centreX) x -= tw * 0.5f;
    if(centreY) y -= sz * 0.5f;

    textSh.use();
    textSh.setMat4("uProj", proj);
    textSh.setVec4("uColor", col);
    textSh.setFloat("uGlow", glow);
    textSh.setInt("uFontTex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.texID);

    float cx = x;
    for(char ch : s){
        if(ch<32||ch>127){ cx += sz*0.3f; continue; }
        const auto& g = font.glyphs[(int)ch];
        
        // Use STB's x0, y0, x1, y1 from bc but relative to texture coords?
        // Wait, I need the actual glyph width and height in pixels.
        // gw = (bc.x1 - bc.x0)
        // gh = (bc.y1 - bc.y0)
        
        // Since I'm storing sx0, sy0 etc, I need the pixel dimensions.
        // Let's use the width/height calculated from atlas coords.
        float gw = (g.sx1 - g.sx0) * font.bitmapW * ratio;
        float gh = (g.sy1 - g.sy0) * font.bitmapH * ratio;
        
        float gx = cx + g.xoff * ratio;
        float gy = y  + g.yoff * ratio;

        float verts[24]={
            gx,    gy,    g.sx0, g.sy0,
            gx+gw, gy,    g.sx1, g.sy0,
            gx,    gy+gh, g.sx0, g.sy1,
            gx+gw, gy,    g.sx1, g.sy0,
            gx+gw, gy+gh, g.sx1, g.sy1,
            gx,    gy+gh, g.sx0, g.sy1,
        };
        glBindBuffer(GL_ARRAY_BUFFER,textVBO);
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(verts),verts);
        glBindVertexArray(textVAO);
        glDrawArrays(GL_TRIANGLES,0,6);

        cx += g.advance * ratio;
    }
}
