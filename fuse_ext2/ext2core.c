#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include "ext2core.h"

static ext2_sb **superblocks;

int init_ext2_core(int img_fd) 
{
	ext2_sb *first_superblock = (ext2_sb*) calloc(1, sizeof(ext2_sb));
	lseek(img_fd, SB_OFFSET, SEEK_SET);
	size_t n = read(img_fd, first_superblock, sizeof(ext2_sb));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}

	int n_group = first_superblock->blocks_count / first_superblock->blocks_per_group;
	superblocks = (ext2_sb**) calloc(n_group, sizeof(ext2_sb*));
	superblocks[0] = first_superblock;



	return 0;
}