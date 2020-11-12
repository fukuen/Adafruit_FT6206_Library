/***************************************************
  This is our touchscreen painting example for the Adafruit TFT
  captouch shield
  ----> http://www.adafruit.com/products/1947
  Check out the links above for our tutorials and wiring diagrams
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <TFT_eSPI.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

#define AXP173_ADDR 0x34
#ifdef MAIXAMIGO
#define PIN_SDA 27
#define PIN_SCL 24
#endif

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void axp173_init() {
    Wire.begin((uint8_t) PIN_SDA, (uint8_t) PIN_SCL, 400000);
    Wire.beginTransmission(AXP173_ADDR);
    int err = Wire.endTransmission();
    if (err) {
        Serial.printf("Power management ic not found.\n");
        return;
    }
    Serial.printf("AXP173 found.\n");
#ifdef MAIXAMIGO
    //LDO4 - 0.8V (default 0x48 1.8V)
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x27);
    Wire.write(0x20);
    Wire.endTransmission();
    //LDO2/3 - LDO2 1.8V / LDO3 3.0V
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x28);
    Wire.write(0x0C);
    Wire.endTransmission();
#else
    // Clear the interrupts
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x46);
    Wire.write(0xFF);
    Wire.endTransmission();
    // set target voltage and current of battery(axp173 datasheet PG.)
    // charge current (default)780mA -> 190mA
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x33);
    Wire.write(0xC1);
    Wire.endTransmission();
    // REG 10H: EXTEN & DC-DC2 control
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.requestFrom(AXP173_ADDR, 1, 1);
    int reg = Wire.read();
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x10);
    Wire.write(reg & 0xFC);
    Wire.endTransmission();
#endif
}

void setup(void) {
//  while (!Serial);     // used for leonardo debugging

  axp173_init();
  
  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));
  
  tft.begin();

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");
  
  tft.fillScreen(TFT_BLACK);
  
  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, TFT_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, TFT_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, TFT_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, TFT_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, TFT_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, TFT_MAGENTA);
 
  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
  currentcolor = TFT_RED;
}

void loop() {
  // Wait for a touch
  if (! ctp.touched()) {
    return;
  }

  // Retrieve a point  
  TS_Point p = ctp.getPoint();
  
 /*
  // Print out raw data from screen touch controller
  Serial.print("X = "); Serial.print(p.x);
  Serial.print("\tY = "); Serial.print(p.y);
  Serial.print(" -> ");
 */

  // flip it around to match the screen.
//  p.x = map(p.x, 0, 320, 320, 0);
//  p.y = map(p.y, 0, 480, 480, 0);

  // Print out the remapped (rotated) coordinates
  Serial.print("("); Serial.print(p.x);
  Serial.print(", "); Serial.print(p.y);
  Serial.println(")");
  

  if (p.y < BOXSIZE) {
     oldcolor = currentcolor;

     if (p.x < BOXSIZE) { 
       currentcolor = TFT_RED; 
       tft.drawRect(0, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
     } else if (p.x < BOXSIZE*2) {
       currentcolor = TFT_YELLOW;
       tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
     } else if (p.x < BOXSIZE*3) {
       currentcolor = TFT_GREEN;
       tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
     } else if (p.x < BOXSIZE*4) {
       currentcolor = TFT_CYAN;
       tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
     } else if (p.x < BOXSIZE*5) {
       currentcolor = TFT_BLUE;
       tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
     } else if (p.x <= BOXSIZE*6) {
       currentcolor = TFT_MAGENTA;
       tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
     }

     if (oldcolor != currentcolor) {
        if (oldcolor == TFT_RED) 
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, TFT_RED);
        if (oldcolor == TFT_YELLOW) 
          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, TFT_YELLOW);
        if (oldcolor == TFT_GREEN) 
          tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, TFT_GREEN);
        if (oldcolor == TFT_CYAN) 
          tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, TFT_CYAN);
        if (oldcolor == TFT_BLUE) 
          tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, TFT_BLUE);
        if (oldcolor == TFT_MAGENTA) 
          tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, TFT_MAGENTA);
     }
  }
  if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
    tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
  }
}