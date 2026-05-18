#include<linux/module.h>       /* Needed by all modules */
#include<linux/kernel.h>       /* Needed for KERN_INFO */
#include<linux/time.h>
#include<linux/io.h>
#include<linux/delay.h>
#include<linux/of.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/platform_device.h>
#include<linux/slab.h>
#include<linux/mod_devicetable.h>
#include<linux/of_device.h>
#include <linux/miscdevice.h>

int device_tree_BBB_driver_probe(struct platform_device *pdev);
int device_tree_BBB_driver_remove(struct platform_device *pdev);

/* Matching table */
// struct để matching giữa driver và devicetree
static const struct of_device_id device_tree_BBB_of_match[] = {
	{ .compatible = "user_data"},
	{}
};
MODULE_DEVICE_TABLE(of, device_tree_BBB_of_match);

int device_tree_BBB_driver_probe(struct platform_device *pdev)
{
	int IRQ = 0;
	struct resource *r;

    pr_info("probe is called\n");

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pr_info("Start %d\n", r -> start);
	pr_info("End %d\n", r -> end);

	r = platform_get_resource_byname(pdev , IORESOURCE_MEM, "user-data");
	pr_info("Start %d\n", r -> start);
	pr_info("End %d\n", r -> end);

	IRQ = platform_get_irq(pdev, 0);
	pr_info("IRQ is %d\n", IRQ);

    return 0;
}

/* Remove Function */
int device_tree_BBB_driver_remove(struct platform_device *pdev)
{
    pr_info("remove is called\n");

	return 0;
}

// Chứa thông tin các hàm được gọi khi devicetree và driver matching
static struct platform_driver device_tree_driver = 
{
	.probe		= device_tree_BBB_driver_probe,
	.remove		= device_tree_BBB_driver_remove,
	.driver		= {
		.name	= "user_data",
		.of_match_table = device_tree_BBB_of_match,
	},
};

module_platform_driver(device_tree_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Blink Led kernel module");