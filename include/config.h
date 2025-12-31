#ifndef CONFIG_H
#define CONFIG_H

// Water Level Sensor Pins (connect to ground when water level is full)
#define ARM_PIN_1 14  // GPIO14 - First water level sensor
#define ARM_PIN_2 27  // GPIO27 - Second water level sensor

// Relay Control Pins (control pumps)
#define RELAY_PIN_1 25  // GPIO25 - First pump relay
#define RELAY_PIN_2 26  // GPIO26 - Second pump relay

// Serial Communication
#define SERIAL_BAUD_RATE 115200

// Timing
#define LOOP_DELAY_MS 200
#define DEBUG_FREQ 5000


#endif // CONFIG_H
