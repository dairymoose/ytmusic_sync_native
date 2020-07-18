#include "inputthread.h"

#include <iostream>
#include <sstream>

MessageThread *InputThread::getMessageThread() const
{
    return messageThread;
}

void InputThread::setMessageThread(MessageThread *value)
{
    messageThread = value;
}

void InputThread::run()
{
    for (;;) {
        std::string msg;
        std::string input;
        uint32_t len = 0;
        std::cin.read((char *)&len, 4);
        if (len > 0) {
            msg = "";
            while (msg.length() < len) {
                msg += std::cin.get();
            }
            if (this->messageThread != nullptr) {
                this->messageThread->replyQueue.push(msg);
            }
        }
    }
}

InputThread::InputThread()
{
    
}
