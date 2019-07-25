#include "FastLED.h"


// SETTINGS

#define LEDS_PIN A0    // label near hole with green wire, does not matter analog or digital
#define SIZE 16        // amount of leds in one edge  (16 for 16x16 matrix)

#define BLOBS_NUM 4    // amount blobs. lot of blobs = laggs
#define BLOBS_SIZE 30  // select empirically depending on the amount
#define BLOBS_SPEED 2  // speed of blobs

#define BRIGHTNESS 255 // brightness from 1 to 255

#define ZIGZAG true    // zigzag or progressive matrix layout
#define TOR_X true     // toroid space on x axis
#define TOR_Y false    // toroid space on y axis



// SETTINGS END

#define SCALE 16      // multiplier of mapsize. instead of float coords
#define LEDS_NUM SIZE*SIZE
#define MAPSIZE SIZE*SCALE


class Blob {
  public:
   uint8_t x;
   uint8_t y;
   int8_t x_m;
   int8_t y_m;
   int r;
   
   Blob() {
     x = random(MAPSIZE);
     y = random(MAPSIZE);
     r = BLOBS_SIZE * random(100, 160);
     x_m = random(1, 3) * BLOBS_SPEED;
     y_m = random(1, 3) * BLOBS_SPEED;
   };
   
  void update() {
     if (x <= -x_m || x >= MAPSIZE - x_m) {
     #if !TOR_X
         x_m *= -1;
     #elif MAPSIZE != 256 
         x = MAPSIZE - x;
     #endif
     }

     if (y <= -y_m || y >= MAPSIZE - y_m) {
     #if !TOR_Y
         y_m *= -1;
     #elif MAPSIZE != 256 
         x = MAPSIZE - x;
     #endif    
     }

     x += x_m;
     y += y_m;
  }
  
  float dist(uint8_t x_, uint8_t y_) {
   int xd = abs(x_ - x);
   int yd = abs(y_ - y);

   #if TOR_X
     xd = min(xd, MAPSIZE-xd);
   #endif
 
   #if TOR_Y
     yd = min(yd, MAPSIZE-yd);
   #endif
    
   return (float) r / sqrt(((long) xd*xd + (long)yd*yd + 1));
  }
  
};

CRGB leds[LEDS_NUM];  
Blob blobs[BLOBS_NUM];  

void setup() {
  randomSeed(analogRead(0));
  FastLED.addLeds<WS2811, LEDS_PIN, RGB>(leds, LEDS_NUM); 
  for (byte b = 0; b < BLOBS_NUM; b++)
    blobs[b] = Blob();
  
 for (byte b = 0; b < BLOBS_NUM / 2; b++) {
   if (TOR_X) blobs[b].x_m *= -1;
   if (TOR_Y) blobs[b].y_m *= -1;
 }
  
}


uint8_t x, y, x_s, y_s, b, i;
float sum;

void loop() {
  for (b = 0; b < BLOBS_NUM; b++) 
      blobs[b].update();
      
   for (x = 0; x < SIZE; x++) {
       x_s = x * SCALE;
       
       for (y = 0; y < SIZE; y++) {
          
            #if ZIGZAG
              i = x*SIZE + (x&1 ? SIZE-1-y : y);  // x&1 == x%2;
            #else 
              i = x*SIZE + y;
            #endif
                      
            y_s = y * SCALE;
            sum = 0;
            
            for (b = 0; b < BLOBS_NUM; b++) 
                sum += blobs[b].dist(x_s, y_s);
                
            sum = constrain(sum, 0, 350);
            leds[i] = CHSV(sum, 255, BRIGHTNESS);
        }
        
    }

    FastLED.show(); 
}
