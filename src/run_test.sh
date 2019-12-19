#!/bin/bash

inputGraph="soc_Slashdot0902"
app=$2

fbase="${inputGraph}_${app}"

IO_OPTIONS="--logfile=${fbase}_log.out"
READGRAPH_OPTIONS="--should_init=1 --graph_path=graphs/${inputGraph}.mtx"
SIMULATION_OPTIONS="--num_pipelines=$1 --num_dst_readers=4 --app=$app --vertex_properties=${fbase}_vp.out"

echo "./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 | tee \"${fbase}_run.out\""
./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 | tee "${fbase}_run.out"

