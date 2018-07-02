#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t pid_parent = getpid();
	pid_t pid_child = fork();

	if(pid_child > 0)
	{
		//print parent pid
		printf("516021910717 Parent Pid = %d\n", pid_parent);
		//wait for child process
		wait(NULL);
	}
	else if(pid_child == 0)
	{
		printf("516021910717 Child pid =%d\n", getpid());
		printf("Execuate ptree in child process:\n");
		execl("./ptreeARM", "ptreeARM", NULL);// execuate ptree int child process
		_exit(0);
	}
	return 0;
}