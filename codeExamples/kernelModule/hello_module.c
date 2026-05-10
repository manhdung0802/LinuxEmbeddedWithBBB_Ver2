#include <linux/init.h>
#include <linux/module.h>

static int __init hello_module_init(void) {
  pr_info("Hello module init\n");
  return 0;
}

static void __exit hello_module_exit(void) {
    pr_info("Hello module exit\n");
}

module_init(hello_module_init);
module_exit(hello_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungNM1");
MODULE_DESCRIPTION("Hello kernel module");