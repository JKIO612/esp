#include <stdio.h>
#include  "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define ICM42670_ADDR 0x68
#define I2C_PORT I2C_NUM_0
#define SDA_PIN 10
#define SCL_PIN 8

void i2c_master_init() {
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = SDA_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_io_num = SCL_PIN,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 100000,
	};
	ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));
}

void i2c_write(uint8_t reg, uint8_t data) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (ICM42670_ADDR << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg, true);
	i2c_master_write_byte(cmd, data, true);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);
}

uint8_t  i2c_read(uint8_t reg) {
	uint8_t data;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ICM42670_ADDR << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg, true);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ICM42670_ADDR << 1) | I2C_MASTER_READ, true);
	i2c_master_read_byte(cmd, &data, I2C_MASTER_LAST_NACK);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return data;
}

void icm42670_init() {
	i2c_write(0x1F, 0b00011111);
}

void get_direction() {
	int16_t x = (i2c_read(0x0B) << 8) | i2c_read(0x0C);
	int16_t y = (i2c_read(0x0D) << 8) | i2c_read(0x0E);	

	if (y > 298 && x > -298 && x < 298) {
		ESP_LOGI("", "UP");
	}

	else if (y < -298 && x > -298 && x < 298) {
		ESP_LOGI("", "DOWN");
	}

	else if (x > 298 && y > -298 && y < 298) {
		ESP_LOGI("", "LEFT");
	}

	else if (x < -298 && y > -298 && y < 298) {
		ESP_LOGI("", "RIGHT");
	}

	else if (y > 200 && x > 298) {
                ESP_LOGI("", "UP LEFT");
        }

	else if (y > 200 && x < -298) {
                ESP_LOGI("", "UP RIGHT");
        }

	else if (y < -200 && x > 298) {
                ESP_LOGI("", "DOWN LEFT");
        }

        else if (y < - 200 && x < -298) {
                ESP_LOGI("", "DOWN RIGHT");
        }
}

void app_main(void){
	i2c_master_init();
	icm42670_init();
	while(1) {
		get_direction();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}




