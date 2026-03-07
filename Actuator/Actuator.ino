/*
 * ESP32-2432S028R Actuator Controller
 * Controls 12V linear actuator via HW-039 motor driver
 * Compatible with ESP32 Arduino Core 3.x
 * 
 * Wiring:
 * ESP32 GPIO 25 → HW-039 R_EN
 * ESP32 GPIO 26 → HW-039 L_EN
 * ESP32 GPIO 32 → HW-039 RPWM
 * ESP32 GPIO 33 → HW-039 LPWM
 * ESP32 5V     → HW-039 VCC (logic power)
 * ESP32 GND    → HW-039 GND
 * 12V Supply   → HW-039 B+ (motor power)
 * 12V GND      → HW-039 B- (motor power)
 * Actuator     → HW-039 M+ and M-
 */

/// Pin definitions
#define R_EN 25
#define L_EN 26
#define RPWM 32
#define LPWM 33

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  
  // Force everything LOW initially
  digitalWrite(R_EN, LOW);
  digitalWrite(L_EN, LOW);
  digitalWrite(RPWM, LOW);
  digitalWrite(LPWM, LOW);
  
  Serial.println("BTS7960 Test - Ready");
  Serial.println("Starting in 3 seconds...");
  delay(3000);
}

void loop() {
  // TEST 1: Extend with full signals
  Serial.println("\n=== TEST 1: EXTENDING ===");
  Serial.println("R_EN = HIGH, RPWM = HIGH");
  digitalWrite(R_EN, HIGH);
  digitalWrite(RPWM, HIGH);
  digitalWrite(L_EN, LOW);
  digitalWrite(LPWM, LOW);
  
  Serial.println("Measure M+ and M- now!");
  delay(5000);  // 5 seconds to measure
  
  // STOP
  Serial.println("\n=== STOPPING ===");
  digitalWrite(R_EN, LOW);
  digitalWrite(RPWM, LOW);
  delay(2000);
  
  // TEST 2: Retract with full signals
  Serial.println("\n=== TEST 2: RETRACTING ===");
  Serial.println("L_EN = HIGH, LPWM = HIGH");
  digitalWrite(L_EN, HIGH);
  digitalWrite(LPWM, HIGH);
  digitalWrite(R_EN, LOW);
  digitalWrite(RPWM, LOW);
  
  Serial.println("Measure M+ and M- now!");
  delay(5000);  // 5 seconds to measure
  
  // STOP
  Serial.println("\n=== STOPPING ===");
  digitalWrite(L_EN, LOW);
  digitalWrite(LPWM, LOW);
  delay(2000);
}