// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/serial.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>

#define SERIAL_RESET_COUNTER 0
#define SERIAL_GET_COUNTER 1
#define SERIAL_BUFSIZE 16

/* Add your code here */
static const struct of_device_id serial_match_table[] = {
    {
	.compatible = "bootlin,serial",
    },
    {}};
MODULE_DEVICE_TABLE(of, serial_match_table);

struct serial_dev {
	void __iomem *regs;
	struct miscdevice miscdev;
	unsigned int counter_fops;
	char rx_buf[SERIAL_BUFSIZE];
	unsigned int buf_rd;
	unsigned int buf_wr;
	wait_queue_head_t wait;
};
void serial_routine(struct serial_dev *serial, char val);

static u32 reg_read(struct serial_dev *serial, unsigned int reg) {
	return readl(serial->regs + (reg * 4));
}

static void reg_write(struct serial_dev *serial, u32 val, unsigned int reg) {
	writel(val, serial->regs + (reg * 4));
}

static ssize_t serial_write(struct file *file, const char __user *buffer, size_t count, loff_t *loff) {
	pr_info("%s\n", __func__);
	struct miscdevice *miscdev_ptr = file->private_data;
	struct serial_dev *serial = container_of(miscdev_ptr, struct serial_dev, miscdev);

	unsigned long ret;
	char *buf = kzalloc(count, GFP_KERNEL);
	ret = copy_from_user(buf, buffer, count);
	if(ret != 0){
		return -EFAULT;
	}

	for (int i = 0; i <count; i++){
		serial_routine(serial, buf[i]);
		if(buf[i] == '\n'){
			serial_routine(serial, '\r');
		}
	}

	*loff = 0;
	kfree(buf);

	return count;
}
static ssize_t serial_read(struct file *file, char __user *buffer, size_t count, loff_t *loff) {
	pr_info("%s\n", __func__);
	char serial_buf;

	struct miscdevice *miscdev_ptr = file->private_data;
	struct serial_dev *serial = container_of(miscdev_ptr, struct serial_dev, miscdev);

	if(!serial){
		return -ENOMEM;
	}

	wait_event_interruptible(serial->wait, serial->buf_rd != serial->buf_wr);

	if(serial->buf_rd != serial->buf_wr){
		serial_buf = serial->rx_buf[serial->buf_rd];
		serial->buf_rd = (serial->buf_rd + 1) % SERIAL_BUFSIZE;
		if(copy_to_user(buffer, &serial_buf, 1)){
			return -EFAULT;
		}
		return 1;
	}

	return 0;
}

long serial_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	struct miscdevice *miscdev_ptr = file->private_data;
	struct serial_dev *serial = container_of(miscdev_ptr, struct serial_dev, miscdev);

	switch (cmd)
	{
	case SERIAL_RESET_COUNTER:
		serial->counter_fops = 0;
		return serial->counter_fops;
		break;
	
	case SERIAL_GET_COUNTER:
		return serial->counter_fops;
		break;
	
	default:
		break;
	}
	return 0;
}

int serial_open(struct inode *inode, struct file *file){
	struct miscdevice *miscdev_ptr = file->private_data;
	struct serial_dev *serial = container_of(miscdev_ptr, struct serial_dev, miscdev);

	serial->counter_fops++;
	pr_info ("%s counter is %d\n", __func__, serial->counter_fops);
	return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = serial_write,
    .read = serial_read,
	.unlocked_ioctl = serial_ioctl,
	.open = serial_open,
};

void serial_routine(struct serial_dev *serial, char val) {
	while ((reg_read(serial, UART_LSR) & UART_LSR_THRE) == 0) {
		cpu_relax();
	}
	reg_write(serial, val, UART_TX);
}

static irqreturn_t serial_irq_handler(int irq, void *dev_id){
	struct serial_dev *serial = (struct serial_dev *)dev_id;

	serial->rx_buf[serial->buf_wr] = reg_read(serial, UART_RX);
	pr_info("%s %d\n", __func__, serial->rx_buf[serial->buf_wr]);
	wake_up(&serial->wait);
	serial->buf_wr++;
	if(serial->buf_wr == SERIAL_BUFSIZE){
		serial->buf_wr = 0;
	}

	return IRQ_HANDLED;
}

static int serial_probe(struct platform_device *pdev) {
	pr_info("Called %s\n", __func__);
	int ret;
	struct serial_dev *serial;
	unsigned int uartclk;
	unsigned int baud_divisor;
	int irq;


	serial = devm_kzalloc(&pdev->dev, sizeof(*serial), GFP_KERNEL);
	if (!serial) {
		return -ENOMEM;
	}

	init_waitqueue_head(&serial->wait);

	serial->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(serial->regs)) {
		return PTR_ERR(serial->regs);
	}

	pm_runtime_enable(&pdev->dev);
	pm_runtime_get_sync(&pdev->dev);

	ret = of_property_read_u32(pdev->dev.of_node, "clock-frequency", &uartclk);
	if (ret) {
		pr_err("clock-frequency property not found in DTB\n");
		return ret;
	}

	baud_divisor = uartclk / 16 / 115200; // xem bảng 19-6 spruh73q
	reg_write(serial, 0x07, UART_OMAP_MDR1);
	reg_write(serial, 0x00, UART_LCR);
	reg_write(serial, UART_LCR_DLAB, UART_LCR);
	reg_write(serial, baud_divisor & 0xff, UART_DLL);
	reg_write(serial, (baud_divisor >> 8) & 0xff, UART_DLM);
	reg_write(serial, UART_LCR_WLEN8, UART_LCR);
	reg_write(serial, 0x00, UART_OMAP_MDR1);
	reg_write(serial, UART_FCR_CLEAR_XMIT | UART_FCR_CLEAR_RCVR, UART_FCR);

	serial_routine(serial, 'H');
	serial_routine(serial, 'E');
	serial_routine(serial, 'L');
	serial_routine(serial, 'L');
	serial_routine(serial, 'O');

	platform_set_drvdata(pdev, serial);

	struct resource *res;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	serial->miscdev = (struct miscdevice) {
	    .minor = MISC_DYNAMIC_MINOR,
	    .name = devm_kasprintf(&pdev->dev, GFP_KERNEL, "serial-%x", res->start),
	    .fops = &fops,
	    .parent = &pdev->dev,
	};

	ret = misc_register(&serial->miscdev);
	if (ret < 0) {
		pr_info("%s failed to register misc device\n", __func__);
		return ret;
	}

	serial->buf_rd = 0;
	serial->buf_wr = 0;
	reg_write(serial, UART_IER_RDI, UART_IER); // kích hoạt ngắt nhận data của UART
	irq = platform_get_irq(pdev, 0);
	if(irq < 0 ) return irq;
	ret = devm_request_irq(&pdev->dev, irq, serial_irq_handler, IRQF_SHARED, pdev->name, serial);

	return 0;
}

static int serial_remove(struct platform_device *pdev) {
	pr_info("Called %s\n", __func__);
	pm_runtime_put_sync(&pdev->dev);
	pm_runtime_disable(&pdev->dev);
	struct serial_dev *serial = platform_get_drvdata(pdev);
	if(serial){
		misc_deregister(&serial->miscdev);
	}
	return 0;
}

static struct platform_driver serial_driver = {
    .driver = {
	.name = "serial",
	.owner = THIS_MODULE,
	.of_match_table = serial_match_table,
    },
    .probe = serial_probe,
    .remove = serial_remove,
};
module_platform_driver(serial_driver);

MODULE_LICENSE("GPL");
