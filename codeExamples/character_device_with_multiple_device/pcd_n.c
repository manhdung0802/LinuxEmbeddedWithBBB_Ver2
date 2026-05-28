#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>

#define DEV_MEM_MAX_PCDDEV1 1024
#define DEV_MEM_MAX_PCDDEV2 512
#define DEV_MEM_MAX_PCDDEV3 1024
#define DEV_MEM_MAX_PCDDEV4 512

#define NO_OF_DEVICES 4

char device_buffder_pcddev1[DEV_MEM_MAX_PCDDEV1];
char device_buffder_pcddev2[DEV_MEM_MAX_PCDDEV2];
char device_buffder_pcddev3[DEV_MEM_MAX_PCDDEV3];
char device_buffder_pcddev4[DEV_MEM_MAX_PCDDEV4];

struct pcdev_private_data {
	char *buffer;
	unsigned size;
	const char *serial_number;
	int perm;
	struct cdev cdev;
};

struct pcdrv_private_data {
	int total_devices;
	dev_t device_number;
	struct class *class_pcd;
	struct device *device_pcd;
	struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};

struct pcdrv_private_data pcdrv_data = {
    .total_devices = NO_OF_DEVICES,
    .pcdev_data = {
	[0] = {
	    .buffer = device_buffder_pcddev1,
	    .size = DEV_MEM_MAX_PCDDEV1,
	    .serial_number = "PCDDEV1",
	    .perm = 0x1, // RDONLY
	},
	[1] = {
	    .buffer = device_buffder_pcddev2, .size = DEV_MEM_MAX_PCDDEV2, .serial_number = "PCDDEV2",
	    .perm = 0x10, // WRONLY
	},
	[2] = {
	    .buffer = device_buffder_pcddev3, .size = DEV_MEM_MAX_PCDDEV3, .serial_number = "PCDDEV3",
	    .perm = 0x11, // RDWR
	},
	[3] = {
	    .buffer = device_buffder_pcddev4, .size = DEV_MEM_MAX_PCDDEV4, .serial_number = "PCDDEV4",
	    .perm = 0x11, // RDWR
	}},
};

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

struct file_operations pcd_fops = {
    .open = pcd_open,
    .release = pcd_close,
    .write = pcd_write,
    .read = pcd_read,
    .llseek = pcd_lseek,
    .owner = THIS_MODULE,
};

static int __init pcd_driver_init(void) {
	int ret;
	pr_info("");

	// 1. Create device number - major and minor
	ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "pcd_device_number"); // mặc định để 0, còn NO_OF_DEVICES là do cần điều khiển 4 device
	if (ret < 0) {
		goto out;
	}
	for (int i = 0; i < NO_OF_DEVICES; i++) {
		pr_info("%s : Device number <major>:<minor> = %d:%d\n", __func__, MAJOR(pcdrv_data.device_number + i), MINOR(pcdrv_data.device_number + i));

		// 2. Initialize cdev struct - gán file operation cho cdev
		cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);

		// 3. Đăng ký cdev structure với VFS - gán major minor cho cdev
		pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE; // gán ở đây vì trong cdev_init có logic reset
		cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number + i, 1);
	}

	// 4. Tạo class device trong /sys/class
	pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");

	for (int i = 0; i < NO_OF_DEVICES; i++) {
		// 5. Tạo device file trong /dev
		pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number + i, NULL, "pcdev-%d", i);
		if (IS_ERR(pcdrv_data.device_pcd)) {
			pr_err("Device created failed\n");
			ret = PTR_ERR(pcdrv_data.device_pcd);
			goto out;
		}
	}

	pr_info("Init succesful\n");

	return 0;
out:
	return ret;
}

static void __exit pcd_driver_exit(void) {
	device_destroy(class_pcd, device_number);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number, 1);
	pr_info("Module unload\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungNM1");
MODULE_DESCRIPTION("Hello kernel module");