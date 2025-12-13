# SerialConsole
A small and simple command line interface designed for Arduino projects. It provides argument parsing and is easily customized to fit any hardware or RAM constraint.

You can bind your own commands to the console like this:

```cpp
console.AddCommand("LED", cmd_LED, "Toggle the builtin LED.");
```

Then you open up the Arduino Serial Monitor, type in LED, and watch it turn on!

In the serial monitor, the console will tell you what the available commands are when you type anything in. If you type an unrecognized command, it will respond by listing out all the commands that are available. There is a builtin "help" function to display the optional helper messages that you put in when you add commands.

SerialConsole is very lightweight on ROM and RAM, can be configured to work with any Arduino Stream object, and should run on any Arduino board that supports millis(). The whole library is barely 200 lines long.

## Installation
1) Click the green "Code" button at the top right of the page and download everything as a .zip.
2) Extract the .zip to get a SerialConsole folder with all the contents from GitHub.
3) Put that folder in your Arduino libraries folder (Document\Arduino\Libraries on windows).
4) #include <SerialConsole.h> in your sketch. Check the BasicExample.ino for a quick start guide.

## Useage
Here is a simplified snippet from the BasicExample.ino to show how it works...
```cpp
#include <SerialConsole.h>

SerialConsole console; // Create a SerialConsole object

void setup(){
	Serial.begin(9600);

	/* ...some other setup stuff... */

	// Add commands to the SerialConsole like this...
	console.AddCommand("led_on", cmd_led_on, "Turn on the builtin LED.");
	// This lets the user type in "led_on" in the Serial Monitor, and then the builtin LED will turn on!
	// The description at the end shows up if you type "help led_on".
}

void loop(){
	console.Listen(); // This goes in your main loop to process your commands
}

// Your commands call functions, which are set up like this...
void cmd_led_on(){
	digitalWrite(LED_BUILTIN, HIGH);
}
```

There are more features shown in the examples, for working with arguments, and for customizing the console's behavior, appearance, and memory footprint.

## Features
There are already tools like this out there. Why did I make a new one? Well, there are some features and values that I feel are core to a good terminal, that seemed to be missing in existing libraries.
* **Precise RAM Control (Fixed Memory Footprint):** Uses the SerialConsoleConfig structure to allow you to precisely define the maximum number of commands, maximum command length, and maximum arguments. This lets you tune the memory footprint to your exact RAM budget on small microcontrollers.
* **Built-in Help System:** Includes automatic command listing for unrecognized inputs and a structured help \<command> function to display optional command-specific helper messages. Error handling is clean and error messsages make sense.
* **Integrated Polling Timer:** Uses millis() internally to allow you to set a configurable poll rate (scanPeriod_ms) without adding any timing complexity to your main loop().
* **Bite Sized:** Good clean readability in the .h and .cpp files, so I can walk in and own the place if I want to change it.
* **Configurable:** The SerialConsoleConfig structure makes it easy to make the SerialConsole act however you want it to. The IO_Stream can be changed from the Arduino Serial class to another one. Delimiters or line terminators can be adjusted. The ">> " prompter can also be changed.