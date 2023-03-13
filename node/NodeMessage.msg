enum MessageType {
    TASK_MESSAGE = 1;
    COMP_MESSAGE = 2;
    SYNC_MESSAGE = 3;
    TAG_MESSAGE = 4;
    EXECUTOR_MESSAGE = 5;
}

packet BaseMessage {
    MessageType type;
}

packet TaskMessage extends BaseMessage {
    int iot_id;
    int tenant_id;
}

packet CompMessage extends BaseMessage {
    simtime_t creation;
}

packet SyncMessage extends BaseMessage {
    int gate_id;
    double period;
    int r[32];
    int l[32];
}

packet TagMessage extends BaseMessage {
    double rho[32];
    double delta[32];
}

packet ExecutorMessage extends BaseMessage {
    double duration;
}