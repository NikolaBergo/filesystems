#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

const char* get_current_proc_path()
{
	char pidstr[10];
	static char path[] = "/proc/";
	int pid = getpid();
	if (!pid) {
		printf("smth went wrong\n");
		return NULL;
	}
	sprintf(pidstr, "%d", pid);
	strcat(path, pidstr);
	return path;
}

int main(int argc, char** argv)
{
	const char* proc_path = get_current_proc_path();
	if (!proc_path) {
		return -1;
	}
	
	DIR *d;
	struct dirent *dir;
	d = opendir(proc_path);
	while (dir) {
		dir = readdir(d);
		printf("%s\n", dir->d_name);
	}
	return 0;
}