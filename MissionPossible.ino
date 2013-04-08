#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

//Ethernet settings
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0xA6, 0xA5 };
IPAddress server(173,194,34,102); // Change this to server address
EthernetClient client;

//LED strip settings
uint16_t stripLength = 240;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(stripLength, 6, NEO_GRB + NEO_KHZ800);

// Delay variables
long previousMillis = 0;
long interval = 20; //delay between updates
uint16_t t = 0; //time step

// Set up cars
uint8_t nrCars = 10;
uint16_t currentPos[] = {0,0,0,0,0,0,0,0,0,0};
uint32_t color[] = {0,0,0,0,0,0,0,0,0,0};
uint8_t velocity[] = {1,2,3,4,5,6,7,8,9,10};


void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  randomSeed(analogRead(0));
  
  //Initialise car colours
  for(int i=0; i<nrCars;i++)
    color[i] = Wheel(random(255));
  
  /*
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
  */
  
  
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
  
  uint8_t length = 4; //length of car
  
  //readPrint();
  
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > interval) {
    
    previousMillis = currentMillis; // save the last time we updated the strip
    
    for(int nr = 0; nr < nrCars; nr++) {
    
      if(currentPos[nr] == stripLength) {
        //Restart car
        currentPos[nr] = 0;
        car(color[nr],length,0);
      }       
    }
    
      race(length,t); 
    
      t+=1;
      if(t == strip.numPixels())
        t = 0; //restart
    
   }
  
  
  // if the server's disconnected, stop the client:
  /*
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }*/
  
  
}

void car(uint32_t c, uint8_t length, uint32_t start) {
    /*
    uint32_t ccolor = strip.getPixelColor(start+1);
    strip.setPixelColor(start,Wheel(ccolor/2)); //Set first and last at different colour
  
    for(uint8_t a=1; a<length-1; a++) {
        strip.setPixelColor(start+a, c);
    }
    strip.setPixelColor(start+length-1,Wheel(ccolor/2));
    */
    
    //Draw car
    for(uint8_t a=0; a<length; a++) {
        strip.setPixelColor(start+a, c);
    }
    
}


void race(uint8_t length,uint16_t i) {
  
  for(int nr = 0; nr < nrCars; nr++) {
        
     if(i % velocity[nr] == 0) { //update position based on velocity
        currentPos[nr] +=1;
        strip.setPixelColor(currentPos[nr]-1,strip.Color(0,0,0)); //Clear previous position
     }
     
     car(color[nr],length,currentPos[nr]);
       
  }
  
  strip.show();
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
