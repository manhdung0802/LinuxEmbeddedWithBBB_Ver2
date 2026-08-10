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
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/dma-direction.h>

#define SERIAL_RESET_COUNTER 0
#define SERIAL_GET_COUNTER 1
#define SERIAL_BUFSIZE 16
#define OMAP_UART_SCR_DMAMODE_CTL3 0x7
#define OMAP_UART_SCR_TX_TRIG_GRANU1 BIT(6)

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
	spinlock_t lock;
	char tx_buf[SERIAL_BUFSIZE];
	struct device *dev;
	struct resource *resrc;
	struct dma_chan *txchan;
	dma_addr_t fifo_dma_addr;
	bool txongoing; // Dùng trong logic ngăn truy câp đồng thời vào cùng 1 kệnh Tx DMA mà không cần giữ lock nào cả
	struct completion txcomplete; // thông báo cho các thread write rằng quá trình truyền data DMA đã xong
};
void serial_routine(struct serial_dev *serial, char val);

static u32 reg_read(struct serial_dev *serial, unsigned int reg) {
	return readl(serial->regs + (reg * 4));
}

static void reg_write(struct serial_dev *serial, u32 val, unsigned int reg) {
	writel(val, serial->regs + (reg * 4));
}

static ssize_t serial_write_pio(struct file *file, const char __user *buffer, size_t count, loff_t *loff) {
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
		spin_lock_irq(&serial->lock);
		serial_buf = serial->rx_buf[serial->buf_rd];
		serial->buf_rd = (serial->buf_rd + 1) % SERIAL_BUFSIZE;
		spin_unlock_irq(&serial->lock);
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

struct file_operations serial_fops_pio = {
    .owner = THIS_MODULE,
    .write = serial_write_pio,
    .read = serial_read,
	.unlocked_ioctl = serial_ioctl,
	.open = serial_open,
};

static void serial_dma_tx_complete(void *param)
{
	struct serial_dev *serial = param;

	complete(&serial->txcomplete);
}

static ssize_t serial_write_dma(struct file *file, const char __user *buffer, size_t count, loff_t *loff) {
	unsigned long flags;
	char first;
	unsigned int len;
	dma_addr_t dma_addr;
	struct dma_async_tx_descriptor *desc;
	dma_cookie_t cookie;
	int ret;

	struct miscdevice *miscdev_ptr = file->private_data;
	struct serial_dev *serial = container_of(miscdev_ptr, struct serial_dev,
						 miscdev);
	spin_lock_irqsave(&serial->lock, flags);
	if(serial->txongoing){
		spin_unlock_irqrestore(&serial->lock, flags);
		return -EBUSY;
	}
	serial->txongoing = true;
	spin_unlock_irqrestore(&serial->lock, flags);

	len = min_t(unsigned int, count, SERIAL_BUFSIZE);
	ret = copy_from_user(serial->tx_buf, buffer, len);
	if(ret){
		spin_lock_irqsave(&serial->lock, flags);
		serial->txongoing = false;
		spin_unlock_irqrestore(&serial->lock, flags);
		return ret;
	}

	init_completion(&serial->txcomplete);

	first = serial->tx_buf[0];

	dma_addr = dma_map_single(serial->dev, serial->tx_buf, SERIAL_BUFSIZE, DMA_TO_DEVICE);
	if(dma_mapping_error(serial->dev, dma_addr)){
		spin_lock_irqsave(&serial->lock, flags);
		serial->txongoing = false;
		spin_unlock_irqrestore(&serial->lock, flags);
		return -ENOMEM;
	}

	desc = dmaengine_prep_slave_single(serial->txchan, dma_addr+1, len-1, DMA_MEM_TO_DEV, 
		DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if(!desc) {
		dma_unmap_single(serial->dev, dma_addr, SERIAL_BUFSIZE, DMA_TO_DEVICE);
		return -EBUSY;
	}

    /* Đăng ký hàm Callback để xử lý khi DMA xong việc */
    desc->callback = serial_dma_tx_complete;
    desc->callback_param = serial;

	cookie = dmaengine_submit(desc);
	ret = dma_submit_error(cookie);
	if(ret){
		dma_unmap_single(serial->dev, dma_addr, SERIAL_BUFSIZE, DMA_TO_DEVICE);
		serial->txongoing = false;
		return ret;
	}


	dma_async_issue_pending(serial->txchan);
	/* Wait for DMA transfer to be over */
	reg_write(serial, first, UART_TX);
	wait_for_completion(&serial->txcomplete);

	dma_unmap_single(serial->dev, dma_addr, 
                     SERIAL_BUFSIZE, DMA_TO_DEVICE);


	spin_lock_irqsave(&serial->lock, flags);
	serial->txongoing = false;
	spin_unlock_irqrestore(&serial->lock, flags);
	return len;
}
struct file_operations serial_fops_dma = {
	.owner = THIS_MODULE,
	.write = serial_write_dma,
    .read = serial_read,
	.unlocked_ioctl = serial_ioctl,
	.open = serial_open,
};

void serial_routine(struct serial_dev *serial, char val) {
	while ((reg_read(serial, UART_LSR) & UART_LSR_THRE) == 0) {
		cpu_relax();
	}
	spin_lock_irq(&serial->lock);
	reg_write(serial, val, UART_TX);
	spin_unlock_irq(&serial->lock);
}

static irqreturn_t serial_irq_handler(int irq, void *dev_id){
	struct serial_dev *serial = (struct serial_dev *)dev_id;

	serial->rx_buf[serial->buf_wr] = reg_read(serial, UART_RX);
	pr_info("%s %d\n", __func__, serial->rx_buf[serial->buf_wr]);
	serial->buf_wr++;
	if(serial->buf_wr == SERIAL_BUFSIZE){
		serial->buf_wr = 0;
	}
	wake_up(&serial->wait);

	return IRQ_HANDLED;
}

static int serial_init_dma(struct serial_dev *serial){
	pr_info("%s\n", __func__);
	int ret;
	struct dma_slave_config txconf = {};

	serial->txchan = dma_request_chan(serial->dev, "tx");
	if(IS_ERR(serial->txchan)){
		ret = PTR_ERR(serial->txchan);  // lưu lỗi trước
		serial->txchan = NULL;
		return ret;
	}

	serial->fifo_dma_addr = dma_map_resource(serial->dev, 
											serial->resrc->start + UART_TX * 4, 
											4, 
											DMA_TO_DEVICE, 
											0);
	if(dma_mapping_error(serial->dev, serial->fifo_dma_addr)){
		dma_release_channel(serial->txchan);
		return -ENOMEM;
	}
	txconf.direction = DMA_MEM_TO_DEV;
	txconf.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
	txconf.dst_addr = serial->fifo_dma_addr;
	ret = dmaengine_slave_config(serial->txchan, &txconf);
	if(ret < 0) {
		dma_unmap_resource(serial->dev, serial->fifo_dma_addr, 4, DMA_TO_DEVICE, 0);
		return ret;
	}

	return 0;
}

static void serial_cleanup_dma(struct serial_dev *serial){
	if (serial->txchan) {
		dmaengine_terminate_sync(serial->txchan);
		dma_unmap_resource(serial->dev, serial->fifo_dma_addr, 4, DMA_TO_DEVICE, 0);
		dma_release_channel(serial->txchan);
	}
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

	spin_lock_init(&serial->lock);

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
	// FCR: chỉ chứa các bit FCR thực sự
	reg_write(serial, UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_XMIT | UART_FCR_CLEAR_RCVR, UART_FCR);
	// SCR: DMA mode + TX trigger granularity (phải thêm định nghĩa UART_OMAP_SCR)
	reg_write(serial, OMAP_UART_SCR_DMAMODE_CTL3 | OMAP_UART_SCR_TX_TRIG_GRANU1, UART_OMAP_SCR);

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
	    .fops = &serial_fops_pio,
	    .parent = &pdev->dev,
	};

	serial->resrc = pdev->resource;
	serial->dev = &pdev->dev;

	ret = serial_init_dma(serial);
	if(ret == -ENODEV){
		serial->miscdev.fops = &serial_fops_pio;
	}
	else if (ret < 0) {
		pr_info("%s cannot init dma\n", __func__);
		return ret;
	} 
	else {
		serial->miscdev.fops = &serial_fops_dma;
	}

	ret = misc_register(&serial->miscdev);
	if (ret < 0) {
		pr_info("%s failed to register misc device\n", __func__);
		serial_cleanup_dma(serial);
		return ret;
	}

	serial->buf_rd = 0;
	serial->buf_wr = 0;
	reg_write(serial, UART_IER_RDI, UART_IER); // kích hoạt ngắt nhận data của UART
	irq = platform_get_irq(pdev, 0);
	if(irq < 0 ) {
		serial_cleanup_dma(serial);
		return irq;
	}
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
	serial_cleanup_dma(serial);
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
