#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define ledPin 2 // led pin number
#define maxBright 100
#define buttonPin 1
#define pixelCount 8
#define colorNumber 6 //number of values in color[] array

//setting things up
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelCount, ledPin);

//define our colors
uint32_t red = strip.Color(maxBright, 0, 0);
uint32_t blue = strip.Color(0, 0, maxBright);
uint32_t green = strip.Color(0, maxBright, 0);
uint32_t magenta = strip.Color(maxBright, 0, maxBright);
uint32_t white = strip.Color(maxBright, maxBright, maxBright);
uint32_t yellow = strip.Color(maxBright, maxBright, 0);
uint32_t teal = strip.Color(0, maxBright, maxBright);

//add them to an array for later use
uint32_t color[] = {red, green, blue, yellow, teal, magenta};

uint8_t s; //scene
uint8_t scene;
uint8_t buttonState = 0;
uint8_t lastState = 0; //current button state

void setup() {
    // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  // End of trinket special code
  //see if we have a last saved valid in EEPROM if so use it
  if (EEPROM.length() > 1) {
    s = EEPROM.read(scene);
  }
  else {
    s = 0;
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(buttonPin, INPUT);
}

void loop() {
  switch (s) {
    case 1:
      pulseUp(75);
      break;
    case 2:
      colorWipe(75);
      break;
    case 3:
      colorFlash(400);
      break;
    case 4:
      theaterChase(100);
      break;
  }
}

int buttonPress() {
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastState) {
    if (buttonState == HIGH) {
      digitalWrite(1, HIGH);
      s++;
      if (s > 4) {
        s = 1;
      }
      EEPROM.update(scene, s);
    }
  }
  lastState = buttonState;
  return s;
}

//modifed from neopixles/examples/strandtest
void theaterChase(uint8_t speed) {
  for (int n = 0; n < colorNumber; n++) {   // cycle all color
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, color[n]);  //turn every third pixel on
        strip.setBrightness(maxBright);
      }
      buttonPress();
      strip.show();
      delay(speed);
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// modifed from neopixles/examples/strandtest
// Fill the dots one after the other with a color
void colorWipe( uint8_t speed) {
  for (int n = 0; n < colorNumber; n++) {   // cycle all color
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color[n]);
      strip.setBrightness(maxBright);
      buttonPress();
      strip.show();
      delay(speed);
    }
  }
}

void pulseUp(uint8_t speed) {
  const int fadeAmount = 10; //how much to increase each loop
  for (int n = 0; n < colorNumber; n++) {
    for (int i = 0; i < strip.numPixels(); i++) { //select all pixels
      strip.setPixelColor(i, color[n]); //set color from our array
    }
    for (int b = maxBright; b >= fadeAmount; b -= fadeAmount) { //start pulse for loop
      strip.setBrightness(b);
      buttonPress();
      strip.show();
      delay(speed);
    }  // end of brightness loop
  }  // end of colour loop
}  // end of pulseUp

void colorFlash (int speed) {
  for (int n = 0; n < colorNumber; n++) {
    for (int i = 0; i < strip.numPixels(); i++) { //select all pins
      strip.setPixelColor(i, color[n]); //set each pin to color
      strip.setBrightness(maxBright);
      strip.show();
    }
    delay(speed); //wait speed
    buttonPress();
  }
}

void allOff() {
  buttonPress();
  for (int i = 0; i < strip.numPixels(); i++) { //select all pins
    strip.setPixelColor(i, 0); //set each pin to off
    strip.show();
  }
}
