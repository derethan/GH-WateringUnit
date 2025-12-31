#include <Arduino.h>
#include "config.h"

#include "PumpController.h"

// ============= PUMP INSTANCES =============
PumpController waterPump1(RELAY_PIN_1, false); // Normal logic
PumpController waterPump2(RELAY_PIN_2, false); // Normal logic

// Array of pump pointers for static helper methods
PumpController *pumps[] = {&waterPump1, &waterPump2};
const uint8_t NUM_PUMPS = 2; // Update this if you add more pumps
const uint8_t PUMP_PINS[] = {RELAY_PIN_1, RELAY_PIN_2};

// ============= GLOBAL VARIABLES =============
unsigned long lastSerialUpdate = 0;
unsigned long lastDebugPrint = 0;

const unsigned long SERIAL_UPDATE_INTERVAL = 100; // Update status every 100ms
const unsigned long HYSTERESIS = 1 * 30 * 1000;   // Hysteresis to prevent rapid toggling (ms) -
const unsigned long PUMP_ON_TIME_MS = 5000;       // Default pump test ON time - 5 seconds (5000 ms)
const unsigned long PUMP_OFF_TIME_MS = 20000;     // Default pump test OFF time - 5 minutes (300000 ms)
const uint16_t PUMP_CYCLES = 1;                   // Default number of

// Command buffer for serial input
String commandBuffer = "";

// ============= FUNCTION DECLARATIONS =============
void printMenu();
void processCommand(String command);

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("Greenhouse - Water Pump Controller");
  Serial.println("========================================\n");

  // Initialize pump pins
  waterPump1.begin();
  waterPump2.begin();

  // Configure water level sensor pins (INPUT_PULLUP for active-low detection)
  pinMode(ARM_PIN_1, INPUT_PULLUP);
  pinMode(ARM_PIN_2, INPUT_PULLUP);

  // Configure relay control pins (OUTPUT)
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  // Initialize relays to OFF
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_2, LOW);
}

void loop()
{

  // Handle serial input
  if (Serial.available())
  {
    char c = Serial.read();

    if (c == '\n' || c == '\r')
    {
      if (commandBuffer.length() > 0)
      {
        processCommand(commandBuffer);
        commandBuffer = "";
        printMenu();
      }
    }
    else if (c >= 32 && c <= 126)
    { // Printable ASCII characters
      commandBuffer += c;
    }
  }

  // Pump control logic with hysteresis - PUMP 1
  // Check water level sensors (LOW = contact with ground = water full)

  uint32_t lastPump = waterPump1.getLastActionTime();

  if ((millis() - lastPump) > HYSTERESIS || lastPump == 0)
  {
    if (digitalRead(ARM_PIN_1) == HIGH)
    {
      // Water level low - turn pump ON
      waterPump1.on();
    }
    else
    {
      // Water level full - turn pump OFF
      waterPump1.off();

      // Optional: Print status
      Serial.println("Pump 1 OFF - Water level full");
    }
  }

  // Print pump statuses periodically
  if (millis() - lastDebugPrint >= DEBUG_FREQ)
  {
    PumpController::printAllStatus(pumps, PUMP_PINS, NUM_PUMPS);
    lastDebugPrint = millis();
  }

  delay(LOOP_DELAY_MS);
}

// ============= MENU DISPLAY =============
void printMenu()
{
  Serial.println("\n--- PUMP CONTROL MENU ---");
  Serial.println("1 - Test Pump 1");
  Serial.println("2 - Test Pump 2");
  Serial.println("3 - Test Pump 3");
  Serial.println("4 - Test Pump 4");
  Serial.println("5 - Turn all pumps ON");
  Serial.println("6 - Turn all pumps OFF");
  Serial.println("7 - Test all pumps");
  Serial.println("8 - Custom test (enter: 8,pump#,onMS,offMS,cycles)");
  Serial.println("S - Print pump status");
  Serial.println("H - Print this menu");
  Serial.print("Enter command: ");
}

// ============= COMMAND PROCESSING =============
void processCommand(String command)
{
  command.toUpperCase();
  command.trim();

  // Single character commands
  if (command == "1")
  {
    PumpController::testSinglePump(pumps, 1, 1000, 1000, 5); // 1s on, 1s off, 5 cycles
  }
  else if (command == "2")
  {
    PumpController::testSinglePump(pumps, 2, 1000, 1000, 5);
  }
  else if (command == "3")
  {
    PumpController::testSinglePump(pumps, 3, 1000, 1000, 5);
  }
  else if (command == "4")
  {
    PumpController::testSinglePump(pumps, 4, 1000, 1000, 5);
  }
  else if (command == "5")
  {
    Serial.println("Turning all pumps ON...");
    PumpController::setAllPumps(pumps, NUM_PUMPS, true);
    delay(5000); // Keep on for 5 seconds
    PumpController::setAllPumps(pumps, NUM_PUMPS, false);
    Serial.println("All pumps turned OFF.");
  }
  else if (command == "6")
  {
    Serial.println("Turning all pumps OFF...");
    PumpController::setAllPumps(pumps, NUM_PUMPS, false);
  }
  else if (command == "7")
  {
    PumpController::testAllPumps(pumps, NUM_PUMPS);
  }
  else if (command == "S")
  {
    PumpController::printAllStatus(pumps, PUMP_PINS, NUM_PUMPS);
  }
  else if (command == "H")
  {
    // Just reprint the menu, This is handled after processing anyway
  }
  else if (command.startsWith("8"))
  {
    // Custom test format: 8,pump#,onMS,offMS,cycles
    // Example: 8,1,500,500,10
    int firstComma = command.indexOf(',');
    if (firstComma > 0)
    {
      int secondComma = command.indexOf(',', firstComma + 1);
      int thirdComma = command.indexOf(',', secondComma + 1);
      int fourthComma = command.indexOf(',', thirdComma + 1);

      if (fourthComma > 0)
      {
        uint8_t pumpNum = command.substring(firstComma + 1, secondComma).toInt();
        uint32_t onTime = command.substring(secondComma + 1, thirdComma).toInt();
        uint32_t offTime = command.substring(thirdComma + 1, fourthComma).toInt();
        uint16_t cycles = command.substring(fourthComma + 1).toInt();

        if (pumpNum >= 1 && pumpNum <= 4 && onTime > 0 && offTime > 0 && cycles > 0)
        {
          PumpController::testSinglePump(pumps, pumpNum, onTime, offTime, cycles);
        }
        else
        {
          Serial.println("Invalid parameters!");
          Serial.println("Format: 8,pump#(1-4),onMS,offMS,cycles");
        }
      }
      else
      {
        Serial.println("Invalid format!");
        Serial.println("Format: 8,pump#(1-4),onMS,offMS,cycles");
      }
    }
  }
  else
  {
    Serial.println("Unknown command!");
  }
}

// ============= END OF FILE =============