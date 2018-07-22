/**
    This file, along with the accompanying implementation file contain code to handle command parsing
    and calling of callback functions
*/

#ifndef Commander_H
#define Commander_H

#include <Arduino.h>

extern "C"
{
	// callback functions always follow the signature: void cmd(void);
	typedef void(*Callback) (void);
}

struct Command {
    String name;
    Callback callback;
};

enum MessageState {
    EMPTY,
    ID_READ,
    MESSAGE_READ
};

#define MAXCOMMANDS        10  
#define IDBUFFERSIZE       8
#define COMMANDBUFFERSIZE  64
#define STREAMBUFFERSIZE   512

#define ACK (char) 0x06
#define NAK (char) 0x15
#define EOT (char) 0x04
#define ENQ (char) 0x05

class Commander 
{
    public:
        // Constructor
        Commander(Stream & stream, 
                    const String commandId,
                    const String deviceId);

        // Attach command
        void attach(Command command);
        void attach(String commandName, Callback callback);

        // For feeding in Serial
        void onLoop();

        // Generic messaging
        void write(String message);
        void writeln(String message);
        void nak();
        void ack();
        void eot();

    private:  
        Command commands[MAXCOMMANDS];
        Stream *stream;
        String commandId;
        String deviceId;

        uint8_t commandIndex;
        MessageState currentState;

        char idFirstCharacter;
        uint8_t idBufferIndex;
        uint8_t idExpectedLength;
        char idBuffer[IDBUFFERSIZE]; 
        
        uint8_t bufferIndex;            
        char commandBuffer[COMMANDBUFFERSIZE + 1]; // +1 for null terminating character

	    char streamBuffer[STREAMBUFFERSIZE]; 

        void onEnquiry();
        void onUnhandled();
        void dispatch();
        void reset();
};

#endif