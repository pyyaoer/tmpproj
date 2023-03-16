enum MessageType {
    TASK_MESSAGE = 1;
    COMP_MESSAGE = 2;
    EXE_ACT_MESSAGE = 3;
    EXE_SCAN_MESSAGE = 4;
    EXE_TASK_MESSAGE = 5;
    EXE_DONE_MESSAGE = 6;
    SYNC_MESSAGE = 7;
    TAG_MESSAGE = 8;
}

packet BaseMessage {
    MessageType type;
}

packet TaskMessage extends BaseMessage {
    int iot_id;
    int tenant_id;
    double task_duration;
}

packet CompMessage extends BaseMessage {
    simtime_t creation;
}

packet ExeActMessage extends BaseMessage {
    int executor_id;
}

packet ExeScanMessage extends BaseMessage {
    int executor_id;
}

packet ExeTaskMessage extends BaseMessage {
    int succ;
    int task_id;
    double duration;
}

packet ExeDoneMessage extends BaseMessage {
    int task_id;
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