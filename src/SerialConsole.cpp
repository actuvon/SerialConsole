/* SerialConsole.cpp
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Kaden Burgart (actuvon)
 */

#include "Arduino.h"
#include "SerialConsole.h"

SerialConsole::SerialConsole(const SerialConsoleConfig& cfg) : _config(cfg){
    _numCommandsDefined = 0;
    _bufferIndex = -1;
    _lastScanMillis = 0;
    ArgCount = 0;

    Triggers = new const char*[_config.numCommands];
    Functions = new Func[_config.numCommands];
    HelpMsg = new const char*[_config.numCommands];
    Arguments = new char*[_config.maxNumArgs];
    _commandBuffer = new char[_config.maxFullLineLength + 1];

    for(int i=0; i<_config.numCommands; i++){
        Triggers[i] = nullptr;
        HelpMsg[i] = nullptr;
        Functions[i] = nullptr;
    }
    _commandBuffer[0] = '\0';
}

SerialConsole::~SerialConsole(){
    delete[] Arguments;
    delete[] _commandBuffer;
    delete[] Functions;
    delete[] HelpMsg;
    delete[] Triggers;
}

void SerialConsole::AddCommand(const char* trigger, Func function, const char* helpMsg){
    if(_numCommandsDefined < _config.numCommands){
        int commandNumber = _numCommandsDefined++;

        Triggers[commandNumber] = trigger;
        Functions[commandNumber] = function;
        HelpMsg[commandNumber] = helpMsg;
    }
    else{
        _config.IO_Stream.print("SerialConsole: ERROR: Could not add command <");
        _config.IO_Stream.print(trigger);
        _config.IO_Stream.println("> because the SerialConsole is already full of commands!");
        _config.IO_Stream.println("Please increase the number of commands with a SerialConsoleConfig when building the SerialConsole. See examples.");
    }
}

// Mop up between commands
void SerialConsole::cleanSlate(){
    _commandBuffer[0] = '\0';
    _bufferIndex = -1;
    ArgCount = 0;

    for(int i=0; i<_config.maxNumArgs; i++) Arguments[i] = _commandBuffer;
    if(_config.showPromptWhenReady) _config.IO_Stream.print(_config.inputPrompter);
}

// Listen for incoming commands
void SerialConsole::Listen(){
    unsigned long now = millis();
    bool execute = false;

    // Check if a reasonable amount of time has passed since the last scan
    if(now - _lastScanMillis > _config.scanPeriod_ms){
        if(_config.IO_Stream.available()){

            // Read characters into the buffer until a line terminator is detected, or there's nothing left to read
            while(_config.IO_Stream.available()){
                if(++_bufferIndex >= _config.maxFullLineLength){
                    _config.IO_Stream.println("\nSerialConsole: ERROR: Received a command that was longer than the max allowed line length, and had to flush the stream :(\nYou may need to change the max allowed line length config setting with a SerialConsoleConfig object. See examples.");
                    while(_config.IO_Stream.available()){
                    	char c = _config.IO_Stream.read();
                    	if(c == _config.cmdTerminator1 || c == _config.cmdTerminator2) break;
            		}
                    cleanSlate();
                    break;
                }

                _commandBuffer[_bufferIndex] = _config.IO_Stream.read();

                // Handle backspace
                if(_commandBuffer[_bufferIndex] == 0x7F || _commandBuffer[_bufferIndex] == 0x08){
                    if(_bufferIndex > 0){
                        _bufferIndex--;
                        if(_config.echoIndividualChars){
                            _config.IO_Stream.print("\b \b");
                        }
                    }
                    _bufferIndex--;
                    continue;
                }

                if(_config.echoIndividualChars) _config.IO_Stream.print(_commandBuffer[_bufferIndex]);

                // Check for terminators
                if(_commandBuffer[_bufferIndex] == _config.cmdTerminator1 || _commandBuffer[_bufferIndex] == _config.cmdTerminator2 || _commandBuffer[_bufferIndex] == '\0'){
                    _commandBuffer[_bufferIndex] = '\0';
                    if(_bufferIndex > 0) execute = true;
                    else cleanSlate();
                    break;
                }
            }

            // Maybe a whole command came in, but maybe we only got part of it and we have to wait until the next Listen() to get the rest
            if(execute){
                // Parse arguments from the buffer
                char* currentArgStart = _commandBuffer;
                for(int i=0; i<=_bufferIndex; i++){
                    if(_commandBuffer[i] == _config.delimiter || _commandBuffer[i] == '\0'){
                        if(ArgCount < _config.maxNumArgs){
                            Arguments[ArgCount++] = currentArgStart;
                            _commandBuffer[i] = '\0';
                            currentArgStart = _commandBuffer + i + 1;
                        }
                    }
                }

                // Print out the command we just read (Arduino IDE mode)
                if(_config.echoFullCommand){
                    _config.IO_Stream.print(_config.inputPrompter);
                    for(int i=0; i<ArgCount; i++){
                        _config.IO_Stream.print(Arguments[i]);
                        if(i<ArgCount-1) _config.IO_Stream.print(_config.delimiter);
                    }
                    _config.IO_Stream.println();
                }
                else _config.IO_Stream.println();

                bool cmdFound = false;
                if(strcmp(Arguments[0], "help") == 0 && ArgCount > 1){
                    for(int i=0; i<_numCommandsDefined; i++){
                        if(strcmp(Arguments[1], Triggers[i]) == 0) {
                            cmdFound = true;
                            if(HelpMsg[i] != nullptr) _config.IO_Stream.println(HelpMsg[i]);
                            else{
                                _config.IO_Stream.print("SerialConsole: No help message available for command");
                                _config.IO_Stream.print(": ");
                                _config.IO_Stream.println(Triggers[i]);
                            }
                        }
                    }
                }
                else{
                    for(int i=0; i<_numCommandsDefined; i++){
                        if(strcmp(Arguments[0], Triggers[i]) == 0){
                            cmdFound = true;

                            if(Functions[i] == nullptr) _config.IO_Stream.println("SerialConsole: A function with an existing trigger was called, but there is no function associated with the trigger.");
                            else (Functions[i])();
                        }
                    }
                }
                if(!cmdFound){
                    if(strcmp(Arguments[0], "help") != 0){
                    	_config.IO_Stream.print("SerialConsole: Command """);
                    	_config.IO_Stream.print(Arguments[0]);
                    	_config.IO_Stream.println(""" not recognized.");
                    }
                    _config.IO_Stream.println("Type help <command> for help on a specific command.");
                    _config.IO_Stream.println("Available commands:");
                    for(int i=0; i<_numCommandsDefined; i++){
                        if(Triggers[i] != nullptr){
                            _config.IO_Stream.print(" - ");_config.IO_Stream.println(Triggers[i]);
                        }
                    }
                }
                cleanSlate(); // Clean up buffer and arguments for the next command
            }
        }
        _lastScanMillis = millis();
    }
}