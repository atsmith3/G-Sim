#!/bin/bash

inputGraph="soc_Slashdot0902.mtx"
`rm ${inputGraph}.bin`
echo "./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=${inputGraph}"
#gdb --args ./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=${inputGraph} #> output/dram_param_sweep_${drl}_${dwl}.csv
./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=${inputGraph} #> output/dram_param_sweep_${drl}_${dwl}.csv
