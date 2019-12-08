#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include "ext2core.h"

static ext2_sb **superblocks;
static int blocksize;

int init_ext2_core(int img_fd) 
{
	ext2_sb *first_superblock = (ext2_sb*) calloc(1, sizeof(ext2_sb));
	lseek(img_fd, SB_OFFSET, SEEK_SET);
	size_t n = read(img_fd, first_superblock, sizeof(ext2_sb));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}

	int n_group = first_superblock->blocks_count / first_superblock->blocks_per_group + 1;
			
	superblocks = (ext2_sb**) calloc(n_group, sizeof(ext2_sb*));
	superblocks[0] = first_superblock;
	
	blocksize = 1024 << first_superblock->log_block_size;
	lseek(img_fd, first_superblock->first_data_block + blocksize, SEEK_SET);
	ext2_group_desc bgroup;
	n = read(img_fd, &bgroup, sizeof(ext2_group_desc));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}
	
	ext2_inode first;
	lseek(img_fd, bgroup.bg_inode_table*blocksize+128, SEEK_SET);
	n = read(img_fd, &first, sizeof(ext2_inode));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}
	
	printf("block[0] = %d", first.i_block[0]);
	lseek(img_fd, first.i_block[0]*blocksize, SEEK_SET);
	n = read(img_fd, &entry, sizeof(ext2_));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}
	


	return 0;
}