#include <LiquidCrystal.h>
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

#include <EEPROM.h>
int addr = 0;
byte eeprom_read ;
byte eeprom_write;
int highscore ;

// attachInterrupt pin( Digital input 2)
int interruptPin = 2;

// Shoot count
volatile int count = 0;

// Opening screen flag
int switch_setup = false;

unsigned long time_start = 0;
int time_remain = 10;
int flag_count = 20;

#define DEBUG // Debug Setting
//#define CLEAR_HIGHSCORE

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println(F("Serial Start"));
#endif
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(interruptPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin), increment, FALLING);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), increment, HIGH);
  readhighscore();

#ifdef CLEAR_HIGHSCORE
  count = 10;
  eeprom_write = byte(count);
  EEPROM.write(addr, eeprom_write);
  readhighscore();
#endif
}

void loop() {
#ifdef DEBUG
  Serial.print(F("switch_setup:"));
  Serial.println(switch_setup);
  Serial.print(F("time_remain:"));
  Serial.println(time_remain);
  Serial.print(F("count:"));
  Serial.println(count);
#endif

  if (time_remain > 0)  { //clear and setup
    if (switch_setup)  {
      lcd.setCursor(6, 0);
      lcd.print(count);
      lcd.setCursor(6, 1);
      if (time_remain < 10) {
        lcd.print(" ");
      }
      time_remain = 10 - (millis() - time_start) / 1000;
      lcd.print(time_remain);
    }
  } else {  // 10count done and flag show
    if (count >= flag_count ) {
#ifdef DEBUG
      Serial.println(F("Flag print"));
#endif
      lcd.setCursor(0, 1);
      lcd.print("flag : flag_10s ");
      delay(5000);
      lcd.setCursor(0, 1);
      lcd.print(" PUSH TO START  ");
    }
    check_highscore() ;
    switch_setup = false;
  }
}
void increment() {
#ifdef DEBUG
  Serial.println(F("button pushed"));
#endif
  if (time_remain < 1 || !switch_setup) {
    before_start();
    time_start = millis();
  }
  if (time_remain > 0 || switch_setup) {
    count++;
#ifdef DEBUG
    Serial.println(count);
#endif
  }
}

void before_start() {
#ifdef DEBUG
  Serial.println(F("LCD clear and start"));
#endif
  if (!switch_setup)  {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Count:   Top:   ");
    lcd.setCursor(13, 0);
    lcd.print(highscore);
    lcd.setCursor(0, 1);
    lcd.print("Time :          ");
    count = 0;
    time_remain = 10;
    switch_setup = true;
  }
}

void readhighscore() {
#ifdef DEBUG
  Serial.println(F("Read Highscore"));
#endif
  eeprom_read = EEPROM.read(addr);
  highscore = int(eeprom_read);
#ifdef DEBUG
  Serial.println(highscore);
#endif
}

void check_highscore() {
  if (highscore < count) {
#ifdef DEBUG
    Serial.println(F("Write Highscore"));
    Serial.println(count);
#endif
    eeprom_write = byte(count);
    EEPROM.write(addr, eeprom_write);
    highscore = count;
    lcd.setCursor(13, 0);
    lcd.print(highscore);
  }
  count = 0;
}
