#include <iostream>
#include <2D-piirto.h>

const std::string vs = R"(
#version 430 core

const vec2 verteksit[6] = vec2[6](
  vec2(-1, -1),
  vec2(-1,  1),
  vec2( 1, -1),

  vec2(-1,  1),
  vec2( 1, -1),
  vec2( 1,  1)
);

const vec2 UVt[6] = vec2[6] (
  vec2(0, 0),
  vec2(0, 1),
  vec2(1, 0),

  vec2(0, 1),
  vec2(1, 0),
  vec2(1, 1)
);

out vec2 uv;

void main(void) {
   vec2 v = verteksit[gl_VertexID];
   uv = UVt[gl_VertexID];
   gl_Position = vec4(v.x, v.y, 1, 1);
}
)";

const std::string fs = R"(
  #version 430 core

  layout(std140, binding = 0) uniform uBlock {
    float konvoluutiomatriisi[9];
    vec4 tekseliOffsetit[9];
  } uniformData;

  in vec2 uv;
  uniform sampler2D tekstuuri0;
  out vec4 color;

  void main(void) {
    vec2 t = texture(tekstuuri0, uv).rg;
    vec2 v = vec2(0,0);
    for(int i=0;i<9; i++) {
      vec2 tekseliOffset = uniformData.tekseliOffsetit[i].xy;
      vec2 rg = texture(tekstuuri0, uv + tekseliOffset).rg;
      rg *= uniformData.konvoluutiomatriisi[i];
      v += rg;
    }
    color = vec4(v.r, v.g, 0, 1);
  }
)";

class KonvoluutioUniformData : public UniformData {
  std::vector<float> konvoluutiomatriisi {};
  std::vector<glm::vec4> tekseliOffsetit {};

  void laskeTekseliOffsetit(int w = 256, int h = 256) {
    tekseliOffsetit.resize(9);
    float x_skaala {1.0f / (float)w};
    float y_skaala {1.0f / (float)h};
    for(int i=0; i<9; i++) {
      float y = y_skaala * (float)(i / 3 - 1);
      float x = x_skaala * (float)(i % 3 - 1);
      tekseliOffsetit[i] = glm::vec4(x, y, 0, 0);
    }
  }
public:
  KonvoluutioUniformData() {
    konvoluutiomatriisi = std::vector<float> {
      0.1f,   0.2f,   0.0f,
      0.2f,   -0.2f,  0.2f,
      0.2f,   0.2f,   0.1f
    };
    laskeTekseliOffsetit();
    maaritteleJasen(konvoluutiomatriisi);
    maaritteleJasen(tekseliOffsetit);
    muodostaData();
  }
};

int main(){
  Ikkuna i;
  Shader s = i.luoShader(vs, fs);
  i.kaytaShaderia(s);
  Kuva k1 = i.generoiKuva();
  Kuva k2 = i.generoiKuva();
  KonvoluutioUniformData uData;
  i.maaritteleUniform(uData);
  while(i.onAuki()) {
    i.piirraKuvaan(k2, k1);
    i.piirraKuva(k1);
    i.piirraKuvaan(k1, k2);
    i.paivita();
  }
}
