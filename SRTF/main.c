#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3
typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("Enter PID, Arrival Time, and Burst Time for process %d: ", i + 1);
        scanf("%d %d %d", &P[i].iPID, &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = P[i].iFinish = P[i].iWaiting = P[i].iResponse = P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("\nPID\t\tArrival\t\tBurst\t\tStart\t\tFinish\t\tResponse\tWaiting\t\tTaT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", 
            P[i].iPID, P[i].iArrival, P[i].iBurst, 
            P[i].iStart, P[i].iFinish, P[i].iResponse, 
            P[i].iWaiting, P[i].iTaT);
    }
}

void exportGanttChart(int n, PCB P[]) {
    int start = (P[0].iArrival != 0);

    char * s = calloc(20, 1);
    printf("|");

    if (start)
        printf("%7s", "|");

    for (int i = 0; i < n; i++) {
        snprintf(s, 20, "P%d", P[i].iPID);
        printf("%4s%3s", s, "|");
        if (i < n - 1 && P[i].iFinish != P[i + 1].iStart)
            printf("%7s", "|");
    }

    printf("\n0");
    for (int i = 0; i < n; i++) {
        if (P[i].iStart != 0 && (i == 0 || P[i].iStart != P[i - 1].iFinish))
            printf("%7d", P[i].iStart);
        printf("%7d", P[i].iFinish);
    }
    puts("\n");
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < *n - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        int condition = 0;
        if (iCriteria == SORT_BY_ARRIVAL) 
            condition = (P[j].iArrival < pivot.iArrival);
        else if (iCriteria == SORT_BY_PID) 
            condition = (P[j].iPID < pivot.iPID);
        else if (iCriteria == SORT_BY_BURST) 
            condition = (P[j].iBurst < pivot.iBurst);
        else if (iCriteria == SORT_BY_START) 
            condition = (P[j].iStart < pivot.iStart);
        
        if (condition) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return i + 1;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    int totalWaiting = 0;
    for (int i = 0; i < n; i++) {
        totalWaiting += P[i].iWaiting;
    }
    printf("\nAverage Waiting Time: %.2f\n", (float)totalWaiting / n);
}

void calculateATaT(int n, PCB P[]) {
    int totalTaT = 0;
    for (int i = 0; i < n; i++) {
        totalTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", (float)totalTaT / n);
}

int main() {
    PCB Input[10];
    PCB ReadyQueue[40];
    PCB TerminatedArray[40];
    PCB RealArray[10];
    int iNumberOfProcess, iQuantumTime = 1, signal;
    memset(&ReadyQueue, 0, sizeof(PCB) * 40);
    memset(&TerminatedArray, 0, sizeof(PCB) * 40);
    memset(&RealArray, 0, sizeof(PCB) * 10);
    memset(&Input, 0, sizeof(PCB) * 10);

    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);
    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0, iReal = 0;
    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);

    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
    int currentTime = ReadyQueue[0].iArrival;

    while (iRemain > 0 || iReady > 0) {
        signal = 0;
        printProcess(iReady, ReadyQueue);
        if (iReady == 0) currentTime += iQuantumTime;
        if (ReadyQueue[0].iBurst > iQuantumTime && iReady > 0) {
            currentTime += iQuantumTime;
            ReadyQueue[0].iBurst -= iQuantumTime;
            ReadyQueue[0].iFinish += iQuantumTime;
            ReadyQueue[0].iWaiting -= iQuantumTime;

            PCB temp = ReadyQueue[0];
            temp.iStart = currentTime - iQuantumTime;
            temp.iFinish = currentTime;
            temp.iBurst = iQuantumTime;
            if (iTerminated == 0 || TerminatedArray[iTerminated - 1].iPID != temp.iPID) pushProcess(&iTerminated, TerminatedArray, temp);
            else TerminatedArray[iTerminated - 1].iFinish = temp.iFinish;
            signal = 1;
        } else if (iReady > 0) {
            ReadyQueue[0].iStart = currentTime;
            currentTime += ReadyQueue[0].iBurst;
            ReadyQueue[0].iFinish = currentTime;
            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
            ReadyQueue[0].iWaiting -= ReadyQueue[0].iBurst;
            ReadyQueue[0].iWaiting += ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

            if (iTerminated == 0 || TerminatedArray[iTerminated - 1].iPID != ReadyQueue[0].iPID) pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            else TerminatedArray[iTerminated - 1].iFinish = ReadyQueue[0].iFinish;
            pushProcess(&iReal, RealArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
            signal = 2;
        }

        for (int i = 0; i < iRemain; i++) {
            if (Input[i].iArrival <= currentTime) {
                if (iReady == 0 && signal == 0) currentTime = Input[i].iArrival;
                pushProcess(&iReady, ReadyQueue, Input[i]);
                removeProcess(&iRemain, i, Input);
                i--;
            }
        }

        if (signal == 1) {
            pushProcess(&iReady, ReadyQueue, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
        }

        quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);

    }

    printf("\n===== SRTF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    

    quickSort(RealArray, 0, iReal - 1, SORT_BY_PID);
    printProcess(iReal, RealArray);

    calculateAWT(iReal, RealArray);
    calculateATaT(iReal, RealArray);
    return 0;
}