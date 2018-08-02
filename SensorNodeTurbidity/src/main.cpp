#include <Arduino.h>
#include <Sim800L.h>
#include <string.h>
#include <math.h>

#define RX 2
#define TX 3

Sim800L GSM(RX, TX);    // Membuat objek SIM800
char number[15];
char text[10];
int turbid;

bool sim808Init(){                                //Inisialisasi SIM800 / Modul GSM
  if (GSM.checkAT()) {Serial.println("AT ERROR");return false;}                //Mengecek status GSM Modul apakah sudah siap atau belum
  if (GSM.checkSimCardInfo()) {Serial.println("SIM ERROR");return false;}       //Mengecek status simcard apakah terpasang atau belum
  if (GSM.setFunctionalityMode(1)) {Serial.println("FUN ERROR");return false;}  //Mengeset fungsionalitas modul : 1 On, 0 Sleep
  if (GSM.checkGsmStatus()) {Serial.println("GSM ERROR");return false;}         //Mengecek Jaringan GSM sudah terhubung atau belum
  if (GSM.checkGprsStatus()) {Serial.println("GPRS ERROR");return false;}        //Mengecek Jaringan GPRS sudah terhubung atau belum

  return true;
}

void setup() {
    Serial.begin(9600); //inisialisasi fungsi serial untuk troubleshoot
    GSM.begin(9600);    //inisialisasi modul gsm
    pinMode(13, OUTPUT);
    strcpy(number, "087853100386");   //set nomor untuk dikirim sms
}

void loop() {
  if (sim808Init()) {              //memastikan modul gsm berjalan
    unsigned int i;                //membuat variable 'i' untuk counter
    unsigned int sensorValue, a;   //membuat varuable 'sensorValue' untuk menyimpan data ADC dan 'a' untuk penyimpanan sementara
    sensorValue = 0;               //memberi nilai 0 sebagai inisialisasi nilai variable sensorValue
    digitalWrite(13, HIGH);
    Serial.println("SIMINIT OK");
    // capture the values to memory
    for(i=0;i<500;i++) {            //mengambil nilai ADC sebanyak 500 kali
      a = analogRead(A7);           // mengambil data
      if (a >= sensorValue) {       // memilih apakah data baru lebih dari data lama
      sensorValue = a;              // jika lebih lama maka data baru disimpan
      }
    }
    Serial.print("SENSOR ADC: ");
    Serial.println(sensorValue);
    float voltage = sensorValue * (5.0 / 1024.0);   //mengubah data ADC ke Voltage
    Serial.print("VOLTAGE: ");
    Serial.println(voltage);
    float turbidity = (sensorValue-908.33)/(-2.7021);    //Mengubah data Voltage menjadi NTU // nilai NTU air keruh 20.8
    Serial.print("TURBIDITY: ");
    Serial.println(turbidity);
    turbid = turbidity;    //mengubah tipe data NTU ke Integer untuk dikirim ke display
    Serial.println(turbid);
    itoa(turbid, text, 10);           //Mengubah tipe data NTU yang decimal menjadi text
    Serial.print("DATA DIKIRIM: ");
    Serial.println(text);
    if (GSM.sendSms(number, text)) {      //mengirimkan data text NTU melalui sms ke display
      Serial.println("SEND ERROR");       //jika gagal menampilkan "SEND ERROR" ke serial monitor
    }
    else Serial.println("SEND OK");       //jika berhasil menampilkan "SEND OK" ke serial monitor
    delay(1000);                          //delay 30 detik jeda pengiriman tiap sms
  }
  else{
    Serial.println("SIMINIT ERROR");
  }
  digitalWrite(13, LOW);

}
