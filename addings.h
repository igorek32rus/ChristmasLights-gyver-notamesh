//
#ifndef ADDINGS_H
#define ADDINGS_H
    
#if CANDLE_KOL >0
    ////////////////////////////////////////////////////////////// Свечи
    DEFINE_GRADIENT_PALETTE( candle_Pal ) {
      0,   255,  0,  0,   //red
     90,   255,255,255,  //full white
    180,   255,255,  0,   //bright yellow
    255,   255,255,255 }; //full white
    
    uint8_t PolCandle=1;    //Положение свечи
    
    void addcandle() 
    {
      uint16_t poz=PolCandle;
      CRGBPalette16 myPal = candle_Pal;
    
      if (NUM_LEDS > CANDLE_KOL)
      {
        uint16_t kol=NUM_LEDS /CANDLE_KOL;      //Количество свечей
        
        for ( uint16_t x = 0; x<kol; x++) 
        {
          if (poz < NUM_LEDS)
            leds[ poz ] = ColorFromPalette( myPal, random8(255));
          poz +=CANDLE_KOL;
        }
      }
    }
#endif

///////////////////////////////////////////////////////////////// Черный фон
void addbackground() 
{
  #if MAX_LEDS < 255
    uint8_t i; 
  #else
    uint16_t i;
  #endif

   for (i = 0; i < NUM_LEDS ; i++ ) 
    if (  (leds[i].r < 5) &&
          (leds[i].g < 5) &&
          (leds[i].b < 5) ) {
        #if     BACKGR_ON == 3 
          leds[i].r +=CRGB(5,5,5);
        #elif   BACKGR_ON == 2
          leds[i].g +=CRGB(5,5,5);
        #elif   BACKGR_ON >0 
          leds[i].b +=CRGB(5,5,5);
        #endif
          }  
}

////////////////////////////////////////////////////////////////// Блеск
void addglitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    #if MAX_LEDS < 255
      leds[ random8(NUM_LEDS) ] += CRGB::White;
    #else
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    #endif
  }
}

////////////////////////////////////////////////////////////////// Бенгальский огонь
void sparkler(uint8_t n)       //Бенгальский огонь
// 0 - нет эффектов
// 1 - Бенгальский огонь
// 2 - 1 яркий светодиод
// 3 - метеорит
// 4 - случайный эффект 
{ uint8_t kol=3;  

  if ( KolLed< 10 ) kol = KolLed /3;
  if (kol>=2)
  { uint8_t nn = n;
    switch (nn) {
      case 1 :  for ( uint8_t x = 0; x<kol; x++) leds[ KolLed-random8(kol*2) ] = CRGB::White; break;            //Бенгальский
      case 2 :  leds[KolLed-1] = CRGB::White; break;                                                            //1 яркий
      case 3 :  leds[KolLed] = CRGB::White;leds[KolLed-1] = CRGB::Red;leds[KolLed-2] = CRGB::Violet; break;     //Метеорит
    }
  }      
}

#if TOP_LENGTH >0
    ////////////////////////////////////////////////////////////// Обработка конца гирлянды
void top() {
  #if TOP_EFFECT == 0
    fill_solid(&leds[NUM_LEDS-TOP_LENGTH],TOP_LENGTH,TOP_COLOR);
  #else
      #if MAX_LEDS < 255  
      static  uint8_t x;
      #else
      static  uint16_t x;
      #endif

      fadeToBlackBy(&leds[NUM_LEDS-TOP_LENGTH], TOP_LENGTH, TOP_FADING);                  //Затухание к черному
      EVERY_N_MILLIS_I(toptimer, TOP_DELAY) {                                    // Sets the original delay time.
        #if TOP_EFFECT == 1
          #if TOP_LENGTH < 255  
            leds[NUM_LEDS-TOP_LENGTH+random8(0,TOP_LENGTH)] = TOP_COLOR;
          #else
            leds[NUM_LEDS-TOP_LENGTH+random16(0,TOP_LENGTH)] = TOP_COLOR;
          #endif 
        #elif TOP_EFFECT == 2
          if ( (x<=NUM_LEDS-TOP_LENGTH) || (x>=NUM_LEDS) ) x=NUM_LEDS-1;
          else                                             x--;
          leds[x] = TOP_COLOR;
        #else
          if ( (x<NUM_LEDS-TOP_LENGTH) || (x>=NUM_LEDS-1) ) x=NUM_LEDS-TOP_LENGTH;
          else                                             x++;
          leds[x] = TOP_COLOR;
        #endif
      }
  #endif
  } // top()
#endif

void BtnHandler()
{   //Обработчик нажатий кнопок

  ///////////////////////////////////////////////////////////////////////////////////////
  //            Для аналоговых кнопок
  
  #if KEY_ADC_ON == 1                                                         //Для аналоговых кнопок
    key_input_new = analogRead(PIN_KEY);                                      //прочитаем аналоговые кнопки
    if  ( ( ( (key_input - KEY_DELTA) > key_input_new) ||                     //Пришло новое значение отличное от прошлого
            ( (key_input + KEY_DELTA) < key_input_new) ) &&
          !key_bounce ) {                                                     // и еще ничего не приходило
      key_bounce = 1;                                                         //Начинаем обрабатывать
      key_time = millis();                                                    //Запомним время                 
    }
    else
      if (  key_bounce &&                                                     //Обрабатываем нажатия
            ((millis() - key_time) >= 50 )  ) {                               //Закончилось время дребезга
        key_bounce = 0;                                                       //Больше не обрабатываем
        key_input = key_input_new;
        #if LOG_ON == 1
            Serial.print(F("Analog Key: "));Serial.println(key_input);
        #endif

        #if KEY_0 >= KEY_DELTA
          if  ( ( (KEY_0 - KEY_DELTA) < key_input) &&                     
                ( (KEY_0 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_0
            Protocol = 1;
            Command = KEY_0;
          }
        #endif
        #if KEY_1 >= KEY_DELTA
          if  ( ( (KEY_1 - KEY_DELTA) < key_input) &&                     
                ( (KEY_1 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_1
            Protocol = 1;
            Command = KEY_1;
          }
        #endif
        #if KEY_2 >= KEY_DELTA
          if  ( ( (KEY_2 - KEY_DELTA) < key_input) &&                     
                ( (KEY_2 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_2
            Protocol = 1;
            Command = KEY_2;
          }
        #endif
        #if KEY_3 >= KEY_DELTA
          if  ( ( (KEY_3 - KEY_DELTA) < key_input) &&                     
                ( (KEY_3 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_3
            Protocol = 1;
            Command = KEY_3;
          }
        #endif
        #if KEY_4 >= KEY_DELTA
          if  ( ( (KEY_4 - KEY_DELTA) < key_input) &&                     
                ( (KEY_4 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_4
            Protocol = 1;
            Command = KEY_4;
          }
        #endif
        #if KEY_5 >= KEY_DELTA
          if  ( ( (KEY_5 - KEY_DELTA) < key_input) &&                     
                ( (KEY_5 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_5
            Protocol = 1;
            Command = KEY_5;
          }
        #endif
        #if KEY_6 >= KEY_DELTA
          if  ( ( (KEY_6 - KEY_DELTA) < key_input) &&                     
                ( (KEY_6 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_6
            Protocol = 1;
            Command = KEY_6;
          }
        #endif
        #if KEY_7 >= KEY_DELTA
          if  ( ( (KEY_7 - KEY_DELTA) < key_input) &&                     
                ( (KEY_7 + KEY_DELTA) > key_input) )  {                       //Нашли нажатую кнопку KEY_7
            Protocol = 1;
            Command = KEY_7;
          }
        #endif
      }
  #endif

  ///////////////////////////////////////////////////////////////////////////////////////
  //            Для кнопни подключенной к библиотеке GyverButton
  
  #if KEY_GYVER_ON >0  
  {
    btn1_gyver.tick();                                                          // обязательная функция отработки. Должна постоянно опрашиваться
    if (btn1_gyver.isSingle())         {                                        //Обработка одного клика
      Protocol = 1;Command = BTN1_1;
      #if LOG_ON == 1
          Serial.println(F("BTN1 Click"));
      #endif
      #if   LED_ON > 0
        LED1_FleshH(1);                                                         //мигнуть светодиодом 1
      #endif      
    }         
    if (btn1_gyver.isDouble())         {                                        //Обработка двойного клика
      Protocol = 1;Command = BTN1_2;
      #if LOG_ON == 1
          Serial.println(F("BTN1 2Click"));
      #endif
      #if   LED_ON > 0
        LED1_FleshH(2);                                                         //мигнуть 2 раза светодиодом 1
      #endif
    }
    if (btn1_gyver.isTriple())         {                                        //Обработка тройного клика
      Protocol = 1;Command = BTN1_3;
      #if LOG_ON == 1
          Serial.println(F("BTN1 3Click"));
      #endif
      #if   LED_ON > 0
        LED1_FleshH(3);                                                         //мигнуть 3 раза светодиодом 1
      #endif
    }
    if (btn1_gyver.hasClicks()) {
      if (btn1_gyver.getClicks() == 4) {                                        //Обработка четыре клика
        Protocol = 1;Command = BTN1_4;
      #if LOG_ON == 1
          Serial.println(F("BTN1 4Click"));
      #endif
      #if   LED_ON > 0
        LED1_FleshH(4);                                                         //мигнуть 3 раза светодиодом 1
      #endif
      }
    }
    if (btn1_gyver.isStep())           {                                        //Обработка удержания
      Protocol = 1;Command = BTN1_PRESS;
      #if LOG_ON == 1
          Serial.println(F("BTN1 Step"));
      #endif
      #if   LED_ON > 0
        LED1_FleshL(1);                                                          //мигнуть  светодиодом 1
      #endif
    }
    if (btn1_gyver.isStep(1))          {                                        //Обработка 1 нажатия и удержания
      Protocol = 1;Command = BTN1_1_PRESS;
      #if LOG_ON == 1
          Serial.println(F("BTN1 1+Step"));
      #endif
      #if   LED_ON > 0
        LED1_FleshL(2);                                                          //мигнуть 2 раза светодиодом 1
      #endif
    }
    if (btn1_gyver.isStep(2))          {                                        //Обработка 2 нажатий и удержания
      Protocol = 1;Command = BTN1_2_PRESS;
      #if LOG_ON == 1
          Serial.println(F("BTN1 2+Step"));
      #endif
      #if   LED_ON > 0
        LED1_FleshL(3);                                                          //мигнуть 3 раза светодиодом 1
      #endif
    }
    if (btn1_gyver.isStep(3))          {                                        //Обработка 3 нажатий и удержания
      Protocol = 1;Command = BTN1_3_PRESS;
      #if LOG_ON == 1
          Serial.println(F("BTN1 3+Step"));
      #endif      
      #if   LED_ON > 0
        LED1_FleshL(4);                                                         //мигнуть 4 раза светодиодом 1
      #endif
    }
    
    #if KEY_GYVER_ON >1
    {
      btn2_gyver.tick();                                                          // обязательная функция отработки. Должна постоянно опрашиваться
      if (btn2_gyver.isSingle())         {                                        //Обработка одного клика
        Protocol = 1;Command = BTN2_1;
        #if LOG_ON == 1
            Serial.println(F("BTN2 Click"));
        #endif
        #if   LED_ON > 0
          LED1_FleshH(1);                                                         //мигнуть светодиодом 1
        #endif      
      }         
      if (btn2_gyver.isDouble())         {                                        //Обработка двойного клика
        Protocol = 1;Command = BTN2_2;
        #if LOG_ON == 1
            Serial.println(F("BTN2 2Click"));
        #endif
         #if   LED_ON > 0
          LED1_FleshH(2);                                                         //мигнуть 2 раза светодиодом 1
        #endif
     }
      if (btn2_gyver.isTriple())         {                                        //Обработка тройного клика
        Protocol = 1;Command = BTN2_3;
        #if LOG_ON == 1
            Serial.println(F("BTN2 3Click"));
        #endif
         #if   LED_ON > 0
          LED1_FleshH(3);                                                         //мигнуть 3 раза светодиодом 1
        #endif
      }
      if (btn2_gyver.hasClicks()) {
        if (btn2_gyver.getClicks() == 4) {                                        //Обработка четыре клика
          Protocol = 1;Command = BTN2_4;
        #if LOG_ON == 1
            Serial.println(F("BTN2 4Click"));
        #endif
        #if   LED_ON > 0
          LED1_FleshH(4);                                                         //мигнуть 4 раза светодиодом 1
        #endif
        }
      }
      if (btn2_gyver.isStep())           {                                        //Обработка удержания
        Protocol = 1;Command = BTN2_PRESS;
        #if LOG_ON == 1
            Serial.println(F("BTN2 Step"));
        #endif
          #if   LED_ON > 0
          LED1_FleshL(1);                                                         //мигнуть  светодиодом 1
        #endif
     }
      if (btn2_gyver.isStep(1))          {                                        //Обработка 1 нажатия и удержания
        Protocol = 1;Command = BTN2_1_PRESS;
        #if LOG_ON == 1
            Serial.println(F("BTN2 1+Step"));
        #endif
         #if   LED_ON > 0
          LED1_FleshL(2);                                                         //мигнуть 2 раза светодиодом 1
        #endif
      }
      if (btn2_gyver.isStep(2))          {                                        //Обработка 2 нажатий и удержания
        Protocol = 1;Command = BTN2_2_PRESS;
        #if LOG_ON == 1
            Serial.println(F("BTN2 2+Step"));
        #endif
         #if   LED_ON > 0
          LED1_FleshL(3);                                                         //мигнуть 3 раза светодиодом 1
        #endif
      }
      if (btn2_gyver.isStep(3))          {                                        //Обработка 3 нажатий и удержания
        Protocol = 1;Command = BTN2_3_PRESS;
        #if LOG_ON == 1
            Serial.println(F("BTN2 3+Step"));
        #endif
         #if   LED_ON > 0
          LED1_FleshL(4);                                                         //мигнуть 4 раза светодиодом 1
        #endif
      }
      #if KEY_GYVER_ON >2 
      {
          btn3_gyver.tick();                                                          // обязательная функция отработки. Должна постоянно опрашиваться
          if (btn3_gyver.isSingle())         {                                        //Обработка одного клика
            Protocol = 1;Command = BTN3_1;
            #if LOG_ON == 1
                Serial.println(F("BTN3 Click"));
            #endif
            #if   LED_ON > 0
              LED1_FleshH(1);                                                         //мигнуть светодиодом 1
            #endif      
          }         
          if (btn3_gyver.isDouble())         {                                        //Обработка двойного клика
            Protocol = 1;Command = BTN3_2;
            #if LOG_ON == 1
                Serial.println(F("BTN3 2Click"));
            #endif
             #if   LED_ON > 0
              LED1_FleshH(2);                                                         //мигнуть 2 раза светодиодом 1
            #endif
          }
          if (btn3_gyver.isTriple())         {                                        //Обработка тройного клика
            Protocol = 1;Command = BTN3_3;
            #if LOG_ON == 1
                Serial.println(F("BTN3 3Click"));
            #endif
           #if   LED_ON > 0
            LED1_FleshH(3);                                                         //мигнуть 3 раза светодиодом 1
          #endif
          }
          if (btn3_gyver.hasClicks()) {
            if (btn3_gyver.getClicks() == 4) {                                        //Обработка четыре клика
              Protocol = 1;Command = BTN3_4;
            #if LOG_ON == 1
                Serial.println(F("BTN3 4Click"));
            #endif
             #if   LED_ON > 0
              LED1_FleshH(4);                                                         //мигнуть 4 раза светодиодом 1
            #endif
            }
          }
          if (btn3_gyver.isStep())           {                                        //Обработка удержания
            Protocol = 1;Command = BTN3_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN3 Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(1);                                                         //мигнуть  светодиодом 1
            #endif
          }
          if (btn3_gyver.isStep(1))          {                                        //Обработка 1 нажатия и удержания
            Protocol = 1;Command = BTN3_1_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN3 1+Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(2);                                                         //мигнуть 2 раза светодиодом 1
            #endif
          }
          if (btn3_gyver.isStep(2))          {                                        //Обработка 2 нажатий и удержания
            Protocol = 1;Command = BTN3_2_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN3 2+Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(3);                                                         //мигнуть 3 раза светодиодом 1
            #endif
          }
          if (btn3_gyver.isStep(3))          {                                        //Обработка 3 нажатий и удержания
            Protocol = 1;Command = BTN3_3_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN3 3+Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(4);                                                         //мигнуть 4 раза светодиодом 1
            #endif
          }    
          #if KEY_GYVER_ON >3  
          {
          btn4_gyver.tick();                                                          // обязательная функция отработки. Должна постоянно опрашиваться
          if (btn4_gyver.isSingle())         {                                        //Обработка одного клика
            Protocol = 1;Command = BTN4_1;
            #if LOG_ON == 1
                Serial.println(F("BTN4 Click"));
            #endif
            #if   LED_ON > 0
              LED1_FleshH(1);                                                         //мигнуть светодиодом 1
            #endif      
          }         
          if (btn4_gyver.isDouble())         {                                        //Обработка двойного клика
            Protocol = 1;Command = BTN4_2;
            #if LOG_ON == 1
                Serial.println(F("BTN4 2Click"));
            #endif
            #if   LED_ON > 0
            LED1_FleshH(2);                                                         //мигнуть 2 раза светодиодом 1
            #endif
         }
          if (btn4_gyver.isTriple())         {                                        //Обработка тройного клика
            Protocol = 1;Command = BTN4_3;
            #if LOG_ON == 1
                Serial.println(F("BTN4 3Click"));
            #endif
             #if   LED_ON > 0
              LED1_FleshH(3);                                                         //мигнуть 3 раза светодиодом 1
            #endif
          }
          if (btn4_gyver.hasClicks()) {
            if (btn4_gyver.getClicks() == 4) {                                        //Обработка четыре клика
              Protocol = 1;Command = BTN4_4;
            #if LOG_ON == 1
                Serial.println(F("BTN4 4Click"));
            #endif
             #if   LED_ON > 0
              LED1_FleshH(4);                                                         //мигнуть 4 раза светодиодом 1
            #endif
            }
          }
          if (btn4_gyver.isStep())           {                                        //Обработка удержания
            Protocol = 1;Command = BTN4_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN4 Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(1);                                                         //мигнуть светодиодом 1
            #endif
          }
          if (btn4_gyver.isStep(1))          {                                        //Обработка 1 нажатия и удержания
            Protocol = 1;Command = BTN4_1_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN4 1+Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(2);                                                         //мигнуть 2 раза светодиодом 1
            #endif
          }
          if (btn4_gyver.isStep(2))          {                                        //Обработка 2 нажатий и удержания
            Protocol = 1;Command = BTN4_2_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN4 2+Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(3);                                                         //мигнуть 3 раза светодиодом 1
            #endif
          }
          if (btn4_gyver.isStep(3))          {                                        //Обработка 3 нажатий и удержания
            Protocol = 1;Command = BTN4_3_PRESS;
            #if LOG_ON == 1
                Serial.println(F("BTN4 3+Step"));
            #endif
             #if   LED_ON > 0
              LED1_FleshL(4);                                                         //мигнуть 4 раза светодиодом 1
            #endif
          }
        }
        #endif
      }
      #endif
    }
    #endif
  }
  #endif
  
}


#endif
