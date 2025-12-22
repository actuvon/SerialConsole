/* SerialConsole: PuttyMode.ino
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Kaden Burgart (actuvon)
 *
 * Demonstrates PuttyMode for use with PuTTY, VSCode, or other terminal emulators
where you type character-by-character rather than in a line-entry box.

In PuttyMode:
- Characters are echoed back as you type them
- Backspace works to delete characters
- The prompt (>>) appears when ready for input
- Full command echo is disabled (since you already saw it being typed)

This is different from the Arduino IDE Serial Monitor which uses a text box for input.

Upload this code and open your terminal (PuTTY, VSCode monitor, etc.).
Type commands directly - you'll see characters appear as you type.

Try entering these commands:
>> hello
>> help hello
>> help led
>> led
>> help setpin
>> setpin 13 0
>> setpin 13 1

*/

#include <SerialConsole.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Common default for ESP32, etc.
#endif

const int LedPin = LED_BUILTIN;
bool PinState = false;

// Create a SerialConsole with PuttyMode configuration
SerialConsole console(PuttyMode());

void setup(){

  // Start up the Arduino Serial interface
  Serial.begin(9600);
  while(!Serial);

  // Set up the LED
  pinMode(LedPin, OUTPUT);
  delay(1);
  digitalWrite(LedPin, PinState);

  // Bind commands to the console
  console.AddCommand("hello", cmd_hello, "Greet your Arduino! (this is a description/help message for a command)");
  console.AddCommand("setpin", cmd_setpin, "Turn a digital output pin on or off\nsetpin <pinNumber> <1 or 0>");
  console.AddCommand("led", cmd_LED, "Toggle the built-in LED");
}

void loop(){
  // Process serial commands as they come in
  console.Listen();
}

// Define a function for our command to bind
void cmd_hello(){
	Serial.println("Why, hello there!");
}

// Define a function for a command that actually does something
void cmd_LED(){
	Serial.println("Toggling the LED");

	PinState = !PinState;
	digitalWrite(LedPin, PinState);
}

// Utilize arguments from the command line
void cmd_setpin(){
	// Check if you have the right number of arguments
	if(console.ArgCount != 3){
		Serial.println("ERROR: Must specify a pin and a state to set it to.");
		return;
	}

	// Arguments[0] is the command name ("setpin")
	int arg1 = String(console.Arguments[1]).toInt(); // Arguments[1] is the first argument (e.g., "13")
	int arg2 = String(console.Arguments[2]).toInt(); // Arguments[2] is the second argument (e.g., "1")

	if(arg1 > 0) pinMode(arg1, OUTPUT);
	else{
		Serial.println("ERROR: The pin can not be negative.");
		return;
	}

	if(arg2 == 0 || arg2 == 1) digitalWrite(arg1, arg2);
	else Serial.println("ERROR: Must set the pin to either 0 or 1.");
}
