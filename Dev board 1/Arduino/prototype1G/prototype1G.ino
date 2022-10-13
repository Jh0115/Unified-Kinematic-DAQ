#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_GPS GPS(&Serial1);

#define BMP_SCK 19
#define BMP_MISO 18
#define SEALEVELPRESSURE_HPA (1013.25)
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);
Adafruit_BMP3XX bmp; // I2C

//uint32_t timer = millis();
//const int chipSelect = 15;

void setup() {
  //-----------------------------------------------------------SD CARD SETUP END-------------------------------------------------------------------------


  //-----------------------------------------------------------SENSOR SETUP BEGIN------------------------------------------------------------------------
  //Start with GPS
  //set baud rate for GPS serial communication
  GPS.begin(9600);

  //Sending this command will tell me all of the data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate (This may need to be higher for the flights)

  //IMU setup
  bno.setExtCrystalUse(true);
  //-----------------------------------------------------------SENSOR SETUP END------------------------------------------------------------------------
}





void loop() {
  File dataFile = SD.open("DATA.txt",FILE_WRITE); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;
    else {
      dataFile.println(GPS.latitude);
      dataFile.println(GPS.longitude);
    }
      // we can fail to parse a sentence in which case we should just wait for another
  }
  
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
  imu::Quaternion quat = bno.getQuat();
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
  
  dataFile.close();
}
