#!/bin/bash

inputGraph=$3
app=$2

fbase="${inputGraph}_${app}"

IO_OPTIONS="--logfile=${fbase}_log.out"
READGRAPH_OPTIONS="--should_init=1 --graph_path=graphs/${inputGraph}.mtx"
SIMULATION_OPTIONS="--num_iter=$4 --num_pipelines=$1 --num_dst_readers=4 --app=$app --vertex_properties=${fbase}_vp.out"

#make clean && make DRAMSIM2=1 MODULE_TRACE=1 $app
#if [ $? -eq 0 ]; then 
  echo "./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 | tee \"${fbase}_run.out\""
  ./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 #| tee "${fbase}_run.out"
  if [ $? -eq 0 ]; then
    `rm -f trace/*_out.csv`
    zip -r ${fbase}_trace.zip trace
    mv dramsim.log ${fbase}_dramsim.out
  fi
  mv simulator_output.log ${fbase}_log.out
#fi
