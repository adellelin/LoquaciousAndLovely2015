#include <SPI.h>
#include <LPD8806.h>                              // C

  int n_strips = 5;
  LPD8806 head;
  LPD8806 leftFrontLeg;
  LPD8806 leftBackLeg;
  LPD8806 rightFrontLeg;
  LPD8806 rightBackLeg;
  LPD8806 strips[5];                                // C                        
//  LPD8806[] strips = new LPD8806[5];                  // Java

  // Offsets for the front-most LED
  int offsets[] = {                                 // C
//  int offsets[] = new int[] {                         // Java
    29,25,33,25,20
  };

  int mode = 0;
  int idx = 0;
  int n_modes = 19;
  int modes[] = {                                   // C
  //int modes[] = new int[] {                           // Java
      400, 250, 250, 250, 250, 200,                   // number of frames each mode goes for. Add here if 
      300, 300, 300, 300, 300, 600,
      50, 50, 50, 50, 50, 400,600
  };
  uint32_t currentColor;                            // C
//  color currentColor;                                 // Java

  int n_colors = 5;
  byte colors[] = {                                 // C
//  int colors[] = new int[] {                          // Java 
    107,18,76,   // Pink
    32,112,104,  // Turquoise
  
    0,17,51,     // Royal Blue
    110,25,32,    // Red
    204,122,33  // Ochre
  };
  
  uint32_t color(byte r, byte g, byte b) {          // C
    return ((uint32_t)(g | 0x80) << 16) |           // C
           ((uint32_t)(r | 0x80) <<  8) |           // C
                       b | 0x80 ;                   // C
  }                                                 // C     
                                                    // C
  byte red(uint32_t color) {                        // C
    return (byte)(color & 0x7F00) >> 8;             // C
  }                                                 // C
                                                    // C
  byte green(uint32_t color) {                      // C
    return (byte)(color & 0x7F0000)>>16;            // C
  }                                                 // C
                                                    // C
  byte blue(uint32_t color) {                       // C
    return (byte)(color & 0x7F);                    // C
  }                                                 // C

  void setup() {    
    Serial.begin(9600);
    Serial.println("Setup strips");
    strips[0] = head = LPD8806(100,2,7);                // C
    strips[1] = leftFrontLeg = LPD8806(80,1,6);         // C
    strips[2] = leftBackLeg = LPD8806(80,0,5);          // C
    strips[3] = rightBackLeg = LPD8806(80,3,8);         // C
    strips[4] = rightFrontLeg = LPD8806(80,4,16);        // C
//    strips[0] = head = new LPD8806(100);              // Java
//    strips[1] = leftFrontLeg = new LPD8806(80);       // Java
//    strips[2] = leftBackLeg = new LPD8806(80);        // Java
//    strips[3] = rightBackLeg = new LPD8806(80);       // Java
//    strips[4] = rightFrontLeg = new LPD8806(80);      // Java

    for (int i=0; i<n_strips; i++) {
      strips[i].begin();
    }
  }

 uint32_t stepColor(byte r, byte g, byte b, int step, float divisor) {   // C
//  color stepColor(int r, int g, int b, int step, float divisor) {          // Java
    r = int(min(127,step/divisor*r));
    g = int(min(127,step/divisor*g));
    b = int(min(127,step/divisor*b));

    return color(r,g,b);
  }
  
  int bound(int n, int max) {
    while (n >= max) n -= max;
    while (n < 0) n += max;
    return n;
  }
  
  void gallop(int idx) {
    int tailsize = 20;
    int x;
    int step;
    int len;
    int hlen;
    
    clear();
    
    for (int i=0; i<n_strips; i++) {
      len = strips[i].numPixels();
      hlen = ((tailsize+len)/2) + tailsize/2;
      step = (idx + i*8) % hlen - tailsize - tailsize/2;
      
      for (int j=0; j<tailsize && step+j<=len/2; j++) {
        if (step+j>=0) {
          x = bound(offsets[i]+step+j, len); 
          // stepColor(byte r, byte g, byte b, int step, float divisor) 
          strips[i].setPixelColor(x,stepColor(127,127,127,j,tailsize));
        
          x = bound(offsets[i]-step-j, len);
          strips[i].setPixelColor(x,stepColor(127,127,127,j,tailsize));
        }
      }
      
      strips[i].show();
    }
  }
 

  
  void colorBleed(int idx, int r, int g, int b, float div) {
    int x;
    
    for (int i=0; i<n_strips; i++) {
      x = strips[i].numPixels()/2;
      
      strips[i].setPixelColor(x,stepColor(r,g,b,idx,div));
      
      for (int j=1; j<idx; j++) {
        if (x+j<strips[i].numPixels())
        //what is idx?
          strips[i].setPixelColor(x+j, stepColor(r,g,b,idx-j,div));
        
        if (x-j>=0)
          strips[i].setPixelColor(x-j, stepColor(r,g,b,idx-j,div));
      }
      
      strips[i].show();
    }
  }
  
  void colorWipe(int idx, uint32_t col) {            // C
//  void colorWipe(int idx, color col) {                 // Java
    for (int strip = 0; strip < 5; strip++) {
      for (int led = 0; led < strips[strip].numPixels (); led++) {

        if ( led <= ((strips[strip].numPixels() / float(modes[mode])) * idx)) {
          strips[strip].setPixelColor(led, col);
        } else {
          // Leave it be
        }
      }
      strips[strip].show();
    }
  }

  void rainbowWarp(int idx) {
    if (idx % 10 == 0) {
      for (int i = 0; i < 5; i++) {
        for (int section = 0; section < 5; section++) {
            int led = (strips[i].numPixels() / 5) * section;
            int col = int(random(n_colors));
            int step = int(random(20));
            //strips[i].setPixelColor(led, color(random(50, 255), random(50, 255), random(50, 255)));
            strips[i].setPixelColor(led, stepColor(colors[col],colors[col+1],colors[col+2],step,10.0));
        }
      }
    }

    for (int strip = 0; strip < 5; strip++) {
      for (int led = strips[strip].numPixels () - 1; led > 0; led--) {
        strips[strip].setPixelColor(led, strips[strip].getPixelColor(led - 1));
      }
      strips[strip].show();
    }
  }

 //  void blinkyBlinky(int idx, uint32_t col) {            // C
    void blinkyBlinky(int idx, int r, int g, int b, float div) {              //Java
  for (int strip = 0; strip < 5 ; strip++) {
      for (int led = 0; led < strips[strip].numPixels (); led++) {
        if ((idx % 30 >= 0) && (idx% 30 < 10 )) {
          if (strip == 0) {
          strips[strip].setPixelColor(led, stepColor(r,g,b,idx,div));
          // strips[int(random(strip))].setPixelColor(led, col);
          //delay(1);
          } else {
          strips[strip].setPixelColor(led, color(0, 0, 0));
        }
      } else if ((idx % 30 >= 10) && (idx% 30 <20)) {
          if ((strip == 1) || (strip ==3)) {
          strips[strip].setPixelColor(led, stepColor(r,g,b,idx,div));
      } else {
          strips[strip].setPixelColor(led, color(0, 0, 0));
        }
      } 
       else if ((idx % 30 >= 20) && (idx % 30 < 30)) {
          if ((strip == 2) || (strip ==4)) 
          strips[strip].setPixelColor(led, stepColor(r,g,b,idx,div));
      } else {
          strips[strip].setPixelColor(led, color(0, 0, 0));
        }
      } 
      strips[strip].show();
    }
    }

    
    void blinkyBlinky2(int idx, uint32_t col) {            // C
    //void blinkyBlinky2(int idx, int r, int g, int b, float div) {              //Java
    for (int strip = 0; strip < 5 ; strip++) {
      for (int led = 0; led < strips[strip].numPixels (); led++) {
        if (idx % 24 <= 12 ) {
          strips[strip].setPixelColor(led, col);
          // strips[int(random(strip))].setPixelColor(led, col);
          //delay(1);
          } else {
          strips[strip].setPixelColor(led, color(250, 0, 0));
        }
      }  
      strips[strip].show();
    }
    }
    
// void sparklySparkly(int idx) {
//  for (int i = 0; i < 5; i++) {
//    for (int led = 0; led < strips[i].numPixels(); led++) {
//          if (idx % 32 == 0){
//            if (random(5)> 3) {
//            strips[i].setPixelColor(led, color(30, 30, 30));
//        } else {
//         strips[i].setPixelColor(led, color(0, 0, 0));
//        }
//      }  else if ((idx % 32 > 0) && (idx % 32 <= 16)){
//           for (float j = 0; j < 5; j++) {
//            //if (random(3) > 2) {
//            strips[i].setPixelColor(led, color( red(strips[i].getPixelColor(led))*(1+(j/10)),
//                  green(strips[i].getPixelColor(led))*(1+j/10),
//                  blue(strips[i].getPixelColor(led))*(1+j/10)));
//
//              }
//           } else if ((idx % 32 > 16) && (idx % 32 <= 32)){
//           for (float j = 0; j < 5; j++) {
//            //if (random(3) > 2) {
//            strips[i].setPixelColor(led, color( red(strips[i].getPixelColor(led))/(1+(j/10)),
//                  green(strips[i].getPixelColor(led))/(1+j/10),
//                  blue(strips[i].getPixelColor(led))/(1+j/10)));
//
//              }
//           }
//          }
//        strips[i].show();
//      }  
//    }

 void sparklySparkly(int idx) {
  for (int i = 0; i < 5; i++) {
    for (int led = 0; led < strips[i].numPixels(); led++) {
          if (idx % 32 == 0){
            if (random(5)> 3) {
            strips[i].setPixelColor(led, color(60, 60, 60));
        } else {
         strips[i].setPixelColor(led, color(0, 0, 0));
        }
       }
      }
        strips[i].show();
      }  
    }

  void clear() {
    for (int i=0; i<n_strips; i++) {
      for (int led=0; led<strips[i].numPixels(); led++) {
        strips[i].setPixelColor(led, color(0,0,0));
      }
    }
  }

  void loop() {
    idx++;
    if (idx == modes[mode]) {
     
      idx = 0;
      mode++;

      if (mode == n_modes) mode = 0;
      Serial.print("Mode is now ");
      Serial.print(mode);
      Serial.println();
    }

    switch(mode) {
      case 0:
        // sparklySparkly(idx);
        gallop(idx);
        //rainbowWarp(idx);
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        colorBleed(idx, colors[(mode-1)*3], colors[(mode-1)*3+1], colors[(mode-1)*3+2], modes[mode] / 4.0);
        break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
       colorWipe(idx, color(colors[(mode-6)*3], colors[(mode-6)*3+1], colors[(mode-6)*3+2]));
       break;
      case 11:
        rainbowWarp(idx);
        break;
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
        blinkyBlinky(idx, colors[(mode-12)*3], colors[(mode-12)*3+1], colors[(mode-12)*3+2], modes[mode]/4.0);
        break;
      case 17:
//        blinkyBlinky2(idx, color(colors[(mode-13)*3], colors[(mode-13)*3+1], colors[(mode-13)*3+2]));
        blinkyBlinky2(idx, color(255,255,10));
        break;
      case 18:
        sparklySparkly(idx);
        break;
    }
    delay(10);
  }

