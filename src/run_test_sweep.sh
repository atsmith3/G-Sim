#!/bin/bash

inputGraphs=('soc_Slashdot0902')
numPipelines=('1' '4' '8' '16' '32' '64' '128' '256' '512' '1024')
for graph in "${inputGraphs[@]}"
  do
    for pipelines in "${numPipelines[@]}"
      do
        `rm graphs/${graph}.mtx.bin`
        echo "./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=graphs/${graph}.mtx --num_pipelines=${pipelines} | tee output/pipline_sweep_DRAMSim2_DDR4_${graph}_${pipelines}.out"
#gdb --args ./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=graphs/${inputGraph} --num_pipelines=$1 #> output/dram_param_sweep_${drl}_${dwl}.csv
        ./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=graphs/${graph}.mtx --num_pipelines=${pipelines} | tee output/pipline_sweep_DRAMSim2_DDR4_${graph}_${pipelines}.out
    done
done
