/* SerialConsole.cpp
 * license: MIT - 2025 - actuvon
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

    _currentArgStart = _commandBuffer;
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
        _config.IO_Stream.print("ERROR: Could not add command <");
        _config.IO_Stream.print(trigger);
        _config.IO_Stream.println("> because the SerialConsole is already full of commands!");
        _config.IO_Stream.println("Please increase the number of commands in the SerialConsoleConfig before building the SerialConsole.");
    }
}

// Mop up between commands
void SerialConsole::cleanSlate(){
    _commandBuffer[0] = '\0';
    _bufferIndex = -1;
    _currentArgStart = _commandBuffer;
    ArgCount = 0;

    for(int i=0; i<_config.maxNumArgs; i++) Arguments[i] = _commandBuffer;
}

// Listen for incomming commands
void SerialConsole::Listen(){
    unsigned long now = millis();
    bool execute = false;

    // Check if a reasonable amount of time has passed since the last scan
    if(now - _lastScanMillis > _config.scanPeriod_ms){
        if(_config.IO_Stream.available() > 0){

            // Listen to the stream until a full command is received
            while(_config.IO_Stream.available()){
                if(++_bufferIndex >= _config.maxFullLineLength){
                    _config.IO_Stream.println("WARNING: Received a command that was longer than the max allowed line length. May need to change config settings.");
                    cleanSlate();
                    break;
                }

                _commandBuffer[_bufferIndex] = _config.IO_Stream.read();

                if(_commandBuffer[_bufferIndex] == _config.delimiter && ArgCount < _config.maxNumArgs){
                    Arguments[ArgCount++] = _currentArgStart;
                    _commandBuffer[_bufferIndex] = '\0';
                    _currentArgStart = _commandBuffer+_bufferIndex+1;
                }
                else if(_commandBuffer[_bufferIndex] == _config.cmdTerminator1 || _commandBuffer[_bufferIndex] == _config.cmdTerminator2 || _commandBuffer[_bufferIndex] == '\0'){
                    if(ArgCount < _config.maxNumArgs) Arguments[ArgCount++] = _currentArgStart;
                    _commandBuffer[_bufferIndex] = '\0';

                    if(_bufferIndex > 0) execute = true;
                    else cleanSlate();  // Don't execute if we're just getting end-of-line characters with no actual content
                    break;
                }
            }

            // Maybe a whole command came in, but maybe we only got part of it and we have to wait until the next Listen() to get the rest
            if(execute){
                // Print out the command we just read
                _config.IO_Stream.print(_config.inputPrompter);
                for(int i=0; i<ArgCount; i++) { 
                    _config.IO_Stream.print(Arguments[i]);
                    if(i<ArgCount-1) _config.IO_Stream.print(_config.delimiter);
                }
                _config.IO_Stream.println();

                bool cmdFound = false;
                if(strcmp(Arguments[0], "help") == 0){
                    if(ArgCount <= 1){
                        _config.IO_Stream.println("Type help <command> for help on a specific command.");
                    }
                    else{
                        for(int i=0; i<_numCommandsDefined; i++){
                            if(strcmp(Arguments[1], Triggers[i]) == 0) {
                                cmdFound = true;
                                if(HelpMsg[i] != nullptr) _config.IO_Stream.println(HelpMsg[i]);
                                else{
                                    _config.IO_Stream.print("Console: No help message available for command ");
                                    _config.IO_Stream.print(i);
                                    _config.IO_Stream.print(": ");
                                    _config.IO_Stream.println(Triggers[i]);
                                }
                            }
                        }
                    }
                }
                else{
                    for(int i=0; i<_numCommandsDefined; i++){
                        if(strcmp(Arguments[0], Triggers[i]) == 0){
                            cmdFound = true;

                            if(Functions[i] == nullptr) _config.IO_Stream.println("\nConsole: A function with an existing trigger was called, but there is no function associated with the trigger.");
                            else (Functions[i])();
                        }
                    }
                }
                if(!cmdFound){
                    _config.IO_Stream.print("\nConsole: Command <");_config.IO_Stream.print(Arguments[0]);_config.IO_Stream.println("> not recognized.");
                    _config.IO_Stream.println("Available commands:");
                    for(int i=0; i<_numCommandsDefined; i++){
                        if(Triggers[i] != nullptr){
                            _config.IO_Stream.print(" - ");_config.IO_Stream.println(Triggers[i]);
                        }
                    }
                    _config.IO_Stream.println("To get help info for a command, type ""help <command>""");
                }

                cleanSlate(); // Clean up buffer and arguments for the next command
            }
        }

        _lastScanMillis = millis();
    }
}