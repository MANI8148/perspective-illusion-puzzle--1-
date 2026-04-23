#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

// Forward declare shader
struct UIShader {
    unsigned ID = 0;
    void use() const;
    void setMat4(const char* n, const glm::mat4& m) const;
    void setVec4(const char* n, const glm::vec4& v) const;
    void setFloat(const char* n, float f) const;
    void setInt(const char* n, int i) const;
    bool load(const char* vs, const char* fs);
};

// Font/text atlas
struct FontAtlas {
    unsigned texID  = 0;
    int      bitmapW= 512, bitmapH = 512;
    float    scale  = 0.f;
    struct Glyph { float xoff, yoff, x1, y1, sx0, sy0, sx1, sy1, advance; };
    Glyph    glyphs[128] {};
    bool     loaded = false;

    bool init(const char* fontPath, float pixelHeight);
    float textWidth(const std::string& s, float sz) const;
};

// ── UIRenderer ─────────────────────────────────────────────────────────────
class UIRenderer {
public:
    bool init(int screenW, int screenH);
    void resize(int w, int h);
    void beginFrame();
    void endFrame();

    // 2D primitives
    void rect(float x, float y, float w, float h,
              glm::vec4 col,
              float radius=0.f, int mode=0,
              glm::vec4 col2=glm::vec4(1));

    void text(const std::string& s, float x, float y,
              float sz, glm::vec4 col, float glow=0.f,
              bool centreX=false, bool centreY=false);

    float textWidth(const std::string& s, float sz) const {
        return font.textWidth(s, sz);
    }

    // Hit test (mouse over rect)
    static bool hit(float mx, float my,
                    float rx, float ry, float rw, float rh){
        return mx>=rx && mx<=rx+rw && my>=ry && my<=ry+rh;
    }

    int  W=0, H=0;
    float time=0.f;
    FontAtlas font;

private:
    UIShader  uiSh, textSh;
    unsigned  quadVAO=0, quadVBO=0;
    unsigned  textVAO=0, textVBO=0;
    glm::mat4 proj {};

    void drawQuad(float x,float y,float w,float h);
};
