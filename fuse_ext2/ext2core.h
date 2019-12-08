#ifndef __EXT2CORE_H
#define __EXT2CORE_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#define __u32 uint32_t
#define __u16 uint16_t
#define __u8 uint8_t
#define __s16 int16_t
#define __s32 int32_t

#define SB_OFFSET 1024


struct ext2_sb {
	__u32	inodes_count;		/* Inodes count */
	__u32	blocks_count;		/* Blocks count */
	__u32	r_blocks_count;	/* Reserved blocks count */
	__u32	free_blocks_count;	/* Free blocks count */
	__u32	free_inodes_count;	/* Free inodes count */
	__u32	first_data_block;	/* First Data Block */
	__u32	log_block_size;	/* Block size */
	__s32	log_frag_size;	/* Fragment size */
	__u32	blocks_per_group;	/* # Blocks per group */
	__u32	frags_per_group;	/* # Fragments per group */
	__u32	inodes_per_group;	/* # Inodes per group */
	__u32	mtime;		/* Mount time */
	__u32	wtime;		/* Write time */
	__u16	mnt_count;		/* Mount count */
	__s16	max_mnt_count;	/* Maximal mount count */
	__u16	magic;		/* Magic signature */
	__u16	state;		/* File system state */
	__u16	errors;		/* Behaviour when detecting errors */
	__u16	minor_rev_level; 	/* minor revision level */
	__u32	lastcheck;		/* time of last check */
	__u32	checkinterval;	/* max. time between checks */
	__u32	creator_os;		/* OS */
	__u32	rev_level;		/* Revision level */
	__u16	def_resuid;		/* Default uid for reserved blocks */
	__u16	def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT2_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 * 
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	__u32	first_ino; 		/* First non-reserved inode */
	__u16   inode_size; 		/* size of inode structure */
	__u16	block_group_nr; 	/* block group # of this superblock */
	__u32	feature_compat; 	/* compatible feature set */
	__u32	feature_incompat; 	/* incompatible feature set */
	__u32	feature_ro_compat; 	/* readonly-compatible feature set */
	__u8	uuid[16];		/* 128-bit uuid for volume */
	char	volume_name[16]; 	/* volume name */
	char	last_mounted[64]; 	/* directory where last mounted */
	__u32	algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT2_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	__u8	prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	__u8	prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__u16	padding1;
	/* 
	 * Journaling support.
	 */
	__u8	journal_uuid[16];	/* uuid of journal superblock */
	__u32	journal_inum;		/* inode number of journal file */
	__u32	journal_dev;		/* device number of journal file */
	__u32	last_orphan;		/* start of list of inodes to delete */
	
	__u32	reserved[197];	/* Padding to the end of the block */
};

typedef struct ext2_sb ext2_sb;

struct ext2_inode {

};


void ext2_core_init(int img_fd);

#endif