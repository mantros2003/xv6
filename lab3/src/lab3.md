# COL331 Lab 3: Tinkering with the Scheduler
The `xv6` scheduler implements a primitive round robin scheduling algorithm. Although this scheduling algorithm ensures that no process is starved of CPU time, there is no notion of "priority" or "types" of processes - all processes get equal CPU time irrespective of the importance of the tasks they perform. 

In this lab, you will implement another fairly simple scheduling algorithm, which will allow processes to specify whether they are "foreground" or "background", and they will be allocated CPU time accordingly. 

## Introduction

Broadly, processes can be classified into the following two types: 

- **Foreground Processes**: These are tasks which require a lot of user interaction, and are time-sensitive. For example, a music player on a phone, or a text editor like Vim running in the terminal. Such tasks need to be scheduled regularly for a smooth user experience.  

- **Background Processes**: These are tasks which are not very time sensitive. These may be long computational tasks which do not require much user interaction. For example, downloading large data from a server may be a background process. 

We would like to add a functionality using which a process can indicate to the operating system whether it is a foreground process or a background process. The scheduler must also be designed so that time slices are given to processes according to the preference each process indicates. 

## System Calls 
In the first part of the lab, you must implement two system calls. In `proc.h`, there is a new field (`int policy`) which need to be set and used by the below system calls:

- `int set_sched_policy(int policy)`: Using this system call, a process can indicate to the operating system whether it is a foreground or a background process. A foreground process would have `policy=0`, while a background process would have `policy=1`. The system call should return `0` if the policy for the process was set successfully, and `-22` in all other cases. 

- `int get_sched_policy(void)`: Using this system call, a process can get the scheduling policy for itself.

For reference, you may refer to the implementations of any of the other system calls in `syscall.h`. You may fill in the definitions of the system call functions in `sysproc.c` (a basic skeleton is provided).

## Scheduler 
The implementation of the `xv6` scheduler is in the `schedule()` function in `proc.c`, which currently implements the round-robin scheduling algorithm. 

Your task is to modify the `schedule` function so that it picks tasks for scheduling as per their scheduling policy, i.e. whether they are foreground or background. 

The scheduling algorithm is simple in principle: Overall, foreground processes must be allotted 90% of the CPU time, and background processes 10%. Within foreground and background processes, we may continue to schedule in a round-robin manner. 

You may implement this scheduling algorithm in any way that you like. However, you must ensure that processes are not starved of CPU time. If there are no background processes, you may switch back to running foreground processes, and vice-versa. 

## Testing 
To create multiple processes, as in `p18-sched`, we have called `pinit` twice in `main`. The `pinit` function has also been modified to take an argument `policy`, which will set the scheduling policy of the process. Currently, there is an infinite task in `init.c`, which prints to the display after a certain number of iterations. 

In the current setup, starting up `xv6` should produce an output like the following on the screen: 
```
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 0
Task done by process with sched policy 1
...
```
i.e. the statement "Task done by process with sched policy 0" should be 9x as frequent as the statement "Task done by process with sched policy 1", as per the scheduling algorithm we have discussed. 

You can extend upon this technique to test the completeness and robustness of your implementation of your algorithm further. 

---

## Deliverables (Tentative)
You will be required to submit the entire ```xv6-step-by-step``` folder with your changes. The code should have the implementation of the system calls and the modified scheduling algorithm as described above. 

In the ```xv6-step-by-step``` root directory, run the following commands:
```
make clean
tar czvf lab3_<entryNumber>.tar.gz *
``` 
This will create a tarball with the name, ```lab3_<entryNumber>.tar.gz``` in the same directory. Submit this tarball on Moodle. Entry number format: 2020CS10567. (All English letters will be in capitals in the
entry number)