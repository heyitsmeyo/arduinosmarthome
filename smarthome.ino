#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>

#define SS_PIN 53   // Mega: 53 for SPI
#define RST_PIN 9   // Any digital pin for reset
#define BUZZER_PIN 8
#define SERVO_PIN 6  // Mega: 6 for PWM
#define MOSI_PIN 51  // Mega: 51 for SPI
#define MISO_PIN 50  // Mega: 50 for SPI
#define SCK_PIN 52   // Mega: 52 for SPI
#define GREEN_LED_PIN 22  // Mega: Any digital pin for green LED
#define RED_LED_PIN 24    // Mega: Any digital pin for red LED

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Allowed RFID card UID (Update this with your card's UID)
byte allowedCardUid[] = {0xAA, 0xBB, 0xCC, 0xDD};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(SERVO_PIN);
  lcd.begin(16, 2);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.print("RFID Tag:");
    for (byte i = 0; i < rfid.uid.size; i++) {
      lcd.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      lcd.print(rfid.uid.uidByte[i], HEX);
    }

    if (compareUid(rfid.uid.uidByte, allowedCardUid, rfid.uid.size)) {
      lcd.setCursor(0, 1);
      lcd.print("Access granted");
      servo.write(90);
      tone(BUZZER_PIN, 1000, 500);
      digitalWrite(GREEN_LED_PIN, HIGH);
      delay(5000);
      digitalWrite(GREEN_LED_PIN, LOW);
      servo.write(0);
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Access refused");
      tone(BUZZER_PIN, 200, 1000);
      digitalWrite(RED_LED_PIN, HIGH);
      delay(2000);
      digitalWrite(RED_LED_PIN, LOW);
    }
  }
}

bool compareUid(byte* uid1, byte* uid2, byte size) {
  for (byte i = 0; i < size; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}
