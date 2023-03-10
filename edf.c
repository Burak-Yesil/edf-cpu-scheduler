/*******************************************************************************
 * Filename    : edf.c
 * Author      : Burak Yesil 
 * Date        : 03 - 04 - 2023
 * Description : Earliest Deadline First Scheduling Algorithm
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System. 
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct ProcessStruct {
  int ProcessID;
  int age;
  int period;
  int deadline;
  int cpuTime;
  int timeRemaining;
  int pre_emptValue;
};


//Usage: Prints the current state of the queue
void getCurrentStateOfProcessQueue(struct ProcessStruct processScheduleQueue[], struct ProcessStruct processArr[], int numOfProcesses, int * queueSize, int time) { 
  printf("%d: processes:", time);
  int oldest = time;
  for (int youngest =0; youngest <= oldest; youngest++) {
    for (int j = 0; j < numOfProcesses; j++) {
      for (int i = 0; i < * queueSize; i++) {
        if (processScheduleQueue[i].ProcessID == processArr[j].ProcessID && processScheduleQueue[i].age == youngest) {
          printf(" %d (%d ms)", processScheduleQueue[i].ProcessID, processScheduleQueue[i].timeRemaining);
        }
      }
    }
  }
}


//Using Insertion Sort to sort the queue based on the process deadlines while maintaining the history
//If two processes have the same deadline the oldest one goes first
void insertionSort(struct ProcessStruct processArr[], int numOfProcesses) {
    int i, j;
    struct ProcessStruct key;
    for (i = 1; i < numOfProcesses; i++) {
        key = processArr[i];
        j = i - 1;
        while (j >= 0 && processArr[j].deadline > key.deadline) {
            processArr[j+1] = processArr[j];
            j = j - 1;
        }
        if (j >= 0 && processArr[j].deadline == key.deadline) {
            while (j >= 0 && processArr[j].deadline == key.deadline && processArr[j].age > key.age) {
                processArr[j+1] = processArr[j];
                j = j - 1;
            }
            if (j >= 0 && processArr[j].deadline == key.deadline && processArr[j].age == key.age && processArr[j].ProcessID > key.ProcessID) {
                processArr[j+1] = processArr[j];
                j = j - 1;
            }
        }
        processArr[j+1] = key;
    }
}

//Removes the first element from the queue. This is because the first element is the one with the shortest deadline.
void popFirstProcessFromProcessQueue(struct ProcessStruct processScheduleQueue[], int* queueSize) {
    for (int i = 1; i < *queueSize; i++) {
        processScheduleQueue[i - 1] = processScheduleQueue[i]; 
    }
    *queueSize = *queueSize - 1; 
}


//Checks to see whether or not two processes are identical
int sameProcess(struct ProcessStruct * one, struct ProcessStruct * two) {
  return (
    (one->ProcessID == two->ProcessID) &&
    (one->cpuTime == two->cpuTime) &&
    (one->period == two->period) &&
    (one->deadline == two->deadline) &&
    (one->timeRemaining == two->timeRemaining) &&
    (one->pre_emptValue == two->pre_emptValue) &&
    (one->age == two->age)
  ) ? 1 : 0;
}

//Prints the start message for a given time and process
void getProcessStart(int time, int ProcessID){
    printf("%d: process %d starts\n", time, ProcessID);
}

//Prints the end message for a given time and process
void getProcessEnd(int time, int ProcessID){
    printf("%d: process: %d ends\n", time, ProcessID);
}


//Invokes preemption for processes 
int invoke_preempt(struct ProcessStruct processArr[], struct ProcessStruct processScheduleQueue[], struct ProcessStruct * currentProc, struct ProcessStruct * previousRun, int prevSize, int numOfProcesses, int * queueSize, int time, int * sumOfWaitingTimes){
  if ( * queueSize > 0 && (prevSize != * queueSize || currentProc -> ProcessID == 0)) {
    if (currentProc -> ProcessID != 0) { 
      getCurrentStateOfProcessQueue(processScheduleQueue, processArr, numOfProcesses, queueSize, time);
      printf("\n");
      insertionSort(processScheduleQueue, * queueSize);
      * currentProc = processScheduleQueue[0];
      if (!sameProcess(currentProc, previousRun)) {
        printf("%d: process %d preempted!\n", time, previousRun -> ProcessID);
        getProcessStart(time, currentProc->ProcessID);

        for (int i = 0; i < * queueSize; i++) {
            if (sameProcess( &processScheduleQueue[i], previousRun)) {
            processScheduleQueue[i].pre_emptValue = time;
            }
        }

        if (processScheduleQueue[0].pre_emptValue != 0) {
          * sumOfWaitingTimes = * sumOfWaitingTimes + time - processScheduleQueue[0].pre_emptValue;
        } else {
          * sumOfWaitingTimes = * sumOfWaitingTimes + time - processScheduleQueue[0].age;
        }
      }
    } else {
      insertionSort(processScheduleQueue, * queueSize);
      if (prevSize != * queueSize) { 
        getCurrentStateOfProcessQueue(processScheduleQueue, processArr, numOfProcesses, queueSize, time);
        printf("\n");
      }
      * currentProc = processScheduleQueue[0];
        getProcessStart(time, currentProc->ProcessID);
      if (processScheduleQueue[0].pre_emptValue != 0) {
        * sumOfWaitingTimes = * sumOfWaitingTimes + time - processScheduleQueue[0].pre_emptValue;
      } else {
        * sumOfWaitingTimes = * sumOfWaitingTimes + time - processScheduleQueue[0].age;
      }
    }
  } 
  
  return 0;
}


//Checks whether or not a time limit has been meet - has deadline passed?
int timeLimitMet(int time, int limit){
  return time == limit;
}

//Increases the size of the scheduler queue array dynamically if need.
int doubleQueueSchedulerSize(int *queueSize, int *maxSize, struct ProcessStruct processScheduleQueue[]){
  if (*queueSize == *maxSize){
    *maxSize = *maxSize * 2;
    struct ProcessStruct *bigScheduleQueue = (struct ProcessStruct *) realloc(processScheduleQueue, *maxSize * sizeof(struct ProcessStruct));
    if (!bigScheduleQueue){
      return -1;
    }
    processScheduleQueue = bigScheduleQueue; 
  }
  return 0;
}

//Pushes a process onto the process scheduler queue
int addProcessToSchedulerQueue(struct ProcessStruct processScheduleQueue[], struct ProcessStruct processArr[], int *queueSize, int time, int * totalProcsCreated, int i){
      processScheduleQueue[*queueSize] = processArr[i];
      processScheduleQueue[*queueSize].deadline = time + processArr[i].period;
      processScheduleQueue[*queueSize].age = time;
      *queueSize = *queueSize + 1;
      *totalProcsCreated = *totalProcsCreated + 1;
      return 0;
}


//The Main algorithm that handles the scheduling at any specific time -> does all of the checking and calls the helper functions
int runEDFScheduler(struct ProcessStruct processArr[], struct ProcessStruct processScheduleQueue[], struct ProcessStruct * currentProc, int numOfProcesses, int * queueSize, int time, int * totalProcsCreated, int * sumOfWaitingTimes, int * actualSizeMalloced, int maxTime) { //bulk of the functionality
  if (time !=0) {
    if (currentProc -> ProcessID != 0 && currentProc -> timeRemaining == 0) { //check if a process finished
      printf("%d: process %d ends\n", time, currentProc -> ProcessID);
    if (timeLimitMet(time, maxTime)) {
        return 0;
      }
      popFirstProcessFromProcessQueue(processScheduleQueue, queueSize);
      currentProc -> ProcessID = 0;
    }
    if (timeLimitMet(time, maxTime)) {
      return 0;
    }

  int prevSize = * queueSize;
  struct ProcessStruct hold = * currentProc;
  struct ProcessStruct * previousRun = &hold;
  for (int i = 0; i < numOfProcesses; i++) {
    if (time % processArr[i].period == 0) {
        for (int j = 0; j < *queueSize; j++) {
            if (processScheduleQueue[j].ProcessID == processArr[i].ProcessID && time == processScheduleQueue[j].deadline) {
                printf("%d: process %d missed deadline (%d ms left)\n",
                       time, processScheduleQueue[j].ProcessID,
                       processScheduleQueue[j].timeRemaining);
                processScheduleQueue[j].deadline = time + processScheduleQueue[j].period;
                if (j == 0) {
                    previousRun->deadline = time + processScheduleQueue[j].period;
                }
            }
        }

        doubleQueueSchedulerSize(queueSize, actualSizeMalloced, processScheduleQueue);
        addProcessToSchedulerQueue(processScheduleQueue, processArr, queueSize, time, totalProcsCreated, i);
        }
    }
    invoke_preempt(processArr, processScheduleQueue, currentProc, previousRun, prevSize, numOfProcesses, queueSize, time, sumOfWaitingTimes);

    
    if (*queueSize != 0) { 
      currentProc -> timeRemaining = currentProc -> timeRemaining - 1; 
      processScheduleQueue[0].timeRemaining = processScheduleQueue[0].timeRemaining - 1;
    }
  }
  return 0;
}


//Defining gcd and lcm functions -> gcd will be called in lcm:
int gcd(int a, int b) //Greatest Common Divisor
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

//LCM function using recursion -> this is used when finding the maxTime
int lcm(int a, int b) //Least Common Multiple
{
    return (a/gcd(a,b))*b;
}




//Main Process
int main() {



  ///////////////////////////////////
  ////// Setting Everything Up //////
  ///////////////////////////////////

  int numOfProcesses = 0;
  long maxTime = 0; 
  int minIncrement = INT_MAX; //Either gonna increment by 1 or a value less then 1 if a cpu time is less t1hen  


  printf("Enter the number of processes to schedule: ");
  scanf("%d", & numOfProcesses);
  struct ProcessStruct *processArr;
  if (!(processArr = malloc(numOfProcesses * sizeof(struct ProcessStruct)))) {
      fprintf(stderr, "Failed to allocate memory for processArr\n");
      exit(EXIT_FAILURE);
  }


  for (int i = 0; i < numOfProcesses; i++) { 
    printf("Enter the CPU time of process %d: ", i + 1);
    scanf("%d", & processArr[i].cpuTime);
    printf("Enter the period of process %d: ", i + 1);
    scanf("%d", & processArr[i].period);
    processArr[i].ProcessID = i + 1;
    processArr[i].deadline = processArr[i].period;
    processArr[i].age = 0;
    processArr[i].pre_emptValue = 0;
    processArr[i].timeRemaining = processArr[i].cpuTime;

    if (processArr[i].period < minIncrement) {
          minIncrement = processArr[i].period;
    }
  }

  // insertionSort(processArr, numOfProcesses); //Sorting the process array in ascending order based on deadlines. This is the initial order of the process array
  if (minIncrement > 1){ //Setting the min amount to increment by and check if a new smallest deadline exists 
      minIncrement = 1;
  }

  int queueSize = numOfProcesses;
  int actualSizeMalloced = queueSize; 

  struct ProcessStruct *processScheduleQueue;
  if (!(processScheduleQueue = malloc(queueSize * sizeof(struct ProcessStruct)))) {
      fprintf(stderr, "Error: Could not allocate memory for processScheduleQueue.\n");
      exit(EXIT_FAILURE);
  }


  for (int i = 0; i < numOfProcesses; i++) { 
    processScheduleQueue[i] = processArr[i];
  }

    ///////////////////////////////////
    ///// Invoking EDF Scheduler /////
    ///////////////////////////////////


  struct ProcessStruct currentProc = processScheduleQueue[0]; 
  int totalProcsCreated = numOfProcesses;
  int sumOfWaitingTimes = 0;


    maxTime = lcm(processArr[0].period, processArr[0].period);
    for (int i = 1; i < numOfProcesses; i++) {
        maxTime = lcm(maxTime, processArr[i].period);
    }
    int time = 0; 
    getCurrentStateOfProcessQueue(processScheduleQueue, processArr, numOfProcesses, &queueSize, time);
    printf("\n");
    getProcessStart(time, currentProc.ProcessID);
    processScheduleQueue[0].timeRemaining = processScheduleQueue[0].timeRemaining - 1;
    currentProc.timeRemaining = currentProc.timeRemaining - 1;


    while (time <= maxTime) { 
        runEDFScheduler(processArr, processScheduleQueue, &currentProc, numOfProcesses, &queueSize, time, &totalProcsCreated, &sumOfWaitingTimes, &actualSizeMalloced, maxTime);
        if (minIncrement > 1){ 
            minIncrement = 1;
        }
        time++; // Increment current time by 1
    }


  for (int i = 1; i < queueSize; i++) { 
    if (processScheduleQueue[i].pre_emptValue == 0) {
      sumOfWaitingTimes += maxTime - processScheduleQueue[i].age;
    } else {
      sumOfWaitingTimes += maxTime - processScheduleQueue[i].pre_emptValue;
    }
  }

  printf("%ld: Max Time reached\n", maxTime);
  printf("Sum of all waiting times: %d\n", sumOfWaitingTimes);
  printf("Number of processes created: %d\n", totalProcsCreated);
  printf("Average Waiting Time: %0.2f\n", (double) sumOfWaitingTimes / totalProcsCreated);

  free(processScheduleQueue);
  free(processArr);
  return 0;
}