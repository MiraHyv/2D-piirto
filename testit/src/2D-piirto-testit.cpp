#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <mock_GLrajapinta.h>
#include <2D-piirto.h>
#include <iostream>

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Gt;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::AnyNumber;
using ::testing::InSequence;

class Piirtotestit : public ::testing::Test {
public:
  NiceMock<mock_GLrajapinta> GL;
  fake_IkkunanOsoite hyvaIkkuna, hyvaIkkuna2;
  std::string src {"shaderin lähdekoodi"};
};

ACTION(tuotaPiirtovika) {
  throw Piirtovika();
}


// Ikkunoiden luominen


//Kun luodaan 1 ikkuna, aloita() ja lopeta() kutsutaan
TEST_F(Piirtotestit, aloitaJaLopetaKutsutaan) {
  EXPECT_CALL(GL, aloita);
  EXPECT_CALL(GL, lopeta);
  Ikkuna i(GL);
}

//Kun luodaan 2 ikkunaa, aloita() ja lopeta() kutsutaan yhdesti
TEST_F(Piirtotestit, aloitaJaLopetaKutsutaanYhdesti) {
  EXPECT_CALL(GL, aloita).Times(1);
  EXPECT_CALL(GL, lopeta).Times(1);
  Ikkuna i(GL);
  Ikkuna j(GL);
}

//Kun luodaan 1 ikkuna, luoIkkuna() kutsutaan
// niin että ikkunan leveys ja korkeus > 0
TEST_F(Piirtotestit, ikkunaLuodaan) {
  EXPECT_CALL(GL, luoIkkuna(Gt(0), Gt(0), _) );
  Ikkuna i(GL);
}

//Kun luodaan 1 ikkuna, tuhoaIkkuna() kutsutaan oikealla osoitteella
TEST_F(Piirtotestit, ikkunaTuhotaan) {
  EXPECT_CALL(GL, luoIkkuna).WillOnce(Return(hyvaIkkuna));
  EXPECT_CALL(GL, tuhoaIkkuna(hyvaIkkuna));
  Ikkuna i(GL);
}

//Kun luodaan 2 ikkunaa, luoIkkuna() ja tuhoaIkkuna() kutsutaan kahdesti
// oikeilla osoitteilla
TEST_F(Piirtotestit, ikkunatLuodaanJaTuhotaan) {
  EXPECT_CALL(GL, luoIkkuna)
    .WillOnce(Return(hyvaIkkuna))
    .WillOnce(Return(hyvaIkkuna2));
  EXPECT_CALL(GL, tuhoaIkkuna(hyvaIkkuna));
  EXPECT_CALL(GL, tuhoaIkkuna(hyvaIkkuna2));
  Ikkuna i(GL);
  Ikkuna j(GL);
}

//Kun kutsutaan paivita, kutsutaan paivitaTapahtumat
TEST_F(Piirtotestit, paivitaPaivittaaTapahtumat) {
  EXPECT_CALL(GL, paivitaTapahtumat);
  Ikkuna i(GL);
  i.paivita();
}

//Kun kutsutaan paivita, kutsutaan ikkunaHalutaanSulkea oikealla osoitteella
TEST_F(Piirtotestit, paivita_oikeaOsoite) {
  EXPECT_CALL(GL, luoIkkuna).WillOnce(Return(hyvaIkkuna));
  EXPECT_CALL(GL, ikkunaHalutaanSulkea(hyvaIkkuna));
  Ikkuna i(GL);
  i.paivita();
}

//Kun kutsutaan paivita, ja käyttäjä ei ole sulkenut ikkunaa,
// kutsutaan paivitaIkkuna oikealla osoitteella.
TEST_F(Piirtotestit, paivitaPaivittaaIkkunan) {
  EXPECT_CALL(GL, luoIkkuna).WillOnce(Return(hyvaIkkuna));
  EXPECT_CALL(GL, ikkunaHalutaanSulkea(hyvaIkkuna)).WillOnce(Return(false));
  EXPECT_CALL(GL, paivitaIkkuna(hyvaIkkuna));
  Ikkuna i(GL);
  i.paivita();
}

//Kun kutsutaan paivita, ja käyttäjä on sulkenut ikkunan,
// ei kutsuta paivitaIkkunaa.
TEST_F(Piirtotestit, eiPaivitetaJosIkkunaOnSuljettu) {
  EXPECT_CALL(GL, luoIkkuna).WillOnce(Return(hyvaIkkuna));
  EXPECT_CALL(GL, ikkunaHalutaanSulkea(hyvaIkkuna)).WillOnce(Return(true));
  EXPECT_CALL(GL, paivitaIkkuna(hyvaIkkuna)).Times(0);
  Ikkuna i(GL);
  i.paivita();
}

//Simulointi: Kun käyttäjä sulkee ikkunan, onAuki() palauttaa falsen
TEST_F(Piirtotestit, ikkunanSulkeminen) {
  EXPECT_CALL(GL, ikkunaHalutaanSulkea)
    .WillOnce(Return(false))
    .WillRepeatedly(Return(true));
  EXPECT_CALL(GL, paivitaIkkuna).Times(1);
  Ikkuna i(GL);
  while(i.onAuki() )
    i.paivita();
}
