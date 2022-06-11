#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

BLEService detectarCaida("0b2d6c68-a344-11eb-bcbc-0242ac130002");
BLEBoolCharacteristic caida("0b2d6c68-a344-11eb-bcbc-0242ac130002", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Empezando...");

  if (!IMU.begin()) {
    Serial.println("Fallo al iniciar la IMU!");
    while (1);
  }

  if (!BLE.begin()) { // Initialize BLE
    Serial.println("Fallo al inicializar el módulo BLE!");
    while (1);
  }
  BLE.setLocalName("DetectorCaidas(Rubén)");
  detectarCaida.addCharacteristic(caida);
  BLE.addService(detectarCaida);
  caida.writeValue(false);
  BLE.advertise();
  Serial.println("Esperando conexión...");
}

void loop() {
  float aceleracion;
  float x, y, z;
  
  BLEDevice central = BLE.central();
  while(central.connected()){
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    aceleracion = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
    if(aceleracion>4){
       caida.writeValue(true);
       Serial.print("Arduino en caída libre (Acerleración: ");
       Serial.print(aceleracion);
       Serial.println(")");
    }
  }
  }
}
