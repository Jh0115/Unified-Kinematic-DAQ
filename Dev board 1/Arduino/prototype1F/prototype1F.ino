//#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//Adafruit_GPS GPS(&Serial);

#define BMP_SCK 19
#define BMP_MISO 18
#define SEALEVELPRESSURE_HPA (1013.25)
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);
Adafruit_BMP3XX bmp; // I2C

uint32_t timer = millis();
const int chipSelect = 15;
const int statLED = 5;
const int SDLED = 6;
char filename[10] = {'D','A','T','A',48,'.','t','x','t'};
int filenum = 48; //48 is the ASCII code for the number 0, 49 is 1, 50 is 2, and so on...

void setup() {
  delay(2000);
  
  // Prepare LEDS
  pinMode(SDLED,OUTPUT);
  pinMode(statLED,OUTPUT);

  // Flash startup statLEDus signal
  digitalWrite(statLED,LOW);
  delay(100);
  flashBegin();

  
  //-----------------------------------------------------------SD CARD SETUP BEGIN------------------------------------------------------------------------
  //Initialize SD card
  delay(1000);
  if (!SD.begin(chipSelect)) {
    while (1){
    flashFailSD();
    }
  }
  
  // Search the SD card for an unused file name and generate that new file every startup.
  int flag1 = 0;
  while(flag1==0){
    if (filenum>57){
      while(1){
        flashFailSD();
      }
    }
    if (!SD.exists(filename)){ // If a card with the current filename does not exist do these things
      File dataFile = SD.open(filename,FILE_WRITE); // create the file and open it
      while(!dataFile); // wait for the file to become available
      dataFile.println(F("time,accelX,accelY,accelZ,quatW,quatX,quatY,quatZ,temp,press,slAlt")); // print a bit of test characters
      dataFile.close(); // Close the file until we need it again
      flag1 = 1; // We do this to escape the while loop
    }
    else{ // If a file with the current file name does already exist, update the 5th character in the filename to try a different file name
      filenum++;
      filename[4] = filenum;
    }
    //delay(100);
  }
  
  // Flash the confirmation signal
  flashSDConfirm();
  //-----------------------------------------------------------SD CARD SETUP END-------------------------------------------------------------------------


  //-----------------------------------------------------------SENSOR SETUP BEGIN------------------------------------------------------------------------
  //Start with GPS
//  //set baud rate for GPS serial communication
//  GPS.begin(9600);
//
//  //Sending this command will tell me all of the data
//  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
//  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate (This may need to be higher for the flights

  //IMU setup
  if(!bno.begin())
  {
    while(1){
      flashFailedSensor();
    }
  }

  STATUSblink();

  bno.setExtCrystalUse(true);

  //BMP setup
  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

  if (!bmp.begin_I2C()) {
  while (1){
    flashFailedSensor();
  }
  }
  //-----------------------------------------------------------SENSOR SETUP END------------------------------------------------------------------------
}





void loop() {
  imu::Quaternion quat = bno.getQuat();
  
  File dataFile = SD.open(filename,FILE_WRITE);
  dataFile.print(millis());
  // Acceleration
  imu::Vector<3> li_ac = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  dataFile.print(",");
  dataFile.print((float)li_ac.x());
  dataFile.print(",");
  dataFile.print((float)li_ac.y());
  dataFile.print(",");
  dataFile.print((float)li_ac.z());
  
  // Quaternion data
  dataFile.print(",");
  dataFile.print(quat.w(), 4);
  dataFile.print(",");
  dataFile.print(quat.x(), 4);
  dataFile.print(",");
  dataFile.print(quat.y(), 4);
  dataFile.print(",");
  dataFile.print(quat.z(), 4);
  dataFile.print(",");
  dataFile.print(bmp.temperature);
  dataFile.print(",");
  dataFile.print(bmp.pressure / 100.0);
  dataFile.print(",");
  dataFile.println(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  dataFile.close();
  
  delay(200);
}

void flashBegin(){
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
  delay(100);
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
  delay(100);
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
  delay(100);
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
  delay(100);
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
  delay(100);
}

void flashFailSD(){
  delay(1000);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(200);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  
}

void flashSDConfirm(){
  delay(1000);
  digitalWrite(SDLED,HIGH);
  delay(1000);
  digitalWrite(SDLED,LOW);
  delay(100);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(100);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(100);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(100);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(100);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(100);
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(100);
  digitalWrite(SDLED,HIGH);
  delay(1500);
  digitalWrite(SDLED,LOW);
}

void flashFailedSensor(){
  delay(100);
  digitalWrite(statLED,HIGH);
  delay(500);
  digitalWrite(statLED,LOW);
  delay(200);
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
  delay(200);
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
}

void SDblink(){
  digitalWrite(SDLED,HIGH);
  delay(100);
  digitalWrite(SDLED,LOW);
  delay(100);
}

void STATUSblink(){
  digitalWrite(statLED,HIGH);
  delay(100);
  digitalWrite(statLED,LOW);
  delay(100);
}
