/***********************************************************************
     CSCE 3600:     System Programming                                **
     ASSIGNMENT:    Group Project                                     **
     DESCRIPTION:   Project 1- Scheduling                             **
                    This Program simulates different scheduling       **
					disciplines that allocate a set of processes to   **
					available processors                              **
     DEPENDENCIES:  NONE                                              **
     AUTHOR:       Srizan Gangol                                      **
     DATE:          07/04/2017                                        **					

 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

// DEFINE ----------------------------------------------------------------------------------//
#define MAX_CYCLES 11000
#define MIN_CYCLES 1000
#define AVG_CYCLES 6000
#define MAX_MEM_KB 100
#define MIN_MEM_KB 1
#define AVG_MEM_KB 20
#define SIZE_KB 1000
#define MAX_MEM_B (MAX_MEM_KB * SIZE_KB)
#define MIN_MEM_B (MIN_MEM_KB * SIZE_KB)
#define AVG_MEM_B (AVG_MEM_KB * SIZE_KB)
#define MEM_MODE 0
#define MAX_TIME 100

// -----------------------------------------------------------------------------------------//
typedef struct pStruct pStruct;

	pStruct *new_pStruct (int pid, int numCycles, int memSize);

	void delete_Process (pStruct *p);		// Delete pStruct
	int pStruct_getPID (pStruct *p);		// Returns PID
	int pStruct_getNumCycles (pStruct *p);	// Returns the number of cycles
	int pStruct_getMemSize (pStruct *p);	// Returns the memory size
	void pStruct_print (pStruct *p);		// Prints the values
	void print_PStruct (pStruct *p);		// Prints the value
	void print_CSV (pStruct *p);		// Prints the value in CSV 

typedef struct ProcessTable ProcessTable;	// Struct for table for calculation

	ProcessTable *new_ProcessTable (int numProcesses);

	void delete_ProcessTable (ProcessTable *pt);						// Delete the Table
	int processTable_removeProcessByPID (ProcessTable *pt, int pid);	// Remove a pid's process
	int processTable_generateNewProcess (ProcessTable *pt);				// Adds a new process 

	//-- Print Functions ---------------------------------------------------------------------//
	void processTable_print (ProcessTable *pt);
	void print_Calc (ProcessTable *pt);
	void processTable_printProcesses (ProcessTable *pt);
	void print_Tupel (ProcessTable *pt);




// Struct for Single Process ----------------------------------------------------------------//
struct pStruct {
   int pid; 			// Process id
   int numCycles; 		// # of CPU cycles
   int memSize; 		// memory Size
};


// Node for Process -------------------------------------------------------------------------//
typedef struct ProcessNode ProcessNode;
	struct ProcessNode {
	   pStruct *process;
	   ProcessNode *next;
	};

	// NEWLY added code
// Struct for processes --------------------------------------------------------------------------//
typedef struct ProcessQueue ProcessQueue;
		struct ProcessQueue {
			int arrivalTime;
			int burstTime;
            int copyburstTime;

			int waitTime;
			int penaltyTime;
			ProcessNode *process;
			ProcessQueue *next;
		};

	ProcessQueue * new_ProcessQueue (ProcessTable *pt);
	int scheduler_roundRobin(ProcessQueue *pq);
	int scheduler_FIFO(ProcessQueue *pq, int processorType );
	int scheduler_SJF(ProcessQueue *pq, int processorType);
	void delete_ProcessQueue(ProcessQueue * pq);

//--Print Function for RoundRobin Scheduling --------------------------------------------------------//
    void print_mainHeader();
    void print_beowHeader();
	void print_scheduleTupel(ProcessTable *pt, ProcessQueue *RR, ProcessQueue *FIFO, ProcessQueue * SJF);
	void print_scheduleCalc(ProcessTable *pt, ProcessQueue *RR, ProcessQueue *FIFO, ProcessQueue * SJF, const int NP, const int PT);


// Process & Calculation --------------------------------------------------------------------//
struct ProcessTable {
   int numProcesses; 	// # of nodes
   int totalCycles; 	// AVG. cycles
   int totalMemory; 	// AVG. memory
   ProcessNode *head; 	// HEAD Pointer
   ProcessNode *tail; 	// Tail Pointer
};

// Assigning Tupel to the Process ----------------------------------------------------------//
pStruct *new_pStruct (int pid, int numCycles, int memSize) {
   pStruct *self = (pStruct *) calloc (1, sizeof(pStruct));
   self->pid = pid;
   self->numCycles = numCycles;
   self->memSize = memSize;  
   return self;
}

// Delete the Struct
void delete_pStruct (pStruct *p) {
   if (p) free(p);
   return;
}

// Return PID
int pStruct_getPID (pStruct *p) {
   if (!p) return -1;
   return p->pid;
}

// Returns # of Cycles
int pStruct_getNumCycles (pStruct *p) {  
   if (!p) return 0;
   return p->numCycles;
}

// Returns MemSize
int pStruct_getMemSize (pStruct *p) {
   if (!p) return 0;
   return p->memSize;
}


// Print out Tupel
void print_PStruct (pStruct *p) {
	printf("\t%d \t\t%d \t\t %d\n", p->pid, p->numCycles, p->memSize);
	//printf("%d,%d,%d\n", p->pid, p->numCycles, p->memSize);
}

// Print out For CSV
void print_CSV (pStruct *p) {
   printf("\n%d,%d,%d\n", p->pid, p->numCycles, p->memSize);
}

// Random Generation for the Cycle with mean 6000
int generateNumCycles () {  
   return (rand() % (MAX_CYCLES - MIN_CYCLES)) + MIN_CYCLES;
}


// Process Table
ProcessTable *new_ProcessTable (int numProcesses) {
   ProcessTable *self = (ProcessTable *) calloc (1, sizeof(ProcessTable));

   srand(time(NULL));
  
   int i;
   for (i=0; i<numProcesses; i++) {
       if (processTable_generateNewProcess(self)) {
           self->numProcesses++;
       }
       else {
           printf("Error creating process %d\n", i);
       }
   }
  
   return self;
}

// Delete the Process Table
void delete_ProcessTable (ProcessTable *pt) {
   if (pt) free(pt);
   return;
}
