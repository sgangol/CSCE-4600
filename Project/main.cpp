/***********************************************************************
     CSCE 3600:     System Programming                                **
     ASSIGNMENT:    Group Project                                     **
     DESCRIPTION:   Project 1- Scheduling                             **
                    This Program simulates different scheduling       **
					disciplines that allocate a set of processes to   **
					available processors                              **
     DEPENDENCIES:  define.h                                          **
     AUTHOR:       Srizan Gangol                                      **
     DATE:          07/04/2017                                        **					

 ***********************************************************************/

#include "define.h"


int main (int argc, char **argv) {

	int inNoP;
	int numOfProcessor;

	if((argc < 3)){
	printf("USUAGE: \t ./process <# of process> <# of processor>\n");
	printf("EG: \t\t ./process 50 4\n");
	return 0;
	}
	else {
		inNoP=atoi(argv[1]);
	}

	if( argc > 2){
		numOfProcessor =  atoi(argv[2]);
	}
	else {
		numOfProcessor = 1;
	}

		ProcessTable *pt1 = new_ProcessTable(inNoP);			// Executes Process Table
		ProcessQueue * RRpq = new_ProcessQueue(pt1);
		ProcessQueue * FIFOpq = new_ProcessQueue(pt1);
		ProcessQueue * SJFpq = new_ProcessQueue(pt1);

		//RR
		if( scheduler_roundRobin(RRpq) == -1 ){
			fprintf(stderr,"Error in starting the RR scheduler.\n");
		}



		//FIFO
		if( scheduler_FIFO(FIFOpq, numOfProcessor) == -1 ){
			fprintf(stderr,"Error in starting FIFO scheduler.\n");
		}

		//SJF
		if( scheduler_SJF(SJFpq, numOfProcessor) == -1 ){
			fprintf(stderr, "Error in starting SJF scheduler.\n");
		}

		//print tuple
		print_scheduleTupel(pt1,RRpq,FIFOpq, SJFpq);
		print_scheduleCalc(pt1, RRpq, FIFOpq, SJFpq, inNoP, numOfProcessor);

	   delete_ProcessTable(pt1);	   							// Delete process table

	   //deleting 3 queues as we allocated the 3 queues
	   delete_ProcessQueue(RRpq);
	   delete_ProcessQueue(FIFOpq);
	   delete_ProcessQueue(SJFpq);


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

// -- NEWLY ADDED CODE ---/
// Process Queue generation
ProcessQueue * new_ProcessQueue (ProcessTable *pt) {

	if(!pt)
		return NULL;

	int nextArrivalTime = 0;

	ProcessQueue * tempNode = NULL;
	ProcessQueue * currNode = NULL;
	ProcessNode * pn = NULL;

	for (pn = pt->head; pn != NULL; nextArrivalTime += 50, pn =pn->next){

		// create new node for ProcessQueue
		ProcessQueue * newNode = (ProcessQueue *) calloc(1, sizeof(ProcessQueue));
		if(!newNode) return NULL;

		newNode->arrivalTime    = nextArrivalTime;
		newNode->burstTime      = (rand() % (MAX_TIME+1));
		newNode->copyburstTime  = newNode->burstTime;
		newNode->process        = pn;
		newNode->waitTime       = 0;
		newNode->penaltyTime    = 0;

		if(tempNode == NULL ){
			tempNode = newNode;
		}
		else {
			currNode->next = newNode;
			//tempNode = newNode;
		}
		currNode = newNode;
	}

 return tempNode;
}

// Round Robin algorithm
int scheduler_roundRobin (ProcessQueue *pq){

	if(!pq)
		return -1;

	ProcessQueue * p = NULL;
	const int switchTime = 10;			//scheduler scheduling time
	int totalTime =0;
	int flag = 0;

	// calculating the waiting time
	for (;;){
		flag =1;

		for(p = pq;p != NULL ;p = p->next){

			 if (p->copyburstTime > 0){

				flag =0;
				if (p->copyburstTime > switchTime ){

					//time the process has been processed, increase totalTime
					totalTime += switchTime;

					//decrease the quantumTime with switchTime
					p->copyburstTime -= switchTime;
				}
				else {

					//Increase value to totalTime to see how much
					//process has taken
					totalTime = totalTime + p->copyburstTime;

					//current waiting time
					p->waitTime = totalTime - p->burstTime;

					//make remaining time 0
					p->copyburstTime =0;

				}
			}
		}
		// if all processed are done
		if (flag == 1)
			break;
	}

	// Turn around time(total penalty)
	for( p = pq; p != NULL; p = p->next){
		p->penaltyTime = p->burstTime + p->waitTime;
	}

	return 1;
}

int scheduler_FIFO(ProcessQueue * pq, int processorType){
	if(!pq) return -1;


	int count =0;

	// finding the waiting time and turn around time for uni processor and return
	if( processorType == 1){

		ProcessQueue * curr = NULL;
		ProcessQueue * prev = pq;

		for(curr = pq; curr !=NULL; curr = curr->next){

			if (count != processorType){
				curr->waitTime =0;
				count++;
				continue;
			}
			else {
				curr->waitTime = prev->burstTime + prev->waitTime;
				prev = prev->next;
			}
		}
		// finding the turn around time
		for(curr = pq; curr != NULL; curr = curr->next){
			curr->penaltyTime = curr->burstTime + curr->waitTime;
		}

	}
	//if there are more that one processor
	else {

		ProcessQueue * curr = pq;

		// array to store the values
		int burstTimeArray[processorType];
		int waitTimeArray[processorType];
		int index =0;
		int flag  =0;
		    count =0;

		    for( ; ; ){

		    	for( index =0; index < processorType && curr != NULL; ++index ){

		    		flag = 0;
		    		if( count == 0){
		    			curr->waitTime =0;
		    			burstTimeArray[index] = curr->burstTime;
		    			waitTimeArray[index] = curr->waitTime;
		    			curr = curr->next;

		    			if( curr == NULL ){
		    				flag = 1;
		    				break;
		    			}
 		    		}
		    		else {
		    			curr->waitTime = burstTimeArray[index] + waitTimeArray[index];
		    			burstTimeArray[index] = curr->burstTime;
		    			waitTimeArray[index]  = curr->waitTime;
		    			curr = curr->next;

		    			if( curr == NULL){
		    				flag = 1;
		    				break;
		    			}
		    		}
		    	}
		    	count =1;

		    	if( flag == 1)
		    		break;
		    }

		    // finding the turn around time
		    for(curr = pq; curr != NULL; curr = curr->next){
		    	curr->penaltyTime = curr->burstTime + curr->waitTime;
		    }
	}
	return 1;
}// end

int scheduler_SJF(ProcessQueue * pq, int processorType){
	if(!pq) return -1;


	// sort the queue in increasing order acc. to burst time
	ProcessQueue * q = NULL;
	ProcessQueue * p = NULL; //sortBurstTime(q);

	// sorting
	for( q = pq; q != NULL; q = q->next){

		for( p = q->next; p !=NULL; p = p->next ){

			if(q->process->process->numCycles> p->process->process->numCycles){
				    int temp = q->arrivalTime;
				q->arrivalTime = p->arrivalTime;
				p->arrivalTime = temp;
			}
		}
	}

	// now apply FIFO
	 if (scheduler_FIFO(pq, processorType) ==-1){
		fprintf(stderr,"Error in running FIFO scheduler.\n");
	 }

	return 1;
}

void delete_ProcessQueue(ProcessQueue * pq){

	while(pq !=NULL){
		ProcessQueue * temp = pq;
		pq = pq->next;
		free(temp);
	}
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

void print_mainHeader(){

	int i;
	printf("\n -");

	for (i =0; i < 18; ++i) {
		printf("--------");
	}
	printf("\n | \t\t  PROCESS INFO \t\t\t | \t RR SCHEDULING \t\t | \t FIFO SCHEDULING \t | \t   SJF SCHEDULING\t |\n");

	printf(" -");
	for (i =0; i < 18; ++i){
		printf("--------");
	}
	printf("\n");

	printf(" |  PID    # of cycles    Size     Arrival_Time  |   Wait_Time     Total_Penalty |   Wait_Time     Total_Penalty |   Wait_Time     Total_Penalty |\n -");

	for (i =0; i < 18; ++i){
		printf("--------");
	}printf("\n");
}

void print_belowHeader(){

	int i =0;
	printf(" -");
	for(i = 0; i < 18; ++i ){
		printf("--------");
	}printf("\n");
}

void print_scheduleTupel(ProcessTable *pt, ProcessQueue *RR, ProcessQueue *FIFO, ProcessQueue * SJF){
	if(!pt || !RR || !FIFO || !SJF ) return;

	//print the main header
	print_mainHeader();


	ProcessNode  * p = pt->head;
	ProcessQueue * r = RR;
	ProcessQueue * f = FIFO;
	ProcessQueue * s = SJF;


	for (; p != NULL; p = p->next, r = r->next , f = f->next, s = s->next){
		printf(" | %3d       %6d \t%5d        %5d\t |   %6d   \t      %6d     |   %6d    \t"
				"      %6d     |    %6d   \t     %6d   \t | \n",
				p->process->pid, p->process->numCycles, p->process->memSize, r->arrivalTime,
				r->waitTime, r->penaltyTime, f->waitTime, f->penaltyTime , s->waitTime, s->penaltyTime);
	}

	//printing the below header line
	print_belowHeader();

}

// NP - total processes running
//PT  - Processor type ( uni, multi etc.)
void print_scheduleCalc(ProcessTable *pt, ProcessQueue *RR, ProcessQueue *FIFO, ProcessQueue * SJF, const int NP, const int PT){

	int totalProcessCycles =0;
	int totalProcessSizes  =0;
    int totalProcessArrivalTime   =0;

    int totalRRWaitTime    =0;
	int totalRRPenaltyTime =0;

	int totalFIFOWaitTime    =0;
	int totalFIFOPenaltyTime =0;

	int totalSJFWaitTime    =0;
	int totalSJFPenaltyTime =0;

	ProcessNode  *p =NULL;
    ProcessQueue *r =RR;
    ProcessQueue *f =FIFO;
    ProcessQueue *s =SJF;

    for( p=pt->head; p !=NULL; p = p->next , r =r->next, f =f->next, s =s->next){

    	totalProcessCycles += p->process->numCycles;
    	totalProcessSizes  += p->process->memSize;
        totalProcessArrivalTime   += r->arrivalTime;

    	totalRRWaitTime    += r->waitTime;
    	totalRRPenaltyTime += r->penaltyTime;

    	totalFIFOWaitTime    += f->waitTime;
    	totalFIFOPenaltyTime += f->penaltyTime;

    	totalSJFWaitTime    += s->waitTime;
    	totalSJFPenaltyTime += s->penaltyTime;
    }

    printf(" | Average:   %5d   \t   %2d  \t     %5d\t |    %5d    \t       %5d     |    %5d    \t       %5d     |     %5d   \t      %5d  \t |\n",
    		totalProcessCycles/NP, totalProcessSizes/NP,  totalProcessArrivalTime/NP, totalRRWaitTime/NP, totalRRPenaltyTime/NP,
			totalFIFOWaitTime/NP, totalFIFOPenaltyTime/NP, totalSJFWaitTime/NP, totalSJFPenaltyTime/NP );

    // print below header line
    print_belowHeader();

    if( PT > 1){
    printf( " | Average of %d core processor:  \t\t |    %5d    \t       %5d\t |    %5d    \t       %5d\t |     %5d   \t      %5d  \t |\n"
    		,PT, totalRRWaitTime/(NP*PT), totalRRPenaltyTime/(NP*PT), totalFIFOWaitTime/(NP*PT)
			,totalFIFOPenaltyTime/(NP*PT), totalSJFWaitTime/(NP*PT), totalSJFPenaltyTime/(NP*PT));

    //print below line
    print_belowHeader();
    }

    printf("\n");
}

