#include "node.h"

class IoT : public Node {

protected:
    int tenant_id;

    virtual void handleMessage(cMessage *msg) override;
    virtual void processTimer(cMessage *msg);
    void processMessage(cMessage *msg);

    void generateTask();

    virtual ~IoT();
    void initialize() override;
};

Define_Module(IoT);

IoT::~IoT() {
    cancelAndDelete(newTaskEvent);
}

void IoT::initialize() {
    newTaskEvent = new cMessage();
    scheduleAt(1, newTaskEvent);
}

void IoT::handleMessage(cMessage *msg)
{
    // process the self-message or incoming packet
    if (msg->isSelfMessage())
        processTimer(msg);
    else
        processMessage(msg);
}

void IoT::processTimer(cMessage *msg) {
}

void IoT::processMessage(cMessage *msg) {
}

void IoT::generateTask() {
    // generate and send out a packet
    cMessage *msg = new cMessage();
    send(msg, "out");
}

