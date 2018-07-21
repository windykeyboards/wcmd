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
}

#define MAXCOMMANDS        10  
#define IDBUFFERSIZE       8
#define COMMANDBUFFERSIZE  64
#define STREAMBUFFERSIZE   512

class Commander 
{
    public:
        // Constructor
        Commander(Stream & stream, 
                    const String commandId,
                    const String deviceId);

        // Attach command
        void attach(Command command);

        // For feeding in Serial
        void onLoop();

        // Generic messaging
        void write(String message);
        void nak();
        void ack();
        void eot();

    private:  
        Callback onUnhandled;
        Command commands[MAXCOMMANDS];
        Stream *stream;
        String commandId;
        String deviceId;

        uint8_t bufferIndex;             
        uint8_t bufferLength;            
        uint8_t bufferLastIndex;
        char idBuffer[IDBUFFERSIZE];          
        char commandBuffer[COMMANDBUFFERSIZE]; 
	    char streamBuffer[STREAMBUFFERSIZE]; 

        int commandIndex = 0;

        void onEnquiry();
        void onUnhandled();
        void dispatch();
};

#endif