# Paikannus harkka   
Harjoitustyö paikannus teknologiat kurssille.  
  
Ohjelmisto jaettu neljään osaa,  
    
    - C ohjelma (src/ include/ main.exe/main.o)  
    - React frontend  
    - Nodejs backend  
    - MongoDB tietokanta  


## C-osio  

C osio ohjelmasta hoitaa tiedostosta ja antennista ubx binääri datan parsimisen.  

Tällä hetkellä Antennista suoraan lukeminen on tuettu vain UNIX pohjaisilla käyttöjärjestelmillä.  

# Kääntäminen   
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

# Käyttö  

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

# Toiminta
C osion koodin löytää kansioista src/ ja /include.  
Ohjelma toimii lukemalla tiedostoa tai sarjaporttia ja lähettämällä curlilla http Post pyynnön palvelimelle. 

**HUOMIO!** Sarjaportista lukemista ei ole toteutettu Windows käyttöjärjestelmillä.  


## Backend (nodejs)  

Ensimmäisenä tulee asentaa node.js [Node.js](https://nodejs.org/en).  

Tämän asennuksen jälkeen navigoi kansioon "./backend".
Aja täälä kansiossa komento:

```
    npm install
```
Backend osio käyttää mongodb tietokantaa hyödyksi, jota en ole lisännyt githubiin.  
Luo siis oma mongodb palvelin, tai pyydä pääsy minun omaani.  

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
navigoi kansioon "./fronend", aja:
```
    npm install
```
Ja sen jälkeen käynnistä sovellus ajamalla:  
```
    npm run dev
```

Verkkosivulle pääset osoitteesta: http://localhost:5173

