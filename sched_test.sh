#! /bin/bash

echo "Executing real time scheduler test cases..."

echo ""
echo 750000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 1: sched_rt_runtime_us = 750000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1
echo ""


echo ""
echo 550000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 2: sched_rt_runtime_us = 550000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1
echo ""


echo ""
echo 500000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 3: sched_rt_runtime_us = 500000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1
echo ""


echo ""
echo 450000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 4: sched_rt_runtime_us = 450000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1
echo ""


echo ""
echo 250000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 5: sched_rt_runtime_us = 250000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1
echo ""

echo ""
echo 50000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 5: sched_rt_runtime_us = 50000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1
echo ""


echo 950000 > /proc/sys/kernel/sched_rt_runtime_us
exit
