/* SerialConsole: CustomConfig.ino
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Kaden Burgart (actuvon)
 *
 * Shows how to customize your SerialConsole object.

NOTE: Since we change all the *normal* settings to something else, running this example is a bit weird.
In the SerialMonitor you will have to change the line ending dropdown to "No Line Ending" or else the console will think
that the line endings you send are a part of a command, and it will be very confused.

With these settings, the SerialConsole won't care when you hit enter, it only cares when it sees a "|" or ";".
So you can type:
l <enter>
e <enter>
d <enter>
; <enter>
and the console will run the "led" command.

*/

#include <SerialConsole.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Common default for ESP32, etc.
#endif

const int LedPin = LED_BUILTIN;
bool PinState = false;

const char* myInputPrompter = "\n☺:";
const char* myDelimiter = "-";

// Create a SerialConsole with custom configuration options...
// NOTE: Since we changed the cmdTerminators, you will have to change the line ending option in the Serial Monitor
//       to "No Line Ending", and then you can try using the new, custom command terminators by entering things like:
//       "hello|led;" which will run the "hello" command, wait 1 second, then run the "led" command
//      This example will work particularly well in PuTTY.
SerialConsole console(
    ([]() {
        SerialConsoleConfig cfg;

        // Memory and capacity settings
        cfg.numCommands = 15;        // Set the maximum allowable number of commands that can be added to this SerialConsole object
        cfg.maxFullLineLength = 40;  // Shorten the maximum allowable length of a full command line with arguments, to save memory
        cfg.maxNumArgs = 10;         // Increase the number of arguments allowed in a command at the cost of RAM

        // Parsing settings
        cfg.cmdTerminator1 = '|';    // First character to mark the end of a command line (default: '\n')
        cfg.cmdTerminator2 = ';';    // Second character to mark the end of a command line (default: '\r')
        cfg.delimiter = '-';         // Character to separate arguments (default: ' ')

        // Timing
        cfg.scanPeriod_ms = 1000;    // Wait at least 1000 milliseconds between Listen() calls or running commands

        // Display settings
        cfg.inputPrompter = "\n☺: "; // Customize the prompt that appears when ready for input

        // Terminal mode settings (see PuttyMode example for easier setup)
        cfg.echoFullCommand = false;      // Echo the full command line after Enter is pressed (default: true, good for Arduino IDE)
        cfg.echoIndividualChars = true; // Echo each character as it's typed (default: false, set true for PuTTY/VSCode)
        cfg.showPromptWhenReady = true; // Show prompt when ready for input (default: false, set true for PuTTY/VSCode)

        // Note: For PuTTY/terminal emulator mode, use PuttyMode() instead of manually setting these three options

        return cfg;
    })()
);

void setup(){
    
  // Start up the Arduino Serial interface
  Serial.begin(9600);
  while(!Serial);

  // Set up the LED
  pinMode(LedPin, OUTPUT);
  delay(1);
  digitalWrite(LedPin, PinState);

  // Bind a new command to the console
  console.AddCommand("hello", cmd_hello, "Greet your Arduino! (this is a description/help message for a command)");
  console.AddCommand("add", cmd_add); // Commands can be created without a help message, but this makes them very sad
  console.AddCommand("led", cmd_LED, "Toggle the builtin LED.");
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
void cmd_add(){
    // Arguments are designed for use as lightweight c strings, but can easily work with Arduino string functions
    float arg1 = String(console.Arguments[1]).toFloat(); 
    float arg2 = String(console.Arguments[2]).toFloat();

    Serial.print(arg1);
    Serial.print(" + ");
    Serial.print(arg2);
    Serial.print(" = ");
    Serial.println(arg1 + arg2);
}
