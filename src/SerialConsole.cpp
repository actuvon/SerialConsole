/* SerialConsole.cpp
 * license: MIT - 2025 - CeleryMortem
 */

#include "Arduino.h"
#include "SerialConsole.h"

SerialConsole::SerialConsole(const SerialConsoleConfig& cfg) : _config(cfg)
{
	_numCommandsDefined = 0;
	_bufferIndex = -1;
	_lastScanMillis = 0;

	Triggers = (char**)malloc(_config.numCommands * sizeof(char*));
	HelpMsg = (char**)malloc(_config.numCommands * sizeof(char*));
	Functions = (Func*)malloc(_config.numCommands * sizeof(Func));

	for(int i=0; i<_config.numCommands; i++){
		Triggers[i] = nullptr;
    HelpMsg[i] = nullptr;
		Functions[i] = nullptr;
	} 

	_commandBuffer = (char*)malloc(_config.maxFullLineLength + 1);
	_commandBuffer[0] = '\0';

	Arguments = (char**)malloc(_config.maxNumArgs * sizeof(char*));
	for(int i=0; i<_config.maxNumArgs; i++) Arguments[i] = (char*)malloc(_config.maxArgLength + 1);
}

void SerialConsole::AddCommand(char* trigger, Func function, char* helpMsg){
	if(_numCommandsDefined < _config.numCommands-1){
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

	for(int i=0; i<_config.maxNumArgs; i++) Arguments[i][0] = '\0';
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
				_bufferIndex++;
				_commandBuffer[_bufferIndex] = _config.IO_Stream.read();

				if(_bufferIndex >= _config.maxFullLineLength-1){
					_config.IO_Stream.println("WARNING: Received a command that was longer than the max allowed line length. May need to change config settings.");
					_commandBuffer[_config.maxFullLineLength] = '\0';
					break;
				}
				if(_commandBuffer[_bufferIndex] == '\n' || _commandBuffer[_bufferIndex] == '\r' || _commandBuffer[_bufferIndex] == '\0'){
					_commandBuffer[_bufferIndex] = '\0';
					execute = true;
					break;
				}
			}

			// Maybe a whole command came in, but maybe we only got part of it and we have to wait until the next Listen() to get the rest
			if(execute){
				// Print out the command we just read
				_config.IO_Stream.print(_config.inputPrompter);_config.IO_Stream.println(_commandBuffer);

				// Parse arguments
				char* arg = strtok(_commandBuffer, _config.delimiter);
				for(int i = 0; i < _config.maxNumArgs; i++){
					if(arg != 0) strcpy(Arguments[i], arg);
					else break;
					arg = strtok(NULL, _config.delimiter); // get the NEXT arg
				}

				bool cmdFound = false;
				if(strcmp(Arguments[0], "help") == 0){
					if(strcmp(Arguments[1], "") == 0){
						_config.IO_Stream.println("Type help <command> for help on a specific command.");
					}
					else{
						for(int i=0; i<_config.numCommands; i++){
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
					for(int i=0; i<_config.numCommands; i++){
						if(strcmp(Arguments[0], Triggers[i]) == 0){
							cmdFound = true;

							if(Functions[i] == 0) _config.IO_Stream.println("\nConsole: A function with an existing trigger was called, but there is no function associated with the trigger.");
							else (Functions[i])();
						}
					}
				}
				if(!cmdFound){
					_config.IO_Stream.println("\nConsole: Command not recognized.");
					_config.IO_Stream.println("Available commands:");
					for(int i=0; i<_config.numCommands; i++){
						if(strcmp(Triggers[i], "") != 0){
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
