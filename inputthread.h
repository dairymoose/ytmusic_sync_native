#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include <QThread>
#include "messagethread.h"

class InputThread : public QThread
{

private:
    MessageThread *messageThread = nullptr;
    
protected:
    virtual void run();
    
public:
    InputThread();
    MessageThread *getMessageThread() const;
    void setMessageThread(MessageThread *value);
};

#endif // INPUTTHREAD_H
