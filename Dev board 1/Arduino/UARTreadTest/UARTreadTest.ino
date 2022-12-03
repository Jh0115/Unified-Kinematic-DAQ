int SR = 20; // 0 to 255
byte EnDi = 00000000; //User settings 8 bit byte of data

void setup() {
  // initialize the serial
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);

}

void loop() {
  Serial.write(SR);
  delay(1);
  Serial.write(EnDi);

  digitalWrite(13,HIGH);
  while(1) {
  }
}
