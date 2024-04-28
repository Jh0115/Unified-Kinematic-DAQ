#include <WiFi.h>
#include <WiFiUdp.h>

#include <Adafruit_BNO055.h>
#include <SPI.h>

int status = WL_IDLE_STATUS;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "networkName";        // your network SSID (name)
char pass[] = "passwordName";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

IPAddress ip_computer = IPAddress(0, 0, 0, 0); //IP address
unsigned int localPort = 1234;      // local port to listen on

WiFiUDP Udp;
boolean connected = false;

// All BNO055 data comes in 16 byte chunks per event (this means 36 total bytes for microseconds (4), accelerometer(16), and gyroscope(16))
int packetSize = 44; // 4 bytes for starting frame delimiter, 1 for destination address, 1 for source address, 1 for type ID, 36 for data, 1 for the checksum (41 total)
int packetsPerUdp = 10; // save 10 packets before sending the data
int bytesTotal = packetsPerUdp*packetSize;
int counter = 0;
int indexLoc;
unsigned long currentTime = 0;

byte startDelim = B11110000;
byte destinID = B00000001;
byte sourceID = B00000011;
byte typeID = B00111100;
byte checkSum = B00000000;

byte packet[44];
byte sendThis[440];
int msgBegin = 7; // the 36 byte long message should begin at slot 4
int accelBegin = 11;
int gyroBegin = 23;

//-------------BNO055 setup----------------------

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup()
{
  //Initialize packet contents
  for (int ii = 0; ii<44; ii++) {
    packet[ii] = 0x00;
  }

  packet[0] = startDelim;
  packet[1] = startDelim;
  packet[2] = startDelim;
  packet[3] = startDelim;
  packet[4] = destinID;
  packet[5] = sourceID;
  packet[6] = typeID;
  //... message ...
  packet[83] = checkSum;

  //Initialize serial and wait for port to open:

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    while (1);
  }

  // connect to wifi
  connectToWiFi(ssid, pass);
  //LEDblink(ledGPIO);

}

void loop()
{
  // Get micros value
  currentTime = micros();

  // Get IMU data
  sensors_event_t angVelocityData, accelerometerData;
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);

  // Convert values to byte arrays
  byte timeBytes[sizeof(unsigned long)];
  byte accelBytes[12];
  byte gyroBytes[12];

  memcpy(timeBytes, &currentTime, sizeof(unsigned long)); //timeBytes now contains the data from micros()
  memcpy(accelBytes, &accelerometerData.acceleration.x, sizeof(accelerometerData.acceleration.x)); //data from accelerometer x
  memcpy(accelBytes+4, &accelerometerData.acceleration.y, sizeof(accelerometerData.acceleration.y)); //data from accelerometer y
  memcpy(accelBytes+8, &accelerometerData.acceleration.z, sizeof(accelerometerData.acceleration.z)); //data from accelerometer z

  memcpy(gyroBytes, &angVelocityData.gyro.x, sizeof(angVelocityData.gyro.x)); //data from gyroscope x
  memcpy(gyroBytes+4, &angVelocityData.gyro.y, sizeof(angVelocityData.gyro.y)); //data from gyroscope y
  memcpy(gyroBytes+8, &angVelocityData.gyro.z, sizeof(angVelocityData.gyro.z)); //data from gyroscope z

  // Put micros in memory slot
  memcpy(packet+msgBegin,timeBytes,sizeof(timeBytes));
  memcpy(packet+accelBegin,accelBytes,sizeof(accelBytes));
  memcpy(packet+gyroBegin,gyroBytes,sizeof(gyroBytes));

  //Put packet in big udp array
  indexLoc = counter*packetSize;
  memcpy(sendThis+indexLoc,packet,sizeof(packet));

  counter += 1;

  if (counter==(packetsPerUdp)) {
    // Send packet over UDP
    Udp.beginPacket(ip_computer, localPort);
    Udp.write(sendThis, 440);
    Udp.endPacket();
    counter = 0;
  }
}

void connectToWiFi(const char * ssid, const char * pwd){

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
          //When connected set 
          //initializes the UDP state
          //This initializes the transfer buffer
          Udp.begin(localPort);
          connected = true;
          break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
          connected = false;
          break;
      default: break;
    }
}

