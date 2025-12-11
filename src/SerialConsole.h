/* SerialConsole.h

license: MIT - 2025 - CeleryMortem

Description: Small and simple command line interface for Arduino boards.

Authored: 2020

Modifications: 2025-11
	-Refactoring
	-Adding a SerialConsoleConfig struct
	-Supporting SerialConsole construction with default values
	-Created the AddCommand function for simpler command binding
	-Adding allowance to use different streams
	-Added configurability for delimiters, end-of-line characters, and the user prompt
*/

#ifndef SerialConsole_h
#define SerialConsole_h

#include <Arduino.h>

typedef void (*Func) (void);

struct SerialConsoleConfig {
	uint8_t numCommands = 10;		    // number of commands that can be declared
	uint8_t maxFullLineLength = 50;     // maximum character count of any full command line (with arguments)
	uint8_t maxArgLength = 10;		    // maximum character count of any single argument
	uint8_t maxNumArgs = 5;			    // maximum number of arguments allowed in a single command line
	char cmdTerminator1 = '\n';         // this character marks the end/submission of a command line
	char cmdTerminator2 = '\r';         // an alternalte char to mark the end/submission of a command line
	unsigned long scanPeriod_ms = 250;  // how long will the SerialConsole wait between two consecutive scans in ms
	Stream& IO_Stream;		            // you can change the type of stream the SerialConsole works with
	const char* inputPrompter = "\n>> ";// a little thingy to show the user they can type - can be modified
	const char* delimiter = " ";	    // arguments are seperated by a space by default

	SerialConsoleConfig() : IO_Stream(Serial) {} // The default IO stream is Serial...
	SerialConsoleConfig(Stream& stm) : IO_Stream(stm) {} // but it can be changed
};

class SerialConsole {
	public:
		char** Arguments; // A pointer to the array of arguments

		char** Triggers; // A pointer to the array of command string triggers
		Func* Functions; // A pointer to the array of command functions
    	char** HelpMsg;  // A pointer to the array of help messages

		SerialConsole(const SerialConsoleConfig& cfg = SerialConsoleConfig());

		void AddCommand(char* trigger, Func function, char* helpMsg = nullptr);

		void Listen(); // Check the serial port for traffic. Run commands if applicable.

	private:
		SerialConsoleConfig _config;
		uint8_t _numCommandsDefined; // How many commands have been added to this SerialConsole?

		unsigned long _lastScanMillis; // The millis() timestamp of the last scan
		char* _commandBuffer; // Temporarilly holds incomming commands
		int _bufferIndex;

		void cleanSlate();
};

#endif