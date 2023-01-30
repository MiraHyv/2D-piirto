#include "2D-piirto.h"
#include <iostream>

std::vector<float> Ikkuna::luoKoekuva(int leveys, int korkeus) const {
  int koko = leveys * korkeus * 4;
  std::vector<float> tulos(koko);
  const glm::vec4 varit[16] {
    glm::vec4(0.8, 0.0, 0.0, 1), //r
    glm::vec4(0.0, 0.8, 0.0, 1), //g
    glm::vec4(0.0, 0.0, 0.8, 1), //b
    glm::vec4(0.0, 0.8, 0.8, 1), //c
    glm::vec4(0.8, 0.0, 0.8, 1), //m
    glm::vec4(0.8, 0.8, 0.0, 1), //y
    glm::vec4(0.8, 0.8, 0.8, 1), //harmaa
    glm::vec4(0.5, 0.5, 0.5, 1), //harmaa
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


Piirto::Piirto(const GLrajapinta& gl) : GL(gl) {
  GL.aloita();
}

Piirto::~Piirto() {
  GL.lopeta();
}

void Ikkuna::paivitaTapahtumat() {
  GL.paivitaTapahtumat();
  if(GL.ikkunaHalutaanSulkea(ikkuna_p) == true) {
    tapahtumat["suljetaan"] = true;
  }
}

Ikkuna::Ikkuna(const GLrajapinta& gl) : GL(gl) {
  static std::weak_ptr<Piirto> piirto_weak {};
  if(piirto_weak.expired()) {
    piirto_shared = std::make_shared<Piirto>(GL);
    piirto_weak = std::weak_ptr<Piirto>(piirto_shared);
  }
  else {
    piirto_shared = std::shared_ptr(piirto_weak);
  }
  shaderTaulukko.emplace(EI_SHADERIA, HUONO_TUNNUS);
  kuvaTaulukko.emplace(EI_KUVAA, HUONO_TUNNUS);
  ikkuna_p = GL.luoIkkuna(IKKUNAN_LEVEYS, IKKUNAN_KORKEUS);
  GL.kaytaKontekstia(ikkuna_p);
  tapahtumat.emplace("suljetaan", false);
  oletusShader = luoShader(OLETUS_VSRC, OLETUS_FSRC);
  kaytaShaderia(oletusShader);
  vao = GL.luoVAO();
  GL.kaytaVAOa(vao);
  //ubo = GL.luoUniformBuffer(UniformData().datanKoko(), UniformData().haeLiitoskohta());
  fbo = GL.luoFBO();
  //GL.kaytaUniformBufferia(ubo);
  GL.irrotaVAO();
  GL.irrotaKonteksti();
}

Ikkuna::~Ikkuna() {
  for(auto& s : shaderTaulukko) {
    GL.tuhoaShader(s.second);
  }
  for(auto& k : kuvaTaulukko) {
    GL.tuhoaTekstuuri(k.second);
  }
  GL.tuhoaFBO(fbo);
  GL.tuhoaVAO(vao);
  GL.tuhoaBuffer(ubo);
  GL.tuhoaIkkuna(ikkuna_p);
}

bool Ikkuna::onAuki() const {
  if(tapahtumat.at("suljetaan") == true) return false;
  else return true;
}

void Ikkuna::paivita() {
  this->paivitaTapahtumat();
  if(this->onAuki()) {
    GL.kaytaKontekstia(ikkuna_p);
    GL.paivitaIkkuna(ikkuna_p);
    GL.irrotaKonteksti();
  }
}

void Ikkuna::piirra() const {
  GL.kaytaKontekstia(ikkuna_p);
  GL.kaytaVAOa(vao);
  GL.kaytaUniformBufferia(ubo);
  GL.piirraKolmioina(6);
  GL.irrotaVAO();
  GL.irrotaKonteksti();
}

Shader Ikkuna::luoShader(std::string vsrc, std::string fsrc) {
  static int i = 1;
  std::string uusiNimi = "shader" + std::to_string(i);
  i++;
  GL.kaytaKontekstia(ikkuna_p);
  int shaderinTunnus = GL.luoShader(vsrc, fsrc);
  GL.kaytaShaderia(shaderinTunnus);
  GL.asetaTekstuurienNimet(shaderinTunnus);
  GL.kaytaShaderia(shaderTaulukko.at(kaytossaOlevaShader));
  Shader tulos {uusiNimi};
  shaderTaulukko.emplace(tulos, shaderinTunnus);
  return tulos;
}

void Ikkuna::kaytaShaderia(Shader s) {
  GL.kaytaShaderia(shaderTaulukko.at(s) );
  kaytossaOlevaShader = s;
}

void Ikkuna::asetaUniform(const UniformData& uData) const {
  GL.kaytaKontekstia(ikkuna_p);
  GL.kaytaVAOa(vao);
  GL.kaytaUniformBufferia(ubo);
  GL.asetaUniform(uData);
  GL.irrotaUniformBuffer();
}

Kuva Ikkuna::generoiKuva() {
  static int i = 1;
  std::string uusiNimi = "kuva" + std::to_string(i);
  i++;
  GL.kaytaKontekstia(ikkuna_p);
  int tekstuurinTunnus = GL.luoTekstuuri();
  GL.kaytaTekstuuria(tekstuurinTunnus, 0);
  std::vector<float> tPikselit {luoKoekuva(TEKSTUURIN_KOKO, TEKSTUURIN_KOKO)};
  GL.asetaTekstuuri(TEKSTUURIN_KOKO, TEKSTUURIN_KOKO, &(tPikselit[0]));
  Kuva tulos {uusiNimi};
  kuvaTaulukko.emplace(tulos, tekstuurinTunnus);
  return tulos;
}

void Ikkuna::piirraKuva(Kuva k) const {
  GL.kaytaKontekstia(ikkuna_p);
  GL.kaytaTekstuuria(kuvaTaulukko.at(k));
  piirra();
}

void Ikkuna::piirraKuvaan(Kuva mista, Kuva mihin) {
  GL.kaytaKontekstia(ikkuna_p);
  GL.kaytaVAOa(vao);
  GL.kaytaFBOa(fbo);
  GL.kaytaUniformBufferia(ubo);
  GL.liitaTekstuuriFBOon(
    kuvaTaulukko.at(mihin),
    fbo,
    TEKSTUURIN_KOKO,
    TEKSTUURIN_KOKO
  );
  GL.asetaNakyma(0, 0, TEKSTUURIN_KOKO, TEKSTUURIN_KOKO);
  GL.kaytaTekstuuria(kuvaTaulukko.at(mista));
  GL.piirraKolmioina(6);
  GL.kaytaFBOa(0);
  GL.asetaNakyma(0, 0, IKKUNAN_LEVEYS, IKKUNAN_KORKEUS);

  GL.irrotaVAO();
  GL.irrotaKonteksti();
}

void Ikkuna::maaritteleUniform(UniformData& uData) {
  GL.kaytaKontekstia(ikkuna_p);
  GL.kaytaVAOa(vao);
  if(ubo != HUONO_TUNNUS) {
    GL.tuhoaBuffer(ubo);
    ubo = HUONO_TUNNUS;
  }
  ubo = GL.luoUniformBuffer(uData.datanKoko(), uData.haeLiitoskohta());
  GL.kaytaUniformBufferia(ubo);
  GL.asetaUniform(uData);
  GL.irrotaUniformBuffer();
  GL.irrotaVAO();
  GL.irrotaKonteksti();
}
