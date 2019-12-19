#!/bin/bash

APP=('bfs' 'cc' 'sssp')
PL=('1' '8' '16' '32')
GRAPHS=('shl_200' 'gemat11' 'mbeacxc' 'lshp1882' 'lock_700' 'citeseer')
GRAPHS_LARGE=('orani678' 'amazon0302' 'G42')
#GRAPHS_LARGE=('soc_Slashdot0902' 'orani678' 'amazon0302' 'G42')

for app in "${APP[@]}"; do
  make clean && make DRAMSIM2=1 MODULE_TRACE=1 ${app}
  for graph in "${GRAPHS[@]}"; do
    for pipeline in "${PL[@]}"; do
      inputGraph="$graph"
      fbase="${inputGraph}_${app}_${pipeline}"
      IO_OPTIONS="--logfile=${fbase}_log.out"
      READGRAPH_OPTIONS="--graph_path=graphs/${inputGraph}.mtx"
      SIMULATION_OPTIONS="--num_iter=8 --num_pipelines=$pipeline --num_dst_readers=4 --app=$app --vertex_properties=${fbase}_vp.out"
      `rm -f trace/*.csv`
      echo "./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 ${fbase}_run.out"
      ./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 ${fbase}_run.out
      if [ $? -eq 0 ]; then
        `rm -f trace/*_out.csv`
        zip -r ${fbase}_trace.zip trace
        mv dramsim.log ${fbase}_dramsim.out
      fi
      mv simulator_output.log ${fbase}_log.out
    done
  done
  for graph in "${GRAPHS_LARGE[@]}"; do
    for pipeline in "${PL[@]}"; do
      inputGraph="$graph"
      fbase="${inputGraph}_${app}_${pipeline}"
      IO_OPTIONS="--logfile=${fbase}_log.out"
      READGRAPH_OPTIONS="--graph_path=graphs/${inputGraph}.mtx"
      SIMULATION_OPTIONS="--num_iter=3 --num_pipelines=$pipeline --num_dst_readers=4 --app=$app --vertex_properties=${fbase}_vp.out"
      `rm -f trace/*.csv`
      echo "./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 ${fbase}_run.out"
      ./g_sim $DRAM_OPTIONS $READGRAPH_OPTIONS $SIMULATION_OPTIONS 2>&1 ${fbase}_run.out
      if [ $? -eq 0 ]; then
        `rm -f trace/*_out.csv`
        zip -r ${fbase}_trace.zip trace
        mv dramsim.log ${fbase}_dramsim.out
      fi
      mv simulator_output.log ${fbase}_log.out
    done
  done
done
