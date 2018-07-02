# Introduction：
This program invokes system call ptree, whose identifier is 287, and prints the whole process tree in DFS order using tabs to indent children with respect to their parents. 
  
The output format of every process is:

printf(/* correct number of \t */);
	
printf("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state, p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);


# Steps:
The program first allocates memory for buffer and nr. 
  
Then it calls ptree to store process data in buffer and nr. 
  
Finally it displays the information stored in buffer.

The running result is shown in /Ptree/testscript.txt
