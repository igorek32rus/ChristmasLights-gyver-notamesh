//
#ifndef RUNNING_FIRE_H
#define RUNNING_FIRE_H


const PROGMEM uint8_t prog_run_fire[]= { PROG_RUN_FIRE };            //массив программ
const uint16_t prog_run_fire_count = sizeof( prog_run_fire );       //размер программы 
                       
void running_fire(uint8_t prog, uint8_t effect, uint8_t clear) 
{   //Бегущая строка
    //prog = номер программы
    //effect = номер эффекта
    const uint8_t speed = 100;                                      // скорость программы (1-255) 
    static uint8_t tek_prog = 255;                                  // текущая программа
    static uint8_t tek_effect = 255;                                // текущий эффект
    static uint8_t tek_clear = 255;                                 // текущий эффект очистки
    static uint16_t tek_index_prog = 0;                             // текущий указатель на программу
    static uint8_t tek_step = 0;                                    // текущий шаг 
    static uint8_t tek_step2 = 0;                                   // текущий шаг 

    if (  (prog != tek_prog) || (effect != tek_effect) || (clear != tek_clear) )
    { //Инициализация
      tek_prog = prog; tek_effect = effect; tek_clear = clear;      //Сохраним
      //Узнаем сколько у нас программ
      uint8_t kol_prog =0;                                          //пока нашли 0 программ
      tek_index_prog =0;                                            // указатель на начало программы
      while ( tek_index_prog<prog_run_fire_count)
       {  kol_prog++;                                               //Нашли программу
          tek_index_prog += pgm_read_byte(prog_run_fire+tek_index_prog)+2;//Прыгаем на следующий эффект
       }
      // определим текущую программу
      tek_prog = tek_prog % kol_prog;                               //Текущая программа
      // определим указатель
      kol_prog =0;                                                  //пока нашли 0 программ
      tek_index_prog =0;                                            // указатель на начало программы
      while ( (tek_index_prog<prog_run_fire_count) && (kol_prog != tek_prog))
       {  kol_prog++;                                               //Нашли программу
          tek_index_prog += pgm_read_byte(prog_run_fire+tek_index_prog)+2;//Прыгаем на следующий эффект
       }
    }
    // Эффекты
  if (  ((tek_index_prog + pgm_read_byte(prog_run_fire+tek_index_prog) +1) <= prog_run_fire_count) &&
        (pgm_read_byte(prog_run_fire+tek_index_prog)>0)  &&
        (pgm_read_byte(prog_run_fire+tek_index_prog+1)>0) &&
        (pgm_read_byte(prog_run_fire+tek_index_prog+1)<=8) )
  { //есть программа  
    uint8_t kol_bit = pgm_read_byte(prog_run_fire+tek_index_prog+1);//количество бит в программе
    uint8_t step = tek_step % kol_bit;                              //Расчитаем шаг в программе
    if (thisdir == -1) step = kol_bit - step - 1;
    uint8_t tek_bit = pgm_read_byte(prog_run_fire+tek_index_prog+step+2);//Расчитаем ячейку в программе
    #if MAX_LEDS < 255
      uint8_t i; 
    #else
      uint16_t i;
    #endif

    if (tek_clear)  fadeToBlackBy(leds, KolLed, tek_clear*5 +1 );
    for (i = 0; i < KolLed; i++ ) { 
      uint8_t x;
      if (tek_effect < 2) x=tek_step*10+tek_step2+ (tek_effect & 1)?i:0;
      else                x=32*step+(tek_effect & 1)?tek_step*10+tek_step2:0;
      if (!tek_clear)                        leds[i] = CRGB::Black;      //очистим
      if ( tek_bit & (0x01 << (i % kol_bit)))                            //установим
        leds[i] = ColorFromPalette(gCurrentPalette,x, 255, currentBlending);
    }
  }
  if (tek_step2 < 10) tek_step2++;
  else
  {   tek_step2 =0;
      tek_step++;
  }

} // running_fire()


#endif
