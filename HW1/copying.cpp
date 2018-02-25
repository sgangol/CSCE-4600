/***********************************************************************
     CSCE 4600:     Operating Systems                                 **
     ASSIGNMENT:    Assignment 1                                      **
     DESCRIPTION:   HW 1                                              **
                    This program is used to compare performance of    **
                    read()-write() function & fread()-fwrite()        **
					Buffered Vs Unbuffered                            **
     USUAGE:        $ time ./a.out <r/f>                              **
     AUTHORS:       Srizan Gangol                                     **
	 References:    opengroup.org                                     **
	                Lecture Slides                                    **
                                                                      **			
 ***********************************************************************/

/*


*/
#include <stdio.h> 							// input output and remove() & rename()
#include<fcntl.h>							// File attribute
#include<unistd.h>							// symbolic constants
#include <time.h>  							// clock_t, clock, CLOCKS_PER_SEC 
//-----------------
#include <string.h>
#include <stdlib.h>							// fread

int main( int argc, char *argv[] )
{
	// ----------------------------- Usuage ------------------------------//
	if(argc <2)
	{
		printf("Usage#1 read:\t$ ./copyfile r \n");
		printf("Usage#2 fread:\t$ ./copyfile f \n");
		exit(-1);
	}
	
	if(strcmp(argv[1], "r")==0)
	{
		int fd1, fd2;							// a file descriptor
		ssize_t nread;							// number of bytes read
		ssize_t nwrite;     					// number of bytes written
		char buf[1024];							// data buffer
		
		// ---------------------------- Time ON -----------------------------//
		clock_t start = clock();
		// ------------------------- Open the file --------------------------//
		fd1 = open("input.bin", O_RDONLY);		// O_RDONLY: Open for read only [opengroup.org]
		fd2 = open("output.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);	
													// O_WRONLY: Open for write only | O_RDWR: Read & Write [opengroup.org]
													// O_CREAT: Create file if it does not exist [opengroup.org]
													// O_TRUNC: Truncate flag [opengroup.org]
		
		// ------------------------ Error Handlers ---------------------------//
		if (fd1 < 0){
			printf("\nError opening the file\n");
		}
		if (fd2 < 0){
			printf("\nError creating the output.bin\n");
		}
		
		//---------------------- Performing read & Write -----------------------//
			int x= 1;
			
			while(nread > 0){
			nread = read(fd1,buf,x);					// Reading the data
			nwrite = write(fd2,buf,nread);				// Writing the data
			}//<------ while
		printf("\nData copied to output.bin\n");
		// ---------------------------- Time OFF -----------------------------//
		
		float elapsed = (clock()-start) * 1000 / CLOCKS_PER_SEC;
		//printf("Time Elapsed for read: %f miliseconds\n", elapsed);		
		
		// ------------------------ Closing the files ------------------------//
		close(fd1);								// Closing Inputfile.txt
		close(fd2);								// Closing Outputfile.txt

	}
	
	else if(strcmp(argv[1], "f")==0)
	{
		FILE * pFile;
		FILE * wFile;
		long lSize;
		char *buffer;
		size_t result;
		
		// ---------------------------- Time ON -----------------------------//
		clock_t start = clock();
		
		// ------------------------- Open a file ----------------------------//
		pFile = fopen ("input.bin" , "rb");
		if (pFile==NULL) {
			printf("\nError opening the file\n");
			exit (1);}
		
		// ----------------------- Obtain file size -------------------------//
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		
		// -------------------------Allocate memory--------------------------//

		buffer = (char*) malloc (sizeof(char)*lSize);
		if (buffer == NULL) {
			printf("\nMemory error\n");
			exit (2);}
		
		// --------------------Copy the file in the buffer -----------------//

		result = fread (buffer,1,lSize,pFile);
		if (result != lSize) {
			printf("\nReading error\n");
			exit (3);}  
		// ---------------------- Write into the file  ---------------------//
		wFile = fopen ("output.bin", "wb");
		fwrite (buffer, sizeof(char),lSize,wFile);
		printf("\nData copied to output.bin\n");
		
		// ---------------------------- Time OFF ---------------------------//
		
		float elapsed = (clock()-start) * 1000 / CLOCKS_PER_SEC;
		//printf("Time Elapsed for read: %f miliseconds\n", elapsed);	
		
		// --------------------------- Terminate ---------------------------//			
		fclose (pFile);
		fclose (wFile);
		free (buffer);
	
	}
		// ----------------------- Invalid Arguement ----------------------//	
	
	else{
		printf("Only 'f' & 'r' are valid arguements\nPlease try again\n");
		printf("Usage#1 read: \t$ ./copyfile r \n");
		printf("Usage#2 fread: \t$ ./copyfile f \n");
		return -1;
	}
 
	
	return 0;
}