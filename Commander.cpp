#include <stdio.h>
#include <Commander.h>

/*
 * Commander constructor
 */
Commander::Commander(Stream & stream, const String commandId, const String deviceId): stream(&stream), commandId(commandId), deviceId(deviceId)
{
    onUnhandled = onUnhandled;
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
            // Read into command buffer


            // Reset on newline

            // If in command and at hit newline, dispatch
		}
	}
}

void Commander::dispatch() {

}

void Commander::write(String messsage) {
    stream->write(messsage);
}

void Commander::nak() {
    write(String(0x15))
}

void Commander::ack() {
    write(String(0x06))
}

void Commander::eot() {
    write(String(0x04))
}

void Commander::onEnquiry() {
    write(deviceId);
    eot();
}

void Commander::onUnhandled() {
    nak();
}