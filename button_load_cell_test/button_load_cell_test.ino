/*
 * HX711 Load Cell Reader
 * Phidgets 50kg Button Load Cell
 * 
 * Wiring:
 * HX711 DT  → ESP32 GPIO 22
 * HX711 SCK → ESP32 GPIO 23
 * HX711 VCC → ESP32 5V (or 3.3V)
 * HX711 GND → ESP32 GND
 * 
 * Load Cell Red    → HX711 E+
 * Load Cell Black  → HX711 E-
 * Load Cell White  → HX711 A-
 * Load Cell Green  → HX711 A+
 */

#include <HX711.h>

// Pin definitions
#define DT_PIN 22
#define SCK_PIN 23

// Create HX711 object
HX711 scale;

// Calibration factor - you'll need to adjust this!
// This is an initial estimate for a 50kg load cell
float calibration_factor = 2280.0;  // Adjust after calibration

// Conversion: 1 kg = 9.81 Newtons
#define KG_TO_NEWTONS 9.81

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n================================");
  Serial.println("HX711 Load Cell Reader");
  Serial.println("Phidgets 50kg Button Load Cell");
  Serial.println("================================\n");
  
  // Initialize HX711
  scale.begin(DT_PIN, SCK_PIN);
  
  Serial.println("Initializing load cell...");
  delay(1000);
  
  if (scale.is_ready()) {
    Serial.println("✓ HX711 found!");
    
    // Set calibration factor
    scale.set_scale(calibration_factor);
    
    // Tare (zero) the scale
    Serial.println("\nTaring... Remove all weight from load cell.");
    delay(2000);
    scale.tare();
    Serial.println("✓ Tare complete!");
    
    Serial.println("\n--- Ready to measure ---");
    Serial.println("Format: Raw | Kilograms | Newtons\n");
    
  } else {
    Serial.println("✗ HX711 not found!");
    Serial.println("Check wiring:");
    Serial.println("  DT  → GPIO 22");
    Serial.println("  SCK → GPIO 23");
    Serial.println("  VCC → 5V or 3.3V");
    Serial.println("  GND → GND");
  }
}

void loop() {
  if (scale.is_ready()) {
    
    // Read raw value (before calibration)
    long raw_value = scale.read();
    
    // Read calibrated weight in kg
    float weight_kg = scale.get_units(10);  // Average of 10 readings
    
    // Convert to Newtons (Force = mass × gravity)
    float force_newtons = weight_kg * KG_TO_NEWTONS;
    
    // Display results
    Serial.print("Raw: ");
    Serial.print(raw_value);
    Serial.print(" | ");
    
    Serial.print("Weight: ");
    Serial.print(weight_kg, 2);
    Serial.print(" kg | ");
    
    Serial.print("Force: ");
    Serial.print(force_newtons, 2);
    Serial.println(" N");
    
    delay(500);  // Update every 500ms
    
  } else {
    Serial.println("HX711 not ready!");
    delay(1000);
  }
}