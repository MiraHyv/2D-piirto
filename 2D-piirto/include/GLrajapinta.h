#pragma once
#include <UniformData.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept> //std::runtime_error
#include <vector>

const GLuint HUONO_TUNNUS = 0;

class PikseliData{
public:
  int leveys {0};
  int korkeus {0};
  std::vector<float> pikselit;
};

class IkkunanOsoite{
friend class GLrajapinta;
protected:
  GLFWwindow* osoite {nullptr};
public:
  GLFWwindow* hae() const;
  bool operator==(const IkkunanOsoite&) const;
  bool operator!=(const IkkunanOsoite&) const;
};

class Piirtovika : public std::runtime_error {
public:
  Piirtovika(std::string = "Jokin piirtovika");
};

class GLrajapinta{
public:
  virtual void aloita() const;
  virtual void lopeta() const;
  virtual IkkunanOsoite luoIkkuna(
    int leveys,
    int korkeus,
    std::string otsikko = "OpenGL-ohjelma"
  ) const;
  virtual void tuhoaIkkuna(IkkunanOsoite&) const;
  virtual void paivitaIkkuna(const IkkunanOsoite&) const;
  virtual void paivitaTapahtumat() const;
  virtual bool ikkunaHalutaanSulkea(const IkkunanOsoite&) const;
  virtual void kaytaKontekstia(const IkkunanOsoite&) const;
  virtual void irrotaKonteksti() const;
  virtual IkkunanOsoite haeKaytossaOlevaKonteksti() const;
  virtual void piirraKolmioina(int verteksienMaara) const;
  virtual int luoShader(
    const std::string& vsrc,
    const std::string& fsrc
  ) const;
  virtual void kaytaShaderia(GLuint) const;
  virtual int haeKaytossaOlevaShader() const;
  virtual void tuhoaShader(GLuint) const;
  virtual void irrotaShader() const;
  virtual GLuint luoVAO() const;
  virtual void kaytaVAOa(GLuint) const;
  virtual int haeKaytossaOlevaVAO() const;
  virtual void tuhoaVAO(GLuint) const;
  virtual void irrotaVAO() const;
  virtual GLuint luoUniformBuffer(GLuint koko, GLuint liitoskohta) const;
  virtual void kaytaUniformBufferia(GLuint) const;
  //virtual void asetaUniform(GLuint koko, const void* uData) const;
  virtual void asetaUniform(const UniformData& uData) const;
  virtual int haeKaytossaOlevaUniformBuffer() const;
  virtual void tuhoaBuffer(GLuint) const;
  virtual void irrotaUniformBuffer() const;
  virtual GLuint luoTekstuuri() const;
  virtual void kaytaTekstuuria(GLuint tx, GLuint tekstuuripaikka = 0) const;
  virtual void asetaTekstuuri(
    int leveys,
    int korkeus,
    const void* pikselit
  ) const;
  virtual void asetaTekstuuri(const PikseliData& pikselit) const;
  virtual void tuhoaTekstuuri(GLuint) const;
  virtual void asetaTekstuurienNimet(GLuint shader) const;
  virtual GLuint luoFBO() const;
  virtual void kaytaFBOa(GLuint) const;
  virtual void tuhoaFBO(GLuint) const;
  virtual void liitaTekstuuriFBOon(GLuint tekstuuri, GLuint paikka = 0) const;
  virtual void liitaTekstuuriFBOon(
    GLuint tekstuuri,
    GLuint FBO,
    int leveys,
    int korkeus,
    GLuint liitoskohta = 0
  ) const;
  virtual void asetaNakyma(int x, int y, int w, int h) const;
};

const GLrajapinta defaultGL;
