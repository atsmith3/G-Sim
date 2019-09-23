#!/bin/bash

SCRATCH_READ_LATENCY=('0' '1' '2' '5' '10')
SCRATCH_WRITE_LATENCY=('0' '1' '2' '5' '10')
SCRATCH_NUM_SIMULTANEOUS_REQ=('2' '4' '8' '16')

DRAM_READ_LATENCY=('0' '5' '10' '15')
DRAM_WRITE_LATENCY=('0' '10' '20' '30')
DRAM_NUM_SIMULTANEOUS_REQ=('2' '4' '8' '16')

GNAME=('arabic_2005' 'soc_Slashdot0902' 'hollywood_2009' 'wikipedia_20070206' 'webbase_2001')


# Sweep DRAM Parameters
for drl in "${DRAM_READ_LATENCY[@]}"
  do
  for dwl in "${DRAM_WRITE_LATENCY[@]}"
    do
    for graph in "${GNAME[@]}"
      do
      inputGraph="/software/graphs/mm/$graph/$graph.mtx"
      echo "./g_sim --dram_read_latency=$drl --dram_write_latency=$dwl --should_init=1 --graph_path=${inputGraph}"
      ./g_sim --dram_read_latency=$drl --dram_write_latency=$dwl --should_init=1 --graph_path=${inputGraph} #> output/dram_param_sweep_${drl}_${dwl}.csv
    done
  done
done
