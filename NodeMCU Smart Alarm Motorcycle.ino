#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "constant.h"
#include <TinyGPS++.h>
#include "RedMP3.h"

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
MP3 mp3(RX_PIN, TX_PIN);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
SoftwareSerial serial_gps(D3, D4);
TinyGPSPlus gps;

FirebaseData firebaseRelay;
FirebaseData firebaseRFID;
FirebaseData firebaseLokasi;

void relayFirebase() {
  if (Firebase.getString(firebaseRelay, "/dataRelay/relayStatus"))
  {
    if (firebaseRelay.dataType() == "string")
    {
      String FBStatus = firebaseRelay.stringData();
      if (FBStatus == "OFF")
      {
        digitalWrite(inputRelayAplikasi, HIGH);
      }
      else if (FBStatus == "ON")
      {
        digitalWrite(inputRelayAplikasi, LOW);
        mp3.playWithVolume(0x02, 0x1e);
        delay(5000);
      }
      else
      {
        //        Serial.println("Salah kode! isi dengan data ON/OFF");
      }
    }
  }
}

int getSpeedMPH() {
  if (Firebase.getString(firebaseLokasi, "/dataLokasi/kecepatanPerpindahanLokasi"))
  {
    if (firebaseLokasi.dataType() == "string")
    {
      int speedMPH = firebaseLokasi.stringData().toInt();
      //      Serial.println(speedMPH);
      return speedMPH;
    }
  }
}

void rfidFirebase() {
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String rfidUid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfidUid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    rfidUid += String(mfrc522.uid.uidByte[i], HEX);
  }
  if (rfidUid == "UID E-KTP" || rfidUid == "UID E-KTP")
  {
    if (kondisi == 0) {
      digitalWrite(inputRelayRFID, LOW);
      kondisi = 1;
      mp3.playWithVolume(0x06, 0x1e);
      yield();
      delay(50);
      Firebase.setString(firebaseRFID, "/dataE-KTP/statusModul", String(kondisi));
      Firebase.setString(firebaseRFID, "/dataE-KTP/tanggalAlarm", dayStamp);
      Firebase.setString(firebaseRFID, "/dataE-KTP/waktuAlarm", timeStamp);
      yield();
      delay(50);
    } else if (kondisi == 1) {
      digitalWrite(inputRelayRFID, HIGH);
      kondisi = 0;
      mp3.playWithVolume(0x05, 0x1e);
      yield();
      delay(50);
      Firebase.setString(firebaseRFID, "/dataE-KTP/statusModul", String(kondisi));
      Firebase.setString(firebaseRFID, "/dataE-KTP/tanggalAlarm", dayStamp);
      Firebase.setString(firebaseRFID, "/dataE-KTP/waktuAlarm", timeStamp);
      yield();
      delay(50);
    }
  }
  else if (rfidUid == "UID E-KTP" || rfidUid == "UID E-KTP") {
    if (kondisi == 0) {
      digitalWrite(inputRelayRFID, LOW);
      kondisi = 1;
      mp3.playWithVolume(0x06, 0x1e);
      yield();
      delay(50);
      Firebase.setString(firebaseRFID, "/dataE-KTP/statusModul", String(kondisi));
      Firebase.setString(firebaseRFID, "/dataE-KTP/tanggalAlarm", dayStamp);
      Firebase.setString(firebaseRFID, "/dataE-KTP/waktuAlarm", timeStamp);
      yield();
      delay(50);
    } else if (kondisi == 1) {
      digitalWrite(inputRelayRFID, HIGH);
      kondisi = 0;
      mp3.playWithVolume(0x01, 0x1e);
      yield();
      delay(50);
      Firebase.setString(firebaseRFID, "/dataE-KTP/statusModul", String(kondisi));
      Firebase.setString(firebaseRFID, "/dataE-KTP/tanggalAlarm", dayStamp);
      Firebase.setString(firebaseRFID, "/dataE-KTP/waktuAlarm", timeStamp);
      yield();
      delay(50);
    }
  }
  else {
    mp3.playWithVolume(0x04, 0x1e);
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void setup() {
  serial_gps.begin(9600);
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  pinMode(inputRelayAplikasi, OUTPUT);
  pinMode(inputRelayRFID, OUTPUT);
  digitalWrite(inputRelayRFID, HIGH);
  digitalWrite(inputRelayAplikasi, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //    Serial.print(".");
  }
  Serial.println("Sukses terkoneksi wifi!");
  kondisi = 0;
  timeClient.begin();
  timeClient.setTimeOffset(25200);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setString(firebaseRFID, "/dataE-KTP/statusModul", String(kondisi));
  delay(500);//Wait chip initialization is complete
  mp3.playWithVolume(0x07, 0x1e);
}
void loop() {
  while (serial_gps.available()) {
    gps.encode(serial_gps.read());
  }
  if (gps.location.isUpdated()) {
    while (!timeClient.update())
    {
      timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    String link = "https://www.google.com/maps/place/" + String(latitude, 6) + "," + String(longitude, 6);
    Firebase.setString(firebaseLokasi, "/dataLokasi/kecepatanPerpindahanLokasi", String(gps.speed.kmph()));
    Firebase.setString(firebaseLokasi, "/dataLokasi/linkGoogleMaps", link);
    Firebase.setFloat(firebaseLokasi, "/dataLokasi/latitude", gps.location.lat());
    Firebase.setFloat(firebaseLokasi, "/dataLokasi/longitude", gps.location.lng());
    Firebase.setString(firebaseLokasi, "/dataLokasi/tanggalPelacakan", dayStamp);
    Firebase.setString(firebaseLokasi, "/dataLokasi/waktuPelacakan", timeStamp);
  }
  relayFirebase();
  rfidFirebase();

  while (kondisi == 1 && gps.speed.kmph() > 15) {
    mp3.playWithVolume(0x02, 0x1e);
    rfidFirebase();
    delay(5000);
  }
}
