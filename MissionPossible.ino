#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

//Ethernet settings
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0xA6, 0xA5 };
//String server = "cos-ugrad.swansea.ac.uk"; // Change this to server address
IPAddress ip(137,44,6,225);
IPAddress server(173,194,41,102); // Google
EthernetClient client;
boolean ethernetEnabled = true;

/*
For proxy:

// if you get a connection, report back via serial:
  if (client.connect("proxy.ysgolccc.org.uk", 8080)) {  // This is connecting to the proxy
    Serial.println("connected");

    // Make a HTTP request through proxy:
    client.println("GET http://www.actualserver.com/search?q=arduino HTTP/1.0");
    client.println();
  }

*/

//LED strip settings
uint16_t stripLength = 240;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(stripLength, 6, NEO_GRB + NEO_KHZ800);

// Delay variables
long previousMillis = 0;
long interval = 20; //delay between updates
uint16_t t = 0; //time step
long previousPoll = 0;
long pollInterval = 10000; //10s

//Fading variables
uint16_t fader = 255;
boolean fadingOut = true;

// Set up cars
const uint8_t nrCars = 10; // number of cars
uint8_t length = 4; //length of car
//uint16_t currentPos[] = {0,0,0,0,0,0,0,0,0,0};
uint16_t currentPos[] = {0,5,10,15,20,25,30,35,40,45};
uint32_t color[nrCars];
int velocity[] = {1,2,3,4,5,6,7,8,9,10};
//uint8_t velocity[] = {0,0,0,0,0,0,0,0,0,0};
int pulsing[]= {false,false,false,false,false,false,false,false,false,false};

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  randomSeed(analogRead(0));
  
  //Initialise car colours
  color[0] = strip.Color(0xFF,0,0); //red
  color[1] = strip.Color(0xFF,0x7A,0); //orange
  color[2] = strip.Color(128,0,128); //purple
  color[3] = strip.Color(0,0xFF,0); //green
  color[4] = strip.Color(255,255,0); //yellow
  color[5] = strip.Color(0x03,0x89,0x9C); //turquoise
  color[6] = strip.Color(0x33,0x33,0x33); //silver
  color[7] = strip.Color(255,20,147); //pink
  color[8] = strip.Color(0,0,255 ); //blue
  color[9] = strip.Color(0xFF,0xFF,0xFF); //white
  
  if(ethernetEnabled) {
    Serial.begin(9600); //Wait for serial monitor
    
    Serial.println("Waiting for DHCP..");
    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      // no point in carrying on, so do nothing forevermore:
      for(;;)
        ;
    }
    //Ethernet.begin(mac,ip); //for static IP
    
    // give the Ethernet shield a second to initialize:
    delay(1000);
    connectToServer();
  }
}

void connectToServer() {
    Serial.println("connecting...");
  
    // if you get a connection, report back via serial:
    //if (client.connect("cos-ugrad.swansea.ac.uk", 80)) {
    if (client.connect(ip,8000)) {
    //if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      //client.println("GET /462694/Experiment/MissionPossible/user2.php HTTP/1.0"); //Change this
      //client.println("GET /search?q=arduino HTTP/1.0");
      client.println("GET /test.html HTTP/1.0");
      client.println();
    } 
    else {
      // if you didn't get a connection to the server:
      Serial.println("connection failed");
    }
}


void loop() {
  
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousPoll > pollInterval) {
    
    previousPoll = currentMillis; // save the last time we polled the server
    
    ethernetEnabled = true;
    connectToServer();  
  }
  
  if(ethernetEnabled)
    getData(); //Read values from server
  
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
      if(t == 255)
        t = 0; //restart
        
      
      //Signal for fading in or out  
      if(fadingOut)
        fader -= 15; //preferably a multiple of 255
      else
        fader += 15;
      
      if(fader >= 255)
        fadingOut = true;
      if(fader <= 0)
        fadingOut = false;
    
   }
  
  
  // if the server's disconnected, stop the client:
  if (!client.connected() && ethernetEnabled) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    ethernetEnabled = false;
  }
  
  
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
        
     if((i % 10) <= velocity[nr]) { //update position based on velocity
        currentPos[nr] +=1;
        strip.setPixelColor(currentPos[nr]-1,strip.Color(0,0,0)); //Clear previous position
     }
     
     if(pulsing[nr]) { 
       car(fade(color[nr]),length,currentPos[nr]);
     }else{
       car(color[nr],length,currentPos[nr]);
     } 
       
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

uint32_t fade(uint32_t colour) {

  // Extract rgb values from packed 32-bit colour value and multiply with fader
  uint8_t newR = fader/255.0*((colour >> 16) & 0xff); //mask is necessary if upper 8 bits is used for other purposes
  uint8_t newG = fader/255.0*((colour >> 8) & 0xff);
  uint8_t newB = fader/255.0*(colour & 0xff);
  
  return strip.Color(newR,newG,newB);
}

boolean getData() {
  int sensorID;
  int vel;
  int fading;
  
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    
    if(c == '(') {
      sensorID = client.read() - '0'; 
      
      if(client.read() == ',') {
        vel = client.read() - '0';
        
        if(client.read() == ',') {
          fading = client.read() - '0';
          
          if(client.read() == ')') {
            Serial.println("Parsing successful.");
            Serial.println(sensorID);
            Serial.println(vel);
            Serial.println(fading);
            
            velocity[sensorID] = vel;
            if(fading == 1)
              pulsing[sensorID] = true;
            else
              pulsing[sensorID] = false;

            return true;
          }
        }
      }
      
      Serial.println("Error in parsing data");
      return false;
    }
   
    Serial.print(c); //Print all other info to serial
  }
}
