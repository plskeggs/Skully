// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <limits.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_STRIP_PIN    6

#define LED_PIN 13

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 18

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  pinMode(LED_STRIP_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
    delay(50);
  }
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.begin(115200);
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  static bool on = false;
  
  // Fill along the length of the strip in various colors...
  digitalWrite(LED_PIN, on);
  on = !on;

#if 1
  switch(rand() % 5)
  {
    case 0:
      colorWipeSine( 107,  85,  47, 0, rand() % 4000 + 6000, 64, 255); // Dark Olive Green
      break;
    case 1:
      colorWipeSine(   0,  90, 110, 0, rand() % 4000 + 6000, 4, 255); // Weird Purple
      break;
    case 2:
      colorWipeSine(  12, 113,   4, 0, rand() % 4000 + 6000, 0, ((rand() % 2) + 1) * 255); // Blood Red
      break;
    default:
      //colorWipe(strip.Color(   0,   0,   0), 1); // Black
      rainbow(rand() % 5 + 5);
      break;
  }
  colorWipe(strip.Color(   0,   0,   0), 1); // Black
  //delay(max(rand() % 50, 50)); 
  
#else
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  digitalWrite(LED_PIN, LOW);
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  digitalWrite(LED_PIN, HIGH);
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue
  digitalWrite(LED_PIN, LOW);
  colorWipe(strip.Color(  0,   0,   0), 50); // Weird Purple
  delay(1000);
#endif
#if 0
  // Do a theater marquee effect in various colors...
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  digitalWrite(LED_PIN, HIGH);
  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  digitalWrite(LED_PIN, LOW);
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness
  digitalWrite(LED_PIN, HIGH);
  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
  digitalWrite(LED_PIN, LOW);
  rainbow(10);             // Flowing rainbow cycle along the whole strip
#endif
}

uint8_t setBrightness(uint8_t v, uint8_t brightness, bool gammaCorrect = true)
{
  uint8_t res = ((uint8_t)((((uint32_t)v) * ((uint32_t)brightness)) / ((uint32_t)255)));
  return gammaCorrect ? strip.gamma8(res) : res;
}

uint32_t strandGamma(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 255)
{
  r = setBrightness(r, brightness);
  g = setBrightness(g, brightness);
  b = setBrightness(b, brightness);
  if ((r < 20) && (g < 20) && (b > 20))
    r = g = b = 0;  
  return strip.Color(r, g, b);     
}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    if (wait)
    {
      strip.show();                          //  Update strip to match
      delay(wait);                           //  Pause for a moment
    }
  }
  if (!wait)
    strip.show();
}

void colorWipeSine(uint8_t r, uint8_t g, uint8_t b, int wait_ms, int duration_ms, int noisy, int steps)
{
  uint8_t brightness;
  int i;
  uint32_t color;
  int delay_ms;
  int d;
  
  delay_ms = duration_ms / (256 * (wait_ms ? wait_ms : 1));
  if (delay_ms > 5000)
    delay_ms = 5000;
  if (delay_ms < 1)
    delay_ms = 1;
  //printf("duration_ms:%d, delay_ms:%d, wait_ms:%d\n", duration_ms, delay_ms, wait_ms);
  Serial.print("duration_ms: ");
  Serial.print(duration_ms);
  Serial.print(" delay_ms: ");
  Serial.print(delay_ms);
  Serial.print(" wait_ms: ");
  Serial.println(wait_ms);
  
  for (i = 0; i < steps; i++)
  {
    brightness = (noisy ? (rand() % noisy) : 0) + (((uint32_t)strip.sine8(i % 256) * (255 - noisy))) / 255;
    color = strandGamma(r, g, b, brightness);
    colorWipe(color, wait_ms);
    d = delay_ms;
    delay(d);
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    //strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    strip.rainbow(firstPixelHue, 1, 255, 96, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
