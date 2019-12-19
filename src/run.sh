#!/bin/bash

compile_app=('bfs' 'cc' 'sssp' 'pr')
pipelines=('1' '8' '16' '32')
graphs=('soc_Slashdot0902')


# Sweep DRAM Parameters
for app in "${APPS[@]}" do
  `make clean && make ${app}`
  for graph in "${DRAM_WRITE_LATENCY[@]}"
    do
    for graph in "${GNAME[@]}"
      do
      inputGraph="/software/graphs/mm/$graph/$graph.mtx"
      echo "./g_sim --dram_read_latency=$drl --dram_write_latency=$dwl --should_init=1 --graph_path=${inputGraph}"
      ./g_sim --dram_read_latency=$drl --dram_write_latency=$dwl --should_init=1 --graph_path=${inputGraph} #> output/dram_param_sweep_${drl}_${dwl}.csv
    done
  done
done
