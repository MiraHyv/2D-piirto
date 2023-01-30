#include <GLrajapinta.h>
#include <exception>
#include <iostream>
#include <vector>
#include <assert.h>

bool IkkunanOsoite::operator==(const IkkunanOsoite& toinen) const {
  if(this->osoite == toinen.osoite) return true;
  else return false;
}

bool IkkunanOsoite::operator!=(const IkkunanOsoite& toinen) const {
  if(this->osoite != toinen.osoite) return true;
  else return false;
}

GLFWwindow* IkkunanOsoite::hae() const {
  return osoite;
}

Piirtovika::Piirtovika(std::string s) : std::runtime_error(s) {}

void GLrajapinta::aloita() const {
  glewExperimental = true;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  if(glfwInit() != GLFW_TRUE) {
    throw Piirtovika("glfwInit epäonnistui");
  }
}

void GLrajapinta::lopeta() const {
  glfwTerminate();
}

IkkunanOsoite GLrajapinta::luoIkkuna(int w, int h, std::string otsikko) const {
  IkkunanOsoite p;
  p.osoite = glfwCreateWindow(w, h, otsikko.c_str(), nullptr, nullptr);
  if(p.osoite == nullptr) {
    throw Piirtovika("Ikkunaa ei voitu luoda: ("
      + std::to_string(w)+", "+std::to_string(h)+", \""+otsikko+"\")");
  }
  //TODO: älä käytä GLrajapinnan funktioita GLrajapinnan funtkioiden sisällä.
  // Sen sijaan: glfwMakeContextCurrent(ikkuna_p.hae());
  kaytaKontekstia(p);
  if(haeKaytossaOlevaKonteksti() != p) {
    throw Piirtovika("Kontekstin käyttöönotto epäonnistui");
  }
  if(glewInit() != GLEW_OK) {
    throw Piirtovika("glewInit epäonnistui");
  }
  return p;
}

void GLrajapinta::tuhoaIkkuna(IkkunanOsoite& ikkuna_p) const {
  assert(ikkuna_p.osoite != nullptr);
  glfwDestroyWindow(ikkuna_p.osoite);
  ikkuna_p.osoite = nullptr;
}

void GLrajapinta::paivitaIkkuna(const IkkunanOsoite& ikkuna_p) const {
  assert(ikkuna_p.osoite != nullptr);
  glfwSwapBuffers(ikkuna_p.hae() );
}

void GLrajapinta::paivitaTapahtumat() const {
  glfwPollEvents();
}

bool GLrajapinta::ikkunaHalutaanSulkea(const IkkunanOsoite& ikkuna_p) const {
  assert(ikkuna_p.osoite != nullptr);
  if(glfwWindowShouldClose(ikkuna_p.osoite) == GLFW_TRUE) return true;
  else return false;
}

void GLrajapinta::kaytaKontekstia(const IkkunanOsoite& ikkuna_p) const {
  assert(ikkuna_p.hae() != nullptr);
  glfwMakeContextCurrent(ikkuna_p.hae());
}

IkkunanOsoite GLrajapinta::haeKaytossaOlevaKonteksti() const {
  IkkunanOsoite p {};
  p.osoite = glfwGetCurrentContext();
  return p;
}

void GLrajapinta::irrotaKonteksti() const {
  glfwMakeContextCurrent(nullptr);
}

void GLrajapinta::piirraKolmioina(int verteksienMaara) const {
  glDrawArrays(GL_TRIANGLES, 0, verteksienMaara);
}

//HUOM: luo ja ota käyttöön
int GLrajapinta::luoShader(
  const std::string& vertexShaderSrc,
  const std::string& fragmentShaderSrc
) const {

  //Luodaan ohjelma joka on se mikä lopulta palautetaan shaderin tunnuksena
  GLuint tunnus = glCreateProgram();
  if(tunnus == 0) {
    throw Piirtovika("Shader-ohjelmaa ei voitu luoda");
  }

  //Väliaikaiset shaderit joista käännetään shader-ohjelma
  unsigned int vShaderTunnus = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fShaderTunnus = glCreateShader(GL_FRAGMENT_SHADER);

  // annetaan lähdekoodit:
  const char* vsrc_c = vertexShaderSrc.c_str();
  glShaderSource(vShaderTunnus, 1, &vsrc_c, NULL);
  const char* fsrc_c = fragmentShaderSrc.c_str();
  glShaderSource(fShaderTunnus, 1, &fsrc_c, NULL);

  //käännetään vertex shader
  glCompileShader(vShaderTunnus);

  //palaute...
  int kTulos = GL_FALSE;
  int palautteenPituus;
  std::string palaute_str;
  glGetShaderiv(vShaderTunnus, GL_COMPILE_STATUS, &kTulos);
  if(kTulos == GL_FALSE) {
    glGetShaderiv(vShaderTunnus, GL_INFO_LOG_LENGTH, &palautteenPituus);
    if(palautteenPituus > 0) {
        char palaute[palautteenPituus+1];
        glGetShaderInfoLog(
          vShaderTunnus,
          palautteenPituus,
          NULL,
          palaute
        );
        palaute_str = palaute;
    }
    throw Piirtovika("Vertex shaderin kääntäminen epäonnistui:\n" + palaute_str);
  }

  //käännetään fragment shader
  glCompileShader(fShaderTunnus);

  //palaute...
  glGetShaderiv(fShaderTunnus, GL_COMPILE_STATUS, &kTulos);
  if(kTulos == GL_FALSE) {
    glGetShaderiv(fShaderTunnus, GL_INFO_LOG_LENGTH, &palautteenPituus);
    if(palautteenPituus > 0) {
        char palaute[palautteenPituus+1];
        glGetShaderInfoLog(
          fShaderTunnus,
          palautteenPituus,
          NULL,
          palaute
        );
        palaute_str = palaute;
    }
    throw Piirtovika(
      "Fragment shaderin kääntäminen epäonnistui:\n" + palaute_str
    );
  }

  //linkitetään ohjelma
  glAttachShader(tunnus, vShaderTunnus);
  glAttachShader(tunnus, fShaderTunnus);
  glLinkProgram(tunnus);

  //vapautetaan tarpeettomat asiat
  glDetachShader(tunnus, vShaderTunnus);
  glDetachShader(tunnus, fShaderTunnus);
  glDeleteShader(vShaderTunnus);
  glDeleteShader(fShaderTunnus);

  //linkityksen palaute...
  int lTulos;
  glGetProgramiv(tunnus, GL_LINK_STATUS, &lTulos);
  if(lTulos == GL_FALSE) {
    glGetProgramiv(tunnus, GL_INFO_LOG_LENGTH, &palautteenPituus);
    if(palautteenPituus > 0) {
        char palaute[palautteenPituus+1];
        glGetProgramInfoLog(
          tunnus,
          palautteenPituus,
          NULL,
          palaute
        );
        palaute_str = palaute;
    }
    throw Piirtovika("Shaderin linkitys epäonnistui:\n" + palaute_str);
  }

  //kokeillaan ohjelman liittämistä
  //TODO: älä käytä GLrajapinnan funktioita
  kaytaShaderia(tunnus);
  if(haeKaytossaOlevaShader() != tunnus) {
    throw Piirtovika("Shaderin käyttöönotto epäonnistui");
  }
  return tunnus;
}

void GLrajapinta::kaytaShaderia(GLuint shader) const {
  //todo: mitä jos shader ei edusta shaderia? glIsProgram? nopeus?
  glUseProgram(shader);
}

int GLrajapinta::haeKaytossaOlevaShader() const {
  int s {0};
  glGetIntegerv(GL_CURRENT_PROGRAM, &s);
  return s;
}

void GLrajapinta::tuhoaShader(GLuint shader) const {
  //todo: mitä jos shader ei edusta shaderia? glIsProgram
  glDeleteProgram(shader);
}

void GLrajapinta::irrotaShader() const {
  kaytaShaderia(0);
}

GLuint GLrajapinta::luoVAO() const {
  GLuint vao {0};
  glGenVertexArrays(1, &vao);
  if(vao == 0) {
    throw Piirtovika("Ei voitu luoda VAOa");
  }
  //TODO: älä käytä GLrajapinnan funktioita
  kaytaVAOa(vao);
  if(haeKaytossaOlevaVAO() != vao) {
    throw Piirtovika("VAOn liittäminen epäonnistui");
  }
  return vao;
}

void GLrajapinta::kaytaVAOa(GLuint vao) const {
  //todo: mitä jos vao ei edusta VAOa?
  glBindVertexArray(vao);
}

int GLrajapinta::haeKaytossaOlevaVAO() const {
  int vao {0};
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
  return vao;
}

void GLrajapinta::tuhoaVAO(GLuint vao) const {
  //todo: mitä jos vao ei edusta VAOa?
  glDeleteVertexArrays(1, &vao);
}

void GLrajapinta::irrotaVAO() const {
  kaytaVAOa(0);
}

GLuint GLrajapinta::luoUniformBuffer(GLuint koko, GLuint liitoskohta) const {
  GLuint ubo {0};
  glGenBuffers(1, &ubo);
  if (ubo == 0) {
    throw Piirtovika ("Ei voitu luoda UBOa");
  }
  kaytaUniformBufferia(ubo);
  if(haeKaytossaOlevaUniformBuffer() != ubo) {
    throw Piirtovika("UBOn liittäminen epäonnistui");
  }
  glBufferData(GL_UNIFORM_BUFFER, koko, NULL, GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, liitoskohta, ubo);
  return ubo;
}

void GLrajapinta::kaytaUniformBufferia(GLuint ubo) const {
  //todo: mitä jos ubo ei edusta UBOa?
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
}

/*TODO: jos annetaan argumenttina väliaikaismuuttuja, seuraa kaatuminen -
  esim. näin: asetaUniform(UniformData() );
*/
void GLrajapinta::asetaUniform(const UniformData& uData) const {
  glBufferData(
    GL_UNIFORM_BUFFER,
    uData.datanKoko(),
    &(uData.haeData()[0]),
    GL_DYNAMIC_DRAW
  );
}

int GLrajapinta::haeKaytossaOlevaUniformBuffer() const {
  int ubo {0};
  glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &ubo);
  return ubo;
}

void GLrajapinta::tuhoaBuffer(GLuint buffer) const {
  //todo: mitä jos buffer ei edusta bufferia?
  glDeleteBuffers(1, &buffer);
}

void GLrajapinta::irrotaUniformBuffer() const {
  kaytaUniformBufferia(0);
}

GLuint GLrajapinta::luoTekstuuri() const {
  GLuint tekstuuri {0};
  glGenTextures(1, &tekstuuri);
  if(tekstuuri == 0) {
    throw Piirtovika("Ei voitu luoda tekstuuria");
  }
  return tekstuuri;
}

void GLrajapinta::kaytaTekstuuria(GLuint tx, GLuint paikka) const{
  glActiveTexture(GL_TEXTURE0 + paikka);
  glBindTexture(GL_TEXTURE_2D, tx);
}

void GLrajapinta::asetaTekstuuri(
  int leveys,
  int korkeus,
  const void* pikselit
) const {
  const GLint txMuoto {GL_RGBA};
  const GLenum pxMuoto {GL_RGBA};
  const GLenum pxTyyppi {GL_FLOAT};
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    txMuoto,
    leveys,
    korkeus,
    0,
    pxMuoto,
    pxTyyppi,
    pikselit
  );
}

void GLrajapinta::asetaTekstuuri(const PikseliData& pData) const {
  const GLint txMuoto {GL_RGBA};
  const GLenum pxMuoto {GL_RGBA};
  const GLenum pxTyyppi {GL_FLOAT};
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    txMuoto,
    pData.leveys,
    pData.korkeus,
    0,
    pxMuoto,
    pxTyyppi,
    &(pData.pikselit[0])
  );
}

void GLrajapinta::tuhoaTekstuuri(GLuint tx) const {
  glDeleteTextures(1, &tx);
}

void GLrajapinta::asetaTekstuurienNimet(GLuint shader) const {
  kaytaShaderia(shader);
  const std::string perusosa = "tekstuuri";
  const int N {16};
  for(int i=0; i<N; i++) {
    std::string tNimi = perusosa + std::to_string(i);
    glUniform1i(glGetUniformLocation(shader, tNimi.c_str()), i);
  }
}

GLuint GLrajapinta::luoFBO() const {
  GLuint FBO {0};
  glGenFramebuffers(1, &FBO);
  if(FBO == 0) {
    throw Piirtovika("Ei voitu luoda FBOa");
  }
  return FBO;
}

void GLrajapinta::kaytaFBOa(GLuint FBO) const {
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void GLrajapinta::tuhoaFBO(GLuint FBO) const {
  glDeleteFramebuffers(1, &FBO);
}

void GLrajapinta::liitaTekstuuriFBOon(GLuint tekstuuri, GLuint paikka) const {
  const GLenum liitoskohta {GL_COLOR_ATTACHMENT0 + paikka};
  glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    liitoskohta,
    GL_TEXTURE_2D,
    tekstuuri,
    0
  );
}

void GLrajapinta::liitaTekstuuriFBOon(
  GLuint tekstuuri,
  GLuint FBO,
  int leveys,
  int korkeus,
  GLuint liitoskohta
) const {
  kaytaFBOa(FBO);
  asetaNakyma(0, 0, leveys, korkeus);//ei täällä ehkä?
  liitaTekstuuriFBOon(tekstuuri, liitoskohta);
}

void GLrajapinta::asetaNakyma(int x, int y, int w, int h) const {
  glViewport(x, y, w, h);
}
