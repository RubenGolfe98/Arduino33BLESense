#include <Arduino_APDS9960.h>

int prox;
float red, gr, bl;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }
}

void loop() {

  // Check if a color reading is available.
  while (!APDS.colorAvailable()) {
    delay(5);
  }

  int r, g, b, a;
  // Read the color.
  APDS.readColor(r, g, b, a);

  if (APDS.proximityAvailable()) {
    // Read the proximity where:
    // - 0   => close
    // - 255 => far
    // - -1  => error
    int proximity = APDS.readProximity();
  
    if (proximity==0 && a >100  ){
      red=(r+g+b) ;
      red=r/red;
 
      gr=(r+g+b) ;
      gr=g/gr;
     
      bl=(r+g+b);
      bl=b/bl;
    
      Serial.print(red, 3);
      Serial.print(",");
      Serial.print(gr, 3);
      Serial.print(",");
      Serial.print(bl, 3);
      Serial.println();
    }   
  }    
}
