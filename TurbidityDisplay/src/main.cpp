#include <Arduino.h>
#include "Sim800L.h"
#include "Wire.h"
#include "LCD.h"
#include "LiquidCrystal_I2C.h"

//Define
#define RX 2
#define TX 3
#define Buzzer 4
#define MESSAGE_LENGTH 160

//Initiate
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7);  // Membuat objek LCD
Sim800L GSM(RX, TX);                        // Membuat objek SIM800


//Global Variables
String state;
String buzzerState;
String sms;
String phone;
String sensorValue;


//Display to LCD
void display(String str, uint8_t col, uint8_t row){   //display ke lcd
  lcd.setCursor(col, row);                            //set cursor ke coloum dan row yang kita inginkan
  lcd.print(str);                                     //print text ke lcd
}

bool sim808Init(){                                //Inisialisasi SIM800 / Modul GSM
  if (GSM.checkAT()) {Serial.println("AT ERROR");return false;}                //Mengecek status GSM Modul apakah sudah siap atau belum
  if (GSM.checkSimCardInfo()) {Serial.println("SIM ERROR");return false;}       //Mengecek status simcard apakah terpasang atau belum
  if (GSM.setFunctionalityMode(1)) {Serial.println("FUN ERROR");return false;}  //Mengeset fungsionalitas modul : 1 On, 0 Sleep
  if (GSM.checkGsmStatus()) {Serial.println("GSM ERROR");return false;}         //Mengecek Jaringan GSM sudah terhubung atau belum
  if (GSM.checkGprsStatus()) {Serial.println("GPRS ERROR");return false;}        //Mengecek Jaringan GPRS sudah terhubung atau belum

  return true;
}

bool checkSms(){
  String message, msg;
  int index = GSM.unread();
  if (index > 0) {
    Serial.print("ADA SMS:");
    Serial.println(index);
    msg = GSM.readSms(index);
    phone = GSM.getNumberSms(msg);
    Serial.print("SMS: ");
    Serial.println(msg);
    Serial.print("PHONE: ");
    Serial.println(phone);
    if (phone == "087853100381" || phone == "+6287853100381") {
      message = GSM.getSmsContain(msg);
      Serial.println(message);
      if (message!="none") {
        sms = message;
        Serial.println(sms.length());
      }
    }
    GSM.delSms(index);
    return true;
  }
  else return false;
}

void UpdateValue(){       //Update Value dari sms yang dikirimkan oleh sensor node
  if (sim808Init()) {     //memastikan inisialisasi gsm OK
    state = "OK";         //jika ok maka status = OK
    Serial.println("SIMINIT OK");
  }
  else {
    state = "LOADING";               //jika tidak maka status = LOADING
    Serial.println("SIMINIT ERROR");
  }
  while(checkSms());
  Serial.println(sms.toInt());
  if (sms.toInt()>0 && sms.toInt()<3) {    //mengecek apakah nilai NTU dibawah 3
    buzzerState = "ON";       //jika iya maka status buzzer menjadi on
  }
  else buzzerState = "OFF";   //jika tidak maka status buzzer menjadi off
  sensorValue = sms;
}

void setup() {
    //Serial Init
    Serial.begin(9600);     //inisialisasi fungsi serial untuk troubleshoot
    GSM.begin(9600);        //inisialisasi modul gsm
    //Buzzer Init
    pinMode(Buzzer, OUTPUT);  //inisialisasi pin untuk buzzer sebagai output
    // LCD Init
    lcd.begin(20,4);          //inisialisasi pin untuk lcd
    lcd.setBacklightPin(3,POSITIVE);  //inisialisasi pin backlight lcd
    lcd.setBacklight(HIGH);           //menyalakan backlight lcd
}

void loop() {
  UpdateValue();                      //Update the value of the sensor and status
  lcd.clear();                        //clear the lcd display
  display("SENSOR DISPLAY", 0, 0);    //Judul
  display("STATUS:", 0, 1);           //Display STATUS
  display(state, 8, 1);               //Status Value : OK / ERROR -> Menunjukkan status dari Modul GSM
  display("TURBIDITY: ", 0, 2);       //Display TURBIDITY
  display(sensorValue, 11, 2);        //Turbidity Value : NTU Unit -> Menunjukkan nilai NTU dari air hasil pembacaan sensor
  display("BUZZER   :", 0, 3);        //Display BUZZER
  display(buzzerState, 11, 3);        //Buzzer Value : ON / OFF -> Kalau ON maka Buzzer akan berbunyi kalau OFF Tidak
  if (buzzerState=="ON") {            //Jika nilai NTU diatas 5 maka status buzzer=ON dan buzzer akan berbunyi
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
  }
  else digitalWrite(Buzzer, LOW);     //Jika nilai NTU tidak diatas 20 maka status buzzer=OFF dan buzzer tidak akan berbunyi
  delay(1000);
}
