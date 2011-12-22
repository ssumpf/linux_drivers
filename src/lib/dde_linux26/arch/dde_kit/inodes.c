/*
 * \brief  DDE Linux 2.6 inodes
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 *
 * Assorted dummies implementing inode and superblock access functions, which
 * are used by the block layer stuff, but not needed in general parts of DDE
 * Linux 2.6.
 */

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/mount.h>

#include "local.h"

/*
 * Linux' global list of all super blocks.
 */
LIST_HEAD(super_blocks);


/*****************
 ** Inode stuff **
 *****************/

struct inode* new_inode(struct super_block *sb)
{
	if (sb->s_op->alloc_inode)
		return sb->s_op->alloc_inode(sb);
	
	return kzalloc(sizeof(struct inode), GFP_KERNEL);
}


void __mark_inode_dirty(struct inode *inode, int flags)
{
	WARN_UNIMPL;
}


void iput(struct inode *inode)
{
	WARN_UNIMPL;
}


void generic_delete_inode(struct inode *inode)
{
	WARN_UNIMPL;
}


int invalidate_inodes(struct super_block * sb)
{
	WARN_UNIMPL;
	return 0;
}


void truncate_inode_pages(struct address_space *mapping, loff_t lstart)
{
	WARN_UNIMPL;
}


void touch_atime(struct vfsmount *mnt, struct dentry *dentry)
{
	WARN_UNIMPL;
}


/**********************
 ** Superblock stuff **
 **********************/

struct super_block * get_super(struct block_device *bdev)
{
	WARN_UNIMPL;
	return NULL;
}


int simple_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	WARN_UNIMPL;
	return 0;
}


void kill_anon_super(struct super_block *sb)
{
	WARN_UNIMPL;
}


void shrink_dcache_sb(struct super_block * sb)
{
	WARN_UNIMPL;
}


void drop_super(struct super_block *sb)
{
	WARN_UNIMPL;
}


struct inode_operations empty_iops = { };
struct file_operations empty_fops = { };

/**
 * Alloc and init a new inode.
 *
 * Basically stolen from linux/fs/inode.c:alloc_inode()
 */
static struct inode *dde_linux26_alloc_inode(struct super_block *sb)
{
	struct inode *inode;
	
	DEBUG_MSG("alloc_inode fn %p", sb->s_op->alloc_inode);
	if (sb->s_op->alloc_inode)
		inode = sb->s_op->alloc_inode(sb);
	else
		inode = kzalloc(sizeof(*inode), GFP_KERNEL);

	if (inode) {
		inode->i_sb = sb;
		inode->i_blkbits = sb->s_blocksize_bits;
		inode->i_flags = 0;
		atomic_set(&inode->i_count, 1);
		inode->i_op = &empty_iops;
		inode->i_fop = &empty_fops;
		inode->i_nlink = 1;
		atomic_set(&inode->i_writecount, 0);
		inode->i_size = 0;
		inode->i_blocks = 0;
		inode->i_bytes = 0;
		inode->i_generation = 0;
		inode->i_pipe = NULL;
		inode->i_bdev = NULL;
		inode->i_cdev = NULL;
		inode->i_rdev = 0;
		inode->dirtied_when = 0;
		inode->i_private = NULL;
	}

	return inode;
}


void __iget(struct inode *inode)
{
	atomic_inc(&inode->i_count);
}


static struct inode *dde_linux26_new_inode(struct super_block *sb, struct list_head *head,
                                           int (*test)(struct inode *, void *),
                                           int (*set)(struct inode *, void *), void *data)
{
	struct inode *ret = dde_linux26_alloc_inode(sb);
	int err = 0;

	if (set)
		err = set(ret, data);

	BUG_ON(err);

	__iget(ret);
	ret->i_state = I_LOCK|I_NEW;

	list_add_tail(&ret->i_sb_list, &sb->s_inodes);

	return ret;
}


struct inode *iget5_locked(struct super_block *sb, unsigned long hashval,
                           int (*test)(struct inode *, void *),
                           int (*set)(struct inode *, void *), void *data)
{
	struct inode *inode = NULL;
	struct list_head *p;

	list_for_each(p, &sb->s_inodes) {
		struct inode *i = list_entry(p, struct inode, i_sb_list);
		if (test) {
			if (!test(i, data)) {
				DEBUG_MSG("test false");
				continue;
			}
		    else {
				inode = i;
				break;
			}
		}
	}

	if (inode)
		return inode;

	return dde_linux26_new_inode(sb, &sb->s_inodes, test, set, data);
}

void unlock_new_inode(struct inode *inode)
{
	inode->i_state &= ~(I_LOCK | I_NEW);
	wake_up_bit(&inode->i_state, __I_LOCK);
}

struct super_block *sget(struct file_system_type *type,
                         int (*test)(struct super_block *, void*),
                         int (*set)(struct super_block *, void*),
                         void *data)
{
	struct super_block *s = NULL;
	struct list_head   *p;
	int err;

	if (test) {
		list_for_each(p, &type->fs_supers) {
			struct super_block *block = list_entry(p,
			                                       struct super_block,
			                                       s_instances);
			if (!test(block, data))
				continue;
			return block;
		}
	}

	s = kzalloc(sizeof(*s), GFP_KERNEL);
	BUG_ON(!s);

	INIT_LIST_HEAD(&s->s_dirty);
	INIT_LIST_HEAD(&s->s_io);
	INIT_LIST_HEAD(&s->s_files);
	INIT_LIST_HEAD(&s->s_instances);
	INIT_HLIST_HEAD(&s->s_anon);
	INIT_LIST_HEAD(&s->s_inodes);
	init_rwsem(&s->s_umount);
	mutex_init(&s->s_lock);
	lockdep_set_class(&s->s_umount, &type->s_umount_key);
	/*
	 * The locking rules for s_lock are up to the
	 * filesystem. For example ext3fs has different
	 * lock ordering than usbfs:
	 */
	lockdep_set_class(&s->s_lock, &type->s_lock_key);
	down_write(&s->s_umount);
	s->s_count = S_BIAS;
	atomic_set(&s->s_active, 1);
	mutex_init(&s->s_vfs_rename_mutex);
	mutex_init(&s->s_dquot.dqio_mutex);
	mutex_init(&s->s_dquot.dqonoff_mutex);
	init_rwsem(&s->s_dquot.dqptr_sem);
	init_waitqueue_head(&s->s_wait_unfrozen);
	s->s_maxbytes = MAX_NON_LFS;
#if 0
	s->dq_op = sb_dquot_ops;
	s->s_qcop = sb_quotactl_ops;
	s->s_op = &default_op;
#endif
	s->s_time_gran = 1000000000;

	err = set(s, data);
	BUG_ON(err);

	s->s_type = type;
	strlcpy(s->s_id, type->name, sizeof(s->s_id));
	list_add_tail(&s->s_list, &super_blocks);
	list_add(&s->s_instances, &type->fs_supers);
	__module_get(type->owner);
	return s;
}

int set_anon_super(struct super_block *s, void *data)
{
	WARN_UNIMPL;
	return 0;
}

int get_sb_pseudo(struct file_system_type *fs_type, char *name,
                  struct super_operations *ops, unsigned long magic,
                  struct vfsmount *mnt)
{
	struct super_block *s = sget(fs_type, NULL, set_anon_super, NULL);
	struct super_operations default_ops = {};
	struct inode *root = NULL;
	struct dentry *dentry = NULL;
	struct qstr d_name = {.name = name, .len = strlen(name)};

	BUG_ON(IS_ERR(s));

	s->s_flags = MS_NOUSER;
	s->s_maxbytes = ~0ULL;
	s->s_blocksize = 1024;
	s->s_blocksize_bits = 10;
	s->s_magic = magic;
	s->s_op = ops ? ops : &default_ops;
	s->s_time_gran = 1;
	root = new_inode(s);

	BUG_ON(!root);
	
	root->i_mode = S_IFDIR | S_IRUSR | S_IWUSR;
	root->i_uid = root->i_gid = 0;
#if 0
	root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;
	dentry = d_alloc(NULL, &d_name);
	dentry->d_sb = s;
	dentry->d_parent = dentry;
	d_instantiate(dentry, root);
#endif
	s->s_root = dentry;
	s->s_flags |= MS_ACTIVE;

	mnt->mnt_sb = s;
	mnt->mnt_root = dget(s->s_root);

	DEBUG_MSG("root mnt sb @ %p", mnt->mnt_sb);

	return 0;
}

void inode_init_once(struct inode *inode)
{
	WARN_UNIMPL;
}

