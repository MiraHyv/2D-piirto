#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <GLrajapinta.h>
#include <iostream>
#include <string>

using ::testing::_;

const uint AIKARAJA {100};

const std::string VSRC = R"(
#version 430 core

const vec2 verteksit[3] = vec2[3](
   vec2(-1, -1),
   vec2( 1, -1),
   vec2( 0, 1)
);

void main(void) {
   vec2 v = verteksit[gl_VertexID];
   gl_Position = vec4(v.x, v.y, 1, 1);
}
)";

const std::string FSRC = R"(
#version 430 core

layout(std140, binding = 0) uniform uBlock {
  float arvot[3];
} uniformData;

out vec3 color;

void main(void) {
  color = vec3(uniformData.arvot[0], uniformData.arvot[1], uniformData.arvot[2]);
}
)";

const std::string VSRC_KUVA = R"(
#version 430 core

const vec2 verteksit[6] = vec2[6](
  vec2(-0.8, -0.8),
  vec2(-0.8,  0.8),
  vec2( 0.8, -0.8),

  vec2(-0.8,  0.8),
  vec2( 0.8, -0.8),
  vec2( 0.8,  0.8)
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

/* piirrä kuva */
const std::string FSRC_KUVA = R"(
#version 430 core

layout(std140, binding = 0) uniform uBlock {
  float arvot[3];
} uniformData;

in vec2 uv;
uniform sampler2D tekstuuri0;
out vec4 color;

void main(void) {
  color = texture(tekstuuri0, uv);
}
)";

/* värin asettaminen glm::vec4:nä */
const std::string FSRC_VEKTORIN_ASETTAMINEN = R"(
  #version 430 core

  layout(std140, binding = 0) uniform uBlock {
    vec4 vari;
  } uniformData;

  out vec3 color;

  void main(void) {
    color = uniformData.vari.rgb;
  }
)";

/* värin asettaminen std::vector<glm::vec4>:nä */
const std::string FSRC_VEKTORIARRAYN_ASETTAMINEN = R"(
  #version 430 core

  layout(std140, binding = 0) uniform uBlock {
    vec4 vari[2];
  } uniformData;

  out vec3 color;

  void main(void) {
    float x = gl_FragCoord.x / 256.0f;
    vec3 c1 = uniformData.vari[0].rgb;
    vec3 c2 = uniformData.vari[1].rgb;
    color = mix(c1, c2, x);
  }
)";

class UniformData_3float : public UniformData{
public:
  void aseta(const std::array<float, 3>& A) {
    asetaFloatArray(0, std::vector<float>(A.begin(), A.end() ));
    muodostaData();
  }

  UniformData_3float() {
    maaritteleJasen(std::vector<float>(3) );
    aseta({1, 0, 0});
  }
};

class UniformData_vektorinAsettaminen : public UniformData{
public:
  UniformData_vektorinAsettaminen() {
    maaritteleJasen(glm::vec4{0.7f, 0.7f, 0.0f, 1.0f} );
    muodostaData();
  }
};

class UniformData_vektoriArraynAsettaminen : public UniformData{
public:
  UniformData_vektoriArraynAsettaminen() {
    maaritteleJasen(std::vector<glm::vec4> {
        glm::vec4(0.9f, 0.9f, 0.9f, 1.0f),
        glm::vec4(0.4f, 0.1f, 0.0f, 1.0f)
      }
    );
    muodostaData();
  }
};

std::vector<float> luoKoekuva(int leveys, int korkeus) {
  int koko = leveys * korkeus * 4;
  std::vector<float> tulos(koko);
  const glm::vec4 varit[16] {
    glm::vec4(0.5, 0.0, 0.0, 1), //r
    glm::vec4(0.0, 0.5, 0.0, 1), //g
    glm::vec4(0.0, 0.0, 0.5, 1), //b
    glm::vec4(0.0, 0.5, 0.5, 1), //c
    glm::vec4(0.5, 0.0, 0.5, 1), //m
    glm::vec4(0.5, 0.5, 0.0, 1), //y
    glm::vec4(0.5, 0.5, 0.5, 1), //harmaa
    glm::vec4(0.2, 0.2, 0.2, 1), //harmaa
    glm::vec4(1.0, 0.0, 0.0, 1), //r
    glm::vec4(0.0, 1.0, 0.0, 1), //g
    glm::vec4(0.0, 0.0, 1.0, 1), //b
    glm::vec4(0.0, 1.0, 1.0, 1), //c
    glm::vec4(1.0, 0.0, 1.0, 1), //m
    glm::vec4(1.0, 1.0, 0.0, 1), //y
    glm::vec4(1.0, 1.0, 1.0, 1), //valkoinen
    glm::vec4(0.0, 0.0, 0.0, 1) //musta
  };
  for(int y=0; y<korkeus; y++) {
    for(int x=0; x<leveys; x++) {
      int i = 4*(y*leveys + x);
      int vari_i = (int)(8*(float)x/leveys) + 8*(int)(2*(float)y/korkeus);
      tulos[i] = varit[vari_i].r;
      tulos[i+1] = varit[vari_i].g;
      tulos[i+2] = varit[vari_i].b;
      tulos[i+3] = varit[vari_i].a;
    }
  }
  return tulos;
}

class GLrajapintatestit : public ::testing::Test {
public:
  IkkunanOsoite mIkkuna {};
  GLuint mShader {0};
  GLuint mVao {0};
  GLuint mUbo {0};
  GLrajapinta GL;

  void aloita(std::string vs = VSRC, std::string fs = FSRC) {
    ASSERT_EQ(mIkkuna, IkkunanOsoite{} );
    ASSERT_EQ(mVao, 0);
    ASSERT_EQ(mShader, 0);
    ASSERT_EQ(mUbo, 0);
    GL.aloita();
    mIkkuna = GL.luoIkkuna(300, 200);
    mShader = GL.luoShader(vs, fs);
    mVao = GL.luoVAO();
  }

  void maaritteleUBO(const UniformData& uData) {
    GL.kaytaKontekstia(mIkkuna);
    GL.kaytaShaderia(mShader);
    GL.kaytaVAOa(mVao);
    mUbo = GL.luoUniformBuffer(uData.datanKoko(), uData.haeLiitoskohta() );
    GL.asetaUniform(uData);
  }

  void piirra(const UniformData& uData) {
    GL.kaytaKontekstia(mIkkuna);
    GL.kaytaVAOa(mVao);
    GL.kaytaUniformBufferia(mUbo);
    GL.asetaUniform(uData);
    GL.piirraKolmioina(3);
//    GL.irrotaUniformBuffer();
//    GL.irrotaVAO();
//    GL.irrotaKonteksti();
  }

  void paivita() {
    GL.kaytaKontekstia(mIkkuna);
    GL.paivitaIkkuna(mIkkuna);
    GL.paivitaTapahtumat();
//    GL.irrotaKonteksti();
  }

  void lopeta() {
    GL.tuhoaShader(mShader);
    mShader = 0;
    GL.tuhoaVAO(mVao);
    mVao = 0;
    GL.tuhoaBuffer(mUbo);
    mUbo = 0;
    GL.tuhoaIkkuna(mIkkuna);
    GL.lopeta();
  }

  GLuint luoTekstuuriKoekuvana() {
    GLuint tekstuuri {GL.luoTekstuuri()};
    GL.kaytaTekstuuria(tekstuuri);
    std::vector<float> pikselit {luoKoekuva(100, 100) };
    GL.asetaTekstuuri(100, 100, &(pikselit[0]) );
    return tekstuuri;
  }

  ~GLrajapintatestit() {
    GL.lopeta();
  }
};

TEST(perus, floatinKokoOn4) {
  ASSERT_EQ(sizeof(float), 4);
}

//TODO: siirrä tämä järkevään paikkaan
TEST_F(GLrajapintatestit, tuhoaminen_ikkunanOsoiteNollataan) {
  IkkunanOsoite w {};
  GL.aloita();
  w = GL.luoIkkuna(300, 200);
  GL.kaytaKontekstia(w);
  GL.irrotaKonteksti();
  GL.tuhoaIkkuna(w);
  ASSERT_EQ(w, IkkunanOsoite {});
}

TEST_F(GLrajapintatestit, ikkunanLuominenJaKayttoonottoEiTuotaVikaa) {
  GL.aloita();
  ASSERT_NO_THROW(GL.luoIkkuna(300, 200) );
}

TEST_F(GLrajapintatestit, ikkunanLuominenEnnenAloittamistaTuottaaVian) {
  ASSERT_THROW(GL.luoIkkuna(300, 200), Piirtovika);
}

TEST_F(GLrajapintatestit, ikkunanLuominenOttaaKontekstinKayttoon) {
  GL.aloita();
  IkkunanOsoite w {GL.luoIkkuna(300, 200) };
  // onko luodun ikkunan osoite sama kuin käytössä olevan kontekstin osoite:
  GLFWwindow* haettuOsoite {w.hae() };
  ASSERT_EQ(glfwGetCurrentContext(), haettuOsoite);
}

TEST_F(GLrajapintatestit, luoShaderPalauttaaMuunKuinNollan) {
  GL.aloita();
  GL.luoIkkuna(300, 200);
  ASSERT_NE(GL.luoShader(VSRC, FSRC), 0);
}

TEST_F(GLrajapintatestit, luoShaderHuonollaLahdekoodillaTuottaaVian) {
  GL.aloita();
  GL.luoIkkuna(300, 200);
  ASSERT_THROW(GL.luoShader("huono shader", "huono shader"), Piirtovika);
  ASSERT_THROW(GL.luoShader("huono shader", FSRC), Piirtovika);
  ASSERT_THROW(GL.luoShader(VSRC, "huono shader"), Piirtovika);
}

TEST_F(GLrajapintatestit, shaderinLuominenOttaaShaderinKayttoon) {
  GL.aloita();
  GL.luoIkkuna(300, 200);
  int shader {GL.luoShader(VSRC, FSRC) };
  // onko luodun shaderin tunnus sama kuin käytössä olevan shaderin tunnus:
  int haettuShader {0};
  glGetIntegerv(GL_CURRENT_PROGRAM, &haettuShader);
  ASSERT_EQ(shader, haettuShader);
}

TEST_F(GLrajapintatestit, luoVaoPalauttaaMuunKuinNollan) {
  GL.aloita();
  GL.luoIkkuna(300, 200);
  ASSERT_NE(GL.luoVAO(), 0);
}

TEST_F(GLrajapintatestit, luoVaoOttaaVAOnKayttoon) {
  GL.aloita();
  GL.luoIkkuna(300, 200);
  uint vao {GL.luoVAO() };
  // onko luodun VAOn tunnus sama kuin vertex arrayhyn liitetyn VAOn tunnus:
  int haettuVao {0};
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &haettuVao);
  ASSERT_EQ(vao, haettuVao);
}

TEST_F(GLrajapintatestit, luoUniformBufferEiTuotaVikaa) {
  GL.aloita();
  IkkunanOsoite w {GL.luoIkkuna(300, 200) };
  UniformData uData {};
  ASSERT_NO_THROW(
    GL.luoUniformBuffer(uData.datanKoko(), uData.haeLiitoskohta() )
  );
}

TEST_F(GLrajapintatestit, luoUniformBufferPalauttaaMuunKuinNollan) {
  GL.aloita();
  GL.luoIkkuna(300, 200);
  UniformData uData {};
  uint ubo {GL.luoUniformBuffer(uData.datanKoko(), uData.haeLiitoskohta() ) };
  ASSERT_NE(ubo, 0);
}

TEST_F(GLrajapintatestit, luoUniformBufferOttaaUBOnKayttoon) {
  GL.aloita();
  GL.luoIkkuna(300, 200);
  UniformData uData {};
  uint ubo {GL.luoUniformBuffer(uData.datanKoko(), uData.haeLiitoskohta() ) };
  int haettuUbo {0};
  glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &haettuUbo);
  ASSERT_EQ(ubo, haettuUbo);
}

TEST_F(GLrajapintatestit, haeKaytossaOlevatAsiat) {
  aloita();
  UniformData uData {};
  maaritteleUBO(uData);
  ASSERT_EQ(GL.haeKaytossaOlevaKonteksti(), mIkkuna);
  ASSERT_EQ(GL.haeKaytossaOlevaShader(), mShader);
  ASSERT_EQ(GL.haeKaytossaOlevaVAO(), mVao);
  ASSERT_EQ(GL.haeKaytossaOlevaUniformBuffer(), mUbo);
}

//TODO: testattavia asioita:
//  tuhoaminen
//  oikean uniformDatan asettaminen
//  irrottaminen

TEST_F(GLrajapintatestit, piirtaminen) {
  //Testataan että ruudulla näkyy punainen kolmio mustalla taustalla
  const int loppu {AIKARAJA};
  aloita();
  UniformData_3float uData{};
  maaritteleUBO(uData);
  for(int i=0; i < loppu; i++) {
    piirra(uData);
    paivita();
  }
  lopeta();
}

TEST_F(GLrajapintatestit, piirtaminen_tekstuuri) {
//Testataan että ruudulla näkyy testikuvio suorakulmiona
  //pikselidata on muotoa float[] {r, g, b, a, r, g, b, a, ...}
  std::vector<float> tPikselit {luoKoekuva(100, 100) };
  aloita(VSRC_KUVA, FSRC_KUVA);
  GL.asetaTekstuurienNimet(mShader);
  GLuint tekstuuri {GL.luoTekstuuri() };
  GL.kaytaTekstuuria(tekstuuri);
  GL.asetaTekstuuri(100, 100, &(tPikselit[0]) );
  for(int i=0; i<AIKARAJA; i++) {
    GL.piirraKolmioina(6);
    GL.paivitaIkkuna(mIkkuna);
    GL.paivitaTapahtumat();
  }
  lopeta();
}

// Piirtäminen: Kaksi shaderia, UBO ja tekstuuri
//    Testataan että ruudulla näkyy testikuvio ja sen päällä kolmio,
//    jonka väri muuttuu sinisestä magentan kautta punaiseen.
TEST_F(GLrajapintatestit, piirtaminen_tekstuuriJaUBO) {
  UniformData_3float uData;
  std::vector<float> pikselit(luoKoekuva(100, 100));
  //aloita, luo ikkuna ja oletusshader
  aloita();
  //määrittele UBO
  maaritteleUBO(uData);
  //luo shader kuvan piirtämistä varten
  GLint shader_kuva {GL.luoShader(VSRC_KUVA, FSRC_KUVA)};
  GL.asetaTekstuurienNimet(shader_kuva);
  //luo ja määrittele tekstuuri
  GLuint tekstuuri {luoTekstuuriKoekuvana()};
  for(int i=0; i<AIKARAJA; i++) {
    //piirrä kuva
    GL.kaytaShaderia(shader_kuva);
    GL.kaytaTekstuuria(tekstuuri);
    GL.piirraKolmioina(6);
    //aseta uniform data ja piirrä kolmio
    GL.kaytaShaderia(mShader);
    float f = (float)i / (float)AIKARAJA;
    uData.aseta({1.5f * f, 0, 1.5f - 1.5f * f});
    GL.kaytaUniformBufferia(mUbo);
    GL.asetaUniform(uData);
    GL.piirraKolmioina(3);
    //päivitä
    paivita();
  }
  //lopeta
  GL.tuhoaShader(shader_kuva);
  GL.tuhoaTekstuuri(tekstuuri);
  lopeta();
}

// FBO:n luominen ja tuhoaminen ei aiheuta vikaa
TEST_F(GLrajapintatestit, FBOnLuominenJaTuhoaminenEiAiheutaVikaa) {
  aloita();
  GLuint fbo {};
  EXPECT_NO_THROW(fbo = GL.luoFBO() );
  EXPECT_NO_THROW(GL.tuhoaFBO(fbo) );
  lopeta();
}


// Tekstuurin piirtäminen FBO:on ja FBO:n piirtäminen ikkunaan
//    Testataan että ruudulla näkyy testikuvio, jonka päällä on pienempi
//    testikuvio
TEST_F(GLrajapintatestit, FBO_piirtaminenTekstuuriin) {
  aloita(VSRC_KUVA, FSRC_KUVA);
  GLuint tekstuuri1 {luoTekstuuriKoekuvana() };
  GLuint tekstuuri2 {luoTekstuuriKoekuvana() };
  GLuint fbo {GL.luoFBO()};
  for(int i=0; i<AIKARAJA; i++) {

    //kuvan piirtäminen kuvaan
    GL.kaytaFBOa(fbo);
    GL.liitaTekstuuriFBOon(tekstuuri1, fbo, 100,100); //100x100: tekstuurin koko
    GL.asetaNakyma(0, 0, 100, 100);
    GL.kaytaTekstuuria(tekstuuri2);
    GL.piirraKolmioina(6);

    //palauta piirtokohde ja piirtoalue
    GL.kaytaFBOa(0);
    GL.asetaNakyma(0, 0, 300, 200);

    //kuvan piirtäminen ruudulle
    GL.kaytaTekstuuria(tekstuuri1);
    GL.piirraKolmioina(6);

    //ruudun päivittäminen
    GL.paivitaIkkuna(mIkkuna);
    GL.paivitaTapahtumat();
  }
  //lopeta
  GL.tuhoaFBO(fbo);
  GL.tuhoaTekstuuri(tekstuuri2);
  GL.tuhoaTekstuuri(tekstuuri1);
  lopeta();
}

// TODO: Alfasekoitus

//Testataan että ruudulla näkyy tummankeltainen kolmio
TEST_F(GLrajapintatestit, vektorinAsettaminen) {
  const int loppu {AIKARAJA};
  UniformData_vektorinAsettaminen uData{};
  aloita(VSRC, FSRC_VEKTORIN_ASETTAMINEN);
  maaritteleUBO(uData);
  for(int i=0; i<loppu; i++) {
    piirra(uData);
    paivita();
  }
  lopeta();
}

//testataan että ruudulla näkyy kolmio, joka on liukuvärjätty vaakasuunnassa
//    vaaleanharmaasta tummanpunaiseen
TEST_F(GLrajapintatestit, vektoriArraynAsettaminen) {
  const int loppu {AIKARAJA};
  UniformData_vektoriArraynAsettaminen uData{};
  aloita(VSRC, FSRC_VEKTORIARRAYN_ASETTAMINEN);
  maaritteleUBO(uData);
  for(int i=0; i<loppu; i++) {
    piirra(uData);
    paivita();
  }
  lopeta();
}
