//
#ifndef NOISE16_PAL_H
#define NOISE16_PAL_H

// noise16 variables -----------------------------------------------------------------------

uint32_t  x,hue_time;                                         // x & hue_time values
uint8_t octaves=2;       //2                                  // how many octaves to use for the brightness
uint8_t hue_octaves=3;   //3                                  // how many octaves to use for the hue
int xscale=57771;        //57771                              // the 'distance' between points on the x axis
uint32_t hxy = 43213;    //43213                              // not sure about this
int hue_scale=20;        //1                                  // the 'distance' between points for the hue noise
int hue_speed = 1;       //31                                 // how fast we move through hue noise
uint8_t x_speed = 0;     //331                                // adjust this value to move along the x axis between frames
int8_t hxyinc = 3;       //3

void noise16_pal() {
  if (KolLed>=10) {
        if (KolLed<=DEV_NOISE16)      
          fill_noise16(leds, KolLed, octaves, x, xscale, hue_octaves, hxy, hue_scale, hue_time);
        else {
          int8_t a;
          for (a=0; a<(KolLed/DEV_NOISE16); a++)
              fill_noise16(&leds[DEV_NOISE16*a], DEV_NOISE16, octaves, x, xscale, hue_octaves, hxy, hue_scale, hue_time);
          if (KolLed > (DEV_NOISE16*a) )
              fill_noise16(&leds[DEV_NOISE16*a], KolLed-DEV_NOISE16*a, octaves, x, xscale, hue_octaves, hxy, hue_scale, hue_time);
        }
      hxy+=hxyinc*thisdir;
      x+=x_speed*thisdir;
      hue_time+=hue_speed*thisdir;
  }
} // noise16_pal()

#endif
