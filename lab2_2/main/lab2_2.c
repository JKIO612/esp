#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

#define I2C_MASTER_SCL_IO       	8	  // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO       	10	  // GPIO number for I2C master data
#define I2C_MASTER_NUM I2C_NUM_0 		  // I2C port number
#define I2C_MASTER_FREQ_HZ      	100000    // I2C master clock frequency
#define SHTC3_ADDR              	0x70	  // Default I2C address for SHTC

//initialize i2c
void i2c_master_init(void)
{
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_MASTER_FREQ_HZ,
	};

	i2c_param_config(I2C_MASTER_NUM, &conf);
	i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

//process of reading the humidity and weather
void read_humidity_and_weather(void)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SHTC3_ADDR << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, 0x78, true);
	i2c_master_write_byte(cmd, 0x66, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);

	vTaskDelay(pdMS_TO_TICKS(25));

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SHTC3_ADDR << 1) | I2C_MASTER_READ, true);
	
	uint8_t data[6];
	
	for(int i = 0; i < 5; i++){
		i2c_master_read_byte(cmd, &data[i], 0);
	}
	i2c_master_read_byte(cmd, &data[5], 1);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);

	float h = (float)((uint16_t)data[3] << 8 | (uint16_t)data[4]) / 65535.0*100.0;
	float t = (float)((uint16_t)data[0] << 8 | (uint16_t)data[1]) / 65535.0*175.0-45.0;

	printf("Temperature is %.1fC (or %.1fF) with a %.0f%% humidity\n", t, t*1.8+32, h);
}

void app_main(void)
{
    	i2c_master_init();
    	while(true) {
    		read_humidity_and_weather();
		vTaskDelay(pdMS_TO_TICKS(2000));
    	}
}
