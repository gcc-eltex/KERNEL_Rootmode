#ifndef _SYSFS_KERN_MODULE_H
#define _SYSFS_KERN_MODULE_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define NAME_KOBJ "km_sysfs"

/* Режимы работы процесса */
#define CRED_USER 0
#define CRED_ROOT 1

static int kmsys_init(void);
static void kmsys_exit(void);
static ssize_t sysfs_show(struct kobject *, struct kobj_attribute *, char *);
static ssize_t sysfs_store(struct kobject *, struct kobj_attribute *,
		       const char *, size_t);

#endif	/* _SYSFS_KERN_MODULE_H */