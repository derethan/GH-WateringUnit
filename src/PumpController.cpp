#include "PumpController.h"

PumpController::PumpController(uint8_t pin, bool inverted)
  : _pin(pin), _inverted(inverted), _state(false), _stateChangeTime(0), _totalOnTime(0), _lastActionTime(0) {
}

void PumpController::begin() {
  pinMode(_pin, OUTPUT);
  off(); // Start with pump off
}

void PumpController::on() {
  setState(true);
}

void PumpController::off() {
  setState(false);
}

void PumpController::setState(bool state) {
  if (state == _state) {
    return; // No state change needed
  }

  // Update total on-time if we were on
  if (_state) {
    _totalOnTime += getStateTime_ms();
  }

  _state = state;
  _stateChangeTime = millis();
  
  // Only track last action time when pump turns OFF (for hysteresis)
  // This allows immediate OFF but prevents rapid re-triggering ON
  if (!state) {
    _lastActionTime = millis();
  }

  // Set pin based on logic (inverted or normal)
  if (_inverted) {
    digitalWrite(_pin, state ? LOW : HIGH);
  } else {
    digitalWrite(_pin, state ? HIGH : LOW);
  }
}

uint32_t PumpController::getStateTime_ms() const {
  return millis() - _stateChangeTime;
}

void PumpController::resetStats() {
  _totalOnTime = 0;
}

void PumpController::testSequence(uint32_t onTime_ms, uint32_t offTime_ms, uint16_t cycles, bool printResults) {
  Serial.println("\n========== PUMP TEST SEQUENCE ==========");
  Serial.print("Pump Pin: ");
  Serial.println(_pin);
  Serial.print("On Time: ");
  Serial.print(onTime_ms);
  Serial.println(" ms");
  Serial.print("Off Time: ");
  Serial.print(offTime_ms);
  Serial.println(" ms");
  Serial.print("Cycles: ");
  Serial.println(cycles);
  Serial.println("Starting test...\n");

  resetStats();

  for (uint16_t cycle = 1; cycle <= cycles; cycle++) {
    // Turn pump on
    on();
    if (printResults) {
      Serial.print("Cycle ");
      Serial.print(cycle);
      Serial.println(" - PUMP ON");
    }
    delay(onTime_ms);

    // Turn pump off
    off();
    if (printResults) {
      Serial.print("Cycle ");
      Serial.print(cycle);
      Serial.println(" - PUMP OFF");
    }
    delay(offTime_ms);
  }

  // Final summary
  if (printResults) {
    Serial.println("\n========== TEST COMPLETE ==========");
    Serial.print("Total On Time: ");
    Serial.print(_totalOnTime + getStateTime_ms());
    Serial.println(" ms");
    uint32_t totalCycleTime = cycles * (onTime_ms + offTime_ms);
    Serial.print("Total Cycle Time: ");
    Serial.print(totalCycleTime);
    Serial.println(" ms");
    Serial.print("Duty Cycle: ");
    Serial.print(((_totalOnTime + onTime_ms) * 100) / totalCycleTime);
    Serial.println("%");
    Serial.println("=========================================\n");
  }
}

void PumpController::printStatus(uint8_t pumpNumber) const {
  Serial.print("Pump ");
  Serial.print(pumpNumber);
  Serial.print(" (Pin ");
  Serial.print(_pin);
  Serial.print("): ");
  Serial.println(_state ? "ON" : "OFF");
}

void PumpController::testSinglePump(PumpController* pumps[], uint8_t pumpNumber, uint32_t onTime_ms, uint32_t offTime_ms, uint16_t cycles) {
  Serial.print("\nTesting Pump ");
  Serial.println(pumpNumber);

  if (pumpNumber >= 1 && pumpNumber <= 4) {
    pumps[pumpNumber - 1]->testSequence(onTime_ms, offTime_ms, cycles, true);
  } else {
    Serial.println("Invalid pump number!");
  }
}

void PumpController::testAllPumps(PumpController* pumps[], uint8_t numPumps) {
  Serial.println("\n========== TESTING ALL PUMPS ==========");
  Serial.println("All pumps will run simultaneously with 1s on/off cycles for 5 iterations.\n");

  uint32_t onTime = 1000;
  uint32_t offTime = 1000;
  uint16_t cycles = 5;

  for (uint16_t cycle = 1; cycle <= cycles; cycle++) {
    Serial.print("Cycle ");
    Serial.print(cycle);
    Serial.println(" - ALL PUMPS ON");
    
    for (uint8_t i = 0; i < numPumps; i++) {
      pumps[i]->on();
    }
    delay(onTime);

    Serial.print("Cycle ");
    Serial.print(cycle);
    Serial.println(" - ALL PUMPS OFF");
    
    for (uint8_t i = 0; i < numPumps; i++) {
      pumps[i]->off();
    }
    delay(offTime);
  }

  Serial.println("\n========== ALL PUMPS TEST COMPLETE ==========\n");
}

void PumpController::setAllPumps(PumpController* pumps[], uint8_t numPumps, bool state) {
  if (state) {
    for (uint8_t i = 0; i < numPumps; i++) {
      pumps[i]->on();
    }
    Serial.println("All pumps turned ON");
  } else {
    for (uint8_t i = 0; i < numPumps; i++) {
      pumps[i]->off();
    }
    Serial.println("All pumps turned OFF");
  }
}

void PumpController::printAllStatus(PumpController* pumps[], const uint8_t pins[], uint8_t numPumps) {
  Serial.println("\n--- PUMP STATUS ---");
  for (uint8_t i = 0; i < numPumps; i++) {
    pumps[i]->printStatus(i + 1);
  }
  Serial.println("-------------------\n");
}

void PumpController::pulsePump(PumpController* pumps[], uint8_t pumpNumber, uint32_t duration_ms) {
  if (pumpNumber >= 1 && pumpNumber <= 4) {
    Serial.print("Pulsing Pump ");
    Serial.print(pumpNumber);
    Serial.print(" for ");
    Serial.print(duration_ms);
    Serial.println(" ms");
    
    pumps[pumpNumber - 1]->on();
    delay(duration_ms);
    pumps[pumpNumber - 1]->off();
    
    Serial.println("Pulse complete");
  } else {
    Serial.println("Invalid pump number!");
  }
}


