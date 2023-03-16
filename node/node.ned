package node;

simple Node {
    parameters:
        int id;
        @display("i=block/routing");
}

simple Executor {
    gates:
        inout host_port;
}

simple IoT extends Node {
    parameters:
        @class(IoT);
        volatile double sleepTime @unit(s) = default(1s);
        volatile double burstTime @unit(s) = default(1s);
        volatile double sendIaTime @unit(s) = default(exponential(0.1s));
        int tenant_id;
    gates:
        inout edge_port;
}

module Edge extends Node {
    parameters:
        @class(Edge);
        volatile double syncTime @unit(s) = default(10s);
        int exe_n = default(8);
    gates:
        inout iot_port[];
        inout pnode_port;
        inout executor_port[exe_n];
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

network m2 {
    parameters:
        @class(m2);
        int pnode_n = 1;
        int subp_n = default(4);
        int direct_edge_n = default(16);
        int edge_per_subp = default(12);
        int edge_n = subp_n * edge_per_subp + direct_edge_n;
        int tenant_n = default(8);
        int iot_per_tenant = default(16);
        int iot_n = tenant_n * iot_per_tenant;
        int exe_n = default(8);
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
                exe_n = parent.exe_n;
            gates:
                iot_port[parent.iot_n];
                executor_port[parent.exe_n];
        }
        iot[iot_n]: IoT {
            parameters:
                id = index;
                tenant_id = id % parent.tenant_n;
        }
    connections allowunconnected:
        for i=0..subp_n-1 {
            pnode.subp_port[i] <--> {  delay = 100ms; } <--> subp[i].pnode_port;
        }
        for i=0..subp_n*edge_per_subp-1 {
            subp[int(i/edge_per_subp)].edge_port[i] <--> {  delay = 10ms; } <--> edge[i].pnode_port;
        }
        for i=subp_n*edge_per_subp..edge_n-1 {
            pnode.edge_port[i] <--> {  delay = 100ms; } <--> edge[i].pnode_port;
        }
        for i=0..iot_n-1 {
            edge[i % edge_n].iot_port[i] <--> {  delay = 5ms; } <--> iot[i].edge_port;
        }
}
