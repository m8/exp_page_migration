#!/bin/bash

awk '{print $1+0, $2, $4}' "local.txt" > "local.txt.tmp"
awk '{print $1+0, $2, $4}' "local_pmem.txt" > "local_pmem.txt.tmp"
awk '{print $1+0, $2, $4}' "dram_dram.txt" > "dram_dram.txt.tmp"
awk '{print $1+0, $2, $4}' "dram_pmem.txt" > "dram_pmem.txt.tmp"
awk '{print $1+0, $2, $4}' "pmem_pmem.txt" > "pmem_pmem.txt.tmp"

gnuplot -persist <<-EOFMarker
set terminal pngcairo size 800,600 enhanced font 'Arial,10'
set output 'throughput.png'
set title 'Throughput vs Size'
set xlabel 'Size (MB)'
set ylabel 'Throughput (MB/second)'
set grid
set key inside at graph 0.95,0.85

plot 'local.txt.tmp' using 1:3 with linespoints title 'Local - DRAM - (Numa 0-0)' lc rgb 'red', \
     'local_pmem.txt.tmp' using 1:3 with linespoints title 'Local - PMEM - (Numa 2-2) ' lc rgb 'orange', \
     'dram_dram.txt.tmp' using 1:3 with linespoints title 'Cross Numa - DRAM - (Numa 0-1) ' lc rgb 'blue', \
     'dram_pmem.txt.tmp' using 1:3 with linespoints title 'Cross Numa - DRAM to PM (Numa 0-3)' lc rgb 'green', \
     'pmem_pmem.txt.tmp' using 1:3 with linespoints title 'Cross Numa - PM to PM (Numa 2-3)' lc rgb 'purple'
EOFMarker

rm *.txt.tmp
