#ifndef CONFIG_H
#define CONFIG_H

// System Configuration
#define F_CPU 16000000UL
#define CAN_BAUD_RATE 250000
#define CAN_MSG_ID 0x14FFFFB0
#define MAX_CONCURRENT_CHANNELS 2
#define MAX_TOTAL_CURRENT 14500 // 14.5A in mA

// EEPROM Addresses
#define EEPROM_CAN_BAUD 0x00
#define EEPROM_CAN_ID 0x04
#define EEPROM_MODE_PAIR1 0x08
#define EEPROM_MODE_PAIR6 0x09

// Safety Parameters
// Current Sensor Configuration
#define CURRENT_SENSOR_ADC_CHANNEL 1  // ADC1 pin
#define ADC_REF_VOLTAGE 5.0          // 5V reference
#define ADC_RESOLUTION 1024          // 10-bit ADC
#define CURRENT_SENSOR_ZERO_POINT 512 // 2.5V for ACS712
#define CURRENT_SENSOR_SENSITIVITY 0.066 // 66mV/A for ACS712 30A
//#define CURRENT_SENSOR_ACS712_RATIO 66 // 30A sensor = 66mV/A

// Debug configuration
#define DEBUG_ENABLED 1  // Set to 0 to disable debug prints
#define DEBUG_UART_BAUD 9600

#endif // CONFIG_H