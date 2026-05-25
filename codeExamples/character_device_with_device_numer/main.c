#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

#define DEV_MEM_SIZE 512

char device_buffder[DEV_MEM_SIZE]; // Tạo mảng 512 byte
dev_t device_numer;
struct cdev pcd_cdev = {
    .owner = THIS_MODULE,
};
struct file_operations pcd_fops;

static int __init pcd_driver_init(void) {
  pr_info("");

  // 1. Create device number
  alloc_chrdev_region(
      &device_numer, 0, 1,
      "pcd"); // mặc định để 0, còn 1 là chỉ có 1 device dùng device_number này

  // 2. Initialize cdev struct
  cdev_init(&pcd_cdev, &pcd_fops);

  // 3. Đăng ký cdev structure với VFS
  pcd_cdev.owner = THIS_MODULE; // gán ở đây vì trong cdev_init có logic reset
  cdev_add(&pcd_cdev, device_numer, 1); // 1 vì chỉ đăng ký 1 minor

  return 0;
}

static void __exit pcd_driver_exit(void) {}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungNM1");
MODULE_DESCRIPTION("Hello kernel module");