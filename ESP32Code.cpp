#define BLYNK_TEMPLATE_ID "TMPL6Ehf_7I8G"
#define BLYNK_TEMPLATE_NAME "Smart Urometer"
#define BLYNK_AUTH_TOKEN "E75hSPPqAaJ_1vB-OXrOzyxuDup3gCGX"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"

// HX711 Pins
#define DOUT 5
#define CLK 18

#define BUZZER_PIN 26 // Define the GPIO pin for the buzzer
#define DENSITY_TRIGGER_PIN 2 // GPIO 2 for density trigger

HX711 scale(DOUT, CLK);

int densityCount = 0;

// Wi-Fi credentials
char ssid[] = "";
char pass[] = "";

// Variables
float weight;
float density = 1017.50; // Initial default density
float volume;
float calibration_factor = 211000; // Adjust as per calibration
volatile bool densityTrigger = false; // Flag for density trigger
float newDensity = 1017.50; // Calculated new density
float weightAt25ml = 0.0; // Weight at the moment of GPIO trigger

// Notification thresholds
float volumeThreshold = 25.0; // Start volume threshold (in ml)
int timeThreshold = 50; // Start time threshold (in seconds)
float initialVolumeThreshold = 25.0; // Store input volume threshold from Blynk
int initialTimeThreshold = 100; // Store input time threshold from Blynk
unsigned long thresholdStartTime = 0; // Timer to track low volume duration
bool belowThreshold = false;

// ISR for Density Calculation Trigger
void IRAM_ATTR densityISR() {
    if (!densityTrigger) {
        densityTrigger = true;
        Serial.println("----- GPIO 2 Triggered: Calculating New Density -----");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Uro weight monitor with Blynk ...");

    // Initialize HX711
    scale.set_scale();
    scale.tare(); // Reset the scale
    long zero_factor = scale.read_average();
    Serial.print("Zero factor: ");
    Serial.println(zero_factor);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    pinMode(DENSITY_TRIGGER_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(DENSITY_TRIGGER_PIN), densityISR, RISING);

    // Connect to Wi-Fi and Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
    Blynk.run();
    if (densityTrigger) {
        calculateDensityOnce();
    }
    measureWeight();
    checkVolumeThreshold();
}

void calculateDensityOnce() {
    if (densityCount < 3) {
        scale.set_scale(calibration_factor);
        weightAt25ml = scale.get_units(5);
        if (weightAt25ml < 0) {
            weightAt25ml = 0.0;
        }
        newDensity = weightAt25ml / 0.000075;
        density = newDensity;

        Serial.println("----- Density Updated -----");
        Serial.print("Captured Weight at Trigger: ");
        Serial.print(weightAt25ml);
        Serial.println(" kg");
        Serial.print("New Density Calculated: ");
        Serial.print(newDensity);
        Serial.println(" kg/m^3");

        densityCount += 1;
    }
}

void measureWeight() {
    scale.set_scale(calibration_factor);
    weight = scale.get_units(5);
    if (weight < 0) {
        weight = 0.0;
    }
    if (weight > 0.4) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(2000);
        digitalWrite(BUZZER_PIN, LOW);
    }
    float vol = weight / density;
    volume = vol * 1000000;

    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.print(" kg | Volume: ");
    Serial.print(volume);
    Serial.print(" ml | Density: ");
    Serial.print(density);
    Serial.println();

    Blynk.virtualWrite(V0, volume);
    Blynk.virtualWrite(V1, density);
}

void checkVolumeThreshold() {
    if (volume < volumeThreshold) {
        if (!belowThreshold) {
            belowThreshold = true;
            thresholdStartTime = millis();
        } else {
            if ((millis() - thresholdStartTime) >= (timeThreshold * 1000)) {
                Blynk.logEvent("low_level", String("Volume below " + String(volumeThreshold) + "ml for " + String(timeThreshold) + " seconds!"));
                Serial.println("Notification sent: Volume below threshold!");

                digitalWrite(BUZZER_PIN, HIGH);
                delay(2000);
                digitalWrite(BUZZER_PIN, LOW);

                volumeThreshold = initialVolumeThreshold;
                timeThreshold = initialTimeThreshold;

                thresholdStartTime = millis();
                belowThreshold = false;
            }
        }
    } else {
        belowThreshold = false;
    }
}

// Handle volume threshold input from the Blynk app (Virtual Pin V2)
BLYNK_WRITE(V2) {
    initialVolumeThreshold = param.asFloat();
    volumeThreshold = initialVolumeThreshold;
    Serial.print("Volume Threshold updated to: ");
    Serial.println(volumeThreshold);
}

// Handle time threshold input from the Blynk app (Virtual Pin V3)
BLYNK_WRITE(V3) {
    initialTimeThreshold = param.asInt();
    timeThreshold = initialTimeThreshold;
    Serial.print("Time Threshold updated to: ");
    Serial.println(timeThreshold);
}
