#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <asm/delay.h>
#include <linux/delay.h>

static int nunchuk_probe(struct i2c_client *client);
static void nunchuk_remove(struct i2c_client *client);
void nunchuk_read_register(struct i2c_client *client, u8 *buf_recv);
void nunchuk_poll(struct input_dev *dev);
bool zpressed;
bool cpressed;
int x;
int y;
int xT;
int yT;

static const struct of_device_id of_nunchuk_table[] = {
	{ 
		.compatible = "nintendo,nunchuk",
	},
	{}
};
MODULE_DEVICE_TABLE(of, of_nunchuk_table);

static struct i2c_driver nunchuk_driver = {
	.probe = nunchuk_probe,
	.remove = nunchuk_remove,
	.driver = {
		.name = "nintendo,nunchuk",
		.of_match_table = of_nunchuk_table,
	}
};

struct nunchuk_dev {
	struct i2c_client *i2c_client;
};

static int nunchuk_probe(struct i2c_client *client){
	int ret;
	u8 buf_send[2];
	u8 buf_recv[6];
	struct input_dev *input;
	struct nunchuk_dev *nunchuk;
	pr_info("%s\n", __func__);

	nunchuk = devm_kzalloc(&client->dev, sizeof(*nunchuk), GFP_KERNEL);
	if (!nunchuk) {
		return -ENOMEM;
	}
	nunchuk->i2c_client = client;

	buf_send[0] = 0xf0;
	buf_send[1] = 0x55;
	ret = i2c_master_send(client, buf_send, 2);
	if (ret < 0) {
		pr_info("%s i2c send failed\n", __func__);
		return ret;
	}

	udelay(1000);

	buf_send[0] = 0xfb;
	buf_send[1] = 0x00;
	ret = i2c_master_send(client, buf_send, 2);
	if (ret < 0) {
		pr_info("%s i2c send failed\n", __func__);
		return ret;
	}

	input = devm_input_allocate_device(&client->dev);
	if (!input){
		pr_info("%s failed to alocate an input device\n", __func__);
		return -ENOMEM;
	}

	input->name = "Wii Nunchuk";
	input->id.bustype = BUS_I2C;

	set_bit(EV_KEY, input->evbit);
	set_bit(BTN_C, input->keybit);
	set_bit(BTN_Z, input->keybit);

	set_bit(EV_ABS, input->evbit);
	set_bit(ABS_X, input->absbit);
	set_bit(ABS_Y, input->absbit);
	set_bit(ABS_TILT_X, input->absbit);
	set_bit(ABS_TILT_Y, input->absbit);
	input_set_abs_params(input, ABS_X, 30, 220, 4, 8);
	input_set_abs_params(input, ABS_Y, 40, 200, 4, 8);
	input_set_abs_params(input, ABS_TILT_X, 30, 220, 4, 8);
	input_set_abs_params(input, ABS_TILT_Y, 40, 200, 4, 8);

	input_set_drvdata(input, nunchuk);

	ret = input_setup_polling(input, nunchuk_poll);
	if (ret < 0){
		pr_info("%s failed to setup polling\n", __func__);
		return ret;
	}

	input_set_poll_interval(input, 10);
	ret = input_register_device(input);
	if (ret < 0){
		pr_info("%s failed to register an input device\n", __func__);
		return ret;
	}

	return 0;
}

void nunchuk_poll(struct input_dev *input){
	u8 buf_recv[6];
	struct nunchuk_dev *nunchuk = input_get_drvdata(input);
	struct i2c_client *client = nunchuk->i2c_client;

	nunchuk_read_register(client, buf_recv);

	zpressed = buf_recv[5] & 0x01? 0 : 1;
	cpressed = (buf_recv[5] >> 1) & 0x01? 0 : 1;
	x = buf_recv[0];
	y = buf_recv[1];
	xT = buf_recv[2];
	yT = buf_recv[3];

	input_report_key(input, BTN_C, cpressed);
	input_report_key(input, BTN_Z, zpressed);
	input_report_abs(input, ABS_X, x);
	input_report_abs(input, ABS_Y, y);
	// input_report_abs(input, ABS_TILT_X, xT);
	// input_report_abs(input, ABS_TILT_Y, yT);
	input_sync(input);
}

static void nunchuk_remove(struct i2c_client *client){
	pr_info("%s\n", __func__);
}

void nunchuk_read_register(struct i2c_client *client, u8 *buf_recv){
	u8 buf_send[1];
	buf_send[0] = 0x00;
	mdelay(10);
	i2c_master_send(client, buf_send, 1);
	mdelay(10);
	i2c_master_recv(client, buf_recv, 6);
}

module_i2c_driver(nunchuk_driver);

MODULE_LICENSE("GPL");