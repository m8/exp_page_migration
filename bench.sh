#!/bin/bash
make

./bench 0 0  > local.txt
./bench 2 2  > local_pmem.txt
./bench 0 1  > dram_dram.txt
./bench 0 2  > dram_pmem.txt
./bench 2 3  > pmem_pmem.txt

bash ./plot.sh