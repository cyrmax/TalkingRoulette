#define TESTING
#define TESTING_VALUE 90

// Define pins
#define UZ_TRIG_PIN 3 // Ножка Trig дальномера
#define UZ_ECHO_PIN 2 // Ножка Echo дальномера
#define BUTTON_PIN 4 // Ножка для подключения кнопки. Ожидается кнопка, в ненажатом состоянии дающая логический 0, а в нажатом - дающая логическую 1, 5 вольт. Кнопка подключена к VCC, а пин параллельно кнопке подтянут к GND.
#define SWITCHER_PIN 5 // Пин для подключения переключателя, по которому к измерению добавляется длина корпуса. Во включенном состоянии даёт логическую единицу.


#include <GyverButton.h> // Библиотека для работы с кнопками
#include <GyverFilters.h>
#include "Definitions.h"
#include "FileNumbers.h"
//#include <GyverUART.h>
// #include <DFRobotDFPlayerMini.h>

GButton btn(BUTTON_PIN, LOW_PULL);

GFilterRA RAFilter;
GMedian3<float> MedianFilter;

unsigned long lastMillis;
float result = 0;

#ifdef TESTING
bool shouldSpeak = false;
unsigned long speakTimer;
#endif

void setup() {
  uart.begin(115200);
  #ifdef TESTING
  delay(4000);
  uart.println("Wellcome. Controller is ready");
  randomSeed(analogRead(0));
  #endif
  btn.setDebounce(BUTTON_DEBOUNCE_TIME);
pinMode(UZ_TRIG_PIN, OUTPUT);
pinMode(UZ_ECHO_PIN, INPUT);  
pinMode(SWITCHER_PIN, INPUT);
pinMode(BUTTON_PIN, INPUT_PULLUP);
RAFilter.setCoef(FILTER_RA_COEF);
RAFilter.setStep(FILTER_RA_STEP);
lastMillis = millis();
#ifdef TESTING
speakTimer = millis();
#endif
}


void loop() {
btn.tick();
bool virtualClick = false;
#ifdef TESTING
//if (millis() - speakTimer > 10000) virtualClick = true;
#endif
  if (btn.isPress() || virtualClick) {
    if (result > MAXIMUM_DISTANCE) result = MAXIMUM_DISTANCE;
    if (result < MINIMUM_DISTANCE) result = MINIMUM_DISTANCE;

#ifdef Testing
virtualClick = false;
speakTimer = millis();
#endif
    
// Speak result
speakResult(result);
  }

  // Measurement and filtering
if (millis() - lastMillis > MEASUREMENT_INTERVAL) {
int measured = measure();
if (digitalRead(SWITCHER_PIN)) measured += LENGTH_OF_BOX;
measured = MedianFilter.filtered(measured);
result = RAFilter.filteredTime(measured);
lastMillis = millis();
}
}


int measure() {
  int pulseDuration;  
  #ifndef TESTING
digitalWrite(UZ_TRIG_PIN, LOW);
delayMicroseconds(2);
digitalWrite(UZ_TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(UZ_TRIG_PIN, LOW);
pulseDuration = pulseIn(UZ_ECHO_PIN, HIGH);
#endif
#ifdef TESTING
pulseDuration = TESTING_VALUE + random(-50, 51);
#endif
return (int)(pulseDuration * SONIC_SPEED_MULTIPLIER);
}


int digitCount(int num) {
  int n = num;
int i = 0;
  while (n > 0) {
n = (int)(n / 10);
    ++i;
  }
  return i;
}



void splitToDigits(int num, int numOfDigits, int *splitted) {
  int div = calcPower(10, (numOfDigits - 1));
int var = num;
int rest;
for (int i = 0; i < numOfDigits; i++) {
rest = (int)(var / div);
splitted[i] = rest;
var = var - rest * div;
div = (int)(div / 10);  
}
}


void speakResult(int num) {
#ifdef TESTING
uart.println("Speak started");
uart.println(num);
#endif
int numOfDigits = digitCount(num);
int digits[numOfDigits];
splitToDigits(num, numOfDigits, digits);
#ifdef TESTING
  uart.print("Num of digits: ");
  uart.print(numOfDigits);
  uart.println();
  uart.println("Digits now contain: ");
for (int i = 0; i < numOfDigits; ++i) {
  uart.print(digits[i]);
  uart.print(" ");
}
uart.println();
#endif

switch (numOfDigits) {
case 4:
speakMeters(digits[0]);
speakSM(digits[1], digits[2]);
speakMM(digits[3]);
break;
case 3:
speakSM(digits[0], digits[1]);
speakMM(digits[2]);
break;
case 2:
speakSM(0, digits[0]);
speakMM(digits[1]);
break;
default: break;
}
}


void speakMeters(int meters) {
  ttsPlay(meters);
  ttsPlay(FILE_M);
}


void speakSM(int sm1, int sm2) {
if (sm1 > 0) ttsPlay(sm1 * 10);
if (sm2 > 0) ttsPlay(sm2);
  ttsPlay(FILE_SM);
}


void speakMM(int mm) {
  ttsPlay(mm);
  ttsPlay(FILE_MM);
}


void ttsPlay(int fileNum) {
while (ttsIsPlaying()) {
  delay(5);
}
#ifdef TESTING
uart.print("Playing file with number ");
  uart.print(fileNum);
  uart.println();
#endif
 
}



bool ttsIsPlaying() {
  return false;
}


int calcPower(int base, int expon) {
int res = 1;
if (expon == 0) return res;

  for (int i = 0; i < expon; ++i) {
    res *= base;
  }
  return res;
}
