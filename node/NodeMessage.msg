enum MessageType {
    TASK_MESSAGE = 1;
    COMP_MESSAGE = 2;
    SYNC_MESSAGE = 3;
    TAG_MESSAGE = 4;
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
    int r[32];
    int l[32];
}

packet TagMessage extends BaseMessage {
    int rho[32];
    int delta[32];
}

