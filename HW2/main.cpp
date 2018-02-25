/***********************************************************************
     CSCE 4600:     Operating Systems                                 **
     ASSIGNMENT:    Assignment 2                                      **
     DESCRIPTION:   HW 2                                              **
     USUAGE:        $ ./process <# of process>                        **
     AUTHORS:       Srizan Gangol                                     **
                                                                      **			
 ***********************************************************************/

#include "define.h"

int main (int argc, char **argv) {
	if((argc < 2)||(argc > 2)){
		printf("USUAGE: \t ./process <# of process>\n");
		printf("EG: \t\t ./process 50 \n");
	}
	else{
		int inNoP=atoi(argv[1]);	
		ProcessTable *pt1 = new_ProcessTable(inNoP);			// Executes Process Table
		
		// Printing out the 3 Tupels
		printf("\n    Process ID \t     # of Cycles \t Mem Size \t\n");
		printf(" --------------------------------------------------\n");
		print_Tupel(pt1);
		//print_CSV(pt1);
		print_Calc(pt1);
		
		
	   delete_ProcessTable(pt1);	   							// Delete process table
	}
   return 0;
}

// Generate Memory Size
int generateMemSize () {
   int maxSize;
   int minSize;
   int avgMem;
  
   switch (MEM_MODE) {
       case 0:
           maxSize = MAX_MEM_KB;
           minSize = MIN_MEM_KB;
           avgMem = AVG_MEM_KB;
           break;
       case 1:
           maxSize = MAX_MEM_B;
           minSize = MIN_MEM_B;
           avgMem = AVG_MEM_B;
           break;
       default: return 0;
   }
  
   // Get Memory Size within the range with the Average of 20
   if ((rand() % (maxSize - minSize)) > (avgMem - minSize))
       return (rand() % (avgMem - minSize)) + minSize;
   else
       return (rand() % (maxSize - avgMem)) + avgMem;
}

// Process Generation
int processTable_generateNewProcess (ProcessTable *pt) {
   if (!pt) return -1;
  
   // Create a new node
   ProcessNode *newNode = (ProcessNode *) calloc (1, sizeof(ProcessNode));
   if (!newNode) return -1;
      
   // There is NOTHING in the process table
   if (pt->head == NULL && pt->tail == NULL) {
		// Create new process
		pStruct *newpStruct = new_pStruct (1, generateNumCycles(), generateMemSize());
		if (!newpStruct) return -1;

		// Set the values in the process node
		newNode->process = newpStruct;
		//newNode->prev = NULL;
		newNode->next = NULL;
			  
		// Add the new node to the process table
		pt->head = newNode;
		pt->tail = newNode;
		pt->totalCycles += pStruct_getNumCycles(newpStruct);
		pt->totalMemory += pStruct_getMemSize(newpStruct);

		// Return the new process id
		return pStruct_getPID(newpStruct);
   }
   // There is something in the process table
   else if (pt->tail) {
		// Create new process      
		pStruct *newpStruct = new_pStruct (pStruct_getPID(pt->tail->process)+1, generateNumCycles(), generateMemSize());
		if (!newpStruct) return -1;

		// Set the values in the process node
		newNode->process = newpStruct;
		//newNode->prev = pt->tail;
		newNode->next = NULL;

		// Add the new node to the process table
		pt->tail->next = newNode;
		pt->tail = newNode;
		pt->totalCycles += pStruct_getNumCycles(newpStruct);
		pt->totalMemory += pStruct_getMemSize(newpStruct);

		// Return the new process
		return pStruct_getPID(newpStruct);
   }
   else {
		// ERROR
		return -1;
   }
  
   return -1;
}


// Remove Process
int processTable_removeProcessByPID (ProcessTable *pt, int pid) {
   if (!pt) return 0;
  
   // If PID was head
   if (pStruct_getPID(pt->head->process) == pid) {
       pStruct *pStructToDelete = pt->head->process;      
       ProcessNode *nodeToDelete = pt->head;
      
       pt->head = nodeToDelete->next;
       if (pt->head == NULL) pt->tail = NULL; 
       pt->numProcesses -= 1;
       pt->totalCycles -= pStruct_getNumCycles(pStructToDelete);
       pt->totalMemory -= pStruct_getMemSize(pStructToDelete);
      
       delete_pStruct (pStructToDelete);
       free(nodeToDelete);
       pStructToDelete = NULL;
       nodeToDelete = NULL;
      
       // Success
       return 1;
   }
   // Else
   else {
       ProcessNode *p;
       ProcessNode *pNext;
       for (p=pt->head; p != NULL; p=p->next) {
           
           pNext = p->next;
           
           if (pStruct_getPID(pNext->process) == pid) {
               pStruct *pStructToDelete = pNext->process;              
               p->next = pNext->next; 
              
               // Update Table
               if (p->next == NULL) pt->tail = p; 	// Last Process            
               pt->numProcesses -= 1;
               pt->totalCycles -= pStruct_getNumCycles(pStructToDelete);
               pt->totalMemory -= pStruct_getMemSize(pStructToDelete);
              
               delete_pStruct (pStructToDelete); pStructToDelete = NULL;
               return 1;
           }  
           // The value exceeded PID
           else if (pStruct_getPID(p->process) > pid) {
               return 0;
           }
       }
   }
  
   return 0;
}

// ---- Print Functions -------------------------------------------------------------------------------//

void print_Calc (ProcessTable *pt) {
   if (!pt) return;
	printf(" --------------------------------------------------\n");
	printf("    [Average]\t\t%d\t\t%d\n",
		pt->totalCycles/pt->numProcesses,
		pt->totalMemory/pt->numProcesses);
	printf(" --------------------------------------------------\n");
}

void print_Tupel (ProcessTable *pt) {
   if (!pt) return;
   ProcessNode *p;
   for (p=pt->head; p != NULL; p=p->next) {
       print_PStruct(p->process);
   }
}
