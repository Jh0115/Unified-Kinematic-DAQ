#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

Adafruit_GPS GPS(&Serial);

uint32_t timer = millis();
const int chipSelect = 2;
const int stat = 7;
const int SDLED = 6;
int filenum = 0;

void setup() {
  delay(5000);
  
  // Prepare LEDS
  pinMode(SDLED,OUTPUT);
  pinMode(stat,OUTPUT);

  // Flash startup status signal
  digitalWrite(stat,LOW);
  delay(100);
  flashBegin();

  //Initialize SD card
  delay(1000);
  if (!SD.begin(chipSelect)) {
    while (1){
    flashFailSD();
    }
  }

  //Check for filename and start new file
  File logFile = SD.open("LOGFILE.txt", FILE_WRITE);
  while(!logFile);
  logFile.println(millis());
  logFile.close();
  
  int fileflag = 0;
  while (fileflag==0){
    filenum = filenum+1;
    if (!SD.exists("datalog"+String(filenum)+".txt")){
      fileflag = 1;
    }
  }

  // Flash the confirmation signal
  flashSDConfirm();

  while(1){
    
  }
  
  //set baud rate for GPS serial communication
  GPS.begin(9600);

  //Sending this command will tell me all of the data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate (This may need to be higher for the flights
  
  //CSV header for SD card
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  dataFile.println("Milliseconds,Latitude(decimal),Longitude(decimal),Speed(knots),Heading(deg),Altitude(cm),Satellites");
  dataFile.close();
  
  //Flash confirmation for end of setup
  flashSetupConfirm();
}

void loop() {
  // Log GPS sentance
  char c = GPS.read();

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())){   // this also sets the newNMEAreceived() flag to false
      flashFailedNMEA();
      return;  // we can fail to parse a sentence in which case we should just wait for another
    }
  }
  
  // approximately every second or so, print out the current stats
  if (millis() - timer > 1000) {
    timer = millis(); // reset the timer
    
    // save data to SD card every second
    File dataFile = SD.open("datalog"+String(filenum)+".txt", FILE_WRITE);
    
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
      
      digitalWrite(stat,HIGH);
      delay(100);
      digitalWrite(stat,LOW);
    }
    else if ((!(GPS.fix))&&(dataFile)){
      dataFile.print(timer);
      dataFile.print(",");
      dataFile.print(0);
      dataFile.print(",");
      dataFile.print(0);
      dataFile.print(",");
      dataFile.print(0);
      dataFile.print(",");
      dataFile.print(0);
      dataFile.print(",");
      dataFile.print(0);
      dataFile.print(",");
      dataFile.println(0);
      dataFile.close();

      dataFile.close();
      
      digitalWrite(stat,HIGH);
      delay(100);
      digitalWrite(stat,LOW);
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

void flashBegin(){
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
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

void flashSetupConfirm(){
  delay(1000);
  digitalWrite(stat,HIGH);
  delay(1000);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(100);
  digitalWrite(stat,HIGH);
  delay(1500);
  digitalWrite(stat,LOW);
}

void flashFailedNMEA(){
  delay(100);
  digitalWrite(stat,HIGH);
  delay(500);
  digitalWrite(stat,LOW);
  delay(200);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
  delay(200);
  digitalWrite(stat,HIGH);
  delay(100);
  digitalWrite(stat,LOW);
}
