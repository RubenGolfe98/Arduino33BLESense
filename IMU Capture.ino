#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

BLEService detectarCaida("0b2d6c68-a344-11eb-bcbc-0242ac130002");
BLEBoolCharacteristic caida("0b2d6c68-a344-11eb-bcbc-0242ac130002", BLERead | BLENotify);

void setup() {
  Serial.begin(250000);
  while(!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() {
  float aceleracion;
  float x, y, z;

  if (Serial.available() > 0) {
     char posibleR = Serial.read();

     if(posibleR=='r'){
           if (IMU.accelerationAvailable()) {
              IMU.readAcceleration(x, y, z);
                 Serial.print(x);
                 Serial.print("\t");
                 Serial.print(y);
                 Serial.print("\t");
                 Serial.println(z);
           }
     }
     
  }
}
