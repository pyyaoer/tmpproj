package node;

simple Node {
    parameters:
        int id;
        @display("i=block/routing");
}

simple IoT extends Node {
    parameters:
        @class(IoT);
        volatile double sleepTime @unit(s) = default(1s);
        volatile double burstTime @unit(s) = default(1s);
        volatile double sendIaTime @unit(s) = default(exponential(1s));
    gates:
        inout edge_port;
}

simple Edge extends Node {
    parameters:
        @class(Edge);
        volatile double syncTime @unit(s) = default(10s);
    gates:
        inout iot_port[];
        inout pnode_port;
}

simple PNodeBase extends Node {
    gates:
        inout edge_port[];
}

simple SubPNode extends PNodeBase {
    parameters:
        @class(SubPNode);
    gates:
        inout pnode_port;
}

simple PNode extends PNodeBase {
    parameters:
        @class(PNode);
    gates:
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
                edge_port[parent.edge_n];
                subp_port[parent.subp_n];
        }
        subp[subp_n]: SubPNode {
            parameters:
                id = index;
            gates:
                edge_port[parent.edge_n];
        }
        edge[edge_n]: Edge {
            parameters:
                id = index;
            gates:
                iot_port[parent.iot_n];
        }
        iot[iot_n]: IoT {
            parameters:
                id = index;
        }
    connections allowunconnected:
        for i=0..subp_n-1 {
            pnode.subp_port[i] <--> {  delay = 100ms; } <--> subp[i].pnode_port;
        }
        for i=0..subp_n*12-1 {
            subp[int(i/16)].edge_port[i] <--> {  delay = 10ms; } <--> edge[i].pnode_port;
        }
        for i=subp_n*12..edge_n-1 {
            pnode.edge_port[i] <--> {  delay = 100ms; } <--> edge[i].pnode_port;
        }
        for i=0..iot_n-1 {
            edge[int(i/2)].iot_port[i] <--> {  delay = 5ms; } <--> iot[i].edge_port;
        }
}
