#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>

/**
 * @class PumpController
 * @brief Controls a single parasitic dosing pump via a MOSFET/Relay
 * 
 * This class manages on/off control and timing for a pump connected
 * to a digital pin through a MOSFET or relay.
 */
class PumpController {
public:
  /**
   * @brief Constructor
   * @param pin Digital pin connected to MOSFET/Relay gate
   * @param inverted If true, pump activates on LOW signal (default: false for HIGH activation)
   */
  PumpController(uint8_t pin, bool inverted = false);

  /**
   * @brief Initialize the pump control pin
   */
  void begin();

  /**
   * @brief Turn pump on
   */
  void on();

  /**
   * @brief Turn pump off
   */
  void off();

  /**
   * @brief Get current pump state
   * @return true if pump is on, false if off
   */
  bool isOn() const { return _state; }

  /**
   * @brief Set pump state
   * @param state true to turn on, false to turn off
   */
  void setState(bool state);

  /**
   * @brief Run a test sequence with specified timing
   * @param onTime_ms Time to keep pump on (milliseconds)
   * @param offTime_ms Time to keep pump off (milliseconds)
   * @param cycles Number of on/off cycles to perform
   * @param printResults If true, print flow metrics to Serial
   */
  void testSequence(uint32_t onTime_ms, uint32_t offTime_ms, uint16_t cycles = 1, bool printResults = true);

  /**
   * @brief Get the duration the pump has been in current state
   * @return Time in milliseconds since last state change
   */
  uint32_t getStateTime_ms() const;

  /**
   * @brief Get total on-time accumulated
   * @return Total milliseconds pump has been on
   */
  uint32_t getTotalOnTime_ms() const { return _totalOnTime; }

  /**
   * @brief Get the time of the last action (state change)
   * @return Timestamp in milliseconds of last on/off action
   */
  uint32_t getLastActionTime() const { return _lastActionTime; }

  /**
   * @brief Reset statistics
   */
  void resetStats();

  /**
   * @brief Print the status of this pump
   * @param pumpNumber Pump identifier (1-4)
   */
  void printStatus(uint8_t pumpNumber) const;

  /**
   * @brief Static helper to test a single pump from an array
   * @param pumps Array of PumpController pointers
   * @param pumpNumber Pump to test (1-4)
   * @param onTime_ms Time pump stays on (milliseconds)
   * @param offTime_ms Time pump stays off (milliseconds)
   * @param cycles Number of on/off cycles
   */
  static void testSinglePump(PumpController* pumps[], uint8_t pumpNumber, uint32_t onTime_ms, uint32_t offTime_ms, uint16_t cycles);

  /**
   * @brief Static helper to test all pumps simultaneously
   * @param pumps Array of PumpController pointers
   * @param numPumps Number of pumps in array
   */
  static void testAllPumps(PumpController* pumps[], uint8_t numPumps);

  /**
   * @brief Static helper to control all pumps at once
   * @param pumps Array of PumpController pointers
   * @param numPumps Number of pumps in array
   * @param state true to turn on, false to turn off
   */
  static void setAllPumps(PumpController* pumps[], uint8_t numPumps, bool state);

  /**
   * @brief Static helper to print status of all pumps
   * @param pumps Array of PumpController pointers
   * @param numPumps Number of pumps in array
   * @param pins Array of pin numbers for display
   */
  static void printAllStatus(PumpController* pumps[], const uint8_t pins[], uint8_t numPumps);

  /**
   * @brief Turn a single pump on for a specified duration
   * @param pumps Array of PumpController pointers
   * @param pumpNumber Pump to pulse (1-4)
   * @param duration_ms Time in milliseconds to keep pump on
   */
  static void pulsePump(PumpController* pumps[], uint8_t pumpNumber, uint32_t duration_ms);

private:
  uint8_t _pin;
  bool _inverted;
  bool _state;
  uint32_t _stateChangeTime;
  uint32_t _totalOnTime;
  uint32_t _lastActionTime;  // Timestamp of last state change for hysteresis
};

#endif // PUMP_CONTROLLER_H
