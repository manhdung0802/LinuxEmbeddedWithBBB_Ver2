#include "asm-generic/errno.h"
#include "linux/device/class.h"
#include "linux/export.h"
#include "linux/gfp.h"
#include "linux/module.h"
#include "linux/platform_device.h"
#include "platform.h"
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>

struct pcdev_private_data {
	struct pcdev_platform_data pdata;
	char *buffer;
	dev_t dev_num;
	struct cdev cdev;
};

struct pcdrv_private_data {
	int total_devices;
	dev_t device_num_base;
	struct class *class_pcd;
	struct device *device_pcd;
};

struct pcdrv_private_data pcdrv_data;

int check_permission(int dev_perm, int acc_mode) {
	if (dev_perm == RDWR) {
		return 0;
	}
	if ((dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE))) {
		return 0;
	}
	if ((dev_perm == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ))) {
		return 0;
	}
	return -EPERM;
}

static ssize_t pcd_read(struct file *filp, char __user *buffer, size_t count, loff_t *l_pos) {
	pr_info("%s\n", __func__);
	return 0;
}
static ssize_t pcd_write(struct file *filp, const char __user *buffer, size_t count, loff_t *l_pos) {
	pr_info("%s\n", __func__);
	return -ENOMEM;
}
static int pcd_open(struct inode *inode, struct file *filp) {
	pr_info("%s\n", __func__);
	return 0;
}
static int pcd_release(struct inode *inode, struct file *filp) {
	pr_info("%s\n", __func__);
	return 0;
}
static loff_t pcd_lseek(struct file *filp, loff_t l_pos, int whence) {
	pr_info("%s\n", __func__);
	return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = pcd_open,
    .release = pcd_release,
    .write = pcd_write,
    .read = pcd_read,
    .llseek = pcd_lseek,
};

int pcd_platform_driver_probe(struct platform_device *pcd_dev) {
	int ret;
	struct pcdev_private_data *dev_data;
	struct pcdev_platform_data *pdata;
	// get platform data
	pdata = (struct pcdev_platform_data *)pcd_dev->dev.platform_data;
	if (!pdata) {
		pr_info("No platform data\n");
		ret = -EINVAL;
		return ret;
	}

	// cấp phát vùng nhớ động cho dev_data
	dev_data = kzalloc(sizeof(struct pcdev_private_data), GFP_KERNEL);
	if (!dev_data) {
		pr_info("Cannot allocate memory\n");
		ret = -ENOMEM;
		return ret;
	}

	dev_data->pdata.size = pdata->size;
	dev_data->pdata.perm = pdata->perm;
	dev_data->pdata.serial_number = pdata->serial_number;
	pr_info("Device is %s %d %d", dev_data->pdata.serial_number, dev_data->pdata.size, dev_data->pdata.perm);

	pr_info("A device is detected\n");
	// Có bao nhiêu device matching thì hàm probe được gọi bấy nhiêu lần
	return 0;
}
int pcd_platform_driver_remove(struct platform_device *pcd_dev) {
	pr_info("A device is removed\n");
	return 0;
}

struct platform_driver pcd_platform_driver = {
    .probe = pcd_platform_driver_probe,
    .remove = pcd_platform_driver_remove,
    .driver = {
	.name = "pseudo-char-device",
    },
};

#define MAX_DEVICES 10
static int __init pcd_platform_driver_init(void) {

	int ret;
	// cấp nhát động 1 device numbẻ for MAX_DEVICES
	ret = alloc_chrdev_region(&pcdrv_data.device_num_base, 0, MAX_DEVICES, "pcdevs");
	if (ret < 0) {
		pr_err("Alloc chrdev failed\n");
		return ret;
	}

	// tạo device, cless trong /sys/class
	pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
	if (IS_ERR(pcdrv_data.class_pcd)) {
		pr_err("Class createion failed\n");
		ret = PTR_ERR(pcdrv_data.class_pcd);
		unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);
		return ret;
	}

	platform_driver_register(&pcd_platform_driver);
	pr_info("pcd platform driver loaded\n");
	return 0;
}

static void __exit pcd_platform_driver_exit(void) {
	platform_driver_unregister(&pcd_platform_driver);

	class_destroy(pcdrv_data.class_pcd);

	unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);

	pr_info("pcd platform driver unloaded\n");
}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);

MODULE_LICENSE("GPL");