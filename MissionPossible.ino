#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0xA6, 0xA5 };
IPAddress server(173,194,34,102); // Change this to server address

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);
EthernetClient client;



void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  randomSeed(analogRead(0));
  
  Serial.begin(9600); //Wait for serial monitor
  Serial.println("Waiting for DHCP..");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.0"); //Change this
    client.println();
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  
  
}

void readPrint() {
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  uint32_t color = Wheel(random(255));
  uint8_t length = 4;
  car(color,length,0);
  race(color, 30,length); 
  
  
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  
  
}

void car(uint32_t c, uint8_t length, uint32_t start) {
    
    uint32_t ccolor = strip.getPixelColor(start);
    strip.setPixelColor(start,Wheel(ccolor/2)); //Set first and last at different colour
  
    for(uint8_t a=1; a<length-1; a++) {
        strip.setPixelColor(start+a, c);
    }
    strip.setPixelColor(start+length-1,Wheel(ccolor/2));
    strip.show();
    
}
// Fill the dots one after the other with a color
void race(uint32_t c, uint8_t wait, uint8_t length) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i,strip.Color(0, 0, 0));
      car(c,length,i+1);
      //strip.setPixelColor(i+length,c);
      
      strip.show();
      
      readPrint();
      
      delay(wait); //This should be changed to use the millis() function, see e.g. http://arduino.cc/en/Tutorial/BlinkWithoutDelay
  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

/* To transition between two colours:

for(int i = 0; i < n; i++) // larger values of 'n' will give a smoother/slower transition.
{
   Rnew = Rstart + (Rend - Rstart) * i / n;
   Gnew = Gstart + (Gend - Gstart) * i / n;
   Bnew = Bstart + (Bend - Bstart) * i / n;
// set pixel color here
}

*/
