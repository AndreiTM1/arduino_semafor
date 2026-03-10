#include <LiquidCrystal.h>



const int RED = 6;
const int YELLOW = 7;
const int GREEN = 8;
const int GREEN_PIETONI = 2;
const int RED_PIETONI = 3;
const int Button = 5;
const int Buzzer = 4;
bool boot = true;

const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const unsigned long TIMP_GALBEN = 3000;
const unsigned long TIMP_ROSU = 15000;
const unsigned long TIMP_PAUZA_SIGURANTA = 2000;
const unsigned long TIMP_REPAUS = 20000;

unsigned long momentSchimbare = 0;
bool butonMemorizat = false;
int stare = 0; 
int stareButonAnterioara = LOW;

byte om[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010,
  0b10010
};

byte button_1[8] = {
  0b00100,
  0b00100,
  0b01110,
  0b00100,
  0b00000,
  0b01110,
  0b01110,
  0b11111
};

byte button_2[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b01110,
  0b00100,
  0b00000,
  0b01110,
  0b11111
};

byte hand[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b10111,
  0b11001,
  0b01110,
  0b00000,
  0b00000
};

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED_PIETONI, OUTPUT);
  pinMode(GREEN_PIETONI, OUTPUT);
  pinMode(Button, INPUT);
  digitalWrite(GREEN, HIGH);
  digitalWrite(RED_PIETONI, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN_PIETONI, LOW);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, HIGH);

  lcd.createChar(0, om);
  lcd.createChar(2, button_1);
  lcd.createChar(3, button_2);
  lcd.createChar(4, hand);
  lcd.begin(16,2);
  lcd.setCursor(2,0);
  lcd.print("PUSH BUTTON");
  lcd.setCursor(2,1);
  lcd.print("TO CROSS! ");

  boot = true;
}

void loop() {
  
  unsigned long acum = millis();
  int citireButonAcum = digitalRead(Button);

  if((boot || stare == 3 || !stare)&& !butonMemorizat)
  {
    lcd.setCursor(12, 1);
       if((acum/1000) % 2 == 0)
         lcd.write((byte)2);
       else lcd.write((byte)3);
  }
   
  if (citireButonAcum == HIGH && stareButonAnterioara == LOW) {
    if(stare == 0 || stare == 3)
    {
      butonMemorizat = true;
      boot = false;
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("PLEASE WAIT");
      lcd.setCursor(3,1);
      lcd.print("FOR GREEN!");
    }
    delay(50);
  }
  stareButonAnterioara = citireButonAcum;

  switch (stare) {
    
    case 0: 
      if (butonMemorizat) {
        butonMemorizat = false;
        momentSchimbare = acum;
        stare = 1; 
        digitalWrite(GREEN, LOW);
        digitalWrite(YELLOW, HIGH);
        lcd.clear();
        lcd.setCursor(3,0);
      lcd.print("PLEASE WAIT");
      lcd.setCursor(3,1);
      lcd.print("FOR GREEN!");
      }
    break;

    case 1: 
      if (acum - momentSchimbare >= TIMP_GALBEN) {
        momentSchimbare = acum;
        stare = 4;
        digitalWrite(YELLOW, LOW);
        digitalWrite(RED, HIGH);
      }
    break;

    case 4: 
      if (acum - momentSchimbare >= TIMP_PAUZA_SIGURANTA) {
        momentSchimbare = acum;
        stare = 2; 
        digitalWrite(RED_PIETONI, LOW);
        digitalWrite(GREEN_PIETONI, HIGH);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   CROSS WITH");
        lcd.setCursor(0,1);
        lcd.print("   CAUTION!");
      }
    break;

    case 2:
    {
      unsigned long timpScurs = acum - momentSchimbare;

      lcd.setCursor(12, 1);
      lcd.write(byte(0));

      if (timpScurs < 12000) {
        digitalWrite(GREEN_PIETONI, HIGH);
        if((acum/500)%2 == 0)
          digitalWrite(Buzzer, LOW);
        else digitalWrite(Buzzer, HIGH);
      } 
      else if (timpScurs < 15000) {
        if((acum/250)%2 == 0)
          digitalWrite(Buzzer, LOW);
        else digitalWrite(Buzzer, HIGH);
        digitalWrite(GREEN_PIETONI, (acum / 500) % 2 == 0);
      }
      if (timpScurs >= TIMP_ROSU) {
        digitalWrite(Buzzer, HIGH);
        momentSchimbare = acum;
        stare = 5; 
        digitalWrite(GREEN_PIETONI, LOW);
        digitalWrite(RED_PIETONI, HIGH);
        lcd.clear();
        lcd.setCursor(5,0);
        lcd.print("DO NOT");
        lcd.setCursor(5,1);
        lcd.print("CROSS!");
      }
    }
    break;

    case 5:
      if (acum - momentSchimbare >= TIMP_PAUZA_SIGURANTA) {
        momentSchimbare = acum;
        stare = 3; 
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, HIGH);
        lcd.setCursor(2,0);
        lcd.print("PUSH BUTTON");
        lcd.setCursor(2,1);
        lcd.print("TO CROSS! ");
      }
    break;

    case 3:
      if (acum - momentSchimbare >= TIMP_REPAUS) {
        if (butonMemorizat) {
          butonMemorizat = false;
          momentSchimbare = acum;
          stare = 1; 
          digitalWrite(GREEN, LOW);
          digitalWrite(YELLOW, HIGH);
        } else {
          stare = 0;
        }
      }
    break;
  }
}
