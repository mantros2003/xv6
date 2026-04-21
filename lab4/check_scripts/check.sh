#!/bin/bash

submission=$1
# if test_dir already exists, delete it
if [ -d "./test_dir" ]; then
	rm -rf ./test_dir
fi
mkdir ./test_dir

echo $submission

# Accept both .tar.gz and .gz or any other gz ;)
fileNameRegex="lab4_[0-9]{4}[A-Z]{2}.[0-9]{4}.*.gz"

echo "Setting the test directory"

tar -xzvf "$submission" -C ./test_dir
cp *.c out* *.sh ./test_dir
cd ./test_dir

# Change the gcc binary
sed -i 's/gcc-10\|gcc-9/gcc/g' Makefile

echo "Executing the test cases"

pkill qemu-system-x86
pkill qemu-system-i386
make clean

make xv6.img

mv init.c init_old.c

total_tests=2

for ((t=1;t<=$total_tests;++t))
do
    start_time=$(date +%s)  # Get start time in seconds since epoch
    cp test_lab4_$t.c init.c
    make clean; make xv6.img
    echo -e "\e[91mRunning test case $t\e[0m"  # Red color
    (
        while true; do
            current_time=$(date +%s)  # Get current time in seconds since epoch
            elapsed=$((current_time - start_time))  # Calculate elapsed time
            printf "\rElapsed time: %02d:%02d:%02d" $((elapsed/3600)) $(( (elapsed%3600)/60)) $((elapsed%60))  # Print elapsed time
            sleep 1  # Wait for 1 second
        done
    ) &
    timer_pid=$!  # Store the PID of the timer process
	echo $PWD
    timeout 10s ./test_lab4.sh 180 | grep -E "Passed|\(\(P\)\)" > res_lab4_$t
    kill $timer_pid  # Stop the timer process
    echo  # Move to the next line after the timer
done
make clean

passed_tests=0

for ((t=1;t<=$total_tests;++t))
do
	echo -n "Test #${t}: "

	# NOTE: we are doing case insensitive matching.  If this is not what you want,
	# just remove the "-i" flag
	if diff -iZwB <(cat out_lab4_$t) <(cat res_lab4_$t) > /dev/null
	then
		echo -e "\e[0;32mPASS\e[0m"
		((passed_tests++))
	else
		echo -e "\e[0;31mFAIL\e[0m"
	fi
done
echo "$passed_tests/$total_tests test cases passed"
