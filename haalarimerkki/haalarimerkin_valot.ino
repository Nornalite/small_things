// Haalarimerkin valot; kahden inputin avulla ledejä voi säätää päälle tai pois yksi kerrallaan
// Ohjelma tehty Seeed XIAO-laudalle
// 24.09.2023, Jenna Salmela

// Inputtien pinneinä toimivat laudan kaksi viimeistä
#define INPUTRIVI 10
#define INPUTONOFF 9

#define LEDIENMAARA 9

// Montako millisekuntia sekunnista valittu ledi viettää välkkyen
#define VALKKYMISAIKA 100

// Mikä arvo inputin pitää ylittää, että ohjelma rekisteröi napinpainalluksen
#define ANALOGSCALE 900

// Ledien määrän saa säädettyä tästä, max. 9 kpl koska laudan portit eivät muuten riitä
int lediPinni[LEDIENMAARA];
bool ledinTila[LEDIENMAARA];
bool inputRiviPaalla;
bool inputOnOffPaalla;
// Tämä pointteri pitää kirjaa siitä, mitä lediä ollaan muokkaamassa On-Off-napilla
// (kiertää ledit läpi inputLedi-napin painallus kerrallaan)
int *pValittuLedi;

void setup() {

  //Serial.begin(9600);  
  
  for (int i = 0; i < LEDIENMAARA; i++)
  {
    // Alustetaan ledit
    // Ledipinnien numerot muistiin ja tilat outputeiksi
    lediPinni[i] = i;
    pinMode(lediPinni[i], OUTPUT);
    // Kaikki ledit aluksi sammutetaan
    ledinTila[i] = false;
  }

  // Toteutetaan ledien sammutus

  // Määritetään input-pinnit
  pinMode(INPUTRIVI, INPUT);
  pinMode(INPUTONOFF, INPUT);

  // Sekä input-pinnien aloitustila
  inputRiviPaalla = false;
  inputOnOffPaalla = false;

  // Laitetaan muutettavan ledin pointteri osoittamaan ensimmäiseen lediin  
  pValittuLedi = NULL;
  LediOnOff();

}

void loop() {

  //Tarkistetaan kohteenvaihtoinputin tila
  LueRiviInput();
  // Tarkistetaan on-off-napin tila
  LueOnOffInput();

  // Sytytetään ja sammutetaan ledit annettujen tilojen mukaisesti
  LediOnOff();

  // Serial.println(*pValittuLedi);           DEBUG

  // Pieni paussi ennen uutta kierrosta (osin estämään häiriöitä inputtia siirrettäessä)
  delay(25);

}

void LueRiviInput()
{
  // Tämä inputti hallitsee muokkauksen kohteena olevaa lediä kiertäen ledilistan läpi
  // yksi kerrallaan.

  //Serial.println(analogRead(INPUTRIVI));

  // Ei inputtia, ei toimenpiteitä.
  if (analogRead(INPUTRIVI) < ANALOGSCALE)
  {
    inputRiviPaalla = false;
    return;
  }
    
  // Kun inputtia tulee, tarkistetaan, onko kyseessä tuore painallus
  // (= input ei ollut päällä ohjelman viime kierroksella)
  // Jos ei, ei jatkotoimenpiteitä.
  if (inputRiviPaalla)
  {
    return;
  }
    
  // Valittu led -pointteri tekee kierroksen;
  // Jos valittua lediä ei ole, otetaan kohteeksi ensimmäinen
  if (pValittuLedi == NULL)
  {
    pValittuLedi = &lediPinni[0];
  }
  // Kierroksen keskellä edetään askel kerrallaan eteenpäin
  else if (*pValittuLedi + 1 < LEDIENMAARA)
  {
    pValittuLedi = &lediPinni[*pValittuLedi + 1];
  }
  // Kierroksen lopussa otetaan tauko, nollataan pointteri
  else
  {
    pValittuLedi = NULL;
  }

  // Pistetään muistiin, että input on ollut vastikään päällä
  inputRiviPaalla = true;

}

void LueOnOffInput()
{
  // On-off-nappi vaihtaa valitun ledin tilan (päälle > pois > päälle jne...)
  
  //Serial.println(analogRead(INPUTONOFF));

  // Ei inputtia, ei toimenpiteitä
  if (analogRead(INPUTONOFF) < ANALOGSCALE)
  {
    inputOnOffPaalla = false;
    return;   
  }

  // Kun inputtia tulee, tarkistetaan, onko kyseessä tuore painallus
  // (= input ei ollut päällä ohjelman viime kierroksella)
  if (inputOnOffPaalla)
  {
    return;
  }

  // Otetaan muistiinpano siitä, että input on ollut vastikään päällä
  inputOnOffPaalla = true;
    
  // Jos valittua lediä ei ole, muutoksia tilaan ei voida tehdä
  if (pValittuLedi == NULL)
  {
    return;
  }
    
  // Vaihdetaan ledin tila päinvastaiseksi kuin se tällä hetkellä on
  ledinTila[*pValittuLedi] = !ledinTila[*pValittuLedi]; 
}

void LediOnOff()
{
 // Tarkistetaan jokaisen ledin haluttu tila ja toteutetaan se
  for (int i = 0; i < LEDIENMAARA; i++)
  {
    // Valitulla ledillä on erikoiskäytöksensä, toteutetaan se ja ohitetaan luupin loppuosa
    if (i == *pValittuLedi)
    {
      ValittuLediBlink();
      continue;
    }
    
    // Tavallisten ledien logiikka on ihan vaan true = päälle, false = pois päältä
    if (ledinTila[i] == true)
    {
      digitalWrite(lediPinni[i], HIGH);
    }
    else
    {
      digitalWrite(lediPinni[i], LOW);
    }
  }
}

void ValittuLediBlink()
{
  // Valittu ledi välkkyy pienissä pätkissä 
  // Tarkistetaan, missä vaiheessa sekuntia ollaan ottamalla 
  // tuhannen (millisekuntien määrän sekunnissa, aka sekuntimäärän) jakojäännös
  long aika = millis();
  int milliSekunteja = (int)(aika % 1000);
  if (milliSekunteja < VALKKYMISAIKA)
  {
  // Välkähtämisikkunan ajan ledi on halutun tilan vastakohta
    if (ledinTila[*pValittuLedi] == true)
    {
      digitalWrite(lediPinni[*pValittuLedi], LOW);
    }
    else
    {
      digitalWrite(lediPinni[*pValittuLedi], HIGH);
    }
  }
  else
  {
    // Välkähtämisikkunan ulkopuolella käytös on normaalia
    if (ledinTila[*pValittuLedi] == true)
    {
      digitalWrite(lediPinni[*pValittuLedi], HIGH);
    }
    else
    {
      digitalWrite(lediPinni[*pValittuLedi], LOW);
    }
  }
}
