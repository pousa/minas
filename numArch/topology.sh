#!/bin/bash

###################################################################
if [ -d "${PATHMINAS}/numArch/output" ]; then
   rm -rf ${PATHMINAS}/numArch/output
   rm -rf /tmp/output/
fi

mkdir ${PATHMINAS}/numArch/output

###################################################################
#sysfs
SYSFS_CPU="/sys/devices/system/cpu"
SYSFS_NODE="/sys/devices/system/node"
####################################################################

######################   NUMA    ###################################
if [ -d "${SYSFS_NODE}/node1" ]; then

####################################################################
#number of nodes and cpus
NODE_POSSIBLE_COUNT=`ls -1d ${SYSFS_NODE}/node[0-9]* | wc -l`
CPU_POSSIBLE_COUNT=`ls -d ${SYSFS_CPU}/cpu[0-9]* | wc -l`
NODE_CPUS=`ls -d ${SYSFS_NODE}/node0/cpu[0-9]* | wc -l`
LLC=`ls -d ${SYSFS_CPU}/cpu0/cache/index[0-9]* | wc -l`
let LLC=$LLC-1;
L1=`cat ${SYSFS_CPU}/cpu0/cache/index0/size | tr -d 'K'` 
MEM=`awk '/MemTotal/ {print $4}' ${SYSFS_NODE}/node0/meminfo`
FREQ=`cat /proc/cpuinfo | grep 'cpu MHz' | head -n 1 | awk '{print $4}'`;

for((i=0;i<$CPU_POSSIBLE_COUNT;i++)); do cpu=`cat /sys/devices/system/cpu/cpu${i}/topology/physical_package_id`;  if [ $cpu -eq 0 ];  then let MAXCORESSOCKET=$MAXCORESSOCKET+1;  fi; done; 

NODE_CPUIDS=""; for i in `ls -d /sys/devices/system/node/node0/cpu[0-9]*`; do NODE_CPUIDS=$NODE_CPUIDS" `basename $i | sed s/cpu//`"; done; PACKAGE_COUNT=1; j=0; for i in ${NODE_CPUIDS}; do let j=$j+1; p=`cat /sys/devices/system/cpu/cpu${i}/topology/physical_package_id`; PACKAGE[${p}]+="${j}"; if [ ${PACKAGE_COUNT} -lt $((${p}+1)) ]; then  let PACKAGE_COUNT=$((${p}+1)) ; fi; done;

####################################################################
#running topology on local way
cd ${PATHMINAS}/numArch/topo
make clean
make

#  isNuma helperThread threadDist vector {cores, ht, ws}

./topo 1 0 1 1 > ${PATHMINAS}/numArch/output/intra_placement.minas
./topo 1 0 0 1 > ${PATHMINAS}/numArch/output/inter_placement.minas

####################################################################
#running Stream on local way
cd ${PATHMINAS}/numArch/stream
make clean
make

export OMP_NUM_THREADS=${CPU_POSSIBLE_COUNT}
./stream_c.exe >> ${PATHMINAS}/numArch/output/streamlocal.minas
export OMP_NUM_THREADS=1
numactl --membind=0 --physcpubind=0 ./stream_c.exe >> ${PATHMINAS}/numArch/output/streamlocal.minas

####################################################################
#running Stream for every node on the machine
for ((j=0;j < ${NODE_POSSIBLE_COUNT} ;j++))
do
core=`ls -d /sys/devices/system/node/node$j/cpu[0-9]* | head -1`
core=`basename $core | sed s/cpu//`
for ((i=0;i<${NODE_POSSIBLE_COUNT};i++))
do
numactl --membind=$i --physcpubind=$core ./stream_c.exe >> ${PATHMINAS}/numArch/output/stream.minas
done
done

####################################################################
#get bandwidth and access time local and remote
echo -e "\n#Local Parallel bandwidth" >> ${PATHMINAS}/numArch/output/numacost.minas
cat ${PATHMINAS}/numArch/output/streamlocal.minas | egrep '(Triad)' | head -1 | awk '{print $2}' >> ${PATHMINAS}/numArch/output/numacost.minas
echo -e "\n#Local Sequential bandwidth" >> ${PATHMINAS}/numArch/output/numacost.minas
cat ${PATHMINAS}/numArch/output/streamlocal.minas | egrep '(Triad)' | head -2 | awk '{print $2}' >> ${PATHMINAS}/numArch/output/numacost.minas

cat ${PATHMINAS}/numArch/output/stream.minas | egrep '(Triad)' > stream.data
rm ${PATHMINAS}/numArch/output/stream.minas
cut -f2  stream.data > tband.data
cut -f3  stream.data > tfn.data
rm stream.data
cut -c6-11 tfn.data > fn.data
cut -c3-11 tband.data > band.data
rm tfn.data tband.data

####################################################################
#computing average bandwidth and nodes bandwidth
echo -e "\n#Remote bandwidth" >> ${PATHMINAS}/numArch/output/numacost.minas
awk '{sum+=$0} END { print sum/NR}' band.data >> ${PATHMINAS}/numArch/output/numacost.minas

columns=""
for ((i=0;i<${NODE_POSSIBLE_COUNT};i++))
do
columns=$columns"- "
done
cat band.data | paste ${columns} > ${PATHMINAS}/numArch/output/bandwidth.minas
rm band.data

####################################################################
#Computing NUMA factor
counter=0
while read n
do
	let i=$counter/${NODE_POSSIBLE_COUNT}
	let j=$counter%${NODE_POSSIBLE_COUNT}
	let counter++

	var="tabela_${i}_${j}"
	declare $var=$n
done < fn.data
rm fn.data

ite=${NODE_POSSIBLE_COUNT}

for ((i=0;i<$ite;i++))
do
  for ((j=0;j<$ite;j++))
  do
    num="tabela_${i}_${j}"
    div="tabela_${i}_${i}"

    var="fator_${i}_${j}"
    declare $var=`echo scale=4\;${!num} / ${!div} | bc`
    echo ${!var} >> tnumafactor.minas
  done
done

echo -e "\n#NUMA factor" >> ${PATHMINAS}/numArch/output/numacost.minas
awk '{sum+=$0} END { print sum/NR}' tnumafactor.minas >> ${PATHMINAS}/numArch/output/numacost.minas

cat tnumafactor.minas | paste ${columns} > ${PATHMINAS}/numArch/output/numafactor_stream.minas
rm tnumafactor.minas

###########################################################################
cd ${PATHMINAS}/MA-place
#NUMA topology
make clean && make
cd ${PATHMINAS}/numArch/machine
make clean && make
./compute-machine >  ${PATHMINAS}/numArch/output/numatopology.minas

CACHE_LEVEL=`ls -d ${SYSFS_CPU}/cpu0/cache/index[0-9]* | wc -l`;

for ((i=0;i<${NODE_POSSIBLE_COUNT};i++))
do
NODE_MEM=`awk '/MemTotal/ {print $4}' ${SYSFS_NODE}/node${i}/meminfo`
  for p in `ls -d ${SYSFS_NODE}/node${i}/cpu[0-9]*`; do
    j="`basename $p | sed s/cpu//`";
      for ((cl=0;cl<${CACHE_LEVEL};cl++))
        do
         CPU_CACHE=`cat ${SYSFS_CPU}/cpu${j}/cache/index${cl}/size | tr -d 'K'`;
        done;
  done;
done;

let llc=`expr ${CACHE_LEVEL}-1`
LLC_SIZE=`cat ${SYSFS_CPU}/cpu0/cache/index${llc}/size | tr -d 'K'`
if [ ${llc} -eq 2 ] 
then
  L2_SIZE=${LLC_SIZE}
  let L2_SIZE=${L2_SIZE}*${NODE_POSSIBLE_COUNT}
else
  L2_SIZE=`cat ${SYSFS_CPU}/cpu0/cache/index2/size | tr -d 'K'`
  let LLC_SIZE=${LLC_SIZE}/1024
  let LLC_SIZE=${LLC_SIZE}*${NODE_POSSIBLE_COUNT}*2
fi


###################################################################################
#Latency for read - REMOTE and LOCAL
cd ${PATHMINAS}/numArch/lmbench3
mkdir ./SCCS
touch ./SCCS/s.ChangeSet
make clean
make build

folder=`ls bin/`
cd bin/$folder

echo $folder

./lat_mem_rd -P 1 -N 1 $LLC_SIZE $L2_SIZE &> tmp.out
echo -e "\n#Local Latency " >> ${PATHMINAS}/numArch/output/numacost.minas
cat tmp.out | tail -2 |  awk '{print $2}' >> ${PATHMINAS}/numArch/output/numacost.minas
rm tmp.out

####################################################################
#running lat_mem for every node on the machine
for ((j=0;j < ${NODE_POSSIBLE_COUNT} ;j++))
do
core=`ls -d /sys/devices/system/node/node$j/cpu[0-9]* | head -1`
core=`basename $core | sed s/cpu//`
for ((i=0;i<${NODE_POSSIBLE_COUNT};i++))
do
(numactl --membind=$i --physcpubind=$core ./lat_mem_rd -P 1 -N 1 $LLC_SIZE $L2_SIZE) &> tmp.out
cat tmp.out | tail -2 | awk '{print $2}' >> tlatencies.minas 
done
done

sed '/^$/d' < tlatencies.minas > latencies.minas
rm tlatencies.minas tmp.out

####################################################################
#Computing NUMA factor
counter=0
while read n
do
	let i=$counter/${NODE_POSSIBLE_COUNT}
	let j=$counter%${NODE_POSSIBLE_COUNT}
	let counter++

	var="tabela_${i}_${j}"
	declare $var=$n
done < latencies.minas
rm latencies.minas

ite=${NODE_POSSIBLE_COUNT}

for ((i=0;i<$ite;i++))
do
  for ((j=0;j<$ite;j++))
  do
    num="tabela_${i}_${j}"
    div="tabela_${i}_${i}"

    var="fator_${i}_${j}"
    lat="latency_${i}_${j}"
    declare $var=`echo scale=4\;${!num} / ${!div} | bc`
    declare $lat=`echo scale=4\;${!num} | bc`

    echo ${!var} >> tnumafactor.minas
    echo ${!lat} >> tlatency.minas
  done
done

echo -e "\n#NUMA factor lmbench" >> ${PATHMINAS}/numArch/output/numacost.minas
awk '{sum+=$0} END { print sum/NR}' tnumafactor.minas >> ${PATHMINAS}/numArch/output/numacost.minas

cat tnumafactor.minas | paste ${columns} > ${PATHMINAS}/numArch/output/numafactor_lmbench.minas
rm tnumafactor.minas

cat tlatency.minas | paste ${columns} > ${PATHMINAS}/numArch/output/latency_lmbench.minas
rm tlatency.minas

##########################################################################
if ! test -d /tmp/output
then
    mkdir /tmp/output
fi

cp ${PATHMINAS}/numArch/output/* /tmp/output

#########################################################################################
# Get Multicore (caches, nodes) read latencies
MACHINE=`uname -m | sed -e 's/ //g' | sed -e 's?/?-?g'`
OS="${MACHINE}-linux-gnu"
cp ${PATHMINAS}/numArch/lmbench3/bin/${OS}/lmbench.a ${PATHMINAS}/numArch/latencies/ 
cd $PATHMINAS/numArch/latencies
make clean && make
hwloc-bind socket:0.pu:0 -- ./compute-latencies

#########################################################################################
# IS UMA MACHINE
else

echo "THIS IS NOT A NUMA MACHINE!"

fi
