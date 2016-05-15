#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
Adafruit_GPS GPS(&Serial1);
#define GPSECHO false     

#define PIN_10 10
#define PIN_12 12
#define RED 255, 0, 0
#define DIMRED 20, 0, 0
#define GREEN 0, 255, 0
#define BLUE 0, 0, 255
#define WHITE 127, 127, 127

/*
 * the function numPixels() seems to be giving weird values so we are going to assign specific count lengths
 * depending which LEDs we are using
 */
#define AD_INIT 0
#define AD_CNT 6
#define BE_INIT 7
#define BE_CNT 14
#define CF_INIT 15
#define CF_CNT 21
#define bothSides 0
#define rightSide 1
#define leftSide 2

enum stateOfCyclist {
  kNormal,
  kTurningLeft,
  kTurningRight,
  kStopping
};

Adafruit_NeoPixel strip_6 = Adafruit_NeoPixel(60, PIN_10, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_12 = Adafruit_NeoPixel(60, PIN_12, NEO_GRB + NEO_KHZ800);

// Initialize state tracker
stateOfCyclist currentState = kNormal;
//toupleSide whichSide = bothSides;

int led = 7;
int baudSpeed = 9600;
//int baudSpeed = 11520;

boolean usingInterrupt = false;
void setup() {
  
  
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  // Serial.begin(baudSpeed);
  // Serial.println("_starting setup");
  // pinMode(led, OUTPUT);


  strip_6.begin();
  strip_12.begin();
  strip_6.show(); // Initialize all pixels to 'off'
  strip_12.show();
  
   Serial.println("Adafruit GPS library basic test!");
     
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);
    delay(1000);
  // Ask for firmware version
  Serial1.println(PMTK_Q_RELEASE);
  
}

uint32_t timer = millis();

void colorRenderer(uint8_t section, uint8_t cnt, uint8_t side, uint8_t c1, uint8_t c2, uint8_t c3) {
  if( side == bothSides ) {
    for(uint8_t i=section; i<=cnt; i++) {
      strip_6.setPixelColor(i, strip_6.Color(c1,c2,c3));
      strip_12.setPixelColor(i, strip_12.Color(c1,c2,c3));
    }
    strip_6.show();
    strip_12.show();
  }
  else if( side == rightSide ) {
    for(uint8_t i=section; i<=cnt; i++) {
      strip_12.setPixelColor(i, strip_12.Color(c1,c2,c3));
    }
    strip_12.show();
  }
  else if( side == leftSide ) {
    for(uint8_t i=section; i<=cnt; i++) {
      strip_6.setPixelColor(i, strip_6.Color(c1,c2,c3));
    }
    strip_6.show();
  }
}

void turningColorRenderer(uint8_t section, uint8_t cnt, uint8_t side, uint8_t c1, uint8_t c2, uint8_t c3) {
  if( side == rightSide ) {
    for(uint8_t i=section; i<=cnt; i++) {
      strip_12.setPixelColor(i, strip_12.Color(c1,c2,c3));
      strip_12.show();
      delay(75);
    }
  
    for(uint8_t i=section; i<=cnt; i++) {
      strip_12.setPixelColor(i, strip_12.Color(0,0,0));
    }
    strip_12.show();
    delay(75);
  }
  else if( side == leftSide ) {
    for(uint8_t i=cnt; i>=section; i--) {
      strip_6.setPixelColor(i, strip_6.Color(c1,c2,c3));
      strip_6.show();
      delay(75);
    }
  
    for(uint8_t i=cnt; i>=section; i--) {
      strip_6.setPixelColor(i, strip_6.Color(0,0,0));
    }
    strip_6.show();
    delay(75);
  }
}

// void colorSolidRed( uint8_t section, uint8_t cnt ) {
//   for(uint8_t i=section; i<=cnt; i++) {
//     strip_6.setPixelColor(i, strip_6.Color(RED));
//     delay(20);
//   }
//     strip_6.show();
//     delay(100);
// }

// void colorTurningRed(uint8_t section, uint8_t cnt) {
//   for(uint8_t i=section; i<=cnt; i++) {
//     strip_6.setPixelColor(i, strip_6.Color(RED));
//     strip_6.show();
//     delay(200);
//   }
  
//   for(uint8_t i=section; i<=cnt; i++) {
//     strip_6.setPixelColor(i, strip_6.Color(0,0,0));
//   }
//   strip_6.show();
// }

// void normal() {
  
//   delay(500);
// }

// void turningLeft() {
  
//   // should only select the 8 LED strip on the left
//   colorTurningRed( 7, 14 );
// }

// void turningRight() {
  
//   // should only select the 8 LED strip on the right
//   colorTurningRed( 7, 14 );
// }

// void stopping() {
//   // everything turns red
  
// }

// void updateState() {
//   // Get info
//   if(false/* Condition for normal setting */) {
//     currentState = kNormal;
//   }
//   else if(false/* Condition for turning left */) {
//     currentState = kTurningLeft;
//   }
//   else if(false/* Condition for turning right */) {
//     currentState = kTurningRight;
//   }
//   else if(false/* Condition for stopping */) {
//     currentState = kStopping;
//   }
// }

uint8_t MYO = 0;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip_6.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip_6.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip_6.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip_6.numPixels(); i=i+3) {
        strip_6.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        strip_12.setPixelColor(i+q, Wheel( (i+j) % 255));
      }
      strip_6.show();
      strip_12.show();

      delay(wait);

      for (uint16_t i=0; i < strip_6.numPixels(); i=i+3) {
        strip_6.setPixelColor(i+q, 0);        //turn every third pixel off
        strip_12.setPixelColor(i+q, 0);
      }
    }
  }
}

void loop() {
  
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();
     
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }

  //testLEDnum();
  //updateState();

  String command = "";
  while(Serial.available()) {
   
    command += String(Serial.read());
  }
  Serial.flush();

   if (command == "Normal") {
        colorRenderer( AD_INIT, AD_CNT, bothSides, DIMRED );
        colorRenderer( BE_INIT, BE_CNT, bothSides, DIMRED );
        colorRenderer( CF_INIT, CF_CNT, bothSides, DIMRED );
        delay(2000);
    }
    else if (command == "left")  {
        for( uint8_t i=0; i<=5; i++) {
          colorRenderer( AD_INIT, AD_CNT, leftSide, RED );
          colorRenderer( CF_INIT, CF_CNT, leftSide, RED );
          colorRenderer( AD_INIT, AD_CNT, rightSide, DIMRED );
          colorRenderer( CF_INIT, CF_CNT, rightSide, DIMRED );
          colorRenderer( BE_INIT, BE_CNT, rightSide, DIMRED );
          turningColorRenderer( BE_INIT, BE_CNT, leftSide, RED );
          colorRenderer( AD_INIT, AD_CNT, leftSide, 0, 0, 0 );
          colorRenderer( CF_INIT, CF_CNT, leftSide, 0, 0, 0 );
          delay(200);
        }
      }
     else if (command == "right")  {
        for( uint8_t i=0; i<=5; i++) {
          colorRenderer( AD_INIT, AD_CNT, rightSide, RED );
          colorRenderer( CF_INIT, CF_CNT, rightSide, RED );
          colorRenderer( AD_INIT, AD_CNT, leftSide, DIMRED );
          colorRenderer( CF_INIT, CF_CNT, leftSide, DIMRED );
          colorRenderer( BE_INIT, BE_CNT, leftSide, DIMRED );
          turningColorRenderer( BE_INIT, BE_CNT, rightSide, RED );
          colorRenderer( AD_INIT, AD_CNT, rightSide, 0, 0, 0 );
          colorRenderer( CF_INIT, CF_CNT, rightSide, 0, 0, 0 );
          delay(200);
        }
      }
     else if (command == "stop")  {
        colorRenderer( AD_INIT, AD_CNT, bothSides, RED );
        colorRenderer( BE_INIT, BE_CNT, bothSides, RED );
        colorRenderer( CF_INIT, CF_CNT, bothSides, RED );
        delay(2000);
      }
     else if (command == "canada")  {
        theaterChaseRainbow(20);
      }
     else if (command == "usa") {
       // usa 
       Serial.println ("aa");
     }
     else if (command == "disco") {
       Serial.println ("aa");
     }
     else if (command == "brightness") {
         Serial.println ("aa");
     }
     else if (command == "off") {
       Serial.println ("aa");
     }
     else {
       Serial.println ("aa");
     }
  }
  
 // MYO++;
 // if( MYO > 5) MYO = 0;


  // Tests
//  colorRenderer( AD_INIT, AD_CNT, bothSides, WHITE );
//  delay(1000);
//
//  colorRenderer( BE_INIT, BE_CNT, bothSides, RED );
//  delay(1000);
//
//  colorRenderer( BE_INIT, BE_CNT, bothSides, DIMRED );
//  delay(1000);
//
//  colorRenderer( CF_INIT, CF_CNT, bothSides, GREEN );
//  delay(1000);
//
//  strip_6.clear();
//  strip_6.show();
//  strip_12.clear();
//  strip_12.show();
//
//  delay(1000);

  // if( false /* Condition for normal setting */) {
  //   //normal();
  //   colorSolidRed( 0, 4 );
  // }
  // else if( false /* Condition for stopping */) {
  //   stopping();
  // }
  // else if( false /* Condition for turning left */) {
  //   turningLeft();
  // }
  // else if( false /* Condition for turning right */) {
  //   turningRight();                                            
  // }
  // delay(100);

