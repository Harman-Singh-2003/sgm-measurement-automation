/*
 * ESP32 Actuator Controller - SAFE VERSION
 * Includes Hardware Deadband and Software Interlocks
 * * Commands (send via Serial Monitor at 115200 baud):
 * 'e' or 'E' - Extend
 * 'r' or 'R' - Retract
 * 's' or 'S' - Stop
 */

// Pin definitions
#define R_EN 25
#define L_EN 26
#define RPWM 32
#define LPWM 33

// State Machine
// Track the current state to prevent redundant commands
enum MotorState { STOPPED, EXTENDING, RETRACTING };
MotorState currentState = STOPPED;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  
  // Force everything LOW initially
  stopActuator();
  
  Serial.println("--- Actuator Controller Ready ---");
  Serial.println("SAFETY INTERLOCKS: ENABLED");
  Serial.println("Type a command and press Enter:");
  Serial.println(" [e] -> Extend");
  Serial.println(" [r] -> Retract");
  Serial.println(" [s] -> Stop");
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == 'e' || cmd == 'E') {
      Serial.println(">>> EXTENDING");
      extendActuator();
    } 
    else if (cmd == 'r' || cmd == 'R') {
      Serial.println("<<< RETRACTING");
      retractActuator();
    } 
    else if (cmd == 's' || cmd == 'S') {
      Serial.println("|| STOPPING");
      stopActuator();
    }
  }
}

// --- Safe Movement Functions ---

void extendActuator() {
  if (currentState == EXTENDING) return; // Do nothing if already extending

  // 1. HARD STOP FIRST (The Interlock)
  // Guarantee all pins are completely LOW
  digitalWrite(L_EN, LOW);
  digitalWrite(LPWM, LOW);
  digitalWrite(R_EN, LOW);
  digitalWrite(RPWM, LOW);
  
  // 2. DEADBAND DELAY
  // Wait 50ms to ensure the physical silicon transistors fully close 
  // before opening the other side. This prevents shoot-through shorts.
  delay(50);
  
  // 3. ENGAGE FORWARD
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
  digitalWrite(RPWM, HIGH);
  
  currentState = EXTENDING;
}

void retractActuator() {
  if (currentState == RETRACTING) return; // Do nothing if already retracting

  // 1. HARD STOP FIRST (The Interlock)
  digitalWrite(R_EN, LOW);
  digitalWrite(RPWM, LOW);
  digitalWrite(L_EN, LOW);
  digitalWrite(LPWM, LOW);
  
  // 2. DEADBAND DELAY
  delay(50);
  
  // 3. ENGAGE REVERSE
  digitalWrite(L_EN, HIGH);
  digitalWrite(R_EN, HIGH);
  digitalWrite(LPWM, HIGH);
  
  currentState = RETRACTING;
}

void stopActuator() {
  if (currentState == STOPPED) return; // Do nothing if already stopped

  // Immediately kill all power
  digitalWrite(R_EN, LOW);
  digitalWrite(RPWM, LOW);
  digitalWrite(L_EN, LOW);
  digitalWrite(LPWM, LOW);
  
  currentState = STOPPED;
}