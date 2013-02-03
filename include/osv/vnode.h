/*
 * Copyright (c) 2005-2007, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SYS_VNODE_H_
#define _SYS_VNODE_H_

#include <sys/stat.h>

#include <osv/prex.h>
#include <osv/uio.h>
#include "file.h"
#include <osv/list.h>
#include "dirent.h"

struct vfsops;
struct vnops;
struct vnode;
struct file;

/*
 * Vnode types.
 */
enum {
	VNON,	    /* no type */
	VREG,	    /* regular file  */
	VDIR,	    /* directory */
	VBLK,	    /* block device */
	VCHR,	    /* character device */
	VLNK,	    /* symbolic link */
	VSOCK,	    /* socks */
	VFIFO	    /* FIFO */
};

/*
 * Reading or writing any of these items requires holding the
 * appropriate lock.
 */
struct vnode {
	struct list	v_link;		/* link for hash list */
	struct mount	*v_mount;	/* mounted vfs pointer */
	struct vnops	*v_op;		/* vnode operations */
	int		v_refcnt;	/* reference count */
	int		v_type;		/* vnode type */
	int		v_flags;	/* vnode flag */
	mode_t		v_mode;		/* file mode */
	off_t		v_size;		/* file size */
	mutex_t		v_lock;		/* lock for this vnode */
	int		v_nrlocks;	/* lock count (for debug) */
	int		v_blkno;	/* block number */
	char		*v_path;	/* pointer to path in fs */
	void		*v_data;	/* private data for fs */
};
typedef struct vnode *vnode_t;

/* flags for vnode */
#define VROOT		0x0001		/* root of its file system */
#define VISTTY		0x0002		/* device is tty */
#define VPROTDEV	0x0004		/* protected device */

/*
 * Vnode attribute
 */
struct vattr {
	int		va_type;	/* vnode type */
	mode_t		va_mode;	/* file access mode */
};

/*
 *  Modes.
 */
#define	VREAD	00004		/* read, write, execute permissions */
#define	VWRITE	00002
#define	VEXEC	00001

#define IO_APPEND	0x0001

typedef	int (*vnop_open_t)	(vnode_t, int);
typedef	int (*vnop_close_t)	(vnode_t, file_t);
typedef	int (*vnop_read_t)	(struct vnode *, struct uio *, int);
typedef	int (*vnop_write_t)	(struct vnode *, struct uio *, int);
typedef	int (*vnop_seek_t)	(vnode_t, file_t, off_t, off_t);
typedef	int (*vnop_ioctl_t)	(vnode_t, file_t, u_long, void *);
typedef	int (*vnop_fsync_t)	(vnode_t, file_t);
typedef	int (*vnop_readdir_t)	(vnode_t, file_t, struct dirent *);
typedef	int (*vnop_lookup_t)	(vnode_t, char *, vnode_t);
typedef	int (*vnop_create_t)	(vnode_t, char *, mode_t);
typedef	int (*vnop_remove_t)	(vnode_t, vnode_t, char *);
typedef	int (*vnop_rename_t)	(vnode_t, vnode_t, char *, vnode_t, vnode_t, char *);
typedef	int (*vnop_mkdir_t)	(vnode_t, char *, mode_t);
typedef	int (*vnop_rmdir_t)	(vnode_t, vnode_t, char *);
typedef	int (*vnop_getattr_t)	(vnode_t, struct vattr *);
typedef	int (*vnop_setattr_t)	(vnode_t, struct vattr *);
typedef	int (*vnop_inactive_t)	(vnode_t);
typedef	int (*vnop_truncate_t)	(vnode_t, off_t);

/*
 * vnode operations
 */
struct vnops {
	vnop_open_t		vop_open;
	vnop_close_t		vop_close;
	vnop_read_t		vop_read;
	vnop_write_t		vop_write;
	vnop_seek_t		vop_seek;
	vnop_ioctl_t		vop_ioctl;
	vnop_fsync_t		vop_fsync;
	vnop_readdir_t		vop_readdir;
	vnop_lookup_t		vop_lookup;
	vnop_create_t		vop_create;
	vnop_remove_t		vop_remove;
	vnop_rename_t		vop_rename;
	vnop_mkdir_t		vop_mkdir;
	vnop_rmdir_t		vop_rmdir;
	vnop_getattr_t		vop_getattr;
	vnop_setattr_t		vop_setattr;
	vnop_inactive_t		vop_inactive;
	vnop_truncate_t		vop_truncate;
};

/*
 * vnode interface
 */
#define VOP_OPEN(VP, F)		   ((VP)->v_op->vop_open)(VP, F)
#define VOP_CLOSE(VP, FP)	   ((VP)->v_op->vop_close)(VP, FP)
#define VOP_READ(VP, U, F)	   ((VP)->v_op->vop_read)(VP, U, F)
#define VOP_WRITE(VP, U, F)	   ((VP)->v_op->vop_write)(VP, U, F)
#define VOP_SEEK(VP, FP, OLD, NEW) ((VP)->v_op->vop_seek)(VP, FP, OLD, NEW)
#define VOP_IOCTL(VP, FP, C, A)	   ((VP)->v_op->vop_ioctl)(VP, FP, C, A)
#define VOP_FSYNC(VP, FP)	   ((VP)->v_op->vop_fsync)(VP, FP)
#define VOP_READDIR(VP, FP, DIR)   ((VP)->v_op->vop_readdir)(VP, FP, DIR)
#define VOP_LOOKUP(DVP, N, VP)	   ((DVP)->v_op->vop_lookup)(DVP, N, VP)
#define VOP_CREATE(DVP, N, M)	   ((DVP)->v_op->vop_create)(DVP, N, M)
#define VOP_REMOVE(DVP, VP, N)	   ((DVP)->v_op->vop_remove)(DVP, VP, N)
#define VOP_RENAME(DVP1, VP1, N1, DVP2, VP2, N2) \
			   ((DVP1)->v_op->vop_rename)(DVP1, VP1, N1, DVP2, VP2, N2)
#define VOP_MKDIR(DVP, N, M)	   ((DVP)->v_op->vop_mkdir)(DVP, N, M)
#define VOP_RMDIR(DVP, VP, N)	   ((DVP)->v_op->vop_rmdir)(DVP, VP, N)
#define VOP_GETATTR(VP, VAP)	   ((VP)->v_op->vop_getattr)(VP, VAP)
#define VOP_SETATTR(VP, VAP)	   ((VP)->v_op->vop_setattr)(VP, VAP)
#define VOP_INACTIVE(VP)	   ((VP)->v_op->vop_inactive)(VP)
#define VOP_TRUNCATE(VP, N)	   ((VP)->v_op->vop_truncate)(VP, N)

__BEGIN_DECLS
int	 vop_nullop(void);
int	 vop_einval(void);
vnode_t	 vn_lookup(struct mount *, char *);
void	 vn_lock(vnode_t);
void	 vn_unlock(vnode_t);
int	 vn_stat(vnode_t, struct stat *);
int	 vn_access(vnode_t, int);
vnode_t	 vget(struct mount *, char *);
void	 vput(vnode_t);
void	 vgone(vnode_t);
void	 vref(vnode_t);
void	 vrele(vnode_t);
int	 vcount(vnode_t);
void	 vflush(struct mount *);
__END_DECLS

#endif /* !_SYS_VNODE_H_ */