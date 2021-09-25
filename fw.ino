// Jai Maa Adi Himani Chamunda //
// Code almost completion data 30 may 2021 03:36 AM

/* PINOUT OF DFPLAYER

    (vcc)1|---------|9(BUSY)
    (RX )2|   MP3   |10(USB-)
    (TX )3| PLAYER  |11(USB+)
  (DAC_R)4|         |12(ADKEY_2)
  (DAC_L)5|         |13(ADKEY_1)
  (SPK_1)6|         |14(IO_2)
    (GND)7|  | _ |  |15(GND)
  (SPK_2)8|--|MEM|--|16(IO_1)

   RX TO 17, tx to 16---nc
*/


/*  PINOUT OF SIM800L
    (ant)1|--|sim|--|7(RING)
    (VCC)2|   800l  |8(DTR)
    (RST)3|         |9(MIC+)
    (RXD)4|         |10(MIC-)
    (TXD)5|         |11(SPK+)
    (GND)6|---------|12(SPK-)


    TXD-------> 4(RX), RXD --------> 2(TX), GND TO GND, reset --------------> 15
*/


/*
  PINOUT FOR RFID - RC522--------------

   ____________________________
  |                            |
  |                            |
  |                            |
  |                            |
  |                            |
  |                            |
  |        RFID - RC522        |
  |                            |
  |                            |
  |                            |
  |                            |
  |                            |
  |   1  2  3  4  5  6  7  8   |


  1. SDA _____> D14
  2. SCK _____> D18
  3. MOSI_____> D23
  4. MISO_____> D19
  5. IRQ______> NC
  6. GND______> GND
  7. RESET____> D32
  8. 3.3V_____> 3.3V

*/

/*
   And below is step by step AT command to use

  Set Connection to GPRS :

  AT+SAPBR=3,1,"Contype","GPRS"
  Set the APN, username and password.

  AT+CSTT="portalnmms","",""
  (the APN, username and password i’m using is 3GPRS

  Enable the GPRS

  AT+SAPBR=1,1
  Check if we already got IP Adress

  AT+SAPBR=2,1
  Enabling HTTP mode :

  AT+HTTPINIT

  Setting HTTP bearer profile :

  AT+HTTPPARA="CID",1
  Give URL of website we want to access :

  AT+HTTPPARA="URL","http://.............................................."
  Start HTTP GET Session :

  AT+HTTPACTION=0
  Read the content of webpage :

  AT+HTTPREAD
  Terminate the session :

  AT+HTTPTERM
*/


// change the emei for set the non changable variable for non cloning machine
// --------------------------------------------Function defination area--------------------------
// wifi function declaration//

//#include <Arduino.h>
#include <ESP32Ping.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "cert.h"
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
//#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Sim800l.h>
//#include <PCF8574.h>  // PCF8574 I/O EXTENDER FOR ARDUINO

#define SDA_PIN 21 //GPIO21 on ESP32
#define SCL_PIN 22 //GPIO22 on ESP32

// Set i2c address
//#define PCF8574_ADDRESS 0x21 // CHANGE ADDRESS OF PCF8574

//PCF8574 pcf8574(PCF8574_ADDRESS, SDA_PIN, SCL_PIN);

//int blue = P0;
//int red = P1;
//int green = P2;
int twelve_v = 12;
//int buzzer = P6;
int five_v = 26;
int qr = 13;

int delaypay = 2000;

// --------------------------------------------parameters for i2c bus master----------------------------


//----------------------------------------------Function defination area ended------------------------

LiquidCrystal_I2C lcd(0x27, 20, 4);  // change the i2c address 0x3F for lcd screen

//-----------------------------------------------pixcel led ---------------------------------------------

//#define pin 13 // extra use notinuse
//#define NUM_LEDS 5  // numbers of led connected
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, pin, NEO_GRB + NEO_KHZ800);

//-------------------------------------------------end---------------------------------------------------//



#define SS_PIN  14    // SDA - D14   // ------------------INIT THE PINS FOR MRFC522--------------------
#define RST_PIN 32

MFRC522 mfrc522(SS_PIN, RST_PIN);




//-------------------------------------------------fota link initiate------------------------------------------
String FirmwareVer = "0.1"; // change firmware version from here to update the desired machine................................................................................................................4


#define URL_fw_Version "https://raw.githubusercontent.com/sandeep9148/"+ ap_name +"/main/bin_version.txt"// change the server url if updation required.........................................................5
#define URL_fw_Bin "https://raw.githubusercontent.com/sandeep9148/" + ap_name +"/main/fw.bin" // change the server url if updation required......................................................................6

//-------------------------------------------------fota link initiate end------------------------------------------

// ---------------------------------------------logs generator link-----------------------------------


bool simstatus = 0;
//--------------------------------------------------- AP mode particles------------------------------------------
//Variables
int i = 0;
int statusCode;
int plates = 0;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;
char mod;   // variable to store selected modes (1 = product 1, 2 = product 2, B = Balance ,D = Cancel)
//-------------------------------------------------------end of ap---------------------------------------------

//(Function Decalration)--------------------------------------------------------------------------------------
bool testWifi(void);
void launchWeb(void);
void ap_credits(void);
void setupAP(void);
bool internet_check(void);
void get_number(void);
void printLocalTime(void);
void get_numbers(void);


//------------------------------------------------------------------------------------------
//Establishing Local server at port 80 whenever required------------------------------------------------------
WebServer server(80);


//-------------------------------------------starting of variable for time zone--------------------

const char* ntpServer = "time.nplindia.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 19800;

//------end-------//

String esid; //------------------------------------global variable for wifi connection-id an password-----------------
String epass = "";


////////////////////////////////////////////////datatypes--------------------------------------------------
# define modem_reset 15           // reset pin with esp for reset the modem
# define payment 27
# define lights 25
# define empty 4
# define comm_snd 5
int  tamper = 2;                  // pin for tamper detect
# define wat_act_comm 33 // activation of activator timer

int payscreenloop = 0;
bool tamp_stat;

// -------------------------------------------datatypes for piouts-------------------------------------


HardwareSerial mySerial(2);//(rx 17, tx 16) serial1   change this parameter inside the esp32 hardwareserial.cpp for make it work otherwise it will continuesly failed to load


char dtmf_cmd;
bool call_flag = false;
bool simflag = false;                   // convert wifi to gprs and vice-versa
bool simflag1 = false;
bool golang = false;                  // stop repeat of activating sim at the start if wifi not available
bool looper = false;
bool poweroff = true;
String respon = "";
String machine_number;
String  user_number;
String ser_number;
bool emp_scr = false; // empty lift status on screen
// ----EM-18 DATA ----- //
String id = "";
char input[10];        // A variable to store the Tag ID being presented
int count = 0;        // A counter variable to navigate through the input[] character array
int l_l = 0;           // while loop intiger
boolean flag = 0;     // A variable to store the Tag match statusmodule
String card = "";

String apn = "portalnmms";                    //APN
String apn_u = "";                     //APN-Username
String apn_p = "";                     //APN-Password
String uid = "";
String url = "";
//---------------------------------- parameters for ap name and password---------------------------
String ap_name;
String ap_pass;


String EMEI;

void wifienable(); // FUNTION FOR CONNECT WITH EXTERNAL WIFI
void firmwareUpdate();
int FirmwareVersionCheck();

unsigned long previousMillis = 0; // will store last time LED was updated
unsigned long previousMillis_2 = 0;
const long interval = 600;
const long mini_interval = 1000;

unsigned long intervals = 30000;


void repeatedCall() {
  static int num = 0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {

    previousMillis = currentMillis;
    //    if (FirmwareVersionCheck()) {
    //      firmwareUpdate();
    //    }

  }
  if ((currentMillis - previousMillis_2) >= mini_interval) {
    previousMillis_2 = currentMillis;
    Serial.print("idle loop...");
    Serial.print(num++);
    Serial.print(" Active fw version:");
    Serial.println(FirmwareVer);
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("wifi connected");
    }
    else
    {
      wifienable();   // enable the wifi ap application
    }
  }
}

// ----------------------------------------------END of global variables ----------------------------- //





void setup()
{
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);
  mySerial.begin(9600);   // mySerial (hardware serial) for sim800l

  pinMode(five_v, OUTPUT);  // five volts output to 5v relay
  pinMode(twelve_v, OUTPUT); // twelve volts output to 12v relay
  pinMode(lights, OUTPUT);// blue light
  pinMode(qr, OUTPUT);
  digitalWrite(five_v, HIGH);
  digitalWrite(twelve_v, HIGH);
  digitalWrite(lights, HIGH);
  digitalWrite(qr, HIGH);


  pinMode(empty, INPUT_PULLUP); // empty signal from lift machine
  pinMode(modem_reset, OUTPUT); // signal to reset the modem
  pinMode(payment, OUTPUT); // signal to lift machine for vend
  pinMode(tamper, OUTPUT); // detect tamper
  pinMode(comm_snd , OUTPUT); // send signal to voice assistance for payment notificiation
  pinMode(wat_act_comm, OUTPUT);

  digitalWrite(comm_snd, HIGH); // data send high as default
  digitalWrite(payment, HIGH); // data send high as default
  digitalWrite(wat_act_comm, HIGH);


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  lcd.begin();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("     Welcome to     ");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("       pePreCa      ");
  delay(2000);
  lcd.setCursor(0, 2);
  lcd.print("Self-Test begin.");
  delay(500);
  lcd.setCursor(0, 2);
  lcd.print("Self-Test begin..");
  delay(500);
  lcd.setCursor(0, 2);
  lcd.print("Self-Test begin...");
  delay(500);
  lcd.setCursor(0, 2);
  lcd.print("Self-Test begin....");
  delay(500);
  lcd.setCursor(0, 2);
  lcd.print("Self-Test begin.....");
  digitalWrite(modem_reset, LOW);
  delay(100);
  digitalWrite(modem_reset, HIGH);
  Serial.println(" ");
  delay(300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("------pePreCa-------");
  Serial.print("p");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print(".");
  Serial.print("e");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("..");
  Serial.print("P");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("...");
  Serial.print("r");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("....");
  Serial.print("e");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("......");
  Serial.print("C");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("......");
  Serial.print("a");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print(".......");
  Serial.print(" ");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("........");
  Serial.print("-");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print(".........");
  Serial.print(" ");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("..........");
  Serial.print("T");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("...........");
  Serial.print("h");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("............");
  Serial.print("e");
  delay(300);
  Serial.print(" ");
  delay(300);
  Serial.print("F");
  delay(300);
  Serial.print("o");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print(".............");
  Serial.print("o");
  delay(300);
  Serial.print("d");
  delay(300);
  Serial.print(" ");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("................");
  Serial.print("E");
  delay(300);
  Serial.print("n");
  delay(300);
  Serial.print("g");
  lcd.setCursor(0, 1);
  lcd.print(".................");
  delay(300);
  Serial.print("i");
  delay(300);
  Serial.print("n");
  delay(300);
  Serial.print("e");
  delay(300);
  Serial.print("e");
  delay(300);
  Serial.print("r");
  delay(300);
  Serial.print("s");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print(".................");
  Serial.print(".");
  delay(300);
  Serial.print(".");
  delay(300);
  Serial.print(".");
  delay(300);
  Serial.print(".");
  delay(300);
  Serial.print(".");
  delay(300);
  Serial.print(".");
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print(".................");
  Serial.print(".");
  delay(300);
  Serial.println(".");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print(".................");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("India's First Smart");
  lcd.setCursor(0, 1);
  lcd.print("  Vending Machine  ");
  delay(1000);


  //------------------------------------------function init area------------------------
  ap_credits();  // call for ap_credits
  get_numbers(); //call for get numbers from eeprom
  init_gsm();     // init gsm
  wifienable();   // enable the wifi ap application
  message();
  loggs();

  if (WiFi.status() == WL_CONNECTED)
  {
    printLocalTime();
    if (FirmwareVersionCheck()) {
      firmwareUpdate();
    }
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Software Version....");
  lcd.setCursor(0, 1);
  lcd.print(FirmwareVer);
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Self-test is done!");
  delay(200);
  //----------------------------------------EEPROM and tamper settings-----------------------------
  digitalWrite(tamper, HIGH);
  bool eedata;
  int tamp = digitalRead(tamper); // ONLY ONE TIME RUN
  Serial.print("Tamp pin Value : ");
  Serial.println(tamp);
  if (tamp == 1) {
    eedata = 1;
    Serial.println("Tampered found");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tempered Found!!!");
    EEPROM.write(200, eedata);
    delay(1000);
    EEPROM.commit();
  }
  else
  {

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Machine is Ok");
    delay(100);
    lcd.setCursor(0, 2);
    lcd.print("System is Booting.");
    delay(100);
    lcd.setCursor(0, 2);
    lcd.print("System is Booting..");
    delay(100);
    lcd.setCursor(0, 2);
    lcd.print("System is Booting...");
    delay(100);
    lcd.clear();

  }

  // -------------------------------if machine is tampered----------------------------------
  bool tttam = EEPROM.read(200);
  Serial.print("value of tamper : - ");
  Serial.println(tttam);
  if (tttam == 1) {
    Serial.println("Machine is tampered, warranty void, please contact manufacturer");
    lcd.clear();
    for (;;) {
      updateSerial();
      lcd.setBacklight(HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Warning!");
      lcd.setCursor(0, 2);
      lcd.print("Error 612");
      lcd.setCursor(0, 3);
      lcd.print("Contact Manufacturer");
      digitalWrite(five_v, HIGH);
      digitalWrite(twelve_v, HIGH);
      digitalWrite(lights, HIGH);
      digitalWrite(qr, HIGH);
    }


  }

  Serial.println("PAYMODE:QR PAY");
  Serial.println("UPI,WALLET READY.");

  digitalWrite(five_v, LOW);
  digitalWrite(twelve_v, LOW);
  digitalWrite(lights, LOW);
  digitalWrite(qr, LOW);
}



//-------------------------------------------------end of setup--------------------------------------





//------------------------------------------------loop startup--------------------------------------

void loop()
{
  empty_L();
  card_reader();
  timezone();
  updateSerial();
  smart_connection();
}

//--------------------------------------------------------end of loop--------------------
void empty_L()
{
  l_l = 0;
  int empty_lift = digitalRead(empty);
  Serial.print("Lift status = ");
  Serial.println(empty_lift);

  if (empty_lift == LOW)
  {
    delay(3000);  // delay for next box dropping
    if (empty_lift == LOW)
    {
      emp_scr = true;
      delay(100);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Machine is Empty");

    }
    while (emp_scr)
    {
      lcd.begin();
      int while_empty = digitalRead(empty);
      //      pcf8574.digitalWrite(buzzer, LOW);
      //      delay(100);
      //      pcf8574.digitalWrite(buzzer, HIGH);
      //      delay(100);
      l_l++;
      if (l_l == 1) {
        mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
        delay(500);
        mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
        delay(500);
        mySerial.println("Machine Is Empty Shutting Down!!! " + ap_name); //text content with machine name
        mySerial.println((char)26);// ASCII code of CTRL+Z
        //digitalWrite(five_v, HIGH);
        //digitalWrite(twelve_v, HIGH);
        digitalWrite(lights, HIGH);
        digitalWrite(qr, HIGH);

      }
      updateSerial();
      Serial.println("Machine is Empty");
      //    if (while_empty == HIGH)
      //    {
      //      emp_scr = false;
      //      lcd.begin();
      lcd.setCursor(0, 0);
      lcd.print("Machine is Empty");
      lcd.setCursor(0, 1);
      lcd.print("Please Don't Pay");
      lcd.setCursor(0, 2);
      lcd.print("Reboot the Machine");
      lcd.setCursor(12, 3);
      lcd.print("pePreCa");

    }
  }
  if(plates == 5)
  {
    mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
    delay(500);
    mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
    delay(500);
    mySerial.println("Only 5 Plates left" + ap_name); //text content with machine name
    mySerial.println((char)26);// ASCII code of CTRL+Z
  }
}

void updateSerial()   //------------------------function for update serial for sim800l serial---------------------------------
{
  String textMessage;
  if (mySerial.available() > 0) {
    textMessage = mySerial.readString();
    Serial.println(textMessage);
    if (textMessage.indexOf("Rs 20") >= 0 || textMessage.indexOf("PEPPAY") >= 0 || textMessage.indexOf("+DTMF: 5") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 20rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("20 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      delay(3000);
      textMessage = "";
      payscreenloop = 0;
      digitalWrite(wat_act_comm, LOW);
      delay(200);
      digitalWrite(wat_act_comm, HIGH);
      digitalWrite(payment, LOW);
      delay(200);
      digitalWrite(payment, HIGH);
      digitalWrite(comm_snd, LOW);
      delay(300);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(4000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(4000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 1; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 40") >= 0 || textMessage.indexOf("PEP40PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 40rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("40 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
       digitalWrite(qr,HIGH);
      for (int i = 1; i <= 2; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 2; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 60") >= 0 || textMessage.indexOf("PEP60PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 60rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("60 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      for (int i = 1; i <= 3; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 3; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 80") >= 0 || textMessage.indexOf("PEP80PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 80rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("80 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      for (int i = 1; i <= 4; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 4; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 100") >= 0 || textMessage.indexOf("PEP100PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 100rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("100 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      for (int i = 1; i <= 5; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 5; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 120") >= 0 || textMessage.indexOf("PEP120PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 120rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("120 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      
      for (int i = 1; i <= 6; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 6; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 140") >= 0 || textMessage.indexOf("PEP140PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 140rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("140 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      
      for (int i = 1; i <= 7; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 7; // plates remains
      digitalWrite(qr,LOW);
    }


    if (textMessage.indexOf("Rs 160") >= 0 || textMessage.indexOf("PEP160PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 160rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("160 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      
      for (int i = 1; i <= 8; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(5000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 8; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 180") >= 0 || textMessage.indexOf("PEP180PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 180rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("140 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      
      for (int i = 1; i <= 9; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 9; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("Rs 200") >= 0 || textMessage.indexOf("PEP200PAY") >= 0) //--------------------plate price ---------------------------
    {
      Serial.println("Received 200rs.");
      Serial.println("Vending Starts.");
      delay(300);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Payment received");
      lcd.setCursor(0, 1);
      lcd.print("200 Rs.");
      lcd.setCursor(0, 2);
      lcd.print("vending starting..");
      digitalWrite(comm_snd, LOW);
      delay(200);
      digitalWrite(comm_snd, HIGH);
      digitalWrite(qr,HIGH);
      
      for (int i = 1; i <= 10; i++) {
        textMessage = "";
        payscreenloop = 0;
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(wat_act_comm, LOW);
        delay(100);
        digitalWrite(payment, LOW);
        delay(1000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        digitalWrite(payment, HIGH);
        digitalWrite(wat_act_comm, HIGH);
        delay(delaypay);

        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("pePreCa");
        lcd.setCursor(0, 1);
        lcd.print("MACHINE IS VENDING");
        lcd.setCursor(1, 2);
        lcd.print(i);
        lcd.setCursor(4, 2);
        lcd.print("...Box");
        delay(delaypay);
      }
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thanks.........");
      lcd.setCursor(0, 1);
      lcd.print("Visit Again...");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wait........");
      lcd.setCursor(0, 2);
      lcd.print("pePreCa");
      lcd.setCursor(0, 3);
      lcd.print("The Food Engineers");
      Serial.println("THNX!VISIT AGAIN");
      delay(1000);
      lcd.begin();
      lcd.clear();
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      textMessage = "";
      plates = plates - 10; // plates remains
      digitalWrite(qr,LOW);
    }

    if (textMessage.indexOf("91489148") >= 0) //------------removing tamper data
    {
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Removing tamper data");
      bool eredata = 0;
      Serial.println("Tampered REMOVED!!!");
      EEPROM.write(200, eredata);
      delay(200);
      bool eeep = EEPROM.read(200);
      Serial.println(eeep);
      EEPROM.commit();
      lcd.setCursor(0, 3);
      lcd.print("Tampered Removed!!!");
      delay(4000);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Machine is Resetting");
      delay(1000);
      payscreenloop = 0;
      for (int x = 0; x <= 10; x++) {
        lcd.setCursor(x, 1);
        lcd.print(".");
        delay(500);
      }
      ESP.restart();

    }
    if (textMessage.indexOf("RING") >= 0) //-----------------------------------------identify incoming call -----------------------
    {

      delay(2000);

      mySerial.println("ATA");
      call_flag = true;
      textMessage = "";
    }
    if (textMessage.indexOf("+DTMF: 1") >= 0 || textMessage.indexOf("PEPFVON") >= 0) //----------------------------------dtmf for 5 v high --------------------------
    {
      digitalWrite(five_v, LOW);
      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("5v Power is on " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("5v power on");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
    }
    if (textMessage.indexOf("+DTMF: 8") >= 0 || textMessage.indexOf("PEPON") >= 0) //----------------------------------dtmf for 5 v high --------------------------
    {
      poweroff = false;
      digitalWrite(five_v, LOW);
      digitalWrite(twelve_v, LOW);
      digitalWrite(lights, LOW);
      digitalWrite(qr, LOW);

      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("Power is on " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("power is on");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
    }
    if (textMessage.indexOf("+DTMF: 9") >= 0 || textMessage.indexOf("PEPOFF") >= 0) //----------------------------------dtmf for 5 v high --------------------------
    {
      digitalWrite(five_v, HIGH);
      digitalWrite(twelve_v, HIGH);
      digitalWrite(lights, HIGH);
      digitalWrite(qr,HIGH);

      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("Power is off " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Power is off");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
      lcd.clear();
      poweroff = true;
      while (poweroff) {
        lcd.noBacklight(); // put code here what to do after machine power off by software
        updateSerial();
      }
    }

    if (textMessage.indexOf("+DTMF: 2") >= 0 || textMessage.indexOf("PEPFVOFF") >= 0) //----------------------------------dtmf for 5 v low ---------------------------
    {
      digitalWrite(five_v, HIGH);
      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("5v Power is off " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("5v power off");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
    }

    if (textMessage.indexOf("+DTMF: 3") >=0 || textMessage.indexOf("PEPLTON") >= 0) //----------------------------------dtmf for lights on ---------------------------
    {
      digitalWrite(lights, LOW);
      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("light is on " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Light is on");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
    }


    if (textMessage.indexOf("+DTMF: 4") >= 0 || textMessage.indexOf("PEPLTOFF") >= 0) //----------------------------------dtmf for lights off ---------------------------
    {
      digitalWrite(lights, HIGH);
      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("light is off " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Light is off");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
    }


    if (textMessage.indexOf("+DTMF: 7") >= 0 || textMessage.indexOf("PEPTVON") >= 0) //----------------------------------dtmf for twelve on ---------------------------
    {
      digitalWrite(twelve_v, LOW);
      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("12v Power is on " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("12v power off");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
    }

    if (textMessage.indexOf("+DTMF: 6") >= 0 || textMessage.indexOf("PEPTVOFF") >= 0) //----------------------------------dtmf for twelve power off ---------------------------
    {
      digitalWrite(twelve_v, HIGH);
      mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
      delay(500);
      mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
      delay(500);
      mySerial.println("12v Power is off " + ap_name); //text content with machine name
      mySerial.println((char)26);// ASCII code of CTRL+Z
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("12v power off");
      payscreenloop = 0;
      delay(1000);
      SPI.begin();  //--------------
      mfrc522.PCD_Init(); //--------------
    }

    if (textMessage.indexOf("+DTMF: *") >= 0 || textMessage.indexOf("PEPAP") >= 0) //----------------------------------ap mode enabled ---------------------------
    {
      ap_launch();
    }

    if (textMessage.indexOf("THALISO") >= 0) //----------------------------------ap mode enabled ---------------------------
    {
      //FETCH THE PLATES AND SET PLATES DETAILS TO THE SCREEN
      int plate = textMessage.indexOf("-");
      plate++;
      String plattes = textMessage.substring(plate);
      Serial.print("No. of Plates is inside of machine :- ");
      Serial.println(plattes);
      plates = plattes.toInt();
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Plates value updated");
      lcd.setCursor(0, 2);
      lcd.print("Plates in machine : ");
      lcd.setCursor(6, 3);
      lcd.print(plates);
      payscreenloop = 0;
      delay(1000);
    }
    if (textMessage.indexOf("+DTMF: #") >= 0 || textMessage.indexOf("PEPRST") >= 0) //----------------------------------reset the machine ---------------------------
    {
      Serial.println("Restarting the machine");
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Machine is Resetting");
      delay(1000);
      payscreenloop = 0;
      for (int x = 0; x <= 10; x++) {
        lcd.setCursor(x, 1);
        lcd.print(".");
        delay(500);
      }
      ESP.restart();
    }

    if (textMessage.indexOf("+HTTPREAD:") >= 0) {
      int len1 = textMessage.lastIndexOf(':');
      int len2 = textMessage.lastIndexOf('O');
      int newlen = len1 + 4;
      String balanc = textMessage.substring(newlen, len2);
      Serial.print("New Balance is :-");
      Serial.println(balanc);
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Balance is  ");
      lcd.setCursor(0, 1);
      lcd.print(balanc);
      payscreenloop = 0;
      delay(1000);
      textMessage = "";
    }

    if (textMessage.indexOf("0,603,0") >= 0) {
      Serial.println("DNS Error! Please try again");
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Error DNS!");
      payscreenloop = 0;
      delay(1000);
      ESP.restart();
    }
    if (textMessage.indexOf("0,601,0") >= 0) {
      Serial.println("Network Error! Please try again");
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Error Network!");
      payscreenloop = 0;
      delay(1000);
      ESP.restart();
    }
    if (textMessage.indexOf("0,604,0") >= 0) {
      Serial.println("Stack is busy! Please try again");
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Stack is Busy!");
      payscreenloop = 0;
      ESP.restart();

    }
  }
}


void init_gsm() //----------------------------------------function for sim parameter init ---------------------------------
{
  int r = 0;
  boolean gsm_Ready = 1;
  Serial.println("initializing GSM module");
  while (gsm_Ready > 0)
  {
    if (r > 20) {
      gsm_Ready = 0;
    }
    mySerial.println("AT");
    Serial.println("AT");
    r++;
    while (mySerial.available())
    {
      if (mySerial.find("OK") > 0)
        gsm_Ready = 0;
    }
    delay(200);
  }
  Serial.println("AT READY");
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-----pePreCa-----");
  lcd.setCursor(0, 1);
  lcd.print("Network Connected");
  payscreenloop = 0;
  simstatus = 1;

  boolean ntw_Ready = 1;
  Serial.println("finding network");
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-----pePreCa-----");
  lcd.setCursor(0, 1);
  lcd.print("Finding Network!");
  payscreenloop = 0;
  int k = 0;
  while (ntw_Ready > 0 || k == 20)
  {
    if (k > 20) {
      ntw_Ready = 0;
    }
    mySerial.println("AT+CPIN?");
    Serial.println("AT+CPIN?");
    k++;
    while (mySerial.available())
    {
      if (mySerial.find("+CPIN: READY") > 0)
        ntw_Ready = 0;

    }

    delay(200);

    mySerial.println("AT+CNMI=2,2,0,0,0\r"); //------------------------- Decides how newly arrived SMS messages should be handled
    updateSerial();
  }
  Serial.println("NTW READY");

  boolean DTMF_Ready = 1;
  Serial.println("turning DTMF ON");
  int m = 0;
  while (DTMF_Ready > 0)
  {
    if (m > 20) {
      DTMF_Ready = 0;
    }
    mySerial.println("AT+DDET=1");
    Serial.println("AT+DDET=1");
    m++;
    while (mySerial.available())
    {
      if (mySerial.find("OK") > 0)
        DTMF_Ready = 0;
    }
    delay(200);
  }
  mySerial.println("AT+CGSN");
  Serial.println("AT+CGSN");
  while (mySerial.available() > 0) {
    String EMEIfull = mySerial.readString();
    EMEI = EMEIfull.substring(9, 27);           // --------------------------------fetch out the emei number from sim800l module-----
    Serial.print("IMEI No is :- ");
    Serial.println(EMEI);
  }
  Serial.println("DTMF READY");
  delay(1000);
}

void message()
{
  mySerial.println("AT+CMGF=1"); // ------------------------------------------Configuring TEXT mode
  delay(500);
  mySerial.print("AT+CMGS=\"" + user_number + "\"\r"); //Mobile phone number to send message
  delay(500);
  mySerial.println("Machine is ready " + ap_name); //text content with machine name
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Machine Serial No. ");
  lcd.setCursor(0, 1);
  lcd.print(ap_name);
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(500);
  payscreenloop = 0;
}


void http() //------------------------------------------------- perform gprs payment --------------------------------
{

  Serial.println(" --- Start GPRS & HTTP --- ");
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enabling GPRS......");
  mySerial.println("AT+HTTPINIT");
  updateSerial();
  delay(1000);
  mySerial.println("AT+HTTPPARA=CID,1");
  updateSerial();
  delay(200);
  mySerial.println("AT+HTTPPARA= http://www.pepreca.com/pos/php/data.php?uid=" + id + "&mod=b");
  updateSerial();
  delay(200);
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PAY VIA GPRS......");
  mySerial.println();
  mySerial.println("AT+HTTPACTION=0");
  updateSerial();
  while (!mySerial.available() > 0) {
  }
  mySerial.println("AT+HTTPREAD");
  updateSerial();

  while (!mySerial.available() > 0) {
  }
  delay(200);
  mySerial.println("AT+HTTPTERM");
  updateSerial();
  delay(100);
}



void gsm_config_gprs() { //  -----------------------------------------------enabling http parameters for enabling gprs-------------------------
  Serial.println(" --- CONFIG GPRS --- ");
  mySerial.println("AT+CGATT=1");
  delay(200);
  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  updateSerial();
  delay(200);
  mySerial.println("AT+CSTT='portalnmms','',''");
  updateSerial();
  delay(2000);
  mySerial.println("AT+SAPBR=1,1");
  updateSerial();
  delay(500);
  mySerial.println("AT+SAPBR=2,1");
  updateSerial();
  delay(500);
}

void wifienable() //------------------------------------enable wifi connection with eeprom saved ssid and password--------------------
{
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  //  pinMode(led, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.println("Startup");

  //---------------------------------------- Read eeprom for ssid and pass-------------------------------------------
  Serial.println("Reading EEPROM ssid");


  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");


  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
  //  WiFi.mode(WIFI_MODE_APSTA); // set the esp in ap as well as station mode at same time

  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
    Serial.println("Succesfully Connected to wifi!!!");
    delay(200);
    if (internet_check())
    {
      Serial.println("Internet connected");//---------------------------------- internet check function----------------
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Internet Connected");
      payscreenloop = 0;
      return;
    }
  }
  else
  {
    Serial.println("Wifi Error!");
    lcd.begin();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("-----pePreCa-----");
    lcd.setCursor(0, 1);
    lcd.print("Wifi ERROR!");
    payscreenloop = 0;
    Serial.println("Press Button for New Wifi Network");
    digitalWrite(qr,HIGH);
    //smart_connection();
  }
}



void ap_launch() //-------------------------------------------launch ap mode-------------------------
{
  launchWeb();
  setupAP();// Setup HotSpot
  Serial.println();
  Serial.println("Waiting.");
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("waiting for connect");
  lcd.setCursor(0, 1);
  lcd.print("..........");
  payscreenloop = 0;

  while ((WiFi.status() != WL_CONNECTED))
  {
    if (golang == false) {
      gsm_config_gprs();
      golang = true;
    }
    lcd.setCursor(15, 2);
    Serial.print("|");
    lcd.print("|");
    delay(100);
    lcd.setCursor(15, 2);
    lcd.print("/");
    Serial.print("/");
    delay(100);
    lcd.setCursor(15, 2);
    lcd.print("-");
    Serial.print("-");
    delay(100);
    server.handleClient();
    payscreenloop = 0;

  }
}


bool testWifi(void) // ----------------------------------------fuction for check the wifi connection ----------------------------
{
  int chek = 0;
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0 );
  lcd.print("Internet Connection");
  lcd.setCursor(0, 1);
  lcd.print("Checking....");
  while (WiFi.status() != WL_CONNECTED)
  {
    lcd.setCursor(15, 1);
    lcd.print("|");
    Serial.print("|");
    delay(100);
    lcd.setCursor(15, 1);
    lcd.print("/");
    Serial.print("/");
    delay(100);
    lcd.setCursor(13, 1);
    lcd.print("-");
    Serial.print("-");
    delay(100);
    String textMessage;
    if (mySerial.available() > 0) {
      textMessage = mySerial.readString();
      Serial.println(textMessage);
      if (textMessage.indexOf("+DTMF: *") >= 0) //----------------------------------ap mode enabled ---------------------------
      {
        chek = 1;
        break;
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("-----pePreCa-----");
      lcd.setCursor(0, 1);
      lcd.print("Wifi Connected");
      return true;
      chek = 0;
      break;
    }
  }
  if (chek == 1) {
    ap_launch();
  }
}

void launchWeb()//-----------------------------------------------code for launching the webserver webpage for wifi setup-----------
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("******pePreCa******");
  lcd.setCursor(0, 1);
  lcd.print("Server started");
  lcd.setCursor(0, 2);
  lcd.print("IP Address:");
  lcd.setCursor(0, 3);
  lcd.print(WiFi.softAPIP());

}

void setupAP(void) //---------------------------scan the available wifi connection and send that data to webserver--------
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == 0) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == 0) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  ap_credits();
  WiFi.softAP(ap_name.c_str(), ap_pass.c_str()); // here we have converted string to const char* and put it inside
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}




void createWebServer()// -----------------------------create web server ------------------------------------
{
  {
    server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE html> <html>\n";
      content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
      content += "<title>pePreCa Smart Machine Wifi Setup Page</title>\n";
      content += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
      content += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
      content += ".button {display: block;width: 80px;background-color: #ECB4A8;border: none;color: white;padding: 13px 50px;text-decoration: none;font-size: 25px;margin: 0px auto 50px;cursor: pointer;border-radius: 4px;}\n";
      content += ".button-on {background-color: #3498db;}\n";
      content += ".button-on:active {background-color: #2980b9;}\n";
      content += ".button-off {background-color: #34495e;}\n";
      content += ".button-off:active {background-color: #2c3e50;}\n";
      content += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
      content += "</style>\n";
      content += "</head>\n";
      content += "<body>\n";
      content += "<h1>pePreCa</h1>\n";
      content += "<h3>wifi Setup Using Access Point(AP) Mode</h3>\n";
      content += "<p>";
      content += ipStr;
      content += "<p>";
      content += "<p><a class=\"button button-on\"href=\"/\">Wifi Networks</a>\n";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><br><label>PASSWORD: </label><input name='pass' length=64><br><input type='submit'></form>";
      content += "<p>";
      content += "<p>";
      content += "<p>";
      content += "<p>";
      content += "</p><form method='get' action='numbers'><label>MACHINE MOBILE NUMBER: </label><input name='machine' length=34><br><label>USER MOBILE NUMBER: </label><input name='user' length=34><br><input type='submit'></form>";
      content += "<p>";
      content += "<p>";
      content += "<p><hr>";
      content += "<marquee scrolldelay ='200'>Close the page after setup complete       - pePreca - The Food Engineers- www.pepreca.com - </marquee>";
      content += "<hr></p></html>";
      server.send(200, "text/html", content);
    });


    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");

      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wifi id :");
        lcd.setCursor(0, 1);
        lcd.print(qsid);
        Serial.println(qsid);
        lcd.setCursor(0, 2);
        lcd.print("Password :");
        lcd.setCursor(0, 3);
        lcd.print(qpass);
        payscreenloop = 0;
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
        delay(6000);

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.restart();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }

      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

    });

    server.on("/numbers", []() {
      String mmob = server.arg("machine");
      String umob = server.arg("user");
      if (mmob.length() > 0 && umob.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 97; i < 165; ++i) {
          EEPROM.write(i, 0);
        }
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Machine Mobile No:.");
        lcd.setCursor(0, 1);
        lcd.print(mmob);
        Serial.println(mmob);
        Serial.println("");
        lcd.setCursor(0, 2);
        lcd.print("User Mobile No:.");
        lcd.setCursor(0, 3);
        lcd.print(umob);
        payscreenloop = 0;
        Serial.println(umob);
        Serial.println("");
        delay(6000);

        Serial.println("writing eeprom machine mobile number:");
        for (int i = 0; i < mmob.length(); ++i)
        {
          EEPROM.write(97 + i, mmob[i]);
          Serial.print("Wrote: ");
          Serial.println(mmob[i]);
        }
        Serial.println("writing eeprom user mobile number:");
        for (int i = 0; i < umob.length(); ++i)
        {
          EEPROM.write(131 + i, umob[i]);
          Serial.print("Wrote: ");
          Serial.println(umob[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new mobile number\"}";
        statusCode = 200;

        for (int x = 0; x <= 15; x++) {
          lcd.begin();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("System is rebooting");
          lcd.setCursor(0, 1);
          lcd.print("*******************");
          delay(500);
        }
        ESP.restart();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error :");
        lcd.setCursor(0, 1);
        lcd.print("404");
        payscreenloop = 0;
      }


      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

    });
  }
}




bool internet_check(void)   //--------------------------fucntion for checking the internet connection availability------------------
{
  if (Ping.ping("www.google.com", 3)) {
    looper = false;
    return true; // if connected with internet
  }
  else
  {

    if (looper == false) {
      Serial.println("Internet is not available Please press AP button for new wifi setup");
      looper = true;
    }
    return false; // if not connected with internet
  }
}


void internet_payment() //---------------------------  payment  via internet -------------------------
{
  if (internet_check() == true) {
    HTTPClient http;
    String urla = ("http://pepreca.com/pos/php/data.php"); //Send request to the server with usre_id and Mode >> http://pepreca.com/pos/php/data.php?uid=user1/2/3/4&mod=a/b/x
    String url = (urla + "?uid=" + id + "&mod=" + mod); //http://pepreca.com/ppos/php/data.php?uid=E13F3443&mod=c&amount=100 for ammend the cash payment machine
    Serial.println(url);
    http.begin(url);
    int httpCode = http.GET(); // response comes >> "lb" = low balance ; "balance" = remaining Amount
    String response = "";

    if (httpCode > 0) {
      // start of fetching get process
      response = http.getString();
      int balance = response.toInt();
      Serial.println(balance);
      if (response == "lb") {
        //digitalWrite(buzzer, 0);
        //delay(1000);
        //digitalWrite(buzzer, 1);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Sorry, Payment ");
        lcd.setCursor(0, 1);
        lcd.print("  Unsuccessful!  ");
        delay(500);
        lcd.setCursor(0, 2);
        lcd.print("  Insufficient  ");
        lcd.setCursor(0, 3);
        lcd.print("    Balance!    ");
      }
      else if (balance) {
        lcd.begin();
        lcd.clear();
        digitalWrite(qr,HIGH);
        lcd.setCursor(0, 0);
        lcd.print("Payment Received");
        lcd.setCursor(0, 1);
        lcd.print("Processing...");
        lcd.setCursor(0, 2);
        lcd.print("Amount : Rs.20");     // for mod b 40 amount will be deducted from balance pav bhaji
        digitalWrite(wat_act_comm, LOW);
        delay(200);
        digitalWrite(wat_act_comm, HIGH);
        digitalWrite(payment, LOW);
        delay(200);
        digitalWrite(payment, HIGH);
        digitalWrite(comm_snd, LOW);
        delay(300);
        digitalWrite(comm_snd, HIGH);

        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Balance: ");
        lcd.setCursor(10, 1);
        lcd.print(balance);
        delay(5000);
        lcd.begin();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Thanks.........");
        lcd.setCursor(0, 1);
        lcd.print("Visit Again...");
        lcd.setCursor(0, 2);
        lcd.print("pePreCa");
        lcd.setCursor(0, 3);
        lcd.print("The Food Engineers");
        plates = plates - 1; // plates remains
        delay(8000);
        payscreenloop = 0;
        digitalWrite(qr,LOW);
        SPI.begin();  //--------------
        mfrc522.PCD_Init(); //--------------
      }
    }
  }
  else
  {
    Serial.println("Internet is not available enabling GPRS");

    //gsm_config_gprs();
    //http();
    lcd.begin();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Internet is not");
    lcd.setCursor(4, 1);
    lcd.print("Available...");
    lcd.setCursor(0, 2);
    lcd.print("Please");
    lcd.setCursor(0, 3);
    lcd.print("Try Again!!!");
  }
}



void smart_connection()  //-------------------------------setup for enabling disabling gprs init parameters -------------------
{
  if (WiFi.status() == WL_CONNECTED) {
    simflag1 = 0;
    if (simflag == 0) {
      mySerial.println("AT+SAPBR=0,1");
      Serial.println("Wifi is active disabling GPRS");
      simflag = 1;
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    smart_connection_part();
  }
}



void smart_connection_part() { //----------------------------- part of gprs init parameters----------------------
  simflag = 0;
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERROR : WiFi !");
 // strip.setPixelColor(0, 0, 255, 0);
  //strip.show();

  if (simflag1 == 0) {
    Serial.println("wifi is not connected GPRS enabling");
    lcd.begin();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CHECKING GPRS..");
    gsm_config_gprs();
    simflag1 = 1;
  }
}



void firmwareUpdate(void) {   // -----------------------------------fucntion for fota update-------------------
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  //  httpUpdate.setLedPin(36, HIGH);
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("New Firmware Found!!");
  lcd.setCursor(0, 2);
  lcd.print("Upgrading.........");
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}



int FirmwareVersionCheck(void) {  // --------------------------function for checking firmware fota version --------------------
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;

  if (client)
  {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
    HTTPClient https;

    if (https.begin( * client, fwurl))
    { // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }

  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.printf("\nDevice already on latest firmware version:%s\n", FirmwareVer);
      return 0;
    }
    else
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      return 1;
    }
  }
  return 0;
}


void reconnect() { //---------------------------fucntion for reconnect the internet if internet not available------------
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= intervals)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

void card_reader() //------------------------- function for card reader --------------------------------
{
  //strip.setPixelColor(0, 255, 0, 0);
  //strip.show();

  //if (payscreenloop < 2) {
  SPI.begin();  //--------------
  mfrc522.PCD_Init(); //--------------
  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("------pePreCa------");
  lcd.setCursor(0, 1);
  lcd.print("SCAN QR CODE or");
  lcd.setCursor(0, 2);
  lcd.print("Swipe Your Card");
  lcd.setCursor(0, 3);
  lcd.print("Plate Remains : ");
  digitalWrite(qr,LOW);
  lcd.setCursor(17, 3);
  lcd.print(plates);
  payscreenloop++;
  id = "";
  // }
  Serial.println("Swipe your card");// ask for swiping the card
  //pcf8574.digitalWrite(buzzer, 1);
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //--------------------------------------------------- Read & store card id-----------------------
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    id.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
    id.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  id.toUpperCase();
  id.replace(" ", "");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
//  strip.setPixelColor(0, 0, 0, 255);
//  strip.show();
  //pcf8574.digitalWrite(buzzer, 0);
  delay(100);
  //pcf8574.digitalWrite(buzzer, 1);
  delay(50);
  //pcf8574.digitalWrite(buzzer, 0);
  delay(100);
  //pcf8574.digitalWrite(buzzer, 1);
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Detected!!!");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait....");
  Serial.println(id);
 // strip.setPixelColor(0, 200, 155, 55);
 // strip.show();
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Server Connected");
  lcd.setCursor(0, 1);
  lcd.print("Data Processing");
  payscreenloop = 0;
  mod = 'b';
  internet_payment();
}

void printLocalTime() { //------------------------function to call the time and date-----------------------
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void timezone() //-----------------------function for enable and disable light using time------------------
{
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  // ----------------------------------------------------TURN LED ON

  if ( (p_tm->tm_hour == 18) && (p_tm->tm_min == 30)) {
    digitalWrite(lights, HIGH);
  }


  // ----------------------------------------------------TURN LED OFF

  if ( (p_tm->tm_hour == 07) && (p_tm->tm_min == 00)) {
    digitalWrite(lights, LOW);
  }
}


void get_numbers() //--------------------------------function to get your own number----------------------
{
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  for (int i = 97; i < 107; ++i)
  {
    machine_number += char(EEPROM.read(i));
  }
  Serial.println();

  Serial.print("Machine Number: ");
  Serial.println(machine_number);


  String nums;
  String head = "+91";
  for (int i = 131; i < 141; ++i)
  {
    nums += char(EEPROM.read(i));
  }
  user_number = String(nums);
  user_number = head + user_number;
  Serial.print("User Number: ");
  delay(200);
  Serial.println(user_number);
}
void ap_credits()
{
  String mac_add = WiFi.macAddress();
  mac_add.replace(":", "");
  Serial.println(mac_add);
  String mac_trim = mac_add.substring(0, 8);
  String name = "pePreCa_v";
  ap_name = name + mac_trim; ////////////////////////name of ap mode////////////////////////////
  ap_pass = "110042110042"; /////////////////////////password for ap mode/////////////////////
}
void loggs()
{
  tamp_stat = EEPROM.read(200);
  HTTPClient httpa;
  String emeii = "865210031725483"; //---------------------------------------// change IMEI number at here!
  if (EMEI.indexOf(emeii) >= 0) {
    Serial.println("Machine is OK");
    String logger1 = ("http://pepreca.com/logs/event_log.php");
    String logger2 = ("?machineId=" + ap_name + "&Firmware_version=" + FirmwareVer);
    String logger3 = ("&User_number=" + user_number + "&Machine_number=" + machine_number);
    String logger4 = ("&IMEI_number=" + emeii + "&SIM_status=" + simstatus + "&tampered=" + tamp_stat);
    String logger = (logger1 + logger2 + logger3 + logger4);
    Serial.println(logger);
    httpa.begin(logger);
    int httpCodes = httpa.GET();
    delay(500);
    String responce = "";
    if (httpCodes > 0) {
      // start of fetching get process
      responce = httpa.getString();
      // int responsed = responce.toInt();
      Serial.println(" logs responce: ");
      Serial.println(responce);
    }
  }
  else
  {
    Serial.println("MACHINE IS CLONED");
    lcd.begin();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Machine is cloned");
    lcd.setCursor(0, 1);
    lcd.print("Location & data Sent");
    digitalWrite(five_v, HIGH);
    digitalWrite(twelve_v, HIGH);
    digitalWrite(lights, HIGH);
    digitalWrite(qr, HIGH);
    emeii = "ERROR!MICL";
    String logger1 = ("http://pepreca.com/logs/event_log.php");
    String logger2 = ("?machineId=" + ap_name + "&Firmware_version=" + FirmwareVer);
    String logger3 = ("&User_number=" + user_number + "&Machine_number=" + machine_number);
    String logger4 = ("&IMEI_number=" + emeii + "&SIM_status=" + simstatus);
    String logger = (logger1 + logger2 + logger3 + logger4);
    //http://pepreca.com/logs/event_log.php?machineId=215365421&Firmware_version=0.2&User_number=9311328736&Machine_number=9971055605&IMEI_number=12457896532546&SIM_status=OK
    Serial.println(logger);
    httpa.begin(logger);
    int httpCodes = httpa.GET();
    delay(500);
    String responce = "";

    if (httpCodes > 0) {
      // start of fetching get process
      responce = httpa.getString();
      int responsed = responce.toInt();
      Serial.println(" logs responce: ");
      Serial.println(responsed);
      delay(20000);
      ESP.restart();
    }
  }
}
// Thank you Jai Mata Di
