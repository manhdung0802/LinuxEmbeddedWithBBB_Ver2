#include <linux/module.h>
#include <linux/printk.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

static ssize_t device_file_device_write(struct file *file, const char __user *buf, size_t len, loff_t *pos);
static ssize_t device_file_device_read(struct file *file, char __user *buf, size_t len, loff_t *pos);
static int device_file_device_open(struct inode *inode, struct file *file);
static int device_file_device_close(struct inode *inodep, struct file *filp);

static const struct file_operations device_file_device_fops = 
{
    .owner			= THIS_MODULE,
    .write			= device_file_device_write,
	.read			= device_file_device_read,
    .open			= device_file_device_open,
    .release		= device_file_device_close
};

struct miscdevice device_file_device = {
    .minor = MISC_DYNAMIC_MINOR, // mỗi device file cần số định danh minor
    .name = "device_file_device", // tên device file
    .fops = &device_file_device_fops, // struct file operation chứa các function pointer để tương tác
};

static ssize_t device_file_device_write(struct file *file, const char __user *buf, size_t len, loff_t *pos)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 1;
}

static ssize_t device_file_device_read(struct file *file, char __user *buf, size_t len, loff_t *pos)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 1;
}

static int device_file_device_open(struct inode *inode, struct file *file)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 0;
}

static int device_file_device_close(struct inode *inodep, struct file *filp)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 0;
}

/* Module Init */
static int __init device_file_module_init(void)
{
    int ret = 0;

    pr_info("Hello Module init\n");

    ret = misc_register(&device_file_device); // Đăng ký device file với misc module
    if (ret) {
        pr_err("can't misc_register\n");
        return -1;
    }

    return 0;
}

/* Module Exit */
static void __exit device_file_module_exit(void)
{
    pr_info("Hello Module exit\n");

    misc_deregister(&device_file_device);
}



module_init(device_file_module_init);
module_exit(device_file_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungNM1");
MODULE_DESCRIPTION("Hello Kenel Module");
