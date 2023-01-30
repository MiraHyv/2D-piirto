#pragma once

#include <GLrajapinta.h>

#include <string>
#include <memory>
#include <map>
#include <vector>

const std::string OLETUS_VSRC = R"(
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

const std::string OLETUS_FSRC =  R"(
#version 430 core

layout(std140, binding = 0) uniform uBlock {
  float r;
  float g;
  float b;
} uniformData;

in vec2 uv;
uniform sampler2D tekstuuri0;
out vec4 color;

void main(void) {
  color = texture(tekstuuri0, uv);
}
)";


typedef std::string Shader;
typedef std::string Kuva;

const Shader EI_SHADERIA {""};
const Kuva EI_KUVAA {""};

const int TEKSTUURIN_KOKO {256};
const int IKKUNAN_LEVEYS {1024};
const int IKKUNAN_KORKEUS {1024};

class Piirto{
friend class Ikkuna;
  const GLrajapinta& GL;
public:
  Piirto(const GLrajapinta& = defaultGL);
  Piirto(const Piirto&) = delete;
  ~Piirto();
};


class Ikkuna{
  std::shared_ptr<Piirto> piirto_shared {};
  const GLrajapinta& GL;
  IkkunanOsoite ikkuna_p {};
  std::map<std::string, bool> tapahtumat;
  std::map<Shader, int> shaderTaulukko;
  std::map<Kuva, int> kuvaTaulukko;
  std::vector<float> luoKoekuva(int, int) const;
  void paivitaTapahtumat();
  int vao {HUONO_TUNNUS};
  int ubo {HUONO_TUNNUS};
  int fbo {HUONO_TUNNUS};
  Shader oletusShader {};
  Shader kaytossaOlevaShader {EI_SHADERIA};
public:
  Ikkuna(const GLrajapinta& gl = defaultGL);
  Ikkuna(const Ikkuna&) = delete;
  ~Ikkuna();
  bool onAuki() const;
  void paivita();
  void piirra() const;
  Shader luoShader(
    std::string vsrc = OLETUS_VSRC,
    std::string frsc = OLETUS_FSRC
  );
  void kaytaShaderia(Shader);
  void asetaUniform(const UniformData&) const;
  Kuva generoiKuva();
  void piirraKuva(Kuva) const;
  void piirraKuvaan(Kuva mista, Kuva mihin);
  void maaritteleUniform(UniformData&);
};
