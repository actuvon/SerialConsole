/* SerialConsole: BasicExample.ino

Set up a basic serial console using Arduino Serial.

Create a few commands to say hello and blink the built-in LED.

*/

#include <SerialConsole.h>

const int LedPin = LED_BUILTIN;
bool PinState = false;

const char* myInputPrompter = "\n☺:";
const char* myDelimiter = "-";

// Create a SerialConsole with custom configuration options...
// NOTE: Since we screwed with the cmdTerminators, you will have to change the line ending option in the Serial Monitor
//       to "No Line Ending", and then you can try using the new, custom command terminators by entering things like:
//       "hello|led;" which will run the "hello" command, wait 1 second, then run the "led" command
//    
SerialConsole console = ([]() {
	SerialConsoleConfig cfg;

	cfg.numCommands = 15;				 // Set the maximum allowable number of commands that can be added to this SerialConsole object
	cfg.maxFullLineLength = 40;  // Shorten the maximum allowable length of a full command line with arguments, to save memory
	cfg.maxArgLength = 5;				 // Decrease the maximum allowable length of each argument to save memory
	cfg.maxNumArgs = 10;         // Increase the number of arguments allowed in a command at the cost of RAM
	cfg.cmdTerminator2 = ';';		 // This is one of two chars you can use to mark the end of a command line
	cfg.cmdTerminator1 = '|';    // In the default configuration this is \n, but you COULD change it
	cfg.scanPeriod_ms = 1000;		 // Wait at least 1000 milliseconds between Listen()ing or running commands
	cfg.inputPrompter = "\n☺: "; // Customize the look of your SerialConsole
	cfg.delimiter = "-";         // A delemiter other than a space can be set

	return cfg;
})();

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