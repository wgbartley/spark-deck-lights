#include "application.h"

// This #include statement was automatically added by the Spark IDE.
#include "Thermistor.h"

// This #include statement was automatically added by the Spark IDE.
#include "elapsedMillis.h"

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


// Timer
elapsedMillis timeElapsed;


// Analog pin the thermistor is connected to
int thermPin = A0;

// Voltage divider resistor value
int thermRes = 10000;

// Configure the Thermistor class
Thermistor Thermistor(thermPin, thermRes);

// Exposed variables
double tempF = 0.0;

uint8_t relayStates = 0;


void setup() {
    Spark.function("toggleRelay", toggleRelay);
    Spark.variable("tempF", &tempF, DOUBLE);
    Spark.variable("relayStates", &relayStates, INT);
    
    for(uint8_t i=0; i<4; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, !bitRead(relayStates, i));
    }
    
    Thermistor.begin();
}


void loop() {
	// Wait 1 second
	if (timeElapsed > 1000) {
	    tempF = Thermistor.getTempF(true);
    	
    	timeElapsed = 0;
	}
	
	// Do nothing
}


int toggleRelay(String command) {
    command.trim();
    command.toUpperCase();
    
    if(command.equals("RELAY1ON")) {
        bitWrite(relayStates, 3, 1);
    } else if(command.equals("RELAY1OFF")) {
        bitWrite(relayStates, 3, 0);
    } else if(command.equals("RELAY2ON")) {
        bitWrite(relayStates, 2, 1);
    } else if(command.equals("RELAY2OFF")) {
        bitWrite(relayStates, 2, 0);
    } else if(command.equals("RELAY3ON")) {
        bitWrite(relayStates, 1, 1);
    } else if(command.equals("RELAY3OFF")) {
        bitWrite(relayStates, 1, 0);
    } else if(command.equals("RELAY4ON")) {
        bitWrite(relayStates, 0, 1);
    } else if(command.equals("RELAY4OFF")) {
        bitWrite(relayStates, 0, 0);
    } else
        return -1;
    
    for(uint8_t i=0; i<4; i++)
        digitalWrite(i, !bitRead(relayStates, i));
    
    return relayStates;
}
