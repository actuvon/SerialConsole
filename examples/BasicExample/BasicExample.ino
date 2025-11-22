/* SerialConsole: BasicExample.ino

Set up a basic serial console using Arduino Serial.

Create a few commands to say hello and blink the built-in LED.

*/

#include <SerialConsole.h>

const int LedPin = LED_BUILTIN;
bool PinState = false;

// Create a SerialConsole
SerialConsole console;

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