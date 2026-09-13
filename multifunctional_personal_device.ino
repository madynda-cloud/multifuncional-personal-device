#include <U8g2lib.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- PINY A KONFIGURACE ---

// OLED displej (SSD1306)
#define OLED_CS 8
#define OLED_DC 9
#define OLED_RES 10
#define OLED_D0 13
#define OLED_D1 11

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R0, OLED_CS, OLED_DC, OLED_RES);

// Ultrazvukový senzor (HC-SR04)
#define TRIG_PIN 2
#define ECHO_PIN 3

// Teplotní senzor (DS18B20)
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Srdeční tep (Pulse Sensor)
const int PulseSensorPurplePin = A0;  
const int LED = 7;                    
int Signal;
int Threshold = 508; 
int beatCount = 0;
unsigned long startTime = 0;
const int bpmInterval = 10000;
bool beatDetected = false;

// Proměnné pro uchování dat
float distance = 0.0;
float temperature = 0.0;
int bpm = 0;

// Čas pro obnovení displeje
unsigned long prepis = 0;

// --- FUNKCE PRO SENZORY ---

// Funkce pro měření vzdálenosti
float zmerVzdalenost() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.0343 / 2;  // Přepočet na cm
}

// Funkce pro měření teploty
float zmerTeplotu() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

// Funkce pro kalibraci Threshold
// void kalibrujThreshold(int novaHodnota) {
//   const float alpha = 0.3; // Míra přizpůsobení (0.0 - 1.0)
//   Threshold = alpha * novaHodnota + (1 - alpha) * Threshold;
//   Serial.print("Novy Threshold (EMA): ");
//   Serial.println(Threshold);
// }


// Funkce pro vykreslení dat na OLED
void vykresli(float dist, float temp, int bpm) {
  oled.clearBuffer();
  oled.setFont(u8g2_font_ncenB08_tr);

  // Zobrazení vzdálenosti
  oled.setCursor(0, 15);
  oled.print("Vzdalenost: ");
  oled.print(dist);
  oled.print(" cm");

  // Zobrazení teploty
  oled.setCursor(0, 30);
  oled.print("Teplota: ");
  if (temp == DEVICE_DISCONNECTED_C) {
    oled.print("Error");
  } else {
    oled.print(temp);
    oled.print(" C");
  }

  // Zobrazení BPM
  oled.setCursor(0, 45);
  oled.print("Tep: ");
  oled.print(bpm);
  oled.print(" BPM");

  oled.sendBuffer();
}

// --- SETUP ---
void setup() {
  Serial.begin(9600);

  // Inicializace OLED
  oled.begin();
  oled.setFont(u8g2_font_ncenB08_tr);

  // Inicializace senzorů
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED, OUTPUT);

  sensors.begin(); // Teplotní senzor
  startTime = millis(); // Časovač pro BPM

  //kalibrujThreshold(Signal); // První kalibrace Threshold
}

// --- LOOP ---
void loop() {
  // --- Měření vzdálenosti ---
  distance = zmerVzdalenost();

  // --- Měření teploty ---
  temperature = zmerTeplotu();

  // --- Měření BPM ---
  Signal = analogRead(PulseSensorPurplePin);
  Serial.println(Signal);
  if (Signal >= Threshold) {
    digitalWrite(LED, HIGH);
    if (!beatDetected) { // Detekce nového úderu
      beatDetected = true;
      beatCount++;
    }
  } else {
    digitalWrite(LED, LOW);
    beatDetected = false;
  }

  // Výpočet BPM po intervalu
  if (millis() - startTime >= bpmInterval) {
    bpm = (beatCount * 60000) / bpmInterval; 
    Serial.println("BPM: " + String(bpm)); // Výpis do sériového monitoru
    
    // Reset časovače a počítadla tepů
    beatCount = 0;
    startTime = millis();

    // Kalibrace Threshold po výpočtu BPM
    //kalibrujThreshold(Signal);
  }

  // --- Obnova displeje ---
  if (millis() - prepis > 500) {
    vykresli(distance, temperature, bpm);
    prepis = millis();
  }

  delay(20); // Krátké zpoždění pro stabilitu
}
