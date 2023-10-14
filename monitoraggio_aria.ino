#include <DHT.h>                  //Libreria Sensore Temperatura e Umidita'
#include <SDS011.h>               //Libreria Sensore Polveri Sottili pm 2.5 e pm 10.0
#include "ThingSpeak.h"           //Libreria invio dati al sito web ThingSpeak
//#include "secrets.h"              //Libreria rilevazione delle password per l'accesso wifi
#include <ESP8266WiFi.h>          //Libreria sensore wifi della scheda
#define DHTPIN D7                //Costante del pin digitale D7 per l'acquisizione della Temperatura e l'Umidita'
#define DHTTYPE DHT22            //Costante DHT22 nome del sensore

#define SECRET_CH_ID 2299024    // sostituire 0000000 con l'ID del canale
#define SECRET_WRITE_APIKEY "4Z7XF4LFTM1QC3D5" // sostituire XYZ con la API Key del canale
#define SECRET_SSID "FASTWEB-6N73UA"      // sostituire MySSID con il SSID della rete WiFi
#define SECRET_PASS "KQDUXT3E2X"  // sostituire MyPassword con la password WiFi

DHT dht(DHTPIN, DHTTYPE);
SDS011 my_sds;

char ssid[] = SECRET_SSID;   //Nome della rete
char pass[] = SECRET_PASS;   //Password della rete
int keyIndex = 0;
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

//Inizializziamo i valori da rilevare tramite i sensori
float temperatura = 0;
float umidita = 0;
float pm2_5 = 0;
float pm10 = 0;
String stato = "Dati aggiornati";

void Temperatura(){
    temperatura=dht.readTemperature(); //Assegna la Temperatura
    Serial.println("Temperatura' " + String(temperatura));  //Stampa Temperatura
}

void Umidita(){
    umidita=dht.readHumidity();   //Assegna l'Umidita'
    Serial.println("Umidita' " + String(umidita));  //Stampa Umidita'
}

void Polveri_sottili(){
    int error;
    error = my_sds.read(&pm2_5,&pm10);
    if (!error){                              //Controlla se e' presente un errore
      Serial.println("P2.5: "+String(pm2_5)); //Stampa pm2.5
      Serial.println("P10:  "+String(pm10));  //Stampa pm10
    }
    else
     Serial.println("Errore polveri sottili."); //Stampa errore quando la rilevazione non e' andata a buon fine
}

void setup() {
  Serial.begin(9600);       //Velocita' trasmissione scheda
  dht.begin();              //Velocita' trasmissione dati dal sensore di temperatura
  my_sds.begin(D1, D2);    //D1 va su TX di SDS011, D2 va su RX di SDS011
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); //Inizializziamo ThingSpeak
}

void loop() {
  //Collegamento alla rete WiFi 
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("In attesa di collegamento WiFi: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);     //collegamento a una rete di sicurezza WPA/WPA2
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("WiFi connesso.");
  } 
  
  //Settiamo i valori acquisiti in precedenza
  Temperatura();
  Umidita();
  Polveri_sottili();
  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, umidita);
  ThingSpeak.setField(3, pm2_5);
  ThingSpeak.setField(4, pm10);
  ThingSpeak.setStatus(stato);    //Settiamo lo stato
  
  
  //Scriviamo sul canale di ThingSpeak 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Aggiornamento del canale avvenuto con successo.");
  }
  else{
    Serial.println("Problema con l'aggiornamento del canale. Codice errore HTTP: " + String(x));
  }
  delay(3600000); //Attende un'ora prima di aggiornare nuovamente il canale con i nuovi dati in ingresso
}
