#! /bin/bash

echo "Executing real time scheduler test cases..."

echo ""
echo 550000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 1: sched_rt_runtime_us = 550000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1 -J 1000 -L 250000
echo ""


echo ""
echo 450000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 2: sched_rt_runtime_us = 450000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1 -J 1000 -L 250000
echo ""


echo ""
echo 300000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 3: sched_rt_runtime_us = 300000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1 -J 1000 -L 250000
echo ""


echo ""
echo 150000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 4: sched_rt_runtime_us = 150000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1 -J 1000 -L 250000
echo ""


echo ""
echo 50000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 5: sched_rt_runtime_us = 50000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1 -J 1000 -L 250000
echo ""


echo ""
echo 20000 > /proc/sys/kernel/sched_rt_runtime_us 
echo "Test case 6: sched_rt_runtime_us = 20000"
echo "#########################################"
./sched_test -c 0 -f 1 -r 1 -n 1 -J 1000 -L 250000

echo ""

# Restore the original setting
echo 950000 > /proc/sys/kernel/sched_rt_runtime_us
exit
