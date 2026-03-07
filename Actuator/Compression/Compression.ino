/*
 * ESP32 Smart Press Controller
 * Combines BTS7960 Actuator Control with HX711 Force Feedback
 */

#include <HX711.h>

// --- ACTUATOR PINS ---
#define R_EN 25
#define L_EN 26
#define RPWM 32
#define LPWM 33

// --- LOAD CELL PINS ---
#define DT_PIN 22
#define SCK_PIN 23

// --- SETTINGS ---
#define KG_TO_NEWTONS 9.81
float calibration_factor = 2280.0;
float targetForceN = 0.0;
unsigned long retractStartTime = 0;

// State Machine
enum MotorState { STOPPED, EXTENDING, RETRACTING, PRESSING };
MotorState currentState = STOPPED;

HX711 scale;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize Actuator Pins
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  stopActuator(); // Force LOW initially
  
  // Initialize Load Cell
  Serial.println("\nInitializing load cell...");
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(calibration_factor);
  
  Serial.println("Taring... Please wait and keep scale empty.");
  delay(1000);
  scale.tare();
  Serial.println("✓ Tare complete!");
  
  // Print Menu
  Serial.println("\n--- SMART PRESS READY ---");
  Serial.println("COMMANDS:");
  Serial.println(" [Any Number] -> Set Target Force (N) & Start Pressing");
  Serial.println(" [t] -> Tare the scale to 0");
  Serial.println(" [r] -> Manual Retract");
  Serial.println(" [s] -> Emergency Stop");
}

void loop() {
  // 1. Check for Serial Commands
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;

    if (input.equalsIgnoreCase("s")) {
      Serial.println("|| EMERGENCY STOP");
      stopActuator();
    } 
    else if (input.equalsIgnoreCase("r")) {
      Serial.println("<<< MANUAL RETRACT");
      retractActuator();
      retractStartTime = millis(); // Track time for auto-timeout
    }
    else if (input.equalsIgnoreCase("t")) {
      Serial.println("0.0 Taring scale...");
      stopActuator(); // Safety stop before taring
      scale.tare();
      Serial.println("✓ Tared");
    }
    else {
      // If it's not a letter, assume it's a target force number
      float requestedForce = input.toFloat();
      if (requestedForce > 0) {
        targetForceN = requestedForce;
        Serial.print("\n>>> STARTING PRESS CYCLE. Target: ");
        Serial.print(targetForceN);
        Serial.println(" N");
        startPressing();
      } else {
        Serial.println("! Invalid force target. Must be > 0.");
      }
    }
  }

  // 2. The Active Pressing Feedback Loop
  if (currentState == PRESSING) {
    // Only read if new data is available to prevent blocking the loop
    if (scale.is_ready()) {
      // Grab a SINGLE reading for maximum speed, apply your math
      float weight_kg = scale.get_units(1); 
      float cal_weight = weight_kg / 18.0; 
      float current_force = cal_weight * KG_TO_NEWTONS;

      // Print live data so you can watch it ramp up
      Serial.print("Live Force: ");
      Serial.print(current_force, 1);
      Serial.println(" N");

      // THE TRIGGER
      if (current_force >= targetForceN) {
        Serial.println("\n!!! TARGET FORCE REACHED !!!");
        Serial.println("Stopping and Retracting...");
        
        stopActuator();
        delay(100); // 100ms Deadband to prevent shoot-through
        retractActuator();
        
        retractStartTime = millis(); // Start the clock on retraction
      }
    }
  }

  // 3. Auto-Stop Retraction (Safety Timeout)
  // Your 6-inch actuator at 5mm/s takes ~30 seconds to fully retract.
  // We let it run for 35s to hit the internal limit switch, then turn off the driver.
  if (currentState == RETRACTING && (millis() - retractStartTime > 35000)) {
    Serial.println("--- Retract sequence complete (Timeout) ---");
    stopActuator();
  }
}

// --- Movement Functions ---

void startPressing() {
  if (currentState == PRESSING) return;
  
  // Ensure we start from a clean stop
  stopActuator();
  delay(50);
  
  // Engage Forward
  digitalWrite(L_EN, HIGH);
  digitalWrite(R_EN, HIGH);
  digitalWrite(RPWM, HIGH);
  
  currentState = PRESSING;
}

void retractActuator() {
  if (currentState == RETRACTING) return;

  stopActuator();
  delay(50); // Deadband
  
  // Engage Reverse
  digitalWrite(L_EN, HIGH);
  digitalWrite(R_EN, HIGH);
  digitalWrite(LPWM, HIGH);
  
  currentState = RETRACTING;
}

void stopActuator() {
  // Immediately kill all power
  digitalWrite(R_EN, LOW);
  digitalWrite(RPWM, LOW);
  digitalWrite(L_EN, LOW);
  digitalWrite(LPWM, LOW);
  
  currentState = STOPPED;
}