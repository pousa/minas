#!/bin/bash

if [ ! -d "/sys/devices/system/node/node1" ]; then
  echo "INFO: NUMA Machine not detected!"
  exit -1;
fi

echo -e "\n Test of TUNED VERSION OF STREAM\n"
echo -e "\n Comparing malloc+First-touch with MAi-array\n"

CPU_POSSIBLE_COUNT=`ls -d ${SYSFS_CPU}/cpu[0-9]* | wc -l`;
export OMP_NUM_THREADS=$CPU_POSSIBLE_COUNT

make all

./stream_dynamic/stream_dynamic > stream_dynamic.txt
sleep 10
./stream_dynamic/stream_dynamic_mai > stream_dynamic_mai.txt
sleep 10
./stream_one/stream_one > stream_one.txt
sleep 10
./stream_one/stream_one_mai > stream_one_mai.txt
sleep 10
./stream_original/stream_original > stream_original.txt
sleep 10
./stream_original/stream_original_mai > stream_original_mai.txt
sleep 10
./stream_random/stream_random > stream_random.txt
sleep 10
./stream_random/stream_random_mai > stream_random_mai.txt

make clean

echo -e "\n DONE\n"
