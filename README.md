# SerialConsole
A small and simple command line interface designed for Arduino projects.

You can bind your own commands to the console like this:

```cpp
console.AddCommand("LED", cmd_LED, "Toggle the builtin LED.");
```

Then you open up the Arduino Serial Monitor, type in LED, and watch it turn on!

In the serial monitor, the console will tell you what the available commands are when you type anything in. If you type an unrecognized command, it will respond by listing out all the commands that are available. There is a builtin "help" function to display the optional helper messages that you put in when you add commands.

SerialConsole is very lightweight on ROM and RAM, can be configured to work with any Arduino Stream object, and should run on any Arduino board that supports millis(). The .cpp file is less than 160 lines long.

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
There are already tools like this out there. Why did I make a new one? Well, none of them really suited me. I wanted a library that was very simple, but still fully met this list of features...
* The ability to run functions from the Serial Monitor.
* The ability to pass parameters to my functions from the Serial Monitor.
* Very easy useage with 1 line to make the console, 1 line to add commands, and 1 line in the main loop.
* An adjustable, predictable, clean memory footprint that doesn't move around - good for chips with baby RAMs, but expandable for bigger chips too.
* A reasonably straightforward configuration tool to adjust all your settings.
* Good clean readability in the .h and .cpp files, so you can walk in and own the place if you want to.