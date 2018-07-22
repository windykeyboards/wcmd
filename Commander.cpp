#include <stdio.h>
#include <Commander.h>

/*
 * Commander constructor
 */
Commander::Commander(Stream & stream, const String commandId, const String deviceId): stream(&stream), commandId(commandId), deviceId(deviceId)
{
    onUnhandled = onUnhandled;
    idExpectedLength = commandId.length();
    idFirstCharacter = commandId.charAt(0);
    reset();
}

void Commander::onUnhandled(Callback Callback) {
    onUnhandled = callback;
}

void Commander::attach(Command command) {
    if (commandIndex < MAXCOMMANDS) {
        commands[commandIndex+1] = command;
        commandIndex++;
    }
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
                        commandBuffer[bufferIndex] = '\0'
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
    String commandIn((char*) commandBuffer);
    for (int i = 0; i <= commandIndex; i++) {
        Command command = commands[i];

        if (command.name == commandId) {
            command.callback();
            reset();
            return;
        }
    }

    onUnhandled();
    reset();
}

void Commaner::reset() {
    idBufferIndex = 0;
    bufferIndex = 0;
    currentState = EMPTY;
}

void Commander::write(String messsage) {
    stream->write(messsage);
}

void Commander::writeln(String message) {
    stream->write(message + '\n');
}

void Commander::nak() {
    write(String(15, HEX));
}

void Commander::ack() {
    write(String(6, HEX));
}

void Commander::eot() {
    write(String(4, HEX));
}

void Commander::onEnquiry() {
    write(deviceId);
    eot();
}

void Commander::onUnhandled() {
    nak();
}