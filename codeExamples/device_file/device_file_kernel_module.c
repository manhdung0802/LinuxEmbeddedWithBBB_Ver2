#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

static ssize_t read_fops(struct file *, char __user *, size_t, loff_t *);
static ssize_t write_fops(struct file *, const char __user *, size_t, loff_t *);
static int open_fops(struct inode *, struct file *);
static int close_fops(struct inode *, struct file *);

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read_fops,
    .write = write_fops,
    .open = open_fops,
    .release = close_fops, 
};

struct miscdevice kernelMisc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "kernel_dung",
    .fops = &fops,
};

static ssize_t read_fops(struct file *fd, char __user *usr, size_t sz, loff_t *pos){
    pr_info("%s \n",__func__);
    return 1;
}
static ssize_t write_fops(struct file *fd, const char __user *usr, size_t sz, loff_t *pos){
    pr_info("%s \n",__func__);
    return 1;
}
static int open_fops(struct inode *inode, struct file *fd){
    pr_info("%s \n",__func__);
    return 0;
}
static int close_fops(struct inode *inode, struct file *fd){
    pr_info("%s \n",__func__);
    return 0;
}

static int __init kernel_init(void){
    pr_info("DungNM1 init\n");
    misc_register(&kernelMisc);
    return 0;
}

static void __exit kernel_exit(void){
    pr_info("DungNM1 exit\n");
    misc_deregister(&kernelMisc);
}

module_init(kernel_init);
module_exit(kernel_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungNM1");
MODULE_DESCRIPTION("Hello kernel module");