#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct{
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[])
{
    for (int i = 0; i < n; i++)
    {
        printf("Process P%d\n", i + 1);
        P[i].iPID = i + 1;
        printf("Arrival time: ");
        scanf("%d", &P[i].iArrival);
        printf("Burst time: ");
        scanf("%d", &P[i].iBurst);
//        P[i].iPID = i + 1;
//        P[i].iArrival = rand() % 21;
//        P[i].iBurst = (rand() % 11) + 2;
    }
    // Print all the current processes
    printf("Current processes:\n");
    printf("PID\tArrival\tBurst\n");
    for (int i = 0; i < n; i++)
    {
        printf("P%d\t%d\t%d\n", P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
}

void printProcess(int n, PCB P[])
{
    for (int i = 0; i < n; i++)
    {
        printf("Tien trinh %d:\n", P[i].iPID);
        printf("Arrival time: %d, Burst time: %d\n", P[i].iArrival, P[i].iBurst);
        printf("Start time: %d, Finish time: %d\n", P[i].iStart, P[i].iFinish);
        printf("Waiting time: %d, Response time: %d, Turn around time: %d\n", P[i].iWaiting, P[i].iResponse, P[i].iTaT);
    }
}

void exportGanttChart (int n, PCB P[])
{
    // top bar
    printf(" ");
    for (int i = 0; i < n; i++)
    {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish)
        {
            for (int j = 0; j < (P[i].iStart - P[i - 1].iFinish); j++) printf("--");
            printf(" ");
        }
        for (int j = 0; j < P[i].iBurst; j++) printf("--");
        printf(" ");
    }
    printf("\n|");
    // Process ID and stuffs
    for (int i = 0; i < n; i++)
    {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish)
        {
            for (int j = 0; j < (P[i].iStart - P[i - 1].iFinish); j++) printf("  ");
            printf("|");
        }
        for (int j = 0; j < P[i].iBurst - 1; j++) printf(" ");
        printf("P%d", P[i].iPID);
        for (int j = 0; j < P[i].iBurst - 1; j++) printf(" ");
        printf("|");
    }
    printf("\n ");
    // bottom bar
    for (int i = 0; i < n; i++)
    {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish)
        {
            for (int j = 0; j < (P[i].iStart - P[i - 1].iFinish); j++) printf("--");
            printf(" ");
        }
        for (int j = 0; j < P[i].iBurst; j++) printf("--");
        printf(" ");
    }
    printf("\n");
    // Timeline
    printf("%d", P[0].iStart);
    for (int i = 0; i < n; i++)
    {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish)
        {
            for (int j = 0; j < (P[i].iStart - P[i - 1].iFinish); j++) printf("  ");
            if (P[i].iStart > 10) printf("\b%d", P[i].iStart);
            else printf("%d", P[i].iStart);
        }
        for (int j = 0; j < P[i].iBurst; j++) printf("  ");
        if (P[i].iFinish > 10) printf("\b%d", P[i].iFinish);
        else printf("%d", P[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q)
{
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[])
{
    for (int i = index; i < *n - 1; i++)
    {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q)
{
    PCB temp;
    temp = *P;
    *P = *Q;
    *Q = temp;
    return 0;
}

int partition (PCB P[], int low, int high, int iCriteria)
{
    int pivot;
    if (iCriteria == SORT_BY_ARRIVAL) pivot = P[high].iArrival;
    else if (iCriteria == SORT_BY_PID) pivot = P[high].iPID;
    else pivot = P[high].iBurst;

    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (iCriteria == SORT_BY_ARRIVAL)
        {
            if (P[j].iArrival < pivot)
            {
                i++;
                swapProcess(&P[i], &P[j]);
            }
            else if (P[j].iArrival == pivot)
            {
                // swap by burst time
                if (P[j].iBurst < pivot)
                {
                    i++;
                    swapProcess(&P[i], &P[j]);
                }
            }
        }
        else if (iCriteria == SORT_BY_PID)
        {
            if (P[j].iPID < pivot)
            {
                i++;
                swapProcess(&P[i], &P[j]);
            }
        }
        else if (iCriteria == SORT_BY_BURST)
        {
            if (P[j].iBurst < pivot)
            {
                i++;
                swapProcess(&P[i], &P[j]);
            }
        }
        else if (iCriteria == SORT_BY_START)
        {
            if (P[j].iStart < pivot)
            {
                i++;
                swapProcess(&P[i], &P[j]);
            }
        }
    }
    i++;
    swapProcess(&P[i], &P[high]);
    return i;
}

void quickSort(PCB P[], int low, int high, int iCriteria)
{
    if (low >= high) return; // base case

    int pivot = partition(P, low, high, iCriteria);
    quickSort(P, low, pivot - 1, iCriteria);
    quickSort(P, pivot + 1, high, iCriteria);
}

void calculateAWT(int n, PCB P[])
{
    double AWT = 0;
    for (int i = 0; i < n; i++)
    {
        AWT += P[i].iWaiting;
    }
    AWT /= n;
    printf("AWT: %.2f\n", AWT);
}

void calculateATaT(int n, PCB P[])
{
    double ATaT = 0;
    for (int i = 0; i < n; i++)
    {
        ATaT += P[i].iTaT;
    }
    ATaT /= n;
    printf("ATaT: %.2f\n", ATaT);
}

int main()
{
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    memset(&ReadyQueue, 0, sizeof(PCB));
    int iNumberOfProcess, iQuantumTime;
    printf("Please input the quantum time: ");
    scanf("%d", &iQuantumTime);
    printf("Please input the number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);
    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    printf("\nReady Queue: ");
    printProcess(1, ReadyQueue);

    int currentTime = ReadyQueue[0].iArrival;

    // Round Robin
    while (iRemain > 0 || iReady > 0)
    {
        if (iReady == 0) currentTime += iQuantumTime;
        if (ReadyQueue[0].iBurst > iQuantumTime && iReady > 0)
        {
            currentTime += iQuantumTime;
            ReadyQueue[0].iBurst -= iQuantumTime;
            ReadyQueue[0].iFinish += iQuantumTime;
            for (int i = 1; i < iReady; i++)
            {
                ReadyQueue[i].iWaiting += iQuantumTime;
            }
            // Create a temp process to display in Gantt chart
            PCB temp = ReadyQueue[0];
            temp.iStart = currentTime - iQuantumTime;
            temp.iFinish = currentTime;
            temp.iBurst = iQuantumTime;
            pushProcess(&iTerminated, TerminatedArray, temp);

            pushProcess(&iReady, ReadyQueue, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
        }
        else if (iReady > 0)
        {
            ReadyQueue[0].iStart = currentTime;
            currentTime += ReadyQueue[0].iBurst;
            ReadyQueue[0].iFinish = currentTime;
            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
            for (int i = 1; i < iReady; i++)
            {
                ReadyQueue[i].iWaiting += ReadyQueue[0].iBurst;
            }
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
        }
        // Check for new processes
        for (int i = 0; i < iRemain; i++)
        {
            if (Input[i].iArrival <= currentTime)
            {
                pushProcess(&iReady, ReadyQueue, Input[i]);
                removeProcess(&iRemain, i, Input);
                i--;
            }
        }
    }

    printf("\n===== RR Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
    return 0;
}