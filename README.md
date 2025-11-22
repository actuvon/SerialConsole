# SerialConsole
A small and simple command line interface designed for Arduino projects.

You can bind your own commands to the console like this:

console.AddCommand("led", cmd_LED, "Toggle the builtin LED.");

Then you open up the Arduino Serial Monitor, type in LED, and watch it turn on!

Find the full .ino with basic examples here: 
https://github.com/CeleryMortem/SerialConsole/blob/main/examples/BasicExample/BasicExample.ino

In the serial monitor, the console will tell you what the available commands are when you type anything in. If you type an unrecognized command, it will respond by listing out all the commands that are available. There is a builtin "help" function to display the optional helper messages that you put in when you add commands.

SerialConsole is very lightweight on ROM and RAM, can be configured to work with any Arduino Stream object, and should run on any Arduino board that supports millis(). The .cpp file is less than 150 lines long.

## Installation
1) Click the green "Code" button at the top right of the page and download everything as a .zip.
2) Extract the .zip to get a SerialConsole folder with all the contents from GitHub.
3) Put that folder in your Arduino libraries folder (Document\Arduino\Libraries on windows).
4) #include <SerialConsole.h> in your sketch. Check the BasicExample.ino for a quick start guide.
