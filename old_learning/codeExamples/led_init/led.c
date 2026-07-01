#include <asm/io.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/printk.h>

#define CONTROL_MODULE 0x44E10000
#define conf_gpmc_ad12 0x830
#define GPIO1 0x4804C000
#define MAP_SIZE 0x1000
#define GPIO_OE 0x134
#define GPIO_DATAOUT 0x13C

void __iomem *controlModule = NULL;
void __iomem *gpio1 = NULL;
u32 data;

ssize_t led_read(struct file *fd, char __user *usr, size_t sz, loff_t *pos);
ssize_t led_write(struct file *fd, const char __user *use, size_t sz,
                  loff_t *pos);
int led_open(struct inode *inode, struct file *fd);
int led_close(struct inode *inode, struct file *fd);
void led_on(void);
void led_off(void);

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .write = led_write,
    .read = led_read,
    .release = led_close,
};

struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "led_kernel",
    .fops = &fops,
};

void led_on(void) {
  u32 data = readl(gpio1 + GPIO_DATAOUT);
  data |= (1 << 12);
  writel(data, gpio1 + GPIO_DATAOUT);
}

void led_off(void) {
  u32 data = readl(gpio1 + GPIO_DATAOUT);
  data &= ~(1 << 12);
  writel(data, gpio1 + GPIO_DATAOUT);
}

ssize_t led_read(struct file *fd, char __user *usr, size_t sz, loff_t *pos) {
  printk("%s %d", __func__, __LINE__);
  return 1;
}
ssize_t led_write(struct file *fd, const char __user *use, size_t sz,
                  loff_t *pos) {
  printk("%s %d", __func__, __LINE__);
  char receive = 0;
  u32 ret = 0;
  ret = copy_from_user(&receive, &use[0], 1);
  if (ret != 0) {
    printk("Copy from user failed\n");
    return -1;
  }
  printk("Data from user is %c", receive);

  switch (receive) {
  case '0':
    printk("Request led off\n");
    led_off();
    break;
  case '1':
    printk("Request led on\n");
    led_on();
    break;
  default:
    printk("Request invalid\n");
    led_off();
    break;
  }

  return 1;
}
int led_open(struct inode *inode, struct file *fd) {
  printk("%s %d", __func__, __LINE__);
  return 0;
}
int led_close(struct inode *inode, struct file *fd) {
  printk("%s %d", __func__, __LINE__);
  return 0;
}

static int __init led_init(void) {
  printk("Led init\n");
  controlModule = ioremap(CONTROL_MODULE, MAP_SIZE);
  if (controlModule == NULL) {
    printk("Remap control module failed\n");
    return -1;
  }
  gpio1 = ioremap(GPIO1, MAP_SIZE);
  if (gpio1 == NULL) {
    printk("Remap gpio1 failed\n");
    return -1;
  }

  misc_register(&misc);

  // Config pinmux as gpio - mode 7
  data = 0x7;
  writel(data, (controlModule + conf_gpmc_ad12));
  printk("Config pinmux for P8_12 is %u \n",
         readl(controlModule + conf_gpmc_ad12));

  // Set output for gpio[12]
  data = readl(gpio1 + GPIO_OE);
  data &= ~(1 << 12);
  writel(data, gpio1 + GPIO_OE);
  printk("Bit 12 OE %d \n", (readl(gpio1 + GPIO_OE) >> 12) & 1);

  // Set value output is 1
  data = readl(gpio1 + GPIO_DATAOUT);
  data |= (1 << 12);
  writel(data, gpio1 + GPIO_DATAOUT);
  printk("Bit 12 is %d \n", (readl(gpio1 + GPIO_DATAOUT) >> 12) & 1);

  return 0;
}
static void __exit led_exit(void) {
  printk("Led exit\n");
  data = readl(gpio1 + GPIO_DATAOUT);
  data &= ~(1 << 12);
  writel(data, gpio1 + GPIO_DATAOUT);
  printk("Led is %u \n", readl(gpio1 + GPIO_DATAOUT));
  misc_deregister(&misc);
  iounmap(controlModule);
  iounmap(gpio1);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DungNM1");
MODULE_DESCRIPTION("Led");