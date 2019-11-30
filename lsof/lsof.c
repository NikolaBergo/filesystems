#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

/*
 * Implementation of lsof command
 * Now it prints only:
 *  CMD     PID      FD      NAME
 * 
 * TBD: user, type
 */
 
struct proc_stat {
	int pid;
	char comm[PATH_MAX];
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
	
	fscanf(f, "%d %s", &(this_proc->pid), this_proc->comm);
	
	fclose(f);
	return 0;
}

int main(int argc, char** argv)
{	
	DIR *d;
	proc_stat* proc = calloc(1, sizeof(proc_stat));
	struct dirent *dir;
	int err = 0;
	
	d = opendir("/proc/");
	if (d == NULL) {
		printf("Can't open dir /proc");
		return -1;
	}
	
	printf("CMD      PID      FD       NAME\n");
	while (1) {
		char current_path[PATH_MAX] = {};
		dir = readdir(d);
		if (dir == NULL)
			break;
	
		if (dir->d_name[0] > '0' && dir->d_name[0] < '9') {
			err = read_process(dir->d_name, proc);
			if (err < 0) {
				return -1;
			}
		
			strcat(current_path, "/proc/");
			strcat(current_path, dir->d_name);
			strcat(current_path, "/fd/");
			
			DIR *fd_dir;
			fd_dir = opendir(current_path);
			
			if (fd_dir == NULL) {
				printf("%s %20d  access denied\n", proc->comm, proc->pid);
				continue;
			}
			
			while((dir = readdir(fd_dir)) != NULL) {
				if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
					continue;
				
				char link_path[PATH_MAX] = {};
				char content[PATH_MAX] = {};
				strcat(link_path, current_path);
				strcat(link_path, dir->d_name);
				
				printf("%s %20d   %10s  ", proc->comm, proc->pid, dir->d_name);
				
				err = readlink(link_path, content, PATH_MAX - 1);
				if (err < 0) {
					printf("access denied\n");
				} else {
					printf("%s     \n", content);
				}
			}
			
			closedir(fd_dir);
		}
	}
	
	closedir(d);
	free(proc);
	
	return 0;
}