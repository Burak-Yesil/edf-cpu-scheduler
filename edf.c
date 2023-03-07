/*******************************************************************************
* Filename    : edf.c
* Author      : Burak Yesil 
* Date        : 03 - 06 - 2023
* Description : Earliest Deadline First Scheduling Algorithm
* Pledge      : I pledge my honor that I have abided by the Stevens Honor System. 
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//Defining Process Struct:
typedef struct 
{
    int processID; //Which process are we working on?
    int cpuTime; //How much CPU time is required 
    int period; //Repeating frequency
    int deadline; //StartTime + Period 
    int startTime; //Used to calculate how much CPU time has been spent
    int remainingTime; //Used when process execution is paused
}ProcessStruct;

//Defining gcd and lcm functions -> gcd will be called in lcm:
int gcd(int a, int b) //Greatest Common Divisor
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int lcm(int a, int b) //Least Common Multiple
{
    return (a/gcd(a,b))*b;
}

void insertionSort(ProcessStruct arr[], int numofProcesses)
{
    //Sorts array in ascending order based on Deadlines.
    //If two deadlines are the same, then the oldest process is the one to be executed first.
    int i, j;
    ProcessStruct currentProcess;
    for (i = 1; i < numofProcesses; i++) {
        currentProcess = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j].deadline > currentProcess.deadline) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = currentProcess;
    }
}

void getCurrentStateOfProcessQueue(int time, ProcessStruct arr[], int size){
    printf("%d: processes:", time);
    for (int i = 0; i < size; i++) {
        printf(" %d (%d ms)", arr[i].processID, arr[i].remainingTime);
    }
    printf("\n");
}

void popFirstProcessFromProcessQueue(ProcessStruct arr[], int* size){
    //Called when a processes remaining time is 0
    for(int i = 0; i < size; i++){
        arr[i] = arr[i+1]; //Shifts all the processes to the left 
    }
    *size-=1;
}

void checkForPastDeadlines(int time, int * processQueueSize, ProcessStruct processQueue[], int processArraySize, ProcessStruct processArray[]){
    printf("Checking for past deadlines\n");
    for(int i = 0; i < processArraySize; i++){
        //Check if each process is past its deadline 
        int passed = time%processArray[i].deadline==0;
        if(passed){ //If the deadline has passed, we must retreive the information on the process 
            for (int j = 0; j < *processQueueSize; j++) {
                //Check the process queue to update info on each instance of the same process
                if(processQueue[j].processID == processArray[i].processID){
                    //update the process info
                    if (processQueue[j].deadline > time){ //If the deadline of the process in the process queue is greater than the current time you don't want to update the deadline
                        continue;
                    }
                    printf("%d: process %d missed deadline (%d ms left)\n", time, processQueue[j].processID, processQueue[j].remainingTime);
                    //Because we now that the current time is some form of x * deadline, modulo returned 0 we can update the deadlines using the current time
                    processQueue[i].deadline = time + processArray[i].period;    
                }

                if 

            }
        }
    }

}



int main(){

    ///////////////////////////////////
    ////// Setting Everything Up //////
    ///////////////////////////////////

    int numOfProcesses; //Stores the number of processes to be scheduled
    int minIncrement = INT_MAX; //Either gonna increment by 1 or a value less then 1 if a cpu time is less then 1 
    int maxtime;

    printf("Enter the number of processes to schedule: ");
    scanf("%d",&numOfProcesses); //Scanning the user for the number of processes to be scheduled
    ProcessStruct* processArray = (ProcessStruct *) malloc(numOfProcesses*sizeof(ProcessStruct)); //Allocating memory for the processes
    //Scanning the user for the CPU Time and Period of each process
    for(int i = 0; i<numOfProcesses; i++){
        processArray[i].processID = i+1; //Assigning the process ID to i 
        printf("Enter the CPU time of process %d: ", i+1);
        scanf("%d",&processArray[i].cpuTime); //Assigning the CPU Time to i 
        printf("Enter the period of process %d: ", i+1);
        scanf("%d",&processArray[i].period); //Assigning the Period to i 


        //Initializing rest of process struct attributes
        processArray[i].deadline = processArray[i].period; 
        processArray[i].startTime = 0; 
        processArray[i].remainingTime = processArray[i].cpuTime; 

        if (processArray[i].period < minIncrement) {
            minIncrement = processArray[i].period;
        }

        //Setting the max time 
        if (i == 0){
            maxtime = lcm(processArray[i].period, processArray[i].cpuTime);
        }
        else{
            maxtime *= lcm(processArray[i].period, processArray[i].cpuTime);
        }
    }


    ///////////////////////////////////
    ///// Invoking EDF Scheduler /////
    ///////////////////////////////////

    insertionSort(processArray, numOfProcesses); //Sorting the process array in ascending order based on deadlines. This is the initial order of the process array

    if (minIncrement > 1){ //Setting the min amount to increment by and check if a new smallest deadline exists 
        minIncrement = 1;
    }

    for (int i = 0; i < numOfProcesses; i++){ //Used for debugging purposes ----> Shows initial sorted process array
        printf("%d\n", processArray[i].processID);
    }


    ProcessStruct* processQueue = (ProcessStruct *) malloc(numOfProcesses*sizeof(ProcessStruct)); //Creating scheduler array and adding the initial processes to the scheduler array
    for(int i = 0; i<numOfProcesses; i++){
        processQueue[i] = processArray[i];
    }


    int time = 0; //Current Time Stamp
    int processQueueSize = numOfProcesses; //Initial size of the process queue is just the number of processes
    int sumOfWaitingTimes = 0; //Sum of waiting times of all processes throughout the entire scheduler execution

    //Initializing first running process in the queue
    ProcessStruct running = processQueue[0]; //Current running process
    int totalProcsCreated = 1; //Total number of processes created throughout the entire scheduler execution
    getCurrentStateOfProcessQueue(0, processQueue, processQueueSize);
    printf("%d: process %d starts", time, processQueue[0].processID);
    running.remainingTime = running.remainingTime - minIncrement; //move this inside the loop!!!!!!!!



    //MAIN SCHEDULER LOOP
    while (time < maxtime){ //Looping through the process array until the time is less than the max time

        if (running.remainingTime < 0){
            //update process details and switch to next process in the process queue
            popFirstProcessFromProcessQueue(processQueue, &processQueueSize);
        }else{
            //otherwise, continue running the current process
            running.remainingTime = running.remainingTime - minIncrement; //move this inside the loop
        }
        
        



        //ORIGINAL APPROACH:    
        // Steps: 
        // Find smallest deadline 
        // Print Start time, Starting process x
        // Increment global time by minIncrement 
        // decrement process time by minIncrement
        // if remaining time is 0
        //     update process struct to be ready for next scheduling so new deadline, start time, etc
        // else
        //     just go to the top of the loop

    }

    printf("%d: Max Time reached\n", maxtime);
    printf("Sum of all waiting times: %d\n", sumOfWaitingTimes);
    printf("Number of processes created: %d\n", totalProcsCreated);
    printf("Average Waiting Time: %f\n", (double)sumOfWaitingTimes/totalProcsCreated);

    free(processArray);
    free(processQueue);

    return 0;
}


