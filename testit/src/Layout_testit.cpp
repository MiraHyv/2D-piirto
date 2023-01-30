#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Layout.hpp>
#include <iostream>

using ::testing::_;

class Layout_testit : public ::testing::Test {
public:
  float teeFloat(float f) {
    return f;
  }

  float haeFloatMuistipaikasta(std::vector<char> D, uint paikka) {
    assert(!D.empty());
    assert(paikka <= D.size() - sizeof(float));
    return *(float*)(&D[paikka]);
  }
};

TEST_F(Layout_testit, yksiFloat) {
  std::vector<LayoutinElementti> muoto {
    {TYYPPI_FLOAT, 1}
  };
  Layout L(muoto);
  std::string oikeaTulos {"[0, 3]: float\n"};
  ASSERT_EQ(L.haeMuotoStringina(), oikeaTulos);
}

TEST_F(Layout_testit, kaksiFloattia) {
  std::vector<LayoutinElementti> muoto {
    {TYYPPI_FLOAT, 1},
    {TYYPPI_FLOAT, 1}
  };
  Layout L(muoto);
  std::string oikeaTulos {
R"([0, 3]: float
[4, 7]: float
)"};
  ASSERT_EQ(L.haeMuotoStringina(), oikeaTulos);
}

TEST_F(Layout_testit, kaksiFloattiaKerralla) {
  std::vector<LayoutinElementti> muoto {
    {TYYPPI_FLOAT, 2}
  };
  Layout L(muoto);
  std::string oikeaTulos {
R"([0, 3]: float
[4, 7]: float
)"};
  ASSERT_EQ(L.haeMuotoStringina(), oikeaTulos);
}

TEST_F(Layout_testit, floatArray) {
  std::vector<LayoutinElementti> muoto {
    {TYYPPI_ARRAY_FLOAT, 3}
  };
  Layout L(muoto);
  std::string oikeaTulos {
R"([0, 15]: float array [0]
[16, 31]: float array [1]
[32, 47]: float array [2]
)"};
  ASSERT_EQ(L.haeMuotoStringina(), oikeaTulos);
}

TEST_F(Layout_testit, kaksiFloattiaJaFloatArray) {
  std::vector<LayoutinElementti> muoto {
    {TYYPPI_FLOAT, 2},
    {TYYPPI_ARRAY_FLOAT, 3}
  };
  Layout L(muoto);
  std::string oikeaTulos {
R"([0, 3]: float
[4, 7]: float
[16, 31]: float array [0]
[32, 47]: float array [1]
[48, 63]: float array [2]
)"};
  ASSERT_EQ(L.haeMuotoStringina(), oikeaTulos);
}

TEST_F(Layout_testit, maaritteleminenFunktiolla) {
  float f1 {0.0};
  float f2 {1.1};
  float f3 {2.2};
  float f4 {5.14573};
  std::vector<float> F {0.1, 1.2, 2.3};
  Layout L;
  L.maaritteleJasen(f1);
  L.maaritteleJasen(f2);
  L.maaritteleJasen(f3);
  L.maaritteleJasen(f4);
  L.maaritteleJasen(F);
  std::string oikeaTulos {
R"([0, 3]: float
[4, 7]: float
[8, 11]: float
[12, 15]: float
[16, 31]: float array [0]
[32, 47]: float array [1]
[48, 63]: float array [2]
)"};
  ASSERT_EQ(L.haeMuotoStringina(), oikeaTulos);
}

TEST_F(Layout_testit, maaritteleminenFunktiollaValiaikaismuuttujin) {
  Layout L;
  L.maaritteleJasen(teeFloat(0.0) );
  L.maaritteleJasen(float {1.1});
  L.maaritteleJasen(std::vector<float> {0.1, 1.2, 2.3});
  std::string oikeaTulos {
R"([0, 3]: float
[4, 7]: float
[16, 31]: float array [0]
[32, 47]: float array [1]
[48, 63]: float array [2]
)"};
  ASSERT_EQ(L.haeMuotoStringina(), oikeaTulos);
}

/* TODO: määrittele kaikki erityyppiset muuttujat samalla funktiolla tyyliin:
  Layout L {float {0.0}, std::vector<float> {1.1, 2.5} };
*/

TEST_F(Layout_testit, haeFloatit) {
  Layout L;
  L.maaritteleJasen(float{1.5});
  L.maaritteleJasen(float{2.9});
  std::vector<float> F {L.haeFloatit() };
  std::vector<float> oikeaVastaus {1.5, 2.9};
  ASSERT_EQ(F, oikeaVastaus);
}

TEST_F(Layout_testit, haeFloatArrayt) {
  Layout L;
  L.maaritteleJasen(std::vector<float> {1.5, 2.9} );
  L.maaritteleJasen(std::vector<float> {2.9, 2.99, 2.999} );
  std::vector< std::vector<float> > oikeaVastaus {
    {1.5, 2.9},
    {2.9, 2.99, 2.999}
  };
  std::vector< std::vector<float> > F {L.haeFloatArrayt() };
  ASSERT_EQ(F, oikeaVastaus);
}

TEST_F(Layout_testit, haeDatanKoko_KunMaariteltyMaaritteleJasen_funktiolla) {
  Layout L;
  L.maaritteleJasen(float {1.5} );
  L.maaritteleJasen(float {2.9} );
  L.maaritteleJasen(std::vector<float> {2.9, 2.99, 2.999} );
  ASSERT_EQ(L.datanKoko(), 64);
  Layout M;
  M.maaritteleJasen(float{1.0});
  M.maaritteleJasen(float{-5.0});
  ASSERT_EQ(M.datanKoko(), 8);
}

TEST_F(Layout_testit, muodostaData) {
  Layout L;
  L.maaritteleJasen(float {0.01f} );
  L.maaritteleJasen(float {1.105f});
  L.maaritteleJasen(std::vector<float> {0.11f, 1.21f, 2.31f});
  L.muodostaData();
  const std::vector<char>& D {L.haeData() };
  ASSERT_EQ(haeFloatMuistipaikasta(D, 0), 0.01f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 4), 1.105f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 16), 0.11f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 32), 1.21f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 48), 2.31f);
}

TEST_F(Layout_testit, asetaFloat) {
  Layout L;
  L.maaritteleJasen(float{});
  L.maaritteleJasen(float{});
  L.asetaFloat(1, 0.1f);
  ASSERT_EQ(L.haeFloatit()[1], 0.1f);
}

TEST_F(Layout_testit, asetaFloatArray) {
  Layout L;
  L.maaritteleJasen(float{});
  L.maaritteleJasen(std::vector<float>(4));
  L.asetaFloatArray(0, std::vector<float> {0.0f, 1.0f, 2.0f, 3.0f} );
  std::vector<float> oikeaVastaus {0.0f, 1.0f, 2.0f, 3.0f};
  ASSERT_EQ(L.haeFloatArrayt()[0], oikeaVastaus);
}

TEST_F(Layout_testit, maarittely_vec4) {
  Layout L;
  L.maaritteleJasen(glm::vec4(1.0f, 0.5f, 0.42f, -1.0f) );
  L.muodostaData();
  const auto& D {L.haeData() };
  ASSERT_EQ(D.size() >= 16, true);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 0), 1.0f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 4), 0.5f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 8), 0.42f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 12), -1.0f);
}

TEST_F(Layout_testit, maarittelyErikseen_vec4) {
  Layout L;
  L.maaritteleJasen(glm::vec4{});
  L.asetaVec4(0, glm::vec4(0.1f, -0.2f, -0.31f, 1.0f) );
  L.muodostaData();
  const auto& D {L.haeData() };
  ASSERT_EQ(D.size() >= 16, true);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 0), 0.1f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 4), -0.2f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 8), -0.31f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 12), 1.0f);
}

TEST_F(Layout_testit, ilmanMitaanMaaritelmia_datanKokoOnNolla) {
  Layout L;
  ASSERT_EQ(L.datanKoko(), 0);
}

TEST_F(Layout_testit, ilmanMitaanMaaritelmia_muodostaDataEiAiheutaKaatumista) {
  Layout L;
  L.muodostaData();
}

TEST_F(Layout_testit, ilmanMitaanMaaritelmia_haeDataPalauttaaTyhjanVektorin) {
  Layout L;
  L.muodostaData();
  const auto& D {L.haeData() };
  ASSERT_EQ(D.empty(), true);
}

TEST_F(Layout_testit, maarittely_vec4array) {
  Layout L;
  L.maaritteleJasen( std::vector<glm::vec4> {
      glm::vec4(0.1f, 0.2f, 0.3f, 0.4f),
      glm::vec4(-0.5f, 0.6f, 0.7f, 0.8f),
      glm::vec4(-1.0f, 2.0f, 3.33f, 4.1f)
    }
  );
  L.muodostaData();
  const auto& D {L.haeData() };
  ASSERT_EQ(D.size(), 48);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 0), 0.1f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 4), 0.2f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 8), 0.3f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 12), 0.4f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 16), -0.5f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 20), 0.6f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 24), 0.7f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 28), 0.8f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 32), -1.0f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 36), 2.0f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 40), 3.33f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 44), 4.1f);
}

TEST_F(Layout_testit, maarittelyErikseen_vec4array) {
  Layout L;
  L.maaritteleJasen(std::vector<glm::vec4>(3) );
  std::vector<glm::vec4> uusiVec4Array {
    glm::vec4(0.1f, 0.2f, 0.3f, 0.4f),
    glm::vec4(-0.5f, 0.6f, 0.7f, 0.8f),
    glm::vec4(-1.0f, 2.0f, 3.33f, 4.1f)
  };
  L.asetaVec4Array(0, uusiVec4Array);
  L.muodostaData();
  const auto& D {L.haeData() };
  ASSERT_EQ(D.size(), 48);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 0), 0.1f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 4), 0.2f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 8), 0.3f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 12), 0.4f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 16), -0.5f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 20), 0.6f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 24), 0.7f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 28), 0.8f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 32), -1.0f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 36), 2.0f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 40), 3.33f);
  ASSERT_EQ(haeFloatMuistipaikasta(D, 44), 4.1f);
}
