package node;

simple Node {
    parameters:
        int id;
        bool sendMsgOnInit = default(false);
        int limit = default(2);
        //volatile double delayTime @unit(s);
        @display("i=block/routing");
}

simple IoT extends Node {
    gates:
        inout edge_port[];
}

simple Edge extends Node {
    gates:
        inout iot_port[];
        inout subp_port;
        inout pnode_port;
}

simple SubPNode extends Node {
    gates:
        inout edge_port[];
        inout pnode_port;
}

simple PNode extends Node {
    gates:
        inout edge_port[];
        inout subp_port[];
}

network m2Clock {
    parameters:
        int pnode_n = 1;
        int subp_n = default(4);
        int edge_n = default(64);
        int iot_n = default(128);
    submodules:
        pnode: PNode {
            parameters:
                id = 0;
            gates:
                edge_port[edge_n];
                subp_port[subp_n];
        }
        subp[subp_n]: SubPNode {
            parameters:
                id = index;
            gates:
                edge_port[edge_n];
        }
        edge[edge_n]: Edge {
            parameters:
                id = index;
            gates:
                iot_port[iot_n];
        }
        iot[iot_n]: IoT {
            parameters:
                id = index;
            gates:
                edge_port[edge_n];
        }
    connections allowunconnected:
        for i=0..subp_n-1 {
            pnode.subp_port[i] <--> { delay = 100ms; } <--> subp[i].pnode_port;
        }
        for i=0..subp_n*12-1 {
            subp[int(i/16)].edge_port[i] <--> { delay = 10ms; } <--> edge[i].subp_port;
        }
        for i=subp_n*12..edge_n-1 {
            pnode.edge_port[i] <--> { delay = 100ms; } <--> edge[i].pnode_port;
        }
        for i=0..iot_n-1 {
            edge[int(i/2)].iot_port[i] <--> { delay = 5ms; } <--> iot[i].edge_port[int(i/2)];
        }
}