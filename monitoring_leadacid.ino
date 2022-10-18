#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
int commandState;

#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "SSD1306AsciiWire.h"

float vl, vh;
bool is_counted = false;

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


#include <virtuabotixRTC.h>


#define buz  9
#define led  2
#define led2 4
const float arduinoVCC = 5.13;//Your Arduino voltage
unsigned long ValueR1 = 9900;
unsigned long ValueR2 = 97100;
double Voltage_Source = 60; //various testing voltage sources
const int alanogPin = A0;//the pin connecting the voltage. 
const int inputResolution =1023;//works with most Arduino boards
const float average_of = 500;//Average of 500 readings

float voltage;

float b_voltage;

int count =0;
float danger,warning;
int missuse_count;
const unsigned long record_time = 300000;

unsigned long previousTime_1 = 0;



int address1 = 180;
int address2 = 101;


// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
//
//// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;
int CS_PIN = 10;

virtuabotixRTC myRTC(8, 7, 6);


File file;



void setup()
{  

  Serial.begin(9600);
  Wire.begin();
 // Wire.setClock(400000L);


  #if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
 

 oled.setFont(TimesNewRoman16_bold);
  oled.clear();
 // pinMode(CS_PIN, OUTPUT);
  pinMode(buz,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);
//  pinMode(sw,INPUT_PULLUP);
  
  initializeSD();
  
  closeFile();

//  rtc.begin();
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//  rtc.adjust(DateTime(2022, 2, 26, 13, 42, 30));

// Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
 //myRTC.setDS1302Time(40, 07, 11, 6, 02, 4, 2022);  //Here you write your actual time/date as shown above 
                              
}

int writeToFile()
{
//  initializeSD();
myRTC.updateTime();
 // DateTime now = rtc.now();

  
  //createFile("voltage.csv");
  file = SD.open("voltage.csv", FILE_WRITE);
    
    if(file){
      file.print(b_voltage);
      file.print(" ,");
      
      file.print(myRTC.dayofmonth);
      file.print("/");
      file.print(myRTC.month);
      file.print("/");
      file.print(myRTC.year);
      file.print(" ");
      file.print(myRTC.hours);
      file.print(":");
      file.print(myRTC.minutes);
      file.print(":");
      file.println(myRTC.seconds);
    Serial.println("Writing to file: ");
    }
    else{
    Serial.println("Couldn't write to file");
    return 0;
    }
   
  
 
}
float getVoltageAverage(){
  
    float voltage_temp_average=0;
    for(int i=0; i < average_of; i++)
    {
       readVoltage();
       voltage_temp_average +=voltage;
    }
      
  return voltage_temp_average / average_of;
   
}//getVoltageAverag

 void readVoltage(){
    
    int A0Value = analogRead(alanogPin);
    float voltage_sensed = A0Value * (arduinoVCC / (float)inputResolution);       
  voltage = voltage_sensed * ( 1 + ( (float) ValueR2 /  (float) ValueR1) );
   
 }




void loop()
{ 
  
  displayV();
  misuse();
  batteryVoltage();
  
  changeV();
  

  
  unsigned long currentTime = millis();
  if(currentTime-previousTime_1 >= record_time){//adjustable ; now 5 minutes interval
    writeToFile();
    previousTime_1 = currentTime;
  }
closeFile();

       
 
}

void initializeSD(){
  Serial.println("Initializing SD card...");
 pinMode(CS_PIN, OUTPUT);

  if (SD.begin()){
    file = SD.open("reset.txt", FILE_WRITE);
    if(file){
      file.println("Reset intance.");
      
    }
    Serial.println("SD card is ready to use.");
  } else{
    Serial.println("SD card initialization failed");
    return;
  }
}



int misuse(){
  if (b_voltage<vl and is_counted==false){
    file = SD.open("missuse.csv",FILE_WRITE);
    count = EEPROM.read(8);
    count++;
    EEPROM.update(8,count);
    missuse_count = count;
    if(file){
       
      file.println(missuse_count);
      file.close(); 
    }
    is_counted =true;
  }
  if(b_voltage>vl){
    is_counted =false;
  }
}

void closeFile()
{
  if (file)
  {
    file.close();
    Serial.println("File closed");
  }
}

void batteryVoltage(){
  b_voltage =getVoltageAverage();
 
 
    if (b_voltage>vh){
      digitalWrite(led,HIGH);
    }
    else{
      digitalWrite(led,LOW);
    }
      
     if (b_voltage<vh and b_voltage >vl){
       digitalWrite(led2,HIGH);
      
       tone(buz,5000);
       delay(100);

       tone(buz,5000);
       delay(140);
    
//     tone(buz,44000);
//     delay(1000);

       noTone(buz);
       delay(500);
     }

    
    if (b_voltage<vh and b_voltage<vl){
      tone(buz,4500);
      delay(140);

      tone(buz,4500);
      delay(150);

      tone(buz,4500);
      delay(160);

//    tone(buz,2000);
//    delay(100);
     
      noTone(buz);
      delay(3000);
      digitalWrite(led2,HIGH);
    }
    else{
      digitalWrite(led2,LOW);
    }
   
}



 void changeV(){
   
    commandState = analogRead(A1);
  //Serial.println(commandState);

  if(commandState >= 0 && commandState <20 ){
    vl = EEPROM.get(address1,vl);  
    vl = getVoltageAverage();
 // vl = EEPROM.get(address1,vl);
 // address1 += sizeof(float);
    EEPROM.put(address1,vl);
    danger = vl;   
  }
       
    
  else if(commandState >= 350 && commandState < 900  ){
         vh = EEPROM.get(address2,vh);
         vh = getVoltageAverage();
     //  vh = EEPROM.get(address2,vh);
    //   address2 += sizeof(float);
         EEPROM.put(address2,vh);
         warning = vh;
       }
        
//  else if(commandState > 900 && commandState < 1000 ){ 
//         vh = EEPROM.get(address2,vh);
//         vh+=0.10;
//         address2 += sizeof(float);
//         EEPROM.put(address2,vh+=0.10);
//       }
//    
//    
//    
//       
// else  if(commandState > 240 && commandState < 340 ){
//         vl = EEPROM.get(address1,vl);
//         vl-=0.10;
//         address1 += sizeof(float);
//         EEPROM.put(address1,vl-=0.10);
//       }
  
     
     
delay(100);
 
 }

void displayV(){
  
  oled.clear();
  oled.set1X();

  oled.print(EEPROM.get(address1,vl));
  oled.print("   ");
  oled.println(EEPROM.get(address2,vh));
  
 // oled.setCursor(0,0);
  oled.println("Battery Voltage");
  //oled.setCursor(0,10); //oled oled
  oled.print(b_voltage);
  oled.println(" V");
  //oled.print(missuse_count);
  oled.print(EEPROM.read(8));
}
 


 
