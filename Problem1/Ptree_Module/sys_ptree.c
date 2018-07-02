#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/list.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");
#define _NR_ptree 287
#define MAXBUFFERSIZE 2048

struct prinfo {
	pid_t parent_pid; /* process id of parent */
	pid_t pid; /* process id */
	pid_t first_child_pid; /* pid of youngest child */
	pid_t next_sibling_pid; /* pid of older sibling */
	long state; /* current state of process */
	long uid; /* user id of process owner */
	char comm[16]; /* name of program executed */
};


void transfer(struct task_struct* old, struct prinfo* new) //transfer task_struct into our new struct
{
	if(old->parent)
		new->parent_pid = old->parent->pid;
	else new->parent_pid = 0;

	new->pid = old->pid;

	if(list_empty(&(old->children)))
		new->first_child_pid = 0;
	else new->first_child_pid = list_entry(old->children.next, struct task_struct, sibling)->pid;
	
	if(list_empty(&(old->sibling)))
		new->next_sibling_pid = 0;
	else new->next_sibling_pid = list_entry(old->sibling.next, struct task_struct, sibling)->pid;
	
	new->state = old->state;
	
	new->uid = old->cred->uid;
	
	get_task_comm(new->comm, old);
}

void dfs(struct task_struct* old, struct prinfo* new, int* p)
{
	struct list_head *head;
	struct task_struct *task;
	transfer(old, &new[*p]);
	++(*p);
	list_for_each(head, &(old->children)) //interate all over the children
	{
		task = list_entry(head, struct task_struct, sibling);
		dfs(task, new, p);
	}
}

static int (*oldcall)(void);

int ptree(struct prinfo *buf,int *nr)
{
	//To copy the data from the kernel space into our user space
	struct prinfo *buffer = (struct prinfo*)kmalloc(sizeof(struct prinfo)* MAXBUFFERSIZE, GFP_KERNEL);
	int p = 0;
	read_lock(&tasklist_lock);//unlock
	dfs(&init_task, buffer, &p);
	read_unlock(&tasklist_lock);//lock

	//copy from kernel space to user space
	if(copy_to_user(buf, buffer, sizeof(struct prinfo)* MAXBUFFERSIZE) || copy_to_user(nr, &p, sizeof(int)))
	{
		printk(KERN_INFO "COPY FAILED\n");
		return 1;
	}

	//release
	kfree(buffer);
	return 0;
}
static int addsyscall_init(void)
{
	long *syscall = (long*)0xc000d8c4;
	oldcall = (int(*)(void))(syscall[_NR_ptree]);
	syscall[_NR_ptree] = (unsigned long)ptree;
	printk(KERN_INFO "module load!\n");
	return 0;
}
static void addsyscall_exit(void)
{
	long *syscall = (long*)0xc000d8c4;
	syscall[_NR_ptree] = (unsigned long)oldcall;
	printk(KERN_INFO "module exit!\n");

}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
