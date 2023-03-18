enum MessageType {
    TASK_MESSAGE = 1;
    COMP_MESSAGE = 2;
    EXE_ACT_MESSAGE = 3;
    EXE_SCAN_MESSAGE = 4;
    EXE_TASK_MESSAGE = 5;
    EXE_DONE_MESSAGE = 6;
    SYNC_MESSAGE = 7;
    INFO_MESSAGE = 8;
    SUBP_SYNC_MESSAGE = 9;
    SUBP_INFO_MESSAGE = 10;
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
    int edge_id;
    double period;
}

packet InfoMessage extends BaseMessage {
}

packet SubpSyncMessage extends BaseMessage {
    int subp_id;
    double period;
}

packet SubpInfoMessage extends BaseMessage {
}