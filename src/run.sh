#!/bin/bash

SCRATCH_READ_LATENCY=('0' '1' '2' '5' '10')
SCRATCH_WRITE_LATENCY=('0' '1' '2' '5' '10')
SCRATCH_NUM_SIMULTANEOUS_REQ=('2' '4' '8' '16')

DRAM_READ_LATENCY=('0' '5' '10' '15')
DRAM_WRITE_LATENCY=('0' '10' '20' '30')
DRAM_NUM_SIMULTANEOUS_REQ=('2' '4' '8' '16')

# Sweep DRAM Parameters
for drl in "${DRAM_READ_LATENCY[@]}"
  do
  for dwl in "${DRAM_WRITE_LATENCY[@]}"
    do
    echo "./g_sim -dram_read_latency $drl -dram_write_latency $dwl"
    ./g_sim --dram_read_latency=$drl --dram_write_latency=$dwl > output/dram_param_sweep_${drl}_${dwl}.csv
  done
done
