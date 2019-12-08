#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include "ext2core.h"

static ext2_sb *superblock;

int init_ext2_core(int img_fd) 
{
	superblock = (ext2_sb*) calloc(1, sizeof(ext2_sb));
	lseek(img_fd, 1024, SEEK_SET);
	size_t n = read(img_fd, superblock, sizeof(ext2_sb));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}
	
	printf("inodes: %d  blocks: %d\n", superblock->inodes_count, superblock->blocks_count);
	return 0;
}