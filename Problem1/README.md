# Introduction：
In Linux, we can use ps to check the current running processes. Furthermore, we can use pstree to see the process tree intuitively. However, in Android, we can only use ps, but cannot use pstree.


# Implement:
This module implements a new system call for android, which takes two arguments and return the process tree information in a depth-first-search (DFS) order. It is assigned 287 as identifier.


# Usage:
The prototype for this system call is:

int ptree(struct prinfo *buf, int *nr);

The argument buf points to a buffer for the process data, and nr points to the size of this buffer (number of entries). The system call copies all entries of the process tree data to the buffer, and stores the number of entries actually copied in nr.

If pointer correlated with the variable in struct prinfo is null, the value in struct prinfo is set to 0. For example, the first_child_pid should be set to 0 if the process does not have a child.


# Details:
Linux maintains a list of all processes in a doubly linked list. Each entry in this list is a task_struct structure, which is defined in include/linux/sched.h. When traversing the process tree data structures, it is necessary to prevent the data structures from changing in order to ensure consistency.

For this purpose the kernel relies on a special lock, the tasklist_lock.
This lock is grabbed before the traversal begins, and only when the traversal is completed will the lock be released. While holding the lock, it is hazardous to execute operations that may result in a sleep, such as memory allocation, copying of data into and out from the kernel etc. 
The following code is used to grab and then release the lock:
	
read_lock(&tasklist_lock);
	
...
	
read_unlock(&tasklist_lock);

  
A revised android kernel is used in this project to support modules, which can be downloaded from http://www.cs.sjtu.edu.cn/~fwu/teaching/res/android- kernel.tar.gz

