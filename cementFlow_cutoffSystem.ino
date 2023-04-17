#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//loadcell
#define CLK  2
#define DOUT  3
//relay
#define relay 4
//rotary encoder
#define SW 5
#define DT 6
#define C 7
//button
#define buttonstart 8
#define buttonstop 12
LiquidCrystal_I2C lcd(0x27 , 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//sda LCD = A4
//scl LCD = A5
HX711 myLoadCell;

int state = 0;
float calibration = 12525; //Kalibrasi di sini
float units;
static char massa[6];
int counter = 0;
float angle = 38.1;
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;
unsigned long start_time, start_time2;


void setup() {
  Serial.begin(9600);
  
  //  relay
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  //  button
  pinMode(buttonstart, INPUT_PULLUP);
  pinMode(buttonstop, INPUT_PULLUP);

  //  rotary encoder
  pinMode(C, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(C);

  //  loadcell
  myLoadCell.begin(DOUT, CLK);
  myLoadCell.set_scale();
  myLoadCell.tare();
  long zero_factor = myLoadCell.read_average();

  //  lcd
  lcd.begin(16, 2);
}


void loop() {
  switch (state) {
    case 0:
      rotary();
      if (digitalRead(buttonstart) == LOW) {
        digitalWrite(relay, HIGH);
        lcd.begin(16, 2);
        state = 1;
      }
      Serial.println(state);
      break;

    case 1:
      loadcell();
      if (units >= angle) {
        digitalWrite(relay, LOW); // relay nyala
        lcd.begin(16, 2);
        state = 0;
      }
      if (digitalRead(buttonstop) == LOW) {
        digitalWrite(relay, LOW); // relay nyala
        lcd.begin(16, 2);
        state = 0;
      }
      Serial.println(state);
      break;
  }

  if ((millis() - start_time) >= 100) {
    start_time = millis();
    tampilan();
    }
  }
void loadcell () {
  myLoadCell.set_scale(calibration);
  units = myLoadCell.get_units(), 1;
  if (units < 0)
  {
    units = 0.00;
  }
  Serial.print("Berat: ");
  Serial.print(units);
  Serial.println(" gr");
  dtostrf(units, -5, 1, massa);
}

void rotary() {
  currentStateCLK = digitalRead(C);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(DT) != currentStateCLK) {
      counter ++;
      angle = angle + 0.05;
    }
    else {
      counter --;
      angle = angle - 0.05;
    }
    if (counter >= 30 ) {
      counter = 0;
    }
    Serial.print("Mode : ");
    Serial.println(angle);
  }
  lastStateCLK = currentStateCLK;
}

void tampilan() {
  switch (state) {
    case 0:
      lcd.setCursor(0, 0); lcd.print("Berat :");
      lcd.setCursor(8, 0);
      lcd.print("WAITING ");
      lcd.setCursor(0, 1); lcd.print("Mode  :");
      lcd.setCursor(8, 1); lcd.print(angle, 1);
      lcd.setCursor(13, 1); lcd.print(" kg");
      break;
    case 1:
      lcd.setCursor(0, 0); lcd.print("Berat :");
      lcd.setCursor(8, 0); lcd.print(massa);
      lcd.setCursor(13, 0); lcd.print(" kg");
      lcd.setCursor(0, 1); lcd.print("Mode  :");
      lcd.setCursor(8, 1); lcd.print(angle, 1);
      lcd.setCursor(13, 1); lcd.print(" kg");
      break;
  }
}
