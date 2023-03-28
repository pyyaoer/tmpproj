//#define FSM_DEBUG
#include "node.h"

IoT::IoT() {
    startStopBurst = sendMessage = nullptr;
    duration = 1;
}

IoT::~IoT() {
    cancelAndDelete(startStopBurst);
    cancelAndDelete(sendMessage);
}

void IoT::initialize() {
    Node::initialize();
    fsm.setName("fsm");

    tenant_id = par("tenant_id").intValue();

    sleepTime = &par("sleepTime");
    burstTime = &par("burstTime");
    sendIATime = &par("sendIaTime");

    startStopBurst = new cMessage("startStopBurst");
    sendMessage = new cMessage("sendMessage");

    scheduleAt(simTime() + SimTime(tenant_id * 1000, SIMTIME_S), startStopBurst);
}

void IoT::handleMessage(cMessage *msg) {
    // process the self-message or incoming packet
    if (msg->isSelfMessage())
        processTimer(msg);
    else
        processMessage(check_and_cast<BaseMessage *>(msg));
}

void IoT::processTimer(cMessage *msg) {
    simtime_t d;
    FSM_Switch(fsm) {
        case FSM_Exit(INIT):
            // transition to SLEEP state
            FSM_Goto(fsm, ACTIVE);
            break;

        case FSM_Enter(SLEEP):
            // schedule end of sleep period (start of next burst)
            d = sleepTime->doubleValue();
            scheduleAt(simTime() + d, startStopBurst);
            EV << "sleeping for " << d << "s\n";
            break;

        case FSM_Exit(SLEEP):
            // schedule end of this burst
            d = burstTime->doubleValue();
            scheduleAt(simTime() + d, startStopBurst);

            // transition to ACTIVE state:
            if (msg != startStopBurst)
                throw cRuntimeError("invalid event in state ACTIVE");
            FSM_Goto(fsm, ACTIVE);
            break;

        case FSM_Enter(ACTIVE):
            // schedule next sending
            d = sendIATime->doubleValue();
            scheduleAt(simTime() + d, sendMessage);
            break;

        case FSM_Exit(ACTIVE):
            // transition to either SEND or SLEEP
            if (msg == sendMessage) {
                FSM_Goto(fsm, SEND);
            }
            else if (msg == startStopBurst) {
                cancelEvent(sendMessage);
                FSM_Goto(fsm, SLEEP);
            }
            else
                throw cRuntimeError("invalid event in state ACTIVE");
            break;

        case FSM_Exit(SEND): {
            // send out a packet
            generateTask();

            // return to ACTIVE
            FSM_Goto(fsm, ACTIVE);
            break;
        }
    };
}

void IoT::processMessage(BaseMessage *msg) {
    CompMessage *cmsg;
    m2 *parent;
    simtime_t latency;
    switch (msg->getType()) {
        case COMP_MESSAGE:
            cmsg = check_and_cast<CompMessage *>(msg);
            latency = cmsg->getArrivalTime()-cmsg->getCreation();
            EV << "Task completed in " << latency << " seconds.\n";
            parent = check_and_cast<m2 *>(getParentModule());
            parent->update_latency(tenant_id, latency.dbl());
            break;
        default:
            break;
    }
    delete msg;
}

void IoT::generateTask() {
    // generate and send out a packet
    TaskMessage *msg = new TaskMessage();
    msg->setType(TASK_MESSAGE);
    msg->setIot_id(id);
    msg->setTenant_id(tenant_id);
    msg->setTask_duration(duration);
    send(msg, "edge_port$o");
}

