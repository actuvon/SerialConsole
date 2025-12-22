/* SerialConsole.h
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Kaden Burgart (actuvon)
 *
 * Small and simple command line interface for Arduino boards.

Initial Draft: 2020

Usage: Check the README and examples on https://github.com/actuvon/SerialConsole

TODO: Add a CloneCommendsTo(otherConsole) method for cases on larger boards where we want the same CLI
      commends on serial and telnet, for example.
*/

#ifndef SerialConsole_h
#define SerialConsole_h

#include <Arduino.h>

typedef void (*Func) (void);

struct SerialConsoleConfig {
    uint8_t numCommands = 6;            // number of commands that can be declared
    uint8_t maxFullLineLength = 40;     // maximum character count of any full command line (with arguments)
    uint8_t maxNumArgs = 5;             // maximum number of arguments allowed in a single command line
    char cmdTerminator1 = '\n';         // this character marks the end/submission of a command line
    char cmdTerminator2 = '\r';         // an alternate char to mark the end/submission of a command line
    unsigned long scanPeriod_ms = 250;  // how long will the SerialConsole wait between two consecutive scans in ms
    Stream& IO_Stream;                  // you can change the type of stream the SerialConsole works with
    const char* inputPrompter = "\n>> ";// a little thingy to show the user they can type - can be modified
    bool echoFullCommand = true;        // echo back the full command line after enter (Arduino IDE mode)
    bool echoIndividualChars = false;   // echo back each character as it's received (terminal mode)
    bool showPromptWhenReady = false;   // display input prompter when ready for input (terminal mode)
    char delimiter = ' ';               // arguments are seperated by a space by default

    SerialConsoleConfig() : IO_Stream(Serial) {} // The default IO stream is Serial...
    SerialConsoleConfig(Stream& stm) : IO_Stream(stm) {} // but it can be changed
};

// Helper function to create a SerialConsoleConfig optimized for PuTTY and other terminal emulators
inline SerialConsoleConfig PuttyMode(Stream& stream = Serial){
    SerialConsoleConfig cfg(stream);
    cfg.echoFullCommand = false;       // Don't echo the full command (already echoed char-by-char)
    cfg.echoIndividualChars = true;    // Echo each character as it's typed
    cfg.showPromptWhenReady = true;    // Show >> prompt when ready for input
    return cfg;
}

class SerialConsole {
    public:
        char** Arguments; // Array of arguments
        uint8_t ArgCount; // The number of arguments detected in the last command

        const char** Triggers; // Array of command string triggers
        Func* Functions; // Array of command functions
        const char** HelpMsg; // Array of help messages

        SerialConsole(const SerialConsoleConfig& cfg = SerialConsoleConfig());
        ~SerialConsole();

        void AddCommand(const char* trigger, Func function, const char* helpMsg = nullptr);
        void Listen(); // Check the serial port for traffic. Run commands if applicable.

        SerialConsole(const SerialConsole&) = delete;
        SerialConsole& operator=(const SerialConsole&) = delete;
        SerialConsole(SerialConsole&&) = delete;
        SerialConsole& operator=(SerialConsole&&) = delete;

    private:
        SerialConsoleConfig _config;
        uint8_t _numCommandsDefined; // How many commands have been added to this SerialConsole?
        unsigned long _lastScanMillis; // The millis() timestamp of the last scan
        char* _commandBuffer; // Temporarily holds incoming commands
        int _bufferIndex;

        void cleanSlate();
};

#endif