#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "MAX30105.h"
#include "heartRate.h"

#define ALERT_BUZZER_PIN    8
#define FINGER_IR_THRESHOLD 50000
#define RATE_AVG_SIZE       4

MAX30105 particleSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte rates[RATE_AVG_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute = 0.0f;
int beatAvg = 0;
uint32_t lastLcdUpdate = 0;

// Estimate SpO2 from Red/IR AC-to-DC Ratio
float calculateSpO2(uint32_t redValue, uint32_t irValue) {
    if (irValue == 0) return 0.0f;
    float ratio = (float)redValue / (float)irValue;
    float spo2 = 110.0f - 18.0f * ratio;
    if (spo2 > 100.0f) spo2 = 99.8f;
    if (spo2 < 70.0f)  spo2 = 70.0f;
    return spo2;
}

void setup() {
    Serial.begin(115200);
    pinMode(ALERT_BUZZER_PIN, OUTPUT);
    digitalWrite(ALERT_BUZZER_PIN, LOW);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Healthcare DAQ");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");

    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        lcd.clear();
        lcd.print("Sensor Error!");
        while (1);
    }

    // Configure sensor: LED brightness, sample average, mode (Red + IR)
    particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);
    delay(1000);
    lcd.clear();
}

void loop() {
    uint32_t irValue  = particleSensor.getIR();
    uint32_t redValue = particleSensor.getRed();

    // Detect heartbeat peak on IR waveform
    if (checkForBeat(irValue) == true) {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60.0f / (delta / 1000.0f);

        if (beatsPerMinute < 220 && beatsPerMinute > 35) {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= RATE_AVG_SIZE;

            beatAvg = 0;
            for (byte x = 0; x < RATE_AVG_SIZE; x++) {
                beatAvg += rates[x];
            }
            beatAvg /= RATE_AVG_SIZE;
        }
    }

    // Refresh LCD every 500 ms without blocking PPG sampling
    if (millis() - lastLcdUpdate >= 500) {
        lastLcdUpdate = millis();

        if (irValue < FINGER_IR_THRESHOLD) {
            lcd.setCursor(0, 0);
            lcd.print("Place Finger on ");
            lcd.setCursor(0, 1);
            lcd.print("Sensor Probe... ");
            digitalWrite(ALERT_BUZZER_PIN, LOW);
        } else {
            float spo2 = calculateSpO2(redValue, irValue);

            lcd.setCursor(0, 0);
            lcd.print("BPM : ");
            lcd.print(beatAvg);
            lcd.print(" bpm    ");

            lcd.setCursor(0, 1);
            lcd.print("SpO2: ");
            lcd.print(spo2, 1);
            lcd.print(" %     ");

            // Trigger alert if SpO2 drops below 92%
            digitalWrite(ALERT_BUZZER_PIN, (spo2 < 92.0f) ? HIGH : LOW);

            Serial.print("BPM=");
            Serial.print(beatAvg);
            Serial.print(", SpO2=");
            Serial.println(spo2, 1);
        }
    }
}
