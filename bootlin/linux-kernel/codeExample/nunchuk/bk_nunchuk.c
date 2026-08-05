// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>


/* Add your code here */
static int zpressed;
static int cpressed;
static int nunchuk_probe(struct i2c_client *client);
static void nunchuk_remove(struct i2c_client *client);
static int nunchuk_read_registers(struct i2c_client *client, char *recv_buf);
static const struct of_device_id nunchuk_of_match[] = {
	{ .compatible = "nintendo,nunchuk"},
	{}
};
MODULE_DEVICE_TABLE(of, nunchuk_of_match);

static const struct i2c_device_id nunchuk_id[] = {
	{ "nunchuk", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, nunchuk_id);

static struct i2c_driver nunchuk_driver = {
	.probe = nunchuk_probe,
	.remove = nunchuk_remove,
	.driver = {
		.name = "nunchuk",
		.of_match_table = nunchuk_of_match,
	},
	.id_table = nunchuk_id,
};

static int nunchuk_probe(struct i2c_client *client)
{
	pr_info("%s 0x%x\n", __func__, client->addr);
	char buf[] = {0xf0, 0x55};
	char recv_buf[6] = {};
	int ret;

	ret = i2c_master_send(client, buf, 2);
	if (ret < 0) {
		dev_err(&client->dev, "i2c send failed (%d)\n", ret);
		return ret;
	}
	ret = i2c_master_recv(client, recv_buf, 6);
	if (ret < 0) {
		dev_err(&client->dev, "i2c recv failed (%d)\n", ret);
		return ret;
	}
	for (int i = 0; i < 6; i++)
		pr_info("Data receive from nunchuk is 0x%02x\n", recv_buf[i]);
	fsleep(1000);
	buf[0] = 0xfb;
	buf[1] = 0x00;
	ret = i2c_master_send(client, buf, 2);
	if (ret < 0) {
		dev_err(&client->dev, "i2c send failed (%d)\n", ret);
		return ret;
	}
	ret = i2c_master_recv(client, recv_buf, 6);
	if (ret < 0) {
		dev_err(&client->dev, "i2c recv failed (%d)\n", ret);
		return ret;
	}
	for (int i = 0; i < 6; i++)
		pr_info("Data receive from nunchuk is 0x%02x\n", recv_buf[i]);
	nunchuk_read_registers(client, recv_buf);
	nunchuk_read_registers(client, recv_buf);
	for (int i = 0; i < 6; i++)
		pr_info("Data2 receive from nunchuk is 0x%02x\n", recv_buf[i]);

	char button_type = recv_buf[5];

	zpressed = (int)((button_type & 0x01) == 1);
	cpressed = (int)(((button_type >> 1) & 0x01) == 1);
	if (zpressed)
		pr_info("Z is release\n");
	if (!zpressed)
		pr_info("Z is press\n");
	if (cpressed)
		pr_info("C is release\n");
	if (!cpressed)
		pr_info("C is press\n");

	pr_info("nintendo,alternate-init is %d\n", device_property_read_bool(&client->dev, "nintendo,alternate-init"));
	return 0;
}

static void nunchuk_remove(struct i2c_client *client)
{
	pr_info("%s\n", __func__);
}

static int nunchuk_read_registers(struct i2c_client *client, char *recv_buf)
{
	fsleep(10000);
	u8 buf[] = {0x00}; // gửi 0x00 để yêu cầu nunchuk cập nhật dữ liệu
	u8 recv_buff[6];
	int ret;

	ret = i2c_master_send(client, buf, 1);
	if (ret < 0) {
		pr_info("%s Cannot send to nunchuk\n", __func__);
		return ret;
	}
	fsleep(10000);
	ret = i2c_master_recv(client, recv_buff, 6);
	if (ret < 0) {
		pr_info("%s Cannot recv from nunchuk\n", __func__);
		return ret;
	}
	(void)memcpy(recv_buf, recv_buff, 6);
	return 0;
}

module_i2c_driver(nunchuk_driver);

MODULE_LICENSE("GPL");
