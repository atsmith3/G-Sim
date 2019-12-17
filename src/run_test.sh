#!/bin/bash

inputGraph="soc_Slashdot0902.mtx"

DRAM_OPTIONS="--dram_read_latency=0 --dram_write_latency=0"
READGRAPH_OPTIONS="--should_init=1 --graph_path=graphs/${inputGraph}"
SIMULATION_OPTIONS="--num_pipelines=$1 --num_dst_readers=4"

`rm graphs/${inputGraph}.bin`
echo "./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS"
#gdb --args ./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS | tee "${2}.out"
./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS | tee "${2}.out"
