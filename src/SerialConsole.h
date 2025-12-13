/* SerialConsole.h

license: MIT - 2025 - actuvon

Description: Small and simple command line interface for Arduino boards.

Initial Draft: 2020

Useage: Check the README and examples on https://github.com/actuvon/SerialConsole

TODO: Change to RAII types so that copying and moving are allowed, and I don't have to bag my own shit.
      Doing everything you want will probably require making the congig object a template so that
      size allocations can be compile-time constants. The config stuff will get messier.
*/

#ifndef SerialConsole_h
#define SerialConsole_h

#include <Arduino.h>

typedef void (*Func) (void);

struct SerialConsoleConfig {
    uint8_t numCommands = 10;           // number of commands that can be declared
    uint8_t maxFullLineLength = 50;     // maximum character count of any full command line (with arguments)
    uint8_t maxNumArgs = 7;             // maximum number of arguments allowed in a single command line
    char cmdTerminator1 = '\n';         // this character marks the end/submission of a command line
    char cmdTerminator2 = '\r';         // an alternalte char to mark the end/submission of a command line
    unsigned long scanPeriod_ms = 250;  // how long will the SerialConsole wait between two consecutive scans in ms
    Stream& IO_Stream;                  // you can change the type of stream the SerialConsole works with
    const char* inputPrompter = "\n>> ";// a little thingy to show the user they can type - can be modified
    char delimiter = ' ';               // arguments are seperated by a space by default

    SerialConsoleConfig() : IO_Stream(Serial) {} // The default IO stream is Serial...
    SerialConsoleConfig(Stream& stm) : IO_Stream(stm) {} // but it can be changed
};

class SerialConsole {
    public:
        char** Arguments; // A pointer to the array of arguments
        uint8_t ArgCount; // The number of arguments detected in the last command

        const char** Triggers; // A pointer to the array of command string triggers
        Func* Functions;       // A pointer to the array of command functions
        const char** HelpMsg;  // A pointer to the array of help messages

        SerialConsole(const SerialConsoleConfig& cfg = SerialConsoleConfig());
        ~SerialConsole();

        void AddCommand(const char* trigger, Func function, const char* helpMsg = nullptr);

        void Listen(); // Check the serial port for traffic. Run commands if applicable.

        // You aren't supposed to copy or move this class - the dynamic memory doesn't handle that.
        SerialConsole(const SerialConsole&) = delete;
        SerialConsole& operator=(const SerialConsole&) = delete;
        SerialConsole(SerialConsole&&) = delete;
        SerialConsole& operator=(SerialConsole&&) = delete;

    private:
        SerialConsoleConfig _config;
        uint8_t _numCommandsDefined; // How many commands have been added to this SerialConsole?

        unsigned long _lastScanMillis; // The millis() timestamp of the last scan
        char* _commandBuffer; // Temporarilly holds incomming commands
        int _bufferIndex;
        char* _currentArgStart;

        void cleanSlate();
};

#endif