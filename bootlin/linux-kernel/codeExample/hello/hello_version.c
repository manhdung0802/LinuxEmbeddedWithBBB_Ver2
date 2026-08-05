// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/utsname.h>
#include <linux/timekeeping.h>

/* Add your code here */
static char *who = "";
module_param(who, charp, 0644); //owner: read write, others: read
MODULE_PARM_DESC(who, "Name of who"); // mô tả param
static int __init hello_init(void)
{

	pr_alert("Hello %s. You are currently using Linux %s\n", who, utsname()->release);
	return 0;
}
static void __exit hello_exit(void)
{
	pr_info("Goodbye ktime is %lld\n", ktime_get_seconds());
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
