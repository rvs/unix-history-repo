/*
 * Copyright (c) 1982, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)inode.h	8.9 (Berkeley) %G%
 */

#include <ufs/ufs/dir.h>
#include <ufs/ufs/dinode.h>

/*
 * The inode is used to describe each active (or recently active) file in the
 * UFS filesystem. It is composed of two types of information. The first part
 * is the information that is needed only while the file is active (such as
 * the identity of the file and linkage to speed its lookup). The second part
 * is * the permanent meta-data associated with the file which is read in
 * from the permanent dinode from long term storage when the file becomes
 * active, and is put back when the file is no longer being used.
 */
struct inode {
	LIST_ENTRY(inode) i_hash;/* Hash chain. */
	struct	vnode  *i_vnode;/* Vnode associated with this inode. */
	struct	vnode  *i_devvp;/* Vnode for block I/O. */
	u_int32_t i_flag;	/* flags, see below */
	dev_t	  i_dev;	/* Device associated with the inode. */
	ino_t	  i_number;	/* The identity of the inode. */

	union {			/* Associated filesystem. */
		struct	fs *fs;		/* FFS */
		struct	lfs *lfs;	/* LFS */
	} inode_u;
#define	i_fs	inode_u.fs
#define	i_lfs	inode_u.lfs

	struct	 dquot *i_dquot[MAXQUOTAS]; /* Dquot structures. */
	u_quad_t i_modrev;	/* Revision level for NFS lease. */
	struct	 lockf *i_lockf;/* Head of byte-level lock list. */
	struct	 lock i_lock;	/* Inode lock. */
	/*
	 * Side effects; used during directory lookup.
	 */
	int32_t	  i_count;	/* Size of free slot in directory. */
	doff_t	  i_endoff;	/* End of useful stuff in directory. */
	doff_t	  i_diroff;	/* Offset in dir, where we found last entry. */
	doff_t	  i_offset;	/* Offset of free space in directory. */
	ino_t	  i_ino;	/* Inode number of found directory. */
	u_int32_t i_reclen;	/* Size of found directory entry. */
	/*
	 * The on-disk dinode itself.
	 */
	struct	dinode i_din;	/* 128 bytes of the on-disk dinode. */
};

#define	i_atime		i_din.di_atime
#define	i_atimensec	i_din.di_atimensec
#define	i_blocks	i_din.di_blocks
#define	i_ctime		i_din.di_ctime
#define	i_ctimensec	i_din.di_ctimensec
#define	i_db		i_din.di_db
#define	i_flags		i_din.di_flags
#define	i_gen		i_din.di_gen
#define	i_gid		i_din.di_gid
#define	i_ib		i_din.di_ib
#define	i_mode		i_din.di_mode
#define	i_mtime		i_din.di_mtime
#define	i_mtimensec	i_din.di_mtimensec
#define	i_nlink		i_din.di_nlink
#define	i_rdev		i_din.di_rdev
#define	i_shortlink	i_din.di_shortlink
#define	i_size		i_din.di_size
#define	i_uid		i_din.di_uid

/* These flags are kept in i_flag. */
#define	IN_ACCESS	0x0001		/* Access time update request. */
#define	IN_CHANGE	0x0002		/* Inode change time update request. */
#define	IN_UPDATE	0x0004		/* Modification time update request. */
#define	IN_MODIFIED	0x0008		/* Inode has been modified. */
#define	IN_RENAME	0x0010		/* Inode is being renamed. */
#define	IN_SHLOCK	0x0020		/* File has shared lock. */
#define	IN_EXLOCK	0x0040		/* File has exclusive lock. */

#ifdef KERNEL
/*
 * Structure used to pass around logical block paths generated by
 * ufs_getlbns and used by truncate and bmap code.
 */
struct indir {
	ufs_daddr_t in_lbn;		/* Logical block number. */
	int	in_off;			/* Offset in buffer. */
	int	in_exists;		/* Flag if the block exists. */
};

/* Convert between inode pointers and vnode pointers. */
#define VTOI(vp)	((struct inode *)(vp)->v_data)
#define ITOV(ip)	((ip)->i_vnode)

#define	ITIMES(ip, t1, t2) {						\
	if ((ip)->i_flag & (IN_ACCESS | IN_CHANGE | IN_UPDATE)) {	\
		(ip)->i_flag |= IN_MODIFIED;				\
		if ((ip)->i_flag & IN_ACCESS)				\
			(ip)->i_atime = (t1)->tv_sec;			\
		if ((ip)->i_flag & IN_UPDATE) {				\
			(ip)->i_mtime = (t2)->tv_sec;			\
			(ip)->i_modrev++;				\
		}							\
		if ((ip)->i_flag & IN_CHANGE)				\
			(ip)->i_ctime = time.tv_sec;			\
		(ip)->i_flag &= ~(IN_ACCESS | IN_CHANGE | IN_UPDATE);	\
	}								\
}

/* This overlays the fid structure (see mount.h). */
struct ufid {
	u_int16_t ufid_len;	/* Length of structure. */
	u_int16_t ufid_pad;	/* Force 32-bit alignment. */
	ino_t	  ufid_ino;	/* File number (ino). */
	int32_t	  ufid_gen;	/* Generation number. */
};
#endif /* KERNEL */
