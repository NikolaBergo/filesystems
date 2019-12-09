#ifndef __EXT2CORE_H
#define __EXT2CORE_H

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/types.h>

#define __u32 uint32_t
#define __u16 uint16_t
#define __u8 uint8_t
#define __s16 int16_t
#define __s32 int32_t

#define SB_OFFSET 1024

typedef struct ext2 ext2;

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

struct ext2_group_desc
{
	__le32	bg_block_bitmap;		/* Blocks bitmap block */
	__le32	bg_inode_bitmap;		/* Inodes bitmap block */
	__le32	bg_inode_table;		/* Inodes table block */
	__le16	bg_free_blocks_count;	/* Free blocks count */
	__le16	bg_free_inodes_count;	/* Free inodes count */
	__le16	bg_used_dirs_count;	/* Directories count */
	__le16	bg_pad;
	__le32	bg_reserved[3];
};

typedef struct ext2_group_desc ext2_group_desc;

struct ext2_inode {
	__le16	i_mode;		/* File mode */
	__le16	i_uid;		/* Low 16 bits of Owner Uid */
	__le32	i_size;		/* Size in bytes */
	__le32	i_atime;	/* Access time */
	__le32	i_ctime;	/* Creation time */
	__le32	i_mtime;	/* Modification time */
	__le32	i_dtime;	/* Deletion Time */
	__le16	i_gid;		/* Low 16 bits of Group Id */
	__le16	i_links_count;	/* Links count */
	__le32	i_blocks;	/* Blocks count */
	__le32	i_flags;	/* File flags */
	union {
		struct {
			__le32  l_i_reserved1;
		} linux1;
		struct {
			__le32  h_i_translator;
		} hurd1;
		struct {
			__le32  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__le32	i_block[15];/* Pointers to blocks */
	__le32	i_generation;	/* File version (for NFS) */
	__le32	i_file_acl;	/* File ACL */
	__le32	i_dir_acl;	/* Directory ACL */
	__le32	i_faddr;	/* Fragment address */
	union {
		struct {
			__u8	l_i_frag;	/* Fragment number */
			__u8	l_i_fsize;	/* Fragment size */
			__u16	i_pad1;
			__le16	l_i_uid_high;	/* these 2 fields    */
			__le16	l_i_gid_high;	/* were reserved2[0] */
			__u32	l_i_reserved2;
		} linux2;
		struct {
			__u8	h_i_frag;	/* Fragment number */
			__u8	h_i_fsize;	/* Fragment size */
			__le16	h_i_mode_high;
			__le16	h_i_uid_high;
			__le16	h_i_gid_high;
			__le32	h_i_author;
		} hurd2;
		struct {
			__u8	m_i_frag;	/* Fragment number */
			__u8	m_i_fsize;	/* Fragment size */
			__u16	m_pad1;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
};

typedef struct ext2_inode ext2_inode;

struct ext2_dir_entry {
	__le32	inode;			/* Inode number */
	__le16	rec_len;		/* Directory entry length */
	__u8	name_len;
	__u8    file_type;
	char	name[];			/* File name, up to EXT2_NAME_LEN */
};

typedef struct ext2_dir_entry ext2_dir_entry;

struct ext2 {
    int blocksize;
    int blocks_per_group;
    int inodes_per_group;
    int inode_size;
    int img_fd;
	int inode_table_offset;
    ext2_sb *first_sb;
    ext2_inode *root;
};

typedef struct ext2 ext2;

#endif