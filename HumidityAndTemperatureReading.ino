#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
 
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }
  if (!HTS.begin()) {
    Serial.println("Failed to initialize humity sensor!");
    while (1);
  }
}
 
void loop() {
  // read the sensor value
  float presion = BARO.readPressure();

  Serial.print(presion);
 
  float temperatura = BARO.readTemperature();
 
  Serial.print(",");
  Serial.print(temperatura);

  float humedad = HTS.readHumidity();

  Serial.print(",");
  Serial.print(humedad);
 
  // print an empty line
  Serial.println();
 
  // wait 1 second to print again
  delay(1000);
}
