/*
 * notamesh - IR controlled 'synchronized' led lighting effects using FastLED across multiple Arduino controllers.
 * 
 *       By: Andrew Tuline
 *     Date: October, 2018
 *      URL: www.tuline.com
 *    Email: atuline@gmail.com
 *     Gist: https://gist.github.com/atuline
 *   GitHub: https://github.com/atuline
 * Pastebin: http://pastebin.com/u/atuline
 *  Youtube: https://www.youtube.com/user/atuline/videos
 * 
 */

//  Переделка и дополнение
//  октябрь 2021
//  Бикин Дмитрий
//  d.bikin@bk.ru
//  Обсуждение проекта https://mysku.ru/blog/aliexpress/68990.html
//  Проект делается на чистом энтузиазме и в свободное время
//  Яндекс кошелек     410011655180757

//  Используются библиотеки
//  FastLED       https://github.com/FastLED/FastLED
//  IRremote      https://github.com/z3t0/Arduino-IRremote
//  GyverButton   https://alexgyver.ru/gyverbutton/
//  GyverTM1637   https://alexgyver.ru/tm1637_display/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include <ESP8266mDNS.h>

#include "mainPage.h"


AsyncWebServer server(80);

void handleRoot(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", MAIN_page);
}

//==============================================================================
//                                   Настройка
//==============================================================================

// 1. Выбрать периферию
// 2. Настроить выбранную периферию
// 3. При выборе пульта или кнопок настроить список команд
// 4. Настроить эффекты

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//                          Подключенная периферия
#define FastLED_ON    1           // Если равен 1  используется FastLED (даже если 0 все равно используется FastLED :)
#define LED_ON        0           // Количество светодиодов (0- светодиоды не используются, максимум 2 светодиода 
#define IR_ON         1           // Если равен 1  используется пульт библиотеки IRremote
#define KEY_ADC_ON    0           // Если равен 1  используются кнопки подключенные к аналоговому входу
#define KEY_GYVER_ON  0           // Количество кнопок подключенных к библиотеке GyverButton (0- библиотека отключена, максимум 4 кнопки) 
#define TM1637_ON     0           // Если равен 1  используются индикатор из библиотеки GyverTM1637

#define LOG_ON        1           // Включить лог  1 - включить лог

//                          Настройка перефирии чуть дальше ;)
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//********************************************************************************
//                          Настройка FastLED

#define MAX_LEDS      254        // Максимальное количество светодиодов,  очень влияет на память 

#define COLOR_ORDER   GRB         //Очередность цвета светодиодов  RGB или GRB
#define CHIPSET       WS2812B     //Тип микросхемы светодиодов в гирлянде

                                  // Если у вас установлены разные гирлянды (с разной очередностью цветов), то можно цвета их сделать одинаковыми
#define COOP_COLOR_START  0       // Если 0 ничего не делаем, начальный светодиод очередность цвета у которого отличается от основного
#define COOP_COLOR_END    200     // конечный светодиод очередность цвета у которого отличается от основного, но не больше MAX_LEDS
#define COOP_COLOR_WHAT   RB      // варианты RB - красный и синий, RG - красный и зеленый, GB - синий и зеленый                               

#define LED_DT        D4          // Номер пина куда подключена гирлянда
//#define LED_CK        11          // Номер пина для подключения тактов, применяется для светодиодов WS2801 или APA102
                                  //ЕСЛИ НЕ ИСПОЛЬЗУЕТСЯ ЗАКОМЕНТИРОВАТЬ

#define POWER_V       5           // напряжение блока питания в Вольтах
#define POWER_I       10000        // Ток блока питания в миллиАмперах

//********************************************************************************

//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//                                Параметры пульта
//  Настройка действия кнопок настраивается ниже
//  Для экономии памяти можно отключить часть протоколов, для этого в файле IRremote.h библиотеке IRremote удалить или 
// закомментировать строчки начинающиеся на DECODE и SEND они выглядят так:
// #define DECODE_BOSEWAVE      1
// #define SEND_BOSEWAVE        1
                                                           
#define PIN_IR        14           // Номер пина, куда подключен датчик пульта,
#define IR_REPEAT     1           //Включить повтор, при долгом нажатии на кнопку пульта будет повтор клавиши

//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//                                Параметры кнопок подключенных к аналоговому входу

#define PIN_KEY       A3          // Номер пина, куда подключены кнопки
                                  //Значения смотреть в протоколе после строки Analog Key     
#define KEY_0         10          // Значение соответствующее нажатой 0 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_1         70          // Значение соответствующее нажатой 1 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_2         140         // Значение соответствующее нажатой 2 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_3         280         // Значение соответствующее нажатой 3 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_4         360         // Значение соответствующее нажатой 4 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_5         520         // Значение соответствующее нажатой 5 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_6         700         // Значение соответствующее нажатой 6 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_7         900         // Значение соответствующее нажатой 7 кнопке
                                  // если значение меньше KEY_DELTA, то кнопка не используется
#define KEY_DELTA     5           // погрешность значения кнопки, то есть от -KEY_DELTA до +KEY_DELTA

//********************************************************************************
//                          Настройка Светодиодов
#define LED1_PIN       26           // Номер пина, куда подключен 1 светодиод
#define LED2_PIN       25           // Номер пина, куда подключен 2 светодиод

//Команды связанные со светодиодами
#define LED1_On           digitalWrite(LED1_PIN, HIGH);           //Включить светодиод 1
#define LED1_Off          digitalWrite(LED1_PIN, LOW);            //Выключить светодиод 1
#define LED1_FleshH(x)    Led1_flesh = 4*x;           //Мигнуть быстро x раз 1 светодиодом    (1-15)  
#define LED1_Flesh(x)     Led1_flesh = 64+(8*x);      //Мигнуть x раз 1 светодиодом           (1-7)
#define LED1_FleshL(x)    Led1_flesh = 128+(16*x);    //Мигнуть медленно x раз 1 светодиодом  (1-7)     
#define LED2_On           digitalWrite(LED2_PIN, HIGH);           //Включить светодиод 2
#define LED2_Off          digitalWrite(LED2_PIN, LOW);            //Выключить светодиод 2
#define LED2_FleshH(x)    Led2_flesh = 4*x;           //Мигнуть быстро x раз 2 светодиодом    (1-15) 
#define LED2_Flesh(x)     Led2_flesh = 64+8*x;        //Мигнуть x раз 2 светодиодом           (1-7)
#define LED2_FleshL(x)    Led2_flesh = 128+16*x;      //Мигнуть медленно x раз 2 светодиодом  (1-7)     

//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//                                Параметры кнопок подключенных к библиотеке GyverButton

//  Максимальное количество кнопок 4
//

#define BTN1_PIN       3           // Номер пина, куда подключена кнопка 1
#define BTN2_PIN       4           // Номер пина, куда подключена кнопка 2
#define BTN3_PIN       5           // Номер пина, куда подключена кнопка 3
#define BTN4_PIN       6           // Номер пина, куда подключена кнопка 4

#define BTN1_LOW       1            // Кнопка 1 0- привязка к питанию 1- привязка к земле
#define BTN2_LOW       1            // Кнопка 2 0- привязка к питанию 1- привязка к земле
#define BTN3_LOW       1            // Кнопка 3 0- привязка к питанию 1- привязка к земле
#define BTN4_LOW       1            // Кнопка 4 0- привязка к питанию 1- привязка к земле

// Команды связанные с действиями кнопки, менять не надо, просто используем
#define BTN1_1         1000        // кнопка 1 клик
#define BTN1_2         1001        // кнопка 1 два клика
#define BTN1_3         1002        // кнопка 1 три клика
#define BTN1_4         1003        // кнопка 1 четыре клика
#define BTN1_PRESS     1005        // кнопка 1 удержание
#define BTN1_1_PRESS   1006        // кнопка 1 один клик + удержание
#define BTN1_2_PRESS   1007        // кнопка 1 два клика + удержание
#define BTN1_3_PRESS   1008        // кнопка 1 три клика + удержание
#define BTN2_1         1100        // кнопка 2 клик
#define BTN2_2         1101        // кнопка 2 два клика
#define BTN2_3         1102        // кнопка 2 три клика
#define BTN2_4         1103        // кнопка 2 четыре клика
#define BTN2_PRESS     1105        // кнопка 2 удержание
#define BTN2_1_PRESS   1106        // кнопка 2 один клик + удержание
#define BTN2_2_PRESS   1107        // кнопка 2 два клика + удержание
#define BTN2_3_PRESS   1108        // кнопка 2 три клика + удержание
#define BTN3_1         1200        // кнопка 3 клик
#define BTN3_2         1201        // кнопка 3 два клика
#define BTN3_3         1202        // кнопка 3 три клика
#define BTN3_4         1203        // кнопка 3 четыре клика
#define BTN3_PRESS     1205        // кнопка 3 удержание
#define BTN3_1_PRESS   1206        // кнопка 3 один клик + удержание
#define BTN3_2_PRESS   1207        // кнопка 3 два клика + удержание
#define BTN3_3_PRESS   1208        // кнопка 3 три клика + удержание
#define BTN4_1         1300        // кнопка 4 клик
#define BTN4_2         1301        // кнопка 4 два клика
#define BTN4_3         1302        // кнопка 4 три клика
#define BTN4_4         1303        // кнопка 4 четыре клика
#define BTN4_PRESS     1305        // кнопка 4 удержание
#define BTN4_1_PRESS   1306        // кнопка 4 один клик + удержание
#define BTN4_2_PRESS   1307        // кнопка 4 два клика + удержание
#define BTN4_3_PRESS   1308        // кнопка 4 три клика + удержание

//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//                                Параметры индикатора из библиотеки GyverTM1637
#define TM1637_CLK    2           // Номер пина, куда подключен индикатор
#define TM1637_DIO    3  
#define TM1637_BRIGHT 7           //Яркость

//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//                                Настройка скорости лога
#define SERIAL_BAUDRATE 115200                                 // Or 115200.

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///                               Настройка эффектов
//////////////////////////////////////////////////////////////////////////////////////


#define BLACKSTART    1           // Первый запуск делать с черного экрана 0- начинать с эффекта, 1- начинать с черного экрана

#define GLITER_ON     0           // Включить блеск 0 - блеск отключен, 1- блеск включен

#define CANDLE_KOL    7           // если больше 0 то будут включена имитация свечей
                                  // число задает через какое расстояние друг от друга располагать свечи
#define CANDLE_ON     0           // Если включена имитация свечей то 1 = при запуске она будет включена
                                  //                                  0 = при запуске она будет выключена

#define CANDLE_40     1           // Управление 40 эффектом (свечи)   1 = при запуске она будет включена по варианту efimova1969
                                  //                                  иначе при запуске она будет включена по моему варианту 

#define BACKGR_ON     7           // Включить заполнение черного цвета фоном 1 - синий фон, 2 - зеленый, 3 - красный
                                  // если стоит SAVE_EEPROM  1 , то параметр хранится в EEPROM и запоминается в момент сохранения настроек


#define DEMO_MODE     2           // Режим при включении 
                                  // 0 - последняя запущенная программа
                                  // 1 - демо режим с последовательным перебором эффектов
                                  // 2 - демо режим с случайным перебором эффектов
                                  // 3 - демо режим с последовательным перебором выбранных эффектов
                                  // 4 - демо режим с случайным перебором выбранных эффектов
                                  
#define MY_MODE     0,1,2,4,5,9,10,11,14,16,17,18,19, \
                    21, 22, 28, 30, 32, 34, 36, 37, 38
                                  //Выбранные эффекты пишутся номера через запятую, 
                                  // можно писать и одинаковые что актуально в демо 3
                                  // Если закомментировать, то демо 3 и 4 отключаются

#define DEMO_TIME     60           // Время отображение эффекта в демо режиме в секундах

#define CHANGE_ON     1           // Включить плавную смену эффектов
#define CHANGE_SPARK  1           // Эффект смены 0 - без эффекта, 1 - искры (бенгальский огонь) 2 - яркий свет
                                  //              3 - метеорит,    4 - случайный эффект (из 1-3)
#define CHANGE_TIME   2           // Время в секундах, за которое произойдет плавная смена эффектов

#define DIRECT_TIME   20          // Через сколько секунд менять направление если 0 - не меняем
#define PALETTE_TIME  30          // Через сколько секунд менять палитру если 0 - не меняем
#define PALETTE_SPEED 20          // скорость перехода с одной палитры в другую (1- 48)
                                                                    
#define DEV_NOISE16 100           // если Зависает на 22 и 37 режимах, то уменьшаем число

#define SAVE_EEPROM   1           // Сохранять настройки запуска в EEPROM
                                  // 0 - ничего не хранить
                                  // 1 - хранить стартовый режим, длинна гирлянды, скорости и расширенные настройки 
                                  //                        (очередность цвета,состояние блеска, фона, свечей)
                                  // 2 - хранить стартовый режим, длинна гирлянды, скорости

#define TOP_POSITION  0           // Позиция вершины елки (используется только в эффекте огня) 
                                  //        0 - не определена 1 - Вершина в начале гирлянды 2 - Вершина в конце гирлянды
#define TOP_LENGTH    0           // Выделять вершину цветом 0 - не выделять, больше 0  сколько светодиодов выделять в вершине
#define TOP_COLOR     CRGB::Red   // Цвет выделения вершины
#define TOP_EFFECT    2           // 0 - Просто залить цветом, 1 - случайное переливание, 2- переливание сверху вниз 3- переливание снизу вверх
#define TOP_DELAY     150         // Задержка эффекта в милисекундах
#define TOP_FADING    5           // Затухание

//////////////////////////////////////////////////////////////////////////////////////
//эффект бегущий огонь из 80-90 годов
#define RUNNING_FIRE  0           // 1 Включить эффекты бегущего огня, начинаются с 43 команды
                                  // Программы бегущего огня
                                  // описание параметров                                   
                                  // первое число - кол-во ячеек в программе (от 1 до 8) показывает сколько шагов в эффекте
                                  // второе число - разрядность эффекта      (от 1 до 8) показывает сколько светодиодов используется в эффекте
                                  // остальные числа это программа, количество этих чисел задается в первом числе, один байт один шаг
                     
#define PROG_0      8,  8, 0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001           // 8 шагов, 8 светодиодов, 1 шаг горит 8 светодиод, 2 шаг горит 7 светодиод, 3 шаг горит 6 светодиод, ... , 8 шаг горит 1 светодиод 
#define PROG_1      7,  7, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001                       // 7 шагов, 7 светодиодов, 1 шаг горит 7 светодиод, 2 шаг горит 6 светодиод, 3 шаг горит 5 светодиод, ... , 7 шаг горит 1 светодиод 
#define PROG_2      6,  6, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001
#define PROG_3      5,  5, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001
#define PROG_4      4,  4, 0b00001000, 0b00000100, 0b00000010, 0b00000001
#define PROG_5      3,  3, 0b00000100, 0b00000010, 0b00000001                                                                       // 3 шага, 3 светодиода, 1 шаг горит 3 светодиод, 2 шаг горит 2 светодиод, 3 шаг горит 1 светодиод 
#define PROG_6      8,  8, 0b11100000, 0b01110000, 0b00111000, 0b00011100, 0b00001110, 0b00000111, 0b10000011, 0b11000001           // 8 шагов, 8 светодиодов, 1 шаг горят 8,7,6 светодиоды, 2 шаг горят 7,6,5 светодиоды, 3 шаг горят 6,5,4 светодиоды, ... , 8 шаг горят 1,8,7 светодиоды 
#define PROG_7      8,  8, 0b11100100, 0b01110010, 0b00111001, 0b10011100, 0b01001110, 0b00100111, 0b10010011, 0b11001001           // 8 шагов, 8 светодиодов, 1 шаг горят 8,7,6,3 светодиоды, 2 шаг горят 7,6,5,2 светодиоды, 3 шаг горят 6,5,4,1 светодиоды, ... , 8 шаг горят 1,8,7,4 светодиоды 
#define PROG_8      8,  8, 0b11110110, 0b01111011, 0b10111101, 0b11011110, 0b01101111, 0b10110111, 0b11011011, 0b11101101           // 8 шагов, 8 светодиодов, 1 шаг горят все светодиоды кроме 4 и 1, 2 шаг горят все светодиоды кроме 3 и 8, 3 шаг горят все светодиоды кроме 2 и 7, ... , 8 шаг горят все светодиоды кроме 5 и 2
#define PROG_9      6,  6, 0b00111000, 0b00011100, 0b00001110, 0b00000111, 0b00100011, 0b00110001

#define PROG_RUN_FIRE     PROG_0 , PROG_1 , PROG_2 , PROG_3 , PROG_4 , PROG_5 , PROG_6 , PROG_7 , PROG_8 , PROG_9                   // Все программы эффектов бегущего огня через запятую

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///                               Все команды которые обрабатывает скетч

//формат такой
// #define Command_XXXX   YYYYY
// где Command_XXXX   - код команды, это лучше не менять
//     YYYYY          - код обработчика ( это либо число получаемое кри нажатии кнопки пульта
//                                            либо Значение соответствующее нажатой кнопке аналоговой клавиатуры, это мы определили выше как значения KEY_0, KEY_1, KEY_2 и подобные
//                                            либо Значение соответствующее нажатой кнопке библиотеке GyverButton, это мы определили выше как значения BTN_1, BTN_2, BTN_3, BTN_4 и BTN_PRESS
//                                            либо Значение 0 если команда не используется )
// Если команда не используется то тогда надо YYYYY заменить на 0 или закомментировать строку

// значения кнопок пульта получаются следующим способом:
// загружаем пример из библиотеки IRremote с названием IRrecvDemo и смотрим в мониторе коды клавиш
// число должно иметь  вид 0x00ff4ab5  (шестнадцатеричное число)

// Пример:        #define Command_Brightness_plus   0xff4ab5                Установить обработку команды "Увеличить максимальную яркость"  на кнопку с кодом 0xff4ab5 с пульта
//                #define Command_Brightness_plus   KEY_0                   Установить обработку команды "Увеличить максимальную яркость"  на 0 кнопку подключенную к аналоговому входу
//                #define Command_Brightness_plus   BTN_1                   Установить обработку команды "Увеличить максимальную яркость"  на нажатие кнопки подключенной к библиотеке GyverButton
//                #define Command_Brightness_minus  BTN_2                   Установить обработку команды "Уменьшить максимальную яркость"  на двойное нажатие кнопки подключенной к библиотеке GyverButton

                                                      // кнопка на моем пульте
#define Command_Brightness_plus_R            BTN1_PRESS                         // Увеличить максимальную яркость, при достижения максимума начать с минимума
#define Command_Start_Stop                   BTN1_1                             // Старт/стоп
#define Command_Glitter                      BTN1_4                             // Включить/выключить сверкание
#define Command_Previous_my_mode             BTN1_3                             // Предыдущий эффект. Отключает демонстрационный режим
#define Command_Next_my_mode                 BTN1_2                             // Следующий эффект. Отключает демонстрационный режим

// Настройка IR
#define Command_Start               0xF076C13B    // вкл
#define Command_Stop                0xFF9186B7    // выкл
#define Command_Brightness_plus     0xAB91951F    // яркость +
#define Command_Brightness_minus    0x51E43D1B    // яркость -
#define Command_Next_mode           0xE5CFBD7F    // режим +
#define Command_Previous_mode       0x8C22657B    // режим -
#define Command_Glitter             0x35A9425F    // вспышки

/////////////////////////////////////////////////////////////////////////////////
///                               Настройка закончена
/////////////////////////////////////////////////////////////////////////////////
