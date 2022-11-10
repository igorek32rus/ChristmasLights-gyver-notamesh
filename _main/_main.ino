//  Сам код скетча, осисание и настройки в файле notamesh4Biblio.ino
//  октябрь 2020
//  Бикин Дмитрий
//  d.bikin@bk.ru
//  Обсуждение проекта https://mysku.ru/blog/aliexpress/68990.html
//  Проект делается на чистом энтузиазме и в свободное время
//  Яндекс кошелек     410011655180757

#include "wifiSettings.h"     // настройки wifi
#include "notamesh4Gyver.h"

/*------------------------------------------------------------------------------------------
--------------------------------------- Начало переменным ---------------------------------
------------------------------------------------------------------------------------------*/
#define SetOff(x)     {if (x<100) x=x+100;}                 //Установить флаг OFF
#define SetOn(x)      {if (x>=100) x=x-100;}                //Установить флаг ON
#define TestOff(x)    (x>=100)                              //Проверка флага OFF
#define GetOff(x)     ((x<100)?x:x-100)                     //Вернуть значение без флага

#define qsubd(x, b)  ((x>b)?wavebright:0)                     // A digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Unsigned subtraction macro. if result <0, then => 0.

#define NOTAMESH_VERSION 103                                  // Just a continuation of seirlight and previously aalight.

#include "FastLED.h"                                          // https://github.com/FastLED/FastLED

#if   SAVE_EEPROM > 0
  #include "EEPROM.h"                                           // This is included with base install
#endif

#if   IR_ON == 1
  #include <IRremoteESP8266.h>
  #include <IRrecv.h>
  #include <IRutils.h>
#endif

#if   KEY_GYVER_ON > 0
  #include "GyverButton.h"                                      // библиотека GyverButton
  #if BTN1_LOW ==0
    GButton btn1_gyver(BTN1_PIN, HIGH_PULL);
  #else
    GButton btn1_gyver(BTN1_PIN, LOW_PULL );
  #endif
  #if   KEY_GYVER_ON > 1
    #if BTN2_LOW ==0
      GButton btn2_gyver(BTN2_PIN, HIGH_PULL);
    #else
      GButton btn2_gyver(BTN2_PIN, LOW_PULL );
    #endif
    #if   KEY_GYVER_ON > 2
      #if BTN3_LOW ==0
        GButton btn3_gyver(BTN3_PIN, HIGH_PULL);
      #else
        GButton btn3_gyver(BTN3_PIN, LOW_PULL );
      #endif
      #if   KEY_GYVER_ON > 3
        #if BTN4_LOW ==0
          GButton btn4_gyver(BTN4_PIN, HIGH_PULL);
        #else
          GButton btn4_gyver(BTN4_PIN, LOW_PULL );
        #endif
      #endif
    #endif
  #endif
  
#endif

#if   LED_ON > 0
  uint8_t Led1_flesh=0x0;                                         //Управление мигания светодиодом 1
  #if   LED_ON > 1
    uint8_t Led2_flesh=0;                                         //Управление мигания светодиодом 2
  #endif
#endif


#if   TM1637_ON == 1
  #include "GyverTM1637.h"                                      // библиотека GyverTM1637
  GyverTM1637 disp_TM1637(TM1637_CLK, TM1637_DIO);
  
#endif


#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#if KEY_ADC_ON == 1                                                 //Для аналоговых кнопок
  int key_input = 0;                                            //Последнее нажатие кнопки
  int key_input_new;                                            //только что пришедьшее нажатие кнопки
  bool key_bounce = 0;                                          //для антидребезга
  uint32_t key_time;                                            //время последнего нажатия
#endif

#if IR_ON == 1
  int RECV_PIN = PIN_IR;
  IRrecv irrecv(RECV_PIN);
  decode_results results;
#endif

#if ( IR_ON == 1 || KEY_ADC_ON == 1 || KEY_GYVER_ON > 0 )
  uint8_t  IR_New_Mode=0;                                        //Выбор эффекта
  uint32_t IR_Time_Mode=0;                                       //время последнего нажатия
#endif

// Fixed definitions cannot change on the fly.
#if IR_ON == 1
  #define IR_MAX_LEDS MAX_LEDS 
#endif




// Initialize changeable global variables.
#if MAX_LEDS < 255
  uint8_t NUM_LEDS;                                           // Number of LED's we're actually using, and we can change this only the fly for the strand length.
  uint8_t KolLed;
#else
  uint16_t NUM_LEDS;                                          // Number of LED's we're actually using, and we can change this only the fly for the strand length.
  uint16_t KolLed;
#endif

uint8_t max_bright = 255;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[MAX_LEDS];                                   // Initialize our LED array. We'll be using less in operation.

 CRGBPalette16 gCurrentPalette;                               // Use palettes instead of direct CHSV or CRGB assignments
 CRGBPalette16 gTargetPalette;                                // Also support smooth palette transitioning
 CRGB solid= CRGB::Black;                                     // Цвет закраски


extern const TProgmemRGBGradientPalettePtr gGradientPalettes[]; // These are for the fixed palettes in gradient_palettes.h
extern const uint8_t gGradientPaletteCount;                     // Total number of fixed palettes to display.
uint8_t gCurrentPaletteNumber = 0;                              // Current palette number from the 'playlist' of color palettes
uint8_t currentPatternIndex = 0;                                // Index number of which pattern is current
uint32_t demo_time = 0;                                         // время демо режима
  
String currentIP = "";


TBlendType currentBlending = LINEARBLEND;                     // NOBLEND or LINEARBLEND


  // EEPROM location definitions.
  #define STARTMODE 0                                           // Расположение в EEPROM номера режима с которого будет старт (байт)
  #define STRANDLEN 1                                           // Расположение в EEPROM длинны гирлянды (2 байта)
  #define STRANDEL  3                                           // Расположение в EEPROM задержки (байт)
  #define EXTFLAG   4                                           // Расположение в EEPROM расширенных флагов
  // #define ISINIT    5                                           // Расположение в EEPROM байта корректности записи

  #define SSIDWIFI  5           // ssid wifi адрес
  #define PASSWIFI  38          // pass wifi адрес

  #define ISINIT    102                                         // Расположение в EEPROM байта корректности записи

  #define INITVAL   0x55                                        // Это значение проверяем в бите корректности EEPROM
  #define INITMODE  0                                           // с этого режима будет старт, по умолчанию 0 (старт с - с черного цвета)
  #define INITLEN   MAX_LEDS                                    // Размер гирлянды при старте
  #define INITDEL   0                                           // размер задержки при старте в миллисекундах
  
uint16_t meshdelay;                                             // Timer for the notamesh. Works with INITDEL.

uint8_t ledMode = 0;                                            // номер текущего режима
#if CHANGE_ON == 1
    uint8_t newMode = 0;                                        // номер нового режима
    #if MAX_LEDS < 255
      uint8_t StepMode=MAX_LEDS;                                // Текущий шаг перехода от нового режима до старого
    #else
      uint16_t StepMode=MAX_LEDS;                               // Текущий шаг перехода от нового режима до старого
    #endif
#endif

uint8_t demorun = DEMO_MODE;                                    // 0 = regular mode, 1 = demo mode, 2 = shuffle mode.
#if RUNNING_FIRE >0
  #define maxMode  122                                          // Maximum number of modes.
#else
  #define maxMode  42                                           // Maximum number of modes.
#endif

String ssid = INITSSID;
String pass = INITPASS;

const char* ap_ssid = AP_SSID;
const char* ap_pass = AP_PASS;

bool on_off = true;

uint8_t Protocol = 0;                                         // Temporary variables to save latest IR input
uint32_t Command = 0;

// Общие переменные ----------------------------------------------------------------------
uint8_t allfreq = 32;                                         // Меняет частоту. Переменная для эффектов one_sin_pal и two_sin. 
uint8_t bgclr = 0;                                            // Общий цвет фона. Переменная для эффектов matrix_pal и one_sin_pal.
uint8_t bgbri = 0;                                            // Общая фоновая яркость. Переменная для эффектов matrix_pal и one_sin_pal.

  //Структура хранения расширенных настроек 
  #pragma pack(push,1)
  typedef union {
            struct {    
            bool RedGreen:  1;  //Очередность  Цветов  RGB
            bool Glitter: 1;    //Флаг включения блеска
            bool Background: 1; //Флаг включения заполнения фона
            bool Candle:  1;    //Флаг включения свечей
            bool DemoRand:  1;  //Флаг случайный перебор эффектов
            bool rezerv0:  1;   //Зарезервировано
            bool rezerv1:  1;   //Зарезервировано
            bool rezerv2:  1;   //Зарезервировано
            };
            unsigned char Byte;
  } ExtendedFlags;
  #pragma pack(pop)

  ExtendedFlags ExtFlag;                                      //Флаги расширенных настроек

  #define     glitter             ExtFlag.Glitter
  #define     background          ExtFlag.Background
  #define     candle              ExtFlag.Candle
    

uint8_t palchg = 3;                                           // Управление палитрой  3 - менять палитру автоматически иначе нет
uint8_t startindex = 0;                                       // С какого цвета начинать. Переменная для эффектов one_sin_pal.
uint8_t thisbeat;                                             // Переменная для эффектов juggle_pal. 
uint8_t thiscutoff = 192;                                     // Если яркость ниже этой, то яркость = 0. Переменная для эффектов one_sin_pal и two_sin.
uint16_t thisdelay = 0;                                       // Задержка delay
uint8_t thisdiff = 1;                                         // Шаг изменения палитры. Переменная для эффектов confetti_pal, juggle_pal и rainbow_march.
int8_t  thisdir = 1;                                          // Направление движения эффекта. принимает значение -1 или 1.
uint8_t thisfade = 224;                                       // Скорость затухания. Переменная для эффектов confetti_pal и juggle_pal.
uint8_t thishue = 0;                                          // Оттенок Переменная для эффектов two_sin.
uint8_t thisindex = 0;                                        // Указатель ан элемент палитры
uint8_t thisinc = 1;                                          // Изменение начального цвета после каждого прохода. Переменная для эффектов confetti_pal и one_sin_pal.
int     thisphase = 0;                                        // Изменение фазы. Переменная для эффектов one_sin_pal, plasma и two_sin.
uint8_t thisrot = 1;                                          // Измение скорости вращения волны. В настоящее время 0.
int8_t  thisspeed = 4;                                        // Изменение стандартной скорости
uint8_t wavebright = 255;                                     // Вы можете изменить яркость волн / полос, катящихся по экрану.

#ifdef MY_MODE
    const PROGMEM uint8_t my_mode[]= { MY_MODE };             //массив выбранных режимов
    const uint8_t my_mode_count = sizeof( my_mode );          //колличество выбрано режимов 
    uint8_t tek_my_mode = 0;                                  //Указатель на текущий режим
#endif

#if CHANGE_SPARK == 4
  uint8_t rand_spark = 0;  
#endif

long summ=0;

void strobe_mode(uint8_t newMode, bool mc);
void bootme();
void meshwait();
void getirl();
void demo_check();

// Display functions -----------------------------------------------------------------------


// Support functions
#include "addings.h"

// Display routines
#include "confetti_pal.h"
#include "gradient_palettes.h"
#include "juggle_pal.h"
#include "matrix_pal.h"
#include "noise16_pal.h"
#include "noise8_pal.h"
#include "one_sin_pal.h"
#include "plasma.h"
#include "rainbow_march.h"
#include "rainbow_beat.h"
#include "serendipitous_pal.h"
#include "three_sin_pal.h"
#include "two_sin.h"
#include "blendwave.h"
#include "fire.h"
#include "candles.h"
#include "colorwave.h"
#include "getirl.h"

#if RUNNING_FIRE == 1
  #include "running_fire.h"
#endif

void(* resetFunc) (void) = 0;

void handleGetSetup(AsyncWebServerRequest *request) {
  String res =       "{ \"modes\": ";
  res +=             "[{\"name\": \"Blend Wave\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Rainbow Beat\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Two Sin\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"One Sin Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Noise8 Pal\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Two Sin\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"One Sin Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Juggle Pal\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Matrix Pal\", \"effectDelay\": \"40\" },";
  res +=             " {\"name\": \"Two Sin\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"One Sin Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Three Sin Pal\", \"effectDelay\": \"50\" },";
  res +=             " {\"name\": \"Serendipitous Pal\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"One Sin Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Two Sin\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Matrix Pal\", \"effectDelay\": \"50\" },";
  res +=             " {\"name\": \"Noise8 Pal\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Plasma\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Two Sin\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Rainbow March\", \"effectDelay\": \"50\" },";
  res +=             " {\"name\": \"Three Sin Pal\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Rainbow March\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Noise16 Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"One Sin Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Plasma\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Confetti Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Two Sin\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Matrix Pal\", \"effectDelay\": \"30\" },";
  res +=             " {\"name\": \"One Sin Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Confetti Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Plasma\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Juggle Pal\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"One Sin Pal\", \"effectDelay\": \"30\" },";
  res +=             " {\"name\": \"Three Sin Pal\", \"effectDelay\": \"50\" },";
  res +=             " {\"name\": \"Rainbow March\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Plasma\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Confetti Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Noise16 Pal\", \"effectDelay\": \"20\" },";
  res +=             " {\"name\": \"Noise8 Pal\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Fire\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Candles\", \"effectDelay\": \"10\" },";
  res +=             " {\"name\": \"Colorwaves\", \"effectDelay\": \"10\" }]";

  res +=             ",";
  res +=             " \"ssid\": \""; res += ssid; res += "\",";
  res +=             " \"pass\": \""; res += pass; res += "\",";
  res +=             " \"leds\": \""; res += NUM_LEDS; res += "\",";
  res +=             " \"ip\": \""; res += currentIP; res += "\",";

  res +=             " \"glitter\": \""; res += String(glitter); res += "\",";
  res +=             " \"candle\": \""; res += String(candle); res += "\",";
  res +=             " \"power\": \""; res += String(on_off); res += "\",";
  res +=             " \"brightness\": \""; res += max_bright; res += "\",";
  res +=             " \"speed\": \""; res += thisdelay; res += "\"";
  res +=             "}";

  // server.send(200, "application/json", res);
  request->send(200, "application/json", res);
}

void handleManifest(AsyncWebServerRequest *request) {
  String res = "{";
  res +=  "\"name\": \"ESP Light\",";
  res +=  "\"short_name\": \"ESP Light\",";
  res +=  "\"lang\": \"ru-RU\",";
  res +=  "\"start_url\": \"/\",";
  res +=  "\"display\": \"fullscreen\",";
  res +=  "\"orientation\": \"portrait\",";
  res +=  "\"theme_color\": \"#171717\",";
  res +=  "\"icons\": [";
  res +=  "  {";
  res +=  "    \"src\": \"favicon.png\",";
  res +=  "    \"sizes\": \"512x512\"";
  res +=  "  }";
  res +=  "]";
  res +=" }";

  // server.send(200, "application/json", res);
  request->send(200, "application/json", res);
}

void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  #if !defined(__AVR__)
    EEPROM.commit();
  #endif
}


String readString(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

void handleSaveSystemSettings(AsyncWebServerRequest *request) {
  if ((request->getParam("ssid")->value() != "") && (request->getParam("ssid")->value().length() <= 32)) {
    #if SAVE_EEPROM > 0
      writeString(SSIDWIFI, request->getParam("ssid")->value());
    #endif
    #if LOG_ON == 1
      Serial.print("New SSID: "); Serial.println(request->getParam("ssid")->value());
    #endif
  }

  if ((request->getParam("pass")->value() != "") && (request->getParam("pass")->value().length() <= 63)) {
    #if SAVE_EEPROM > 0
      writeString(PASSWIFI, request->getParam("pass")->value());
    #endif
    #if LOG_ON == 1
      Serial.println("New pass: *****");
    #endif
  }

  if (request->getParam("leds")->value() != "") {
    int count_leds = request->getParam("leds")->value().toInt();
    if (count_leds > 0 && count_leds < 255) NUM_LEDS = count_leds;
    #if SAVE_EEPROM > 0
      NUM_LEDS = request->getParam("leds")->value().toInt();
      #if MAX_LEDS < 255
        EEPROM.write(STRANDLEN, NUM_LEDS);                              //Сохранить в память
        EEPROM.write(STRANDLEN+1, 0);                                   //Сохранить в память
      #else
        EEPROM.write(STRANDLEN,   (uint16_t)(NUM_LEDS) & 0x00ff);       //Сохранить в память
        EEPROM.write(STRANDLEN+1, (uint16_t)(NUM_LEDS) >> 8);           //Сохранить в память
      #endif
      #if !defined(__AVR__)
          EEPROM.commit();
      #endif
    #endif
    #if LOG_ON == 1
      Serial.print(F("Length Garland ")); Serial.println(NUM_LEDS);
    #endif
  }

  // server.send(200, "application/json", "{\"status\":\"updated\"}");
  request->send(200, "application/json", "{\"status\":\"updated\"}");
  return;
  
}

void handleSetMode(AsyncWebServerRequest *request) {
  if (request->getParam("mode")->value() != "") {
    newMode = request->getParam("mode")->value().toInt();
    StepMode = 1;
    SetOff(demorun);
    // server.send(200, "application/json", "{\"status\":\"ok\"}");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
    return;
  }

  // server.send(200, "application/json", "{\"status\":\"error\"}");
  request->send(200, "application/json", "{\"status\":\"error\"}");
}

void handleSetBrightness(AsyncWebServerRequest *request) {
  if (request->getParam("brightness")->value() != "") {
    max_bright = request->getParam("brightness")->value().toInt();
    LEDS.setBrightness(max_bright);
    // server.send(200, "application/json", "{\"status\":\"ok\"}");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
    return;
  }

  // server.send(200, "application/json", "{\"status\":\"error\"}");
  request->send(200, "application/json", "{\"status\":\"error\"}");
}

void handleSetSpeed(AsyncWebServerRequest *request) {
  if (request->getParam("speed")->value() != "") {
    int tempSpeed = request->getParam("speed")->value().toInt();
    if ((tempSpeed > 0) && (tempSpeed < 1000)) {
      thisdelay = tempSpeed;
    }
    // server.send(200, "application/json", "{\"status\":\"ok\"}");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
    return;
  }

  // server.send(200, "application/json", "{\"status\":\"error\"}");
  request->send(200, "application/json", "{\"status\":\"error\"}");
}

void handleSetSolidColor(AsyncWebServerRequest *request) {
  if ((request->getParam("r")->value() != "") && (request->getParam("g")->value() != "") && (request->getParam("b")->value() != "")) {
    SetOff(demorun); ledMode = 255; palchg=0;
    solid = CRGB(request->getParam("r")->value().toInt(), request->getParam("g")->value().toInt(), request->getParam("b")->value().toInt());
    #if LOG_ON == 1
      Serial.println(F("Solid_Color"));
    #endif
    // server.send(200, "application/json", "{\"status\":\"ok\"}");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
    return;
  }

  // server.send(200, "application/json", "{\"status\":\"error\"}");
  request->send(200, "application/json", "{\"status\":\"error\"}");
}

void handleSetGlitter(AsyncWebServerRequest *request) {
  glitter = !glitter;
  // server.send(200, "application/json", "{\"status\":\"ok\", \"glitter\":\"" + String(glitter) + "\"}");
  request->send(200, "application/json", "{\"status\":\"ok\", \"glitter\":\"" + String(glitter) + "\"}");
}

void handleSetCandle(AsyncWebServerRequest *request) {
  candle = !candle;
  PolCandle = random8(CANDLE_KOL);
  // server.send(200, "application/json", "{\"status\":\"ok\", \"candle\":\"" + String(candle) + "\"}");
  request->send(200, "application/json", "{\"status\":\"ok\", \"candle\":\"" + String(candle) + "\"}");
}

void handleAutochange(AsyncWebServerRequest *request) {
  SetOn(demorun); //meshwait(); 
  // server.send(200, "application/json", "{\"status\":\"ok\"}");
  request->send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleOnOff(AsyncWebServerRequest *request) {
  on_off = !on_off;

  if (!on_off) {
    SetOff(demorun); ledMode = 255; palchg=0;
    solid = CRGB(0, 0, 0);
    LEDS.setBrightness(0);
  } else {
    SetOn(demorun); ledMode = 0; //meshwait(); 
    LEDS.setBrightness(max_bright);
  }

  // server.send(200, "application/json", "{\"status\":\"ok\", \"power\":\"" + String(on_off) + "\"}");
  request->send(200, "application/json", "{\"status\":\"ok\", \"power\":\"" + String(on_off) + "\"}");
}

void resetESP(AsyncWebServerRequest *request) {
  request->send(200, "application/json", "{\"status\":\"ok\"}");
  resetFunc();
}

/*
 * 
 * 
 */



/*------------------------------------------------------------------------------------------
--------------------------------------- Start of code --------------------------------------
------------------------------------------------------------------------------------------*/
void setup() {
#if KEY_ADC_ON == 1
  pinMode(PIN_KEY, INPUT);                                                        //Для аналоговых кнопок
#endif

#if   LED_ON > 0
  pinMode(LED1_PIN, OUTPUT);                                                      //пин первого светодиода
  #if   LED_ON > 1
    pinMode(LED2_PIN, OUTPUT);                                                    //пин второго светодиода
    #endif
  LED1_On;                                                                        //Включим светодиод
#endif


#if   TM1637_ON == 1
 disp_TM1637.clear();
 disp_TM1637.brightness(TM1637_BRIGHT);                                          // яркость, 0 - 7 (минимум - максимум)
  
#endif


#if LOG_ON == 1
  Serial.begin(SERIAL_BAUDRATE);                                                  // Setup serial baud rate

  Serial.println(F(" ")); Serial.println(F("---SETTING UP---"));
#endif


  delay(1000);                                                                    // Slow startup so we can re-upload in the case of errors.

  #if IR_ON == 1
    irrecv.enableIRIn();                                                          // Start the receiver
  #endif  
  
  #if SAVE_EEPROM >0
  
    #if !defined(__AVR__)
      EEPROM.begin(ISINIT+1);
    #endif
    ledMode = EEPROM.read(STARTMODE);                           // Расположение в EEPROM номера режима с которого будет старт (байт) 
    NUM_LEDS = EEPROM.read(STRANDLEN);                          // Расположение в EEPROM длинны гирлянды (2 байта)
    #if MAX_LEDS < 255
      if (EEPROM.read(STRANDLEN+1))                             //Почемуто светодиодов болше чем размер переменной
        NUM_LEDS = MAX_LEDS;                                    // Need to ensure NUM_LEDS < MAX_LEDS elsewhere.
    #else                                                                              
      NUM_LEDS += (uint16_t)EEPROM.read(STRANDLEN+1)<<8;        // Need to ensure NUM_LEDS < MAX_LEDS elsewhere.
    #endif
    meshdelay = EEPROM.read(STRANDEL);                          // Расположение в EEPROM задержки (байт)

    #if SAVE_EEPROM == 1
      ExtFlag.Byte = EEPROM.read(EXTFLAG);                      // Прочитаем расширенные настройки
    #else
      if (COLOR_ORDER == RGB)       ExtFlag.RedGreen  = 1;
      else                          ExtFlag.RedGreen  = 0;
      ExtFlag.Glitter     = GLITER_ON;                        //Флаг включения блеска
      ExtFlag.Background  = BACKGR_ON;                        //Флаг включения заполнения фона
      ExtFlag.Candle      = CANDLE_ON;                        //Флаг включения свечей
    #endif 

    ssid = readString(SSIDWIFI);
    pass = readString(PASSWIFI);

    if (  (EEPROM.read(ISINIT) != INITVAL) ||                   // проверка правильности в EEPROM байта корректности записи
          (NUM_LEDS > MAX_LEDS )  ||
          ((ledMode> maxMode) && (ledMode != 100) ) )      
    {                                                           // Не корректен
      EEPROM.write(STARTMODE, INITMODE);                        // сохраним в EEPROM номера режима с которого будет старт (байт)
      #if MAX_LEDS < 255
        EEPROM.write(STRANDLEN, INITLEN);                       // сохраним в EEPROM длинны гирлянды (2 байта)
      #else
        EEPROM.write(STRANDLEN,   (uint16_t)(INITLEN) & 0x00ff); 
        EEPROM.write(STRANDLEN+1, (uint16_t)(INITLEN) >> 8);     
      #endif
      EEPROM.write(STRANDEL, INITDEL);                          // сохраним в EPROM задержку (байт) 

      if (COLOR_ORDER == RGB)       ExtFlag.RedGreen  = 1;
      else                          ExtFlag.RedGreen  = 0;
      ExtFlag.Glitter     = GLITER_ON;                        //Флаг включения блеска
      ExtFlag.Background  = BACKGR_ON;                        //Флаг включения заполнения фона
      ExtFlag.Candle      = CANDLE_ON;                        //Флаг включения свечей
      EEPROM.write(EXTFLAG, ExtFlag.Byte);                    // сохраним в EPROM расширенные настройки 

      writeString(SSIDWIFI, INITSSID);
      writeString(PASSWIFI, INITPASS);

      EEPROM.write(ISINIT, INITVAL);                            // сохраним в EEPROM байта корректности записи
      #if !defined(__AVR__)
         EEPROM.commit();
      #endif

      ledMode = INITMODE;
      NUM_LEDS = INITLEN;
      meshdelay = INITDEL;

      ssid = INITSSID;
      pass = INITPASS;
    }
 #else
    if (COLOR_ORDER == RGB)       ExtFlag.RedGreen  = 1;
    else                          ExtFlag.RedGreen  = 0;
    ExtFlag.Glitter     = GLITER_ON;                        //Флаг включения блеска
    ExtFlag.Background  = BACKGR_ON;                        //Флаг включения заполнения фона
    ExtFlag.Candle      = CANDLE_ON;                        //Флаг включения свечей

    #ifdef  MY_MODE
      switch (demorun)  {
        case 3:   ledMode = pgm_read_byte(my_mode+tek_my_mode);break;               // демо 3
        case 4:   ledMode = pgm_read_byte(my_mode + random8(my_mode_count));break;    // демо 4
        case 1: 
        case 2:   ledMode = INITMODE; break;
      }
    #else
      ledMode = INITMODE;
    #endif           
    NUM_LEDS = MAX_LEDS;
    meshdelay = INITDEL;

    ssid = INITSSID;
    pass = INITPASS;
 #endif
  
 if (NUM_LEDS < (TOP_LENGTH+1) ) NUM_LEDS=(TOP_LENGTH+1);                                 //Проверка

  LEDS.setBrightness(max_bright);                                                 // Set the generic maximum brightness value.

  if (ExtFlag.RedGreen) 
    #if LED_CK
      LEDS.addLeds<CHIPSET, LED_DT, LED_CK, RGB>(leds, MAX_LEDS);
    #else 
      LEDS.addLeds<CHIPSET, LED_DT, RGB >(leds, MAX_LEDS);                   //Для светодиодов WS2812B
    #endif
  else
    #if LED_CK
      LEDS.addLeds<CHIPSET, LED_DT, LED_CK, GRB>(leds, MAX_LEDS);
    #else 
      LEDS.addLeds<CHIPSET, LED_DT, GRB >(leds, MAX_LEDS);                   //Для светодиодов WS2812B
    #endif

  
  set_max_power_in_volts_and_milliamps(POWER_V, POWER_I);                         //Настройка блока питания

  #if LOG_ON == 1
    Serial.print(F("Initial delay: ")); Serial.print(meshdelay*100); Serial.println(F("ms delay."));
    Serial.print(F("Initial strand length: ")); Serial.print(NUM_LEDS); Serial.println(F(" LEDs"));
    Serial.println(F("EXTEND Setup"));
    if (ExtFlag.RedGreen)   Serial.println(F("RGB LEDS"));      else  Serial.println(F("GRB LEDS"));
    if (ExtFlag.Glitter)    Serial.println(F("Glitter On"));    else  Serial.println(F("Glitter Off"));
    if (ExtFlag.Background) Serial.println(F("Background On")); else  Serial.println(F("Background Off"));
    if (ExtFlag.Candle)     Serial.println(F("Candle On"));     else  Serial.println(F("Candle Off"));
  #endif
  
  #if BLACKSTART == 1
    solid = CRGB::Black;                 //Запуск с пустого поля
    #if CHANGE_ON == 1
      newMode = ledMode;
      StepMode = 1;      
    #endif
    ledMode =255;
  #endif
  
  gCurrentPalette = gGradientPalettes[0];             
  gTargetPalette = gGradientPalettes[0];      
  strobe_mode(ledMode, 1);                                                        // Initialize the first sequence

    //Генератор случайных чисел
  uint16_t g=0;
  for (uint8_t x = 0; x < 8; x++) g = g*4 + (analogRead(2) & 3);
  #if defined(ESP32)
      g = g  ^ (uint16_t)esp_random();
  #elif defined(ESP8266)
      g = g  ^ (uint16_t)ESP8266_DREG(0x20E44);
  #endif
  randomSeed(g);
  random16_set_seed(g);                                                        // Awesome randomizer of awesomeness
  #if LOG_ON == 1
    Serial.print(F("Initial Random: ")); Serial.println(g); 
  #endif
  
  #if LOG_ON == 1
    if (DEMO_MODE) { Serial.print(F("DEMO MODE "));Serial.println(DEMO_MODE);}
    #if defined(ESP32)
      uint64_t chipid = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
      Serial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32)); //print High 2 bytes
      Serial.printf("%08X\n", (uint32_t)chipid); //print Low 4bytes.
    #endif
    Serial.println(F("---SETUP COMPLETE---"));
  #endif  

    LED1_Off;                                         //Выключим светодиод
  
  #if LOG_ON == 1
    Serial.print("Попытка подключения к SSID: "); Serial.println(ssid);
  #endif  

 
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);   // this is a temporary line, to be removed after SDK update to 1.5.4
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  unsigned long tempTime = millis();  // старт таймера для подключения к wifi

  #if LOG_ON == 1
    Serial.println("Connecting to WiFi");
  #endif

  while ((WiFi.status() != WL_CONNECTED) && (abs(long(millis() - tempTime)) <= 7000)) {
    delay(500);
    #if LOG_ON == 1
      Serial.print(".");
    #endif
  }
  
  #if LOG_ON == 1
    Serial.println("");
  #endif

  if (WiFi.status() != WL_CONNECTED) { // СОЗДАТЬ ТОЧКУ ДОСТУПА
    WiFi.disconnect(true);

    Serial.println("Error connection to WiFi STA!");
    Serial.print("Setting soft-AP ... ");
    Serial.println(WiFi.softAP(ap_ssid, ap_pass) ? "Ready" : "Failed!");

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    currentIP = String(myIP[0]) + "." + myIP[1] + "." + myIP[2] + "." + myIP[3];
  }

  if (WiFi.status() == WL_CONNECTED) {
    #if LOG_ON == 1
      IPAddress myIP = WiFi.localIP();
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(myIP);
      currentIP = String(myIP[0]) + "." + myIP[1] + "." + myIP[2] + "." + myIP[3];
    #endif
  }

  // server.on("/", handleRoot);
  // server.on("/getSetup", handleGetSetup);
  // server.on("/setMode", handleSetMode);
  // server.on("/setBrightness", handleSetBrightness);
  // server.on("/setColor", handleSetSolidColor);
  // server.on("/setSpeed", handleSetSpeed);
  // server.on("/saveSystem", handleSaveSystemSettings);
  // server.on("/setGlitter", handleSetGlitter);
  // server.on("/setCandle", handleSetCandle);
  // server.on("/setAutochange", handleAutochange);
  // server.on("/powerOnOff", handleOnOff);

  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.png");
  });

  server.on("/", HTTP_GET, handleRoot);
  server.on("/getSetup", HTTP_GET, handleGetSetup);
  server.on("/setMode", HTTP_GET, handleSetMode);
  server.on("/setBrightness", HTTP_GET, handleSetBrightness);
  server.on("/setColor", HTTP_GET, handleSetSolidColor);
  server.on("/setSpeed", HTTP_GET, handleSetSpeed);
  server.on("/saveSystem", HTTP_GET, handleSaveSystemSettings);
  server.on("/setGlitter", HTTP_GET, handleSetGlitter);
  server.on("/setCandle", HTTP_GET, handleSetCandle);
  server.on("/setAutochange", HTTP_GET, handleAutochange);
  server.on("/powerOnOff", HTTP_GET, handleOnOff);

  server.on("/manifest.json", HTTP_GET, handleManifest);
  server.on("/reset", HTTP_GET, resetESP);

  server.begin();


  #if LOG_ON == 1
    Serial.println("HTTP server started");
  #endif

  if (MDNS.begin("esplight")) {
    #if LOG_ON == 1
      Serial.println("MDNS responder started");
    #endif
  }

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
} // setup()



//------------------MAIN LOOP---------------------------------------------------------------
void loop() {
  // server.handleClient();
  MDNS.update();

  #if ( IR_ON == 1 || KEY_ADC_ON == 1 || KEY_GYVER_ON > 0 )
    getirl();                                                                   // Обработка команд с пульта и аналоговых кнопок
  #endif
  
  demo_check();                                                                 // Работа если включен демонстрационный режим
  
  EVERY_N_MILLISECONDS(50) {                                                  // Плавный переход от одной палитры в другую
       #if CHANGE_ON == 1
        if (StepMode == MAX_LEDS )
      #endif

     nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, PALETTE_SPEED);

     #if   LED_ON > 0                                                         //Управление мигания светодиодом 1
        if (Led1_flesh>0){                                                      //Управление светодиодами
          Led1_flesh--;
          if (  Led1_flesh >= 128) {
            if (  Led1_flesh & 8)     LED1_On;
            else                      LED1_Off;
            if ( Led1_flesh == 128) Led1_flesh = 0;
          }
          else if (  Led1_flesh >=64) { 
             if (  Led1_flesh & 4)     LED1_On;
              else                     LED1_Off;
              if ( Led1_flesh == 64) Led1_flesh = 0;
          }
          else {
              if (  Led1_flesh & 2)     LED1_On;
              else                      LED1_Off;
          }
      }
      #if   LED_ON > 1                                                        //Управление мигания светодиодом 2
          if (Led2_flesh>0){                                                    //Управление светодиодами
            Led2_flesh--;
            if (  Led2_flesh >= 128) {
              if (  Led2_flesh & 8)     LED2_On;
              else                      LED2_Off;
              if ( Led2_flesh == 128) Led2_flesh = 0;
            }
            else if (  Led2_flesh >=64) { 
                if (  Led2_flesh & 4)     LED2_On;
                else                      LED2_Off;
                if ( Led2_flesh == 64) Led2_flesh = 0;
            }
            else {
                if (  Led2_flesh & 2)     LED2_On;
                else                      LED2_Off;
            }
          }
      #endif
    #endif
  }

  #if PALETTE_TIME>0
    if ( palchg)  {
        EVERY_N_SECONDS(PALETTE_TIME) {                                            // Смена палитры
          if (palchg==3) {
            if (gCurrentPaletteNumber < (gGradientPaletteCount-1))  gCurrentPaletteNumber++; 
            else                                                    gCurrentPaletteNumber = 0; 
            #if LOG_ON == 1
              Serial.print(F("New Palette: "));  Serial.println(gCurrentPaletteNumber);
            #endif
          }
          gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];                // We're just ensuring that the gTargetPalette WILL be assigned.
        }
    }
  #endif
  
  #if DIRECT_TIME > 0
    EVERY_N_SECONDS(DIRECT_TIME) {                                            // Меняем направление
      thisdir = thisdir*-1;  
    }
  #endif
  
  EVERY_N_MILLIS_I(thistimer, thisdelay) {                                    // Sets the original delay time.
    thistimer.setPeriod(thisdelay);                                           // This is how you update the delay value on the fly.
    KolLed = NUM_LEDS-TOP_LENGTH;                                             // Выводим Эффект на все светодиоды
    strobe_mode(ledMode, 0);                                                  // отобразить эффект;
    #if CHANGE_ON == 1
      if ((StepMode < (NUM_LEDS-TOP_LENGTH) ) && ( (ledMode <220) || (ledMode >= 230) ) )
      {                                                                         // требуется наложить новый эффект
        KolLed = StepMode;                                                      // Выводим Эффект на все светодиоды
        if (StepMode > 10)  strobe_mode(newMode, 0);                            // отобразить эффект;
        #if CHANGE_SPARK == 4
          sparkler(rand_spark);  
        #else
          sparkler(CHANGE_SPARK);                                                             // бенгальский огонь
        #endif
      }
    #endif
  }

  #if CHANGE_ON == 1                                                            // Включена плавная смена эффектов
    if ( StepMode< (NUM_LEDS-TOP_LENGTH) ) {                                    // есть шаги, исполняем
        uint16_t change_time = (1000L *CHANGE_TIME) / (NUM_LEDS-TOP_LENGTH);    // время в мСек на каждый светодиод
        if (change_time < 20) change_time = 20;
//        static uint8_t change_increment = (uint32_t)change_time * (NUM_LEDS-TOP_LENGTH) / (1000L *CHANGE_TIME)+1;
        EVERY_N_MILLISECONDS( change_time) {                      // Движение плавной смены эффектов
//            if (StepMode >10) StepMode+=change_increment;
//            else              
              StepMode++;
            if (StepMode == 10) strobe_mode(newMode, 1);
            if (StepMode>= (NUM_LEDS - TOP_LENGTH) )
              { ledMode = newMode;
                StepMode = MAX_LEDS - TOP_LENGTH;
                
               #if LOG_ON == 1
                  Serial.println(F("End SetMode"));
                #endif
             }
            nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, PALETTE_SPEED);  
        }
    }
  #endif

  #if TOP_LENGTH >0
    top();                                                                    // Обработка конца гирлянды
  #endif
  if(glitter) addglitter(10);                                                 // If the glitter flag is set, let's add some.
  #if CANDLE_KOL >0
    if(candle)  addcandle();                                                 
  #endif
   
  if(background) addbackground();                                             // Включить заполнение черного цвета фоном

  BtnHandler();                                                               //Обработчик нажатий кнопок

  #if ( IR_ON == 1 || KEY_ADC_ON == 1 || (KEY_GYVER_ON > 0) )
    if ( (IR_Time_Mode > 0) &&                                                //Идет отчет времени
         ((millis() - IR_Time_Mode) >= 2000 )  ) {                            //И прошло больше 2 секунд
            IR_Time_Mode=0;
            if (IR_New_Mode <=maxMode) SetMode(IR_New_Mode);
            IR_New_Mode=0;
         }
  #endif

  #if IR_ON == 1
    // while (!irrecv.isIdle());                                                   // if not idle, wait till complete
  
      if (irrecv.decode(&results)) {  
          /* respond to button */
  
          if (!Protocol) {
            Protocol = 1;                                        // update the values to the newest valid input

            #if IR_REPEAT == 1
              if ( results.value != 0xffffffff)                    //Если не повтор то вставить новую команду
                Command = results.value;
              else Protocol=2;
            #else
                Command = results.value;
            #endif
          }
          irrecv.resume(); // Set up IR to receive next value.
      }
  #endif
  
  #if defined(ESP32)
    delay(10); 
  #endif   

  //поворот цвета
#if COOP_COLOR_START >0                   //Поворот цвета гирлянд включен 
  #if MAX_LEDS < 255  
    uint8_t zz;
  #else
    uint16_t zz;
  #endif
  uint8_t zzz;
  for (zz = COOP_COLOR_START-1; zz < COOP_COLOR_END; zz++) {
    #if COOP_COLOR_WHAT == RG
      zzz = leds[zz].r; leds[zz].r = leds[zz].g; leds[zz].g = zzz;
    #elif COOP_COLOR_WHAT == GB
      zzz = leds[zz].g; leds[zz].g = leds[zz].b; leds[zz].b = zzz;
    #else
      zzz = leds[zz].r; leds[zz].r = leds[zz].b; leds[zz].b = zzz;
    #endif      
  }
#endif

  FastLED.show();                                                             // Power managed display of LED's.

  //поворот цвета
#if COOP_COLOR_START >0                   //Поворот цвета гирлянд включен 
  for (zz = COOP_COLOR_START-1; zz < COOP_COLOR_END; zz++) {
    #if COOP_COLOR_WHAT == RG
      zzz = leds[zz].r; leds[zz].r = leds[zz].g; leds[zz].g = zzz;
    #elif COOP_COLOR_WHAT == GB
      zzz = leds[zz].g; leds[zz].g = leds[zz].b; leds[zz].b = zzz;
    #else
      zzz = leds[zz].r; leds[zz].r = leds[zz].b; leds[zz].b = zzz;
    #endif      
  }
#endif
  
} // loop()



//-------------------OTHER ROUTINES----------------------------------------------------------
void strobe_mode(uint8_t mode, bool mc){                   // mc stands for 'Mode Change', where mc = 0 is strobe the routine, while mc = 1 is change the routine

  if(mc) {
      fill_solid(leds,NUM_LEDS-TOP_LENGTH,CRGB(0,0,0));                    // Clean up the array for the first time through. Don't show display though, so you may have a smooth transition.

      #if   TM1637_ON == 1
        disp_TM1637.clear();
        disp_TM1637.displayInt(mode);
      #endif

      #if LOG_ON == 1
        Serial.print(F("Mode: ")); 
        Serial.println(mode);
        Serial.println(millis());
      #endif
      #if PALETTE_TIME>0
      if (palchg==0) palchg=3;
      #else
      if (palchg==0) palchg=1;
      #endif
    }
    
     
  switch (mode) {                                          // First time through a new mode, so let's initialize the variables for a given display.

    case  0: if(mc) {thisdelay=10; palchg=0; } blendwave(); break; 
    case  1: if(mc) {thisdelay=10; palchg=0; } rainbow_beat(); break;
    case  2: if(mc) {thisdelay=10; allfreq=2; thisspeed=1; thatspeed=2; thishue=0; thathue=128; thisdir=1; thisrot=1; thatrot=1; thiscutoff=128; thatcutoff=192;} two_sin(); break;
    case  3: if(mc) {thisdelay=20; allfreq=4; bgclr=0; bgbri=0; startindex=64; thisinc=2; thiscutoff=224; thisphase=0; thiscutoff=224; thisrot=0; thisspeed=4; wavebright=255;} one_sin_pal(); break;
    case  4: if(mc) {thisdelay=10; } noise8_pal(); break;
    case  5: if(mc) {thisdelay=10; allfreq=4; thisspeed=-1; thatspeed=0; thishue=64; thathue=192; thisdir=1; thisrot=0; thatrot=0; thiscutoff=64; thatcutoff=192;} two_sin(); break;
    case  6: if(mc) {thisdelay=20; allfreq=10; bgclr=64; bgbri=4; startindex=64; thisinc=2; thiscutoff=224; thisphase=0; thiscutoff=224; thisrot=0; thisspeed=4; wavebright=255;} one_sin_pal(); break;
    case  7: if(mc) {thisdelay=10; numdots=2; thisfade=16; thisbeat=8; thisdiff=64;} juggle_pal(); break;
    case  8: if(mc) {thisdelay=40; thisindex=128; thisdir=1; thisrot=0; bgclr=200; bgbri=6;} matrix_pal(); break;
    case  9: if(mc) {thisdelay=10; allfreq=6; thisspeed=2; thatspeed=3; thishue=96; thathue=224; thisdir=1; thisrot=0; thatrot=0; thiscutoff=64; thatcutoff=64;} two_sin(); break;
    case 10: if(mc) {thisdelay=20; allfreq=16; bgclr=0; bgbri=0; startindex=64; thisinc=2; thiscutoff=224; thisphase=0; thiscutoff=224; thisrot=0; thisspeed=4; wavebright=255;} one_sin_pal(); break;
    case 11: if(mc) {thisdelay=50; mul1=5; mul2=8; mul3=7;} three_sin_pal(); break;
    case 12: if(mc) {thisdelay=10; } serendipitous_pal(); break;
    case 13: if(mc) {thisdelay=20; allfreq=8; bgclr=0; bgbri=4; startindex=64; thisinc=2; thiscutoff=224; thisphase=0; thiscutoff=224; thisrot=0; thisspeed=4; wavebright=255;} one_sin_pal(); break;
    case 14: if(mc) {thisdelay=10; allfreq=20; thisspeed=2; thatspeed=-1; thishue=24; thathue=180; thisdir=1; thisrot=0; thatrot=1; thiscutoff=64; thatcutoff=128;} two_sin(); break;
    case 15: if(mc) {thisdelay=50; thisindex=64; thisdir=-1; thisrot=1; bgclr=100; bgbri=10;} matrix_pal(); break;
    case 16: if(mc) {thisdelay=10; } noise8_pal(); break; // By: Andrew Tuline
    case 17: if(mc) {thisdelay=10; } plasma(11,23,4,18); break;
    case 18: if(mc) {thisdelay=20; allfreq=10; thisspeed=1; thatspeed=-2; thishue=48; thathue=160; thisdir=-1; thisrot=1; thatrot=-1; thiscutoff=128; thatcutoff=192;} two_sin(); break;
    case 19: if(mc) {thisdelay=50; palchg=0; thisdir=1; thisrot=1; thisdiff=1;} rainbow_march(); break;
    case 20: if(mc) {thisdelay=10; mul1=6; mul2=9; mul3=11;} three_sin_pal(); break;
    case 21: if(mc) {thisdelay=10; palchg=0; thisdir=1; thisrot=2; thisdiff=10;} rainbow_march(); break;
    case 22: if(mc) {thisdelay=20; palchg=0; hxyinc = random16(1,15); octaves=random16(1,3); hue_octaves=random16(1,5); hue_scale=random16(10, 50);  x=random16(); xscale=random16(); hxy= random16(); hue_time=random16(); hue_speed=random16(1,3); x_speed=random16(1,30);} noise16_pal(); break;
    case 23: if(mc) {thisdelay=20; allfreq=6; bgclr=0; bgbri=0; startindex=64; thisinc=2; thiscutoff=224; thisphase=0; thiscutoff=224; thisrot=0; thisspeed=4; wavebright=255;} one_sin_pal(); break;
    case 24: if(mc) {thisdelay=10; } plasma(23,15,6,7); break;
    case 25: if(mc) {thisdelay=20; thisinc=1; thisfade=2; thisdiff=32;} confetti_pal(); break;
    case 26: if(mc) {thisdelay=10; thisspeed=2; thatspeed=3; thishue=96; thathue=224; thisdir=1; thisrot=1; thatrot=2; thiscutoff=128; thatcutoff=64;} two_sin(); break;
    case 27: if(mc) {thisdelay=30; thisindex=192; thisdir=-1; thisrot=0; bgclr=50; bgbri=0;} matrix_pal(); break;
    case 28: if(mc) {thisdelay=20; allfreq=20; bgclr=0; bgbri=0; startindex=64; thisinc=2; thiscutoff=224; thisphase=0; thiscutoff=224; thisrot=0; thisspeed=4; wavebright=255;} one_sin_pal(); break;
    case 29: if(mc) {thisdelay=20; thisinc=2; thisfade=8; thisdiff=64;} confetti_pal(); break;
    case 30: if(mc) {thisdelay=10; } plasma(8,7,9,13); break;
    case 31: if(mc) {thisdelay=10; numdots=4; thisfade=32; thisbeat=12; thisdiff=20;} juggle_pal(); break;
    case 32: if(mc) {thisdelay=30; allfreq=4; bgclr=64; bgbri=4; startindex=64; thisinc=2; thiscutoff=224; thisphase=0; thiscutoff=128; thisrot=1; thisspeed=8; wavebright=255;} one_sin_pal(); break;
    case 33: if(mc) {thisdelay=50; mul1=3; mul2=4; mul3=5;} three_sin_pal(); break;
    case 34: if(mc) {thisdelay=10; palchg=0; thisdir=-1; thisrot=1; thisdiff=5;} rainbow_march(); break;
    case 35: if(mc) {thisdelay=10; } plasma(11,17,20,23); break;
    case 36: if(mc) {thisdelay=20; thisinc=1; thisfade=1;} confetti_pal(); break;
    case 37: if(mc) {thisdelay=20; palchg=0; octaves=1; hue_octaves=2; hxy=6000; x=5000; xscale=3000; hue_scale=50; hue_speed=15; x_speed=100;} noise16_pal(); break;
    case 38: if(mc) {thisdelay=10; } noise8_pal(); break;
    case 39: if(mc) {thisdelay=10; palchg=0;} fire(); break;
    case 40: if(mc) {thisdelay=10; palchg=0;} candles(); break;
    case 41: if(mc) {thisdelay=10; } colorwaves(); break;
//  case 42 .. 121: if(mc) {thisdelay=10; } running_fire(9,3,1); break;
    case 200: fill_solid(leds, MAX_LEDS, CRGB::Black); fill_solid(leds,NUM_LEDS,CRGB(255,255,255)); break;   //Зажеч гирлянду длинной NUM_LEDS (регулировка длинны гирлянды)
    case 201: fill_solid(leds, MAX_LEDS, CRGB::Black); fill_solid(leds,meshdelay,CRGB(255,255,255)); break;  //Зажеч гирлянду длинной meshdelay
    case 220: fill_solid(leds, MAX_LEDS, CRGB::Black); fill_solid(leds,NUM_LEDS,CRGB(255,255,255)); break;   //Зажеч гирлянду длинной NUM_LEDS (регулировка длинны гирлянды)
    case 221: fill_solid(leds, MAX_LEDS, CRGB::Black); fill_solid(leds,NUM_LEDS,CRGB(255,255,255)); break;   //Зажеч гирлянду длинной NUM_LEDS (регулировка длинны гирлянды)
    case 255: if(mc) {palchg=0; } fill_solid(leds, NUM_LEDS-TOP_LENGTH,  solid);break;     //Установить цвет
    default : 
              #if RUNNING_FIRE == 1
                 if (mode>=42 && mode <122) {
                  uint8_t mode_x = mode - 42;
                  if(mc) {thisdelay=10;}  running_fire(mode_x >> 3,(mode_x >> 1) % 3,mode_x % 1);
                  }
                  else
              #endif
                ledMode = 0;  
              break;        //нет такого режима принудительно ставим нулевой
  } // switch mode

  #if LOG_ON == 1
    if(mc) {
        if ( palchg==0 ) Serial.println(F("Change palette off"));
        else 
        if ( palchg==1 ) Serial.println(F("Change palette Stop"));
        else 
        if ( palchg==3 ) Serial.println(F("Change palette ON"));
    } 
  #endif

} // strobe_mode()



void demo_check(){
  
  if( (demorun > 0) && (demorun <=100) ) { 
    if ((millis() - demo_time) >= (DEMO_TIME*1000L )) {           //Наступило время смены эффекта
      demo_time = millis();                                       //Запомним время 
      gCurrentPaletteNumber = random8(0,gGradientPaletteCount);   //Случайно поменяем палитру
      #if CHANGE_ON == 1
        switch (demorun)  {
          case 2:   newMode = random8(0,maxMode);                 // демо 2
                    break;
         #ifdef MY_MODE
          case 3:   if (tek_my_mode>=(my_mode_count-1)) tek_my_mode=0;// демо 3
                    else tek_my_mode++;
                    newMode = pgm_read_byte(my_mode+tek_my_mode);    
                    break; 
          case 4:   newMode = pgm_read_byte(my_mode+random8(my_mode_count));    // демо 4
                    break; 
         #endif           
          case 1:   if (newMode >= maxMode) newMode = 0;          // демо 1 
                    else newMode++;
                    break;
        }
        StepMode = 1;
        #if CHANGE_SPARK == 4
          rand_spark = random8(3)+1;  
        #endif

        #if LOG_ON == 1
          Serial.println(F("Start SetMode"));
        #endif
      #else
       gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];  //Применим палитру
        #if LOG_ON == 1
          Serial.print(F("New Palette: "));  Serial.println(gCurrentPaletteNumber);
        #endif
       switch (demorun)  {
          case 2:   ledMode = random8(0,maxMode);                 // демо 2
                    break;
         #ifdef MY_MODE
          case 3:   if (tek_my_mode>=(my_mode_count-1)) tek_my_mode=0;// демо 3
                    else tek_my_mode++;
                    ledMode = pgm_read_byte(my_mode+tek_my_mode);    
                    break; 
          case 4:   ledMode = pgm_read_byte(my_mode+random8(my_mode_count));    // демо 4
                    break; 
         #endif           
          case 1:   if (ledMode >= maxMode) ledMode = 0;          // демо 1 
                    else ledMode++;
                    break;
        }
        strobe_mode(ledMode,1);                                 // Does NOT reset to 0.
        #if CANDLE_KOL >0
          PolCandle = random8(CANDLE_KOL);
        #endif
      #endif
    } // if lastSecond
  } // if demorun
  
} // demo_check()
