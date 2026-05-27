#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>

#define DEV_MEM_SIZE 512

char device_buffder[DEV_MEM_SIZE]; // Tạo mảng 512 byte
dev_t device_numer;

loff_t pcd_lseek(struct file *filp, loff_t off, int whence) {
	loff_t temp;
	pr_info("pcd_lseek current f_pos %lld\n", filp->f_pos);
	switch (whence) {
	case SEEK_SET:
		if ((off > DEV_MEM_SIZE) || (off < 0)) {
			return -EINVAL;
		}
		filp->f_pos = off;
		break;
	case SEEK_CUR:
		temp = filp->f_pos + off;
		if ((temp > DEV_MEM_SIZE) || (temp < 0)) {
			return -EINVAL;
		}
		filp->f_pos += off;
		break;
	case SEEK_END:
		temp = DEV_MEM_SIZE + off;
		if ((temp > DEV_MEM_SIZE) || (temp < 0)) {
			return -EINVAL;
		}
		filp->f_pos = DEV_MEM_SIZE + off;
		break;
	default:
		return -EINVAL;
	}
	pr_info("pcd_lseek after f_pos %lld\n", filp->f_pos);
	return filp->f_pos;
}
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count,
		 loff_t *f_pos) {

	pr_info("pcd_read is call with count %zu and f_pos is %llu\n", count, *f_pos);

	// adjust count
	if ((*f_pos + count) > DEV_MEM_SIZE) {
		count = DEV_MEM_SIZE - *f_pos;
	}

	// copy to user
	if (copy_to_user(buff, &device_buffder[*f_pos], count)) {
		return -EFAULT;
	}

	// update current file position
	*f_pos += count;

	pr_info("pcd_read read %zu bytes and f_pos is %lld\n", count, *f_pos);

	return count;
}
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count,
		  loff_t *f_pos) {

	pr_info("pcd_write is call with count %zu and f_pos is %llu\n", count, *f_pos);

	// adjust count
	if ((*f_pos + count) > DEV_MEM_SIZE) {
		count = DEV_MEM_SIZE - *f_pos;
	}

	if (count == 0) {
		pr_err("No space left on device\n");
		return -ENOMEM;
	}

	// copy from user
	if (copy_from_user(&device_buffder[*f_pos], buff, count)) {
		return -EFAULT;
	}

	// update current file position
	*f_pos += count;

	pr_info("pcd_write write %zu bytes and f_pos is %lld\n", count, *f_pos);

	return count;
}
int pcd_open(struct inode *inode, struct file *filp) {
	pr_info("pcd_open\n");
	return 0;
}
int pcd_close(struct inode *inode, struct file *filp) {
	pr_info("pcd_close");
	return 0;
}

struct cdev pcd_cdev;

struct file_operations pcd_fops = {
    .open = pcd_open,
    .release = pcd_close,
    .write = pcd_write,
    .read = pcd_read,
    .llseek = pcd_lseek,
    .owner = THIS_MODULE,
};

struct class *class_pcd;
struct device *device_pcd;

static int __init pcd_driver_init(void) {
	pr_info("");

	// 1. Create device number - major and minor
	alloc_chrdev_region(&device_numer, 0, 1, "pcd_device_number"); // mặc định để 0, còn 1 là chỉ có 1 device dùng device_number này

	pr_info("%s : Device number <major>:<minor> = %d:%d\n", __func__, MAJOR(device_numer), MINOR(device_numer));

	// 2. Initialize cdev struct - gán file operation cho cdev
	cdev_init(&pcd_cdev, &pcd_fops);

	// 3. Đăng ký cdev structure với VFS - gán major minor cho cdev
	pcd_cdev.owner = THIS_MODULE;	      // gán ở đây vì trong cdev_init có logic reset
	cdev_add(&pcd_cdev, device_numer, 1); // 1 vì chỉ đăng ký 1 minor

	// 4. Tạo class device trong /sys/class
	class_pcd = class_create(THIS_MODULE, "pcd_class");

	// 5. Tạo device file trong /dev
	device_pcd = device_create(class_pcd, NULL, device_numer, NULL, "pcd");

	pr_info("Init succesful\n");

	return 0;
}

static void __exit pcd_driver_exit(void) {
	device_destroy(class_pcd, device_numer);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_numer, 1);
	pr_info("Module unload\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungNM1");
MODULE_DESCRIPTION("Hello kernel module");