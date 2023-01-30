#pragma once
#include <gmock/gmock.h>
#include <GLrajapinta.h>

struct GLFWwindow{
  int w{1};
};

class fake_IkkunanOsoite : public IkkunanOsoite {
public:
  fake_IkkunanOsoite() {
    static GLFWwindow* uusiOsoite = (GLFWwindow*)1;
    osoite = uusiOsoite++;
  }
};

class mock_GLrajapinta : public GLrajapinta {
public:
  mock_GLrajapinta() {}
  ~mock_GLrajapinta() {}
  MOCK_METHOD(void, aloita, (), (const, override) );
  MOCK_METHOD(void, lopeta, (), (const, override) );
  MOCK_METHOD(
    IkkunanOsoite,
    luoIkkuna,
    (int leveys, int korkeus, std::string otsikko),
    (const, override)
  );
  MOCK_METHOD(void, tuhoaIkkuna, (IkkunanOsoite&), (const, override) );
  MOCK_METHOD(void, paivitaIkkuna, (const IkkunanOsoite&), (const, override) );
  MOCK_METHOD(void, paivitaTapahtumat, (), (const, override) );
  MOCK_METHOD(
    bool,
    ikkunaHalutaanSulkea,
    (const IkkunanOsoite&),
    (const, override)
  );
  MOCK_METHOD(void, kaytaKontekstia, (const IkkunanOsoite&), (const, override));
  MOCK_METHOD(void, irrotaKonteksti, (), (const, override));
  MOCK_METHOD(IkkunanOsoite, haeKaytossaOlevaKonteksti, (), (const, override));
  MOCK_METHOD(void, piirraKolmioina, (int), (const, override) );
  MOCK_METHOD(
    int,
    luoShader,
    (const std::string&, const std::string&),
    (const, override)
  );
  MOCK_METHOD(void, kaytaShaderia, (GLuint), (const, override) );
  MOCK_METHOD(int, haeKaytossaOlevaShader, (), (const, override) );
  MOCK_METHOD(void, tuhoaShader, (GLuint), (const, override) );
  MOCK_METHOD(void, irrotaShader, (), (const, override) );
  MOCK_METHOD(GLuint, luoVAO, (), (const, override) );
  MOCK_METHOD(void, kaytaVAOa, (GLuint), (const, override) );
  MOCK_METHOD(int, haeKaytossaOlevaVAO, (), (const, override) );
  MOCK_METHOD(void, tuhoaVAO, (GLuint), (const, override) );
  MOCK_METHOD(void, irrotaVAO, (), (const, override) );
  MOCK_METHOD(GLuint, luoUniformBuffer, (GLuint, GLuint), (const, override) );
  MOCK_METHOD(void, kaytaUniformBufferia, (GLuint), (const, override) );
  //MOCK_METHOD(void, asetaUniform, (GLuint, const void*), (const, override) );
  MOCK_METHOD(void, asetaUniform, (const UniformData&), (const, override) );
  MOCK_METHOD(int, haeKaytossaOlevaUniformBuffer, (), (const, override) );
  MOCK_METHOD(void, tuhoaBuffer, (GLuint), (const, override) );
  MOCK_METHOD(void, irrotaUniformBuffer, (), (const, override) );
  MOCK_METHOD(GLuint, luoTekstuuri, (), (const, override) );
  MOCK_METHOD(
    void,
    kaytaTekstuuria,
    (GLuint t, GLuint p), 
    (const, override)
  );
  MOCK_METHOD(
    void,
    asetaTekstuuri,
    (int, int, const void*),
    (const, override)
  );
  MOCK_METHOD(void, tuhoaTekstuuri, (GLuint), (const, override) );
  MOCK_METHOD(void, asetaTekstuurienNimet, (GLuint), (const, override) );
  MOCK_METHOD(GLuint, luoFBO, (), (const, override) );
  MOCK_METHOD(void, kaytaFBOa, (GLuint), (const, override) );
  MOCK_METHOD(void, tuhoaFBO, (GLuint), (const, override) );
  MOCK_METHOD(void, liitaTekstuuriFBOon, (GLuint, GLuint), (const, override) );
  MOCK_METHOD(
    void,
    liitaTekstuuriFBOon,
    (GLuint, GLuint, int, int, GLuint),
    (const, override)
  );
  MOCK_METHOD(void, asetaNakyma, (int, int, int, int), (const, override) );
};
