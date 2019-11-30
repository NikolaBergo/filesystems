#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#define MAX_CMD_LEN 512

/*
 * Implementation of ps command with 2 options:
 *    ps   prints processes with the same tty as the called process
 *    ps -e  prints all processes  
 */
 
struct proc_stat {
	int pid;
	char comm[MAX_CMD_LEN];
	char state;
	int ppid;
	int group_id;
	int session_id;
	int tty_num;
	unsigned long int utime;
	unsigned long int stime;
};
typedef struct proc_stat proc_stat;

int read_process(const char *pid, proc_stat *this_proc) 
{
	char result_path[50] = {};
	char stat_path[] = "/stat";
	char proc_path[] = "/proc/";
	unsigned long int unused;
	
	strcat(result_path, proc_path);
	strcat(result_path, pid);
	strcat(result_path, stat_path);
	
	FILE *f = fopen(result_path, "r");
	if (!f) {
		printf("error when opening stat file for process %s\n", pid);
		return -1;
	}
	
	fscanf(f, "%d %s %c %d %d %d %d", &(this_proc->pid), 
					  this_proc->comm,
					  &(this_proc->state),
					  &(this_proc->ppid),
					  &(this_proc->group_id),
					  &(this_proc->session_id),
					  &(this_proc->tty_num));
	fscanf(f, "%lu %lu %lu %lu %lu %lu", &unused, &unused, &unused, &unused, &unused, &unused);
	fscanf(f, "%lu %lu", &(this_proc->utime), &(this_proc->stime));
	
	fclose(f);
	return 0;
}

void print_proc(proc_stat *proc) 
{
	char format_cmd[100] = {};
	for (int i = 1; i < strlen(proc->comm) - 1; i++)
		format_cmd[i-1] = proc->comm[i];
	
	unsigned long int format_time = (proc->utime + proc->stime) / sysconf(_SC_CLK_TCK);
	int seconds = format_time % 60;
	int mins = (format_time / 60) % (60*24);
	int days = format_time / (60*24);
	
	printf("%8d%10d%10d:%02d:%02d %s\n", proc->pid, proc->tty_num, days, mins, seconds, format_cmd);
}

int main(int argc, char** argv)
{	
	DIR *d;
	proc_stat* proc = calloc(1, sizeof(proc_stat));
	struct dirent *dir;
	int tty = 0;
	int err = 0;
	d = opendir("/proc/");
	
	// by default ps prints all processes of the same tty
	if (argc == 1) {
		int pid = getpid();
		char pid_string[10] = {};
		sprintf(pid_string, "%d", pid);
		read_process(pid_string, proc);
		tty = proc->tty_num;
	}
	
	printf("     PID       TTY         TIME    CMD\n");
	while (1) {
		int err = 0;
		dir = readdir(d);
		if (dir == NULL)
			break;
		if (dir->d_name[0] > '0' && dir->d_name[0] < '9') {
			err = read_process(dir->d_name, proc);
			if (err < 0) {
				return -1;
			}
			
			if (argc == 1 && (proc->tty_num == tty)) {
				print_proc(proc);
			}
			
			// ps -e prints all processes
			if (argc == 2 && strcmp(argv[1], "-e") == 0)
				print_proc(proc);
		}
	}
	
	closedir(d);
	free(proc);
	
	return 0;
}