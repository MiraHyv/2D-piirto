#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

enum LayoutinTyyppi {
  TYYPPI_FLOAT,
  TYYPPI_ARRAY_FLOAT,
  TYYPPI_VEC4,
  TYYPPI_ARRAY_VEC4
};

typedef std::pair<LayoutinTyyppi, uint> LayoutinElementti;

class Layout {
protected:
  std::vector<char> data;
  std::vector<LayoutinElementti> rakenne;
  std::vector<float> muuttujat_float;
  std::vector<std::vector<float> > muuttujat_array_float;
  std::vector<glm::vec4> muuttujat_vec4;
  std::vector<std::vector<glm::vec4> > muuttujat_array_vec4;
  uint haeAlunMuistipaikka(uint jasen_i, uint alajasen_i = 0) const;
public:
  Layout(const std::vector<LayoutinElementti>& muoto);
  Layout();
  std::string haeMuotoStringina() const;
  void maaritteleJasen(const float&);
  void maaritteleJasen(const std::vector<float>&);
  void maaritteleJasen(const glm::vec4&);
  void maaritteleJasen(const std::vector<glm::vec4>&);
  const std::vector<float>& haeFloatit() const;
  const std::vector<std::vector<float> >& haeFloatArrayt() const;
  uint datanKoko() const;
  std::vector<char> muodostaData();
  void asetaFloat(uint indeksi, float arvo);
  void asetaFloatArray(uint indeksi, std::vector<float> arvo);
  void asetaVec4(uint indeksi, glm::vec4);
  void asetaVec4Array(uint indeksi, std::vector<glm::vec4>);
  const std::vector<char>& haeData() const;
};
