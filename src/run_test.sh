#!/bin/bash

inputGraph="soc_Slashdot0902.mtx"
`rm graphs/${inputGraph}.bin`
echo "./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=graphs/${inputGraph} --num_pipelines=$1"
gdb --args ./g_sim --dram_read_latency=0 --dram_write_latency=0 --should_init=1 --graph_path=graphs/${inputGraph} --num_pipelines=$1 | tee "${2}.out"
