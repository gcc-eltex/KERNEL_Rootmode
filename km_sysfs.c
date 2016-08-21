#include "km_sysfs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivannikov Igor");

static struct kobject		*kobj;
static struct kobj_attribute	kobj_attr = __ATTR(eva, 0664, sysfs_show,
						   sysfs_store);

static int kmsys_init(void)
{
	/* Повторно задаем права, поскольку они не проходят в инициализации */
	kobj = kobject_create_and_add(NAME_KOBJ, NULL);
	kobj_attr.attr.mode = S_IRWXU | S_IRWXG | S_IRWXO;
	sysfs_create_file(kobj, &(kobj_attr.attr));
	pr_info("kmsys_init: module is installed");
	return 0;
}

static void kmsys_exit(void)
{
	sysfs_remove_file(kobj, &kobj_attr.attr);
	kobject_del(kobj);
	pr_info("kmsys_exit: module removed");
}

module_init(kmsys_init);
module_exit(kmsys_exit);

/*
 * sysfs_store - вызывается при записи пользовательского процесса в файл
 * /sys/km_sysfs/.., переводя его в "режим", соответствующий записанному
 * значению.
 */
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,
		       const char *buf, size_t lenth)
{
	struct cred *cred;

	if (lenth != sizeof(int))
		goto err_type;
	/*
	 * Чтобы кооректно работать с файловой системой необходимо заменить
	 * помимо euid еще и fsuid
	 */
	cred = prepare_creds();
	switch (*((int *)buf)) {
		case CRED_USER:
			cred->euid = cred->uid;
			cred->egid = cred->gid;
			cred->fsuid = cred->uid;
			cred->fsgid = cred->gid;
			commit_creds(cred);
			pr_info("b_store: user set CRED_USER");
		break;
		case CRED_ROOT:
			cred->euid.val = 0;
			cred->egid.val = 0;
			cred->fsuid.val = 0;
			cred->fsgid.val = 0;
			commit_creds(cred);
			pr_info("b_store: user set CRED_ROOT");
		break;
		default:
			goto err_val;
	}
	return lenth;

err_type:
	pr_info("b_store: user passed invalid type");
	return -1;

err_val:
	pr_info("b_store: user passed invalid value");
	abort_creds(cred);
	return -1;
}

/*
 * sysfs_show - вызывается при чтении пользовательского процесса из фала
 * /sys/km_sysfs/.., записывая в его буфер текущий режим.
 */
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
		      char *buf)
{
	const struct cred *cred = current_cred();

	if (cred->euid.val == 0 && cred->egid.val == 0 &&
	    cred->fsuid.val == 0 && cred->fsgid.val == 0)
		*((int *)buf) = CRED_ROOT;
	else
		*((int *)buf) = CRED_USER;
	return sizeof(int);
}