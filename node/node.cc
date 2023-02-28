#include "node.h"

Node::Node() {
    event = tictocMsg = nullptr;
}

Node::~Node() {
    cancelAndDelete(event);
}

void Node::initialize() {
    event = new cMessage("event");
    tictocMsg = nullptr;

    counter = par("limit");
    //WATCH(counter);
    if (par("sendMsgOnInit").boolValue() == true) {
        tictocMsg = new cMessage("tictocMsg");
        scheduleAt(2, event);
    }
}

void Node::handleMessage(cMessage *msg) {
    if (msg == event) {
        counter --;
        if (counter == 0) {
            EV << getName() << "\'s counter reached zero, deleting message\n";
            delete msg;
            tictocMsg = nullptr;
        }
        else {
            send(tictocMsg, "out");
            EV << "Send message out again\n";
        }
    }
    else {
        simtime_t delay = par("delayTime");
        EV << "Received message \'" << msg->getName() << "\'\n";
        tictocMsg = msg;
        scheduleAt(simTime()+delay, event);
    }
}
