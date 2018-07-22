#include <stdio.h>
#include <Commander.h>

/*
 * Commander constructor
 */
Commander::Commander(Stream & stream, const String commandId, const String deviceId): stream(&stream), commandId(commandId), deviceId(deviceId)
{
    idExpectedLength = commandId.length();
    idFirstCharacter = commandId.charAt(0);
    reset();
}

void Commander::attach(Command command) {
    if (commandIndex < MAXCOMMANDS) {
        commands[commandIndex+1] = command;
        commandIndex++;
    }
}

void Commander::attach(String commandName, Callback callback) {
    attach((Command){commandName, callback});
}

void Commander::onLoop() {
    while (stream->available())
	{
		size_t bytesAvailable = min(stream->available(), STREAMBUFFERSIZE);
		stream->readBytes(streamBuffer, bytesAvailable);

		for (size_t byteNo = 0; byteNo < bytesAvailable; byteNo++)
		{
            char current = (char) streamBuffer[byteNo];

            switch (currentState) {
                case EMPTY: 
                    // Move to ID_READ if we match on the first character
                    if (current == idFirstCharacter) {
                        currentState = ID_READ;
                        idBuffer[idBufferIndex] = current;
                        idBufferIndex++;
                    }

                    break;
                case ID_READ:
                    // If we've found a match for the command ID, move to the read state
                    if (idBufferIndex == idExpectedLength && String(idBuffer) == commandId) {
                        String id((char*) idBuffer);

                        if (id == commandId) {
                            currentState = MESSAGE_READ;
                            
                            // Add the first command character here
                            commandBuffer[bufferIndex] = current;
                            bufferIndex++;
                        } else {
                            reset();
                        }
                    } else {
                        // Accumulate
                        idBuffer[idBufferIndex] = current;
                        idBufferIndex++;
                    }

                    break;
                case MESSAGE_READ:
                    if (current == '\n') {
                        // Terminate and dispatch
                        commandBuffer[bufferIndex] = '\0';
                        dispatch();
                    } else {
                        if (bufferIndex < COMMANDBUFFERSIZE) { 
                            // Accumulate
                            commandBuffer[bufferIndex] = current;
                            bufferIndex++;
                        } else {
                            // Command's longer than the buffer - reset
                            reset();
                        }
                    }

                    break;
            }
		}
	}
}

void Commander::dispatch() {
    // Short circuit if it's a who-am-i
    if (commandBuffer[0] == ENQ) {
        onEnquiry();
        reset();
        return;
    }
    
    // Strip the null character for comparisons sake
    String commandIn((char*) commandBuffer);
    commandIn = commandIn.substring(0, commandIn.length() - 1);

    for (int i = 0; i <= commandIndex; i++) {
        Command command = commands[i];

        if (command.name.equals(commandIn)) {
            command.callback();
            reset();
            return;
        }
    }

    onUnhandled();
    reset();
}

void Commander::reset() {
    idBufferIndex = 0;
    bufferIndex = 0;
    currentState = EMPTY;
}

void Commander::write(String messsage) {
    stream->print(messsage);
}

void Commander::writeln(String message) {
    stream->print(message + '\n');
}

void Commander::nak() {
    write(NAK);
}

void Commander::ack() {
    write(ACK);
}

void Commander::eot() {
    write(EOT);
}

void Commander::onEnquiry() {
    write(deviceId);
    eot();
}

void Commander::onUnhandled() {
    nak();
}