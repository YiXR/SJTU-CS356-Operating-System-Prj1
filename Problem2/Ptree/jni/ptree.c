#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#define MAXBUFFERSIZE 2048
#define MAXNUMBER 256
struct prinfo
{
	pid_t parent_pid;
	pid_t pid;
	pid_t first_child_pid;
	pid_t next_sibling_pid;
	long state;
	long uid;
	char comm[16];
};

void show(struct prinfo* buf, int* nr)
{
	int array[MAXNUMBER];// To determine the numbe of tabs
	int tab[MAXNUMBER];// To save the number of tabs
	memset(tab, 0, MAXNUMBER * sizeof(int));
	memset(array, 0, MAXNUMBER * sizeof(int));

	int  TAB = 0;
	int i, j;
	for(i = 0; i < *nr; ++i)
	{
		if(i == *nr-1 || (buf[i].next_sibling_pid != buf[i+1].pid && buf[i].first_child_pid != buf[i+1].pid))// To check if some next_sibling_pid refer to the root parent
			buf[i].next_sibling_pid = 0;
		if(TAB == 0 || buf[i].pid == buf[array[TAB-1]].first_child_pid)
			array[TAB++] = i;
		else 
		{
			while(buf[i].pid != buf[array[TAB-1]].next_sibling_pid)
			{
				--TAB;
				buf[array[TAB]].next_sibling_pid = 0;
			}
			array[TAB-1] = i;
		}	
		tab[i] = TAB-1;
	}

	//print
	for(i = 0; i < *nr; ++i)
	{
		for(j = 0; j < tab[i]; ++j)
			printf("\t");
		printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[i].comm, buf[i].pid, buf[i].state, buf[i].parent_pid, buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);
	}
}

int main(int argc, char *argv[])
{
	struct prinfo *buf = (struct prinfo*)malloc(sizeof(struct prinfo) *MAXBUFFERSIZE);
	int nr;
	syscall(287, buf, &nr);// syscall ptree
	show(buf, &nr);// show the ptree
	free(buf);  //release buf
	return 0;
}
