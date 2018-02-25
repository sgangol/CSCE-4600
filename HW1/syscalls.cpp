/*********************************************************************
     NAME:  Srizan Gangol         USERID:sg0440                     **
     ASSIGNMENT:  CSCE4600                                          **
     DESCRIPTION:  HW 1                                          	**
					This program demonstrate fork(), exit(), 		**
					getpid(), getppid(), exec(), wait(), kill()		**
                                                                    **
 ********************************************************************/

#include <stdio.h>		// Standard Input-Output
#include <stdlib.h>		// For exit()
#include <unistd.h>		// Standard symbolic constants and types
#include <signal.h>		// For Signal
#include <sys/types.h>	// For types
#include <sys/wait.h>	// For Wait

int  main()
{
    // For another process
    pid_t pid=fork();										// fork() 1/7
	
	// Fork Failed to execute #ERROR
	if (pid<0){ 
		fprintf(stderr, "Fork Failed\n");
		exit(-1);											// exit() 2/7
	}
	// Child Process Created	
	else if (pid == 0){	
		printf("Child's Process ID = %d\n",getpid());		//getpid() 3/7
		printf("Parent's Process ID = %d\n",getppid());		//getppid() 4/7
		execl("/bin/date", "date", 0, (char *)0);			// exec() 6/7
		perror("execl() failure!\n\n");
		_exit(1);
	}
	// Parent Process
	else{
		printf("syscalls.cpp\n\n");
	}
	// parent will wait for the child to complete
	wait(NULL);												//wait() 5/7
	
	
    kill(pid, SIGTERM);										//kill() 7/7
    
    return 0;
}