# Paikannus harkka   
Harjoitustyö paikannus teknologiat kurssille.

Harkkatyö on sovellus, joka lukee ja käsittelee paikannustietoja UBX-tiedostoista tai ublox paikantimella. Sovellus koostuu tiedon parsijasta (C) ,backendistä (Node.js) ja frontendistä (React), ja se käyttää MongoDB-tietokantaa mittaustietojen tallentamiseen.
  
Ohjelmisto jaettu neljään osaa,  
    
    - C ohjelma (src/ include/ CReadUbx.o/CReadUbx.exe)  
    - React frontend  
    - Nodejs backend  
    - MongoDB tietokanta  


## Sisällysluettelo
1. [Projektin kuvaus](#projektin-kuvaus)
2. [Vaatimukset](#vaatimukset)
3. [C-osio](#c-osio)
    - [Kääntäminen](#kääntäminen)
    - [Käyttö](#käyttö)
4. [Backend (Node.js)](#backend-nodejs)
5. [Frontend (Vite+React)](#frontend-vitereact)
6. [Yleisiä ongelmia](#yleisiä-ongelmia)
7. [Esimerkkejä](#esimerkkejä-käytöstä)

## Vaatimukset

- Node.js (v16 tai uudempi): [Node.js](https://nodejs.org/en)
- MongoDB-palvelin (paikallinen tai pilvipohjainen)
- CMake (v3.10 tai uudempi) ja GCC (backendin ja C-koodin kääntämiseen)

## C-osio  

C osio ohjelmasta hoitaa tiedostosta ja antennista ubx binääri datan parsimisen.  

Tällä hetkellä Antennista suoraan lukeminen on tuettu vain UNIX pohjaisilla käyttöjärjestelmillä.  

## Kääntäminen   
Linux ympäristössä C osio voidaan kääntää Make:lla. Navigoi siis projektin juurikansioon ja aja  
```  
    make  
```  
Ohjelma kääntyy tiedostoon CReadUbx.o  

Windowssilla kääntäminen vaatii CMaken asennusta, ohjelma voidaan tosin myös Linuxilla kääntää   CMakella halutessa.  
  
Asenna siis CMake ja aja komennot:  
```  
    cmake -B build  
    cmake --build build  
```  
Käännetyn ohjelman löydät sen jälkeen build/CReadUbx  

## Käyttö  

### Sarjaportista lukien  

Huomio, että sarjaportista ubx datan lukeminen vaatii UBX_NAV_HPPOSLLH viestien käyttöönoton u-center ohjelmasta. U-center ei ole natiivi linux ohjelma, joten se vaatii wine asennuksen.

Wine ei myöskään suoraa osaa lukea sarjaporttia, vaan joudut itse tekemään symboolisen linkin osoittamaan antennin sarjaporttiin. 

Ohje:

Selvitä ensiksi antennin sarjaportti, voit tehdä tämän etsimällä /dev kansiosta oikean laitteen, laite on /dev/ttyACM*, tai /dev/ttyUSB*

Voit tarkistaa laitteen ajamalla:
```
    cat /dev/<Laite>
```
Jos terminaaliin alkaa tulla NMEA muotoista dataa, laite on oikea.

Sitten voit luoda symboolisen linkin:
```
    cd ~/.wine/dosdevices
    ln -s /dev/<Laite> ./com<numero> (Vaihda numero ensimmäiseen vapaaseen)
```
Tämän jälkeen u-centerin ajaessa winellä, uusi com portti näkyy laitteissa.
Kun olet valinnut oikean laitteen ucenter ohjelmassa, navigoi ylävalikosta:
```
    "View" -> "Messages view"
```
Sen jälkeen etsi valikosta:
```
    UBX -> NAV -> UBX_NAV_HPPOSLLH
```
Paina siitä hiiren oikealla napilla ja "enable message"  
Tämän jälkeen viesti on otettu käyttöön ja voit käynnistää ohjelman allamainitulla tavalla.  

### Yleisesti  

```  
Usage:  
    -h                Show this help menu  
    -f <filename>     Read UBX data from the specified file  
    -s <serial_port>  Read UBX data from the specified serial port  

    Note: reading from serial port is not implemented for Windows OS!  
```  
Esimerkki:  
```
    ./CReadUbx.o -s /dev/ttyACM0  
```  

Ohjelman käynnistäessä ilman argumentteja ("./CReadUbx.o"), ohjelma käyttää tiedostoa AJOMATKA.  ubx ja lähettää sen sisältäviä mittauksia.  

#### Toiminta  
C osion koodin löytää kansioista src/ ja /include.  
Ohjelma toimii lukemalla tiedostoa tai sarjaporttia ja lähettämällä curlilla http Post pyynnön palvelimelle. 

**HUOMIO!** Sarjaportista lukemista ei ole toteutettu Windows käyttöjärjestelmillä.  


## Backend (nodejs)  

Ensimmäisenä tulee asentaa node.js [Node.js](https://nodejs.org/en).  

Tämän asennuksen jälkeen navigoi kansioon "./backend".
Aja tässä kansiossa komento:

```
    npm install
```
Backend osio käyttää mongodb tietokantaa hyödyksi, jota en ole lisännyt githubiin.  
Luo siis oma mongodb tietokanta, tai pyydä pääsy minun omaani.  

Itse toteutin tietokannan mongodb atlaksella: [mongodb.com](https://www.mongodb.com/products/platform/atlas-database)

Mongodb clusterin tehtyä, luo tiedosto .env backend kansioon ja lisää sinne:
```
    MONGO_URI=<Sinun tietokantasi linkki>
```

Tämän jälkeen voit käynnistää backendin ajamalla:

```
    npm run dev
```
## Frontend (Vite+React)

Frontend menee samaan tyyliin kun backend, eli asenna node.js,
navigoi kansioon "./frontend", aja:
```
    npm install
```
Ja sen jälkeen käynnistä sovellus ajamalla:  
```
    npm run dev
```

Verkkosivulle pääset osoitteesta: http://localhost:5173


## Yleisiä ongelmia

1. **MongoDB-yhteys ei toimi**
   - Varmista, että MongoDB-palvelin on käynnissä.
   - Tarkista, että `.env`-tiedostossa oleva `MONGO_URI` on oikein.

2. **Frontend tai backend ei käynnisty**
   - Varmista, että Node.js on asennettu.
   - Aja `npm install` oikeassa kansiossa ennen `npm run dev` -komentoa.

3. **Sarjaportin lukeminen ei toimi**
   - Varmista, että oikea sarjaportti on valittu.
   - Tarkista, että UBX_NAV_HPPOSLLH-viestit on otettu käyttöön u-center-ohjelmassa.

## Esimerkkejä käytöstä

1. **Lue paikannustiedot tiedostosta**
    ```bash
   ./CReadUbx.o -f AJOMATKA.ubx
    ```
2. **Lue paikannustiedost sarjaportista**
    ```bash
    ./CReadUbx.o -s /dev/ttyACM0
    ```
3. **Käynnistä back- ja fronend**
    ```bash
    cd backend
    npm run dev

    cd fronend
    npm run dev
    ```