// save GPS data to SD card

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

Adafruit_GPS GPS(&Serial1);

uint32_t timer = millis();
const int chipSelect = 15;

void setup() {
  // Announce status of GPS and SD card
  Serial.begin(115200);
  //delay(3000);
  //Serial.println("GPS logger via SD card");

  //Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }

  digitalWrite(10,HIGH);

  //set baud rate for GPS serial communication
  GPS.begin(9600);

  //Sending this command will tell me all of the data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate (This may need to be higher for the flights
  
  //CSV header for SD card
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  dataFile.println("Milliseconds,Latitude(decimal),Longitude(decimal),Speed(knots),Heading(deg),Altitude(cm),Satellites");
  dataFile.close();
}

void loop() {
  // Log GPS sentance
  char c = GPS.read();

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
  
  // approximately every second or so, print out the current stats
  if (millis() - timer > 1000) {
    timer = millis(); // reset the timer
    
    // save data to SD card every second
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    
    if ((GPS.fix)&&(dataFile)) {
      float la = convertMinutesToDecimal(GPS.latitude,GPS.lat);
      float lo = convertMinutesToDecimal(GPS.longitude,GPS.lon);

      //Print data
      dataFile.print(timer);
      dataFile.print(",");
      dataFile.print(la, 8);
      dataFile.print(",");
      dataFile.print(lo, 8);
      dataFile.print(",");
      dataFile.print(GPS.speed);
      dataFile.print(",");
      dataFile.print(GPS.angle);
      dataFile.print(",");
      dataFile.print(GPS.altitude);
      dataFile.print(",");
      dataFile.println((int)GPS.satellites);

      dataFile.close();
    }
    else {
    Serial.println("error opening datalog.txt, may not have GPS fix.");
    }
  }

}

float convertMinutesToDecimal(float loc, char hem){
  //Start by turning minutes into decimals
  int deg = round((loc/100)-0.500000);
  loc = (loc/100.000000)-deg;

  float decimals = loc/0.60000;
  
  decimals = decimals+deg;

  // determine positive or negative based on hemisphere
  if ((hem=='W')||(hem=='S')){
    decimals = decimals*-1;
  }

  return decimals;
}
