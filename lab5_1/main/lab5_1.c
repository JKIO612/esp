#include <stdio.h>
//#include <cstdint>
//#include <iostream>
//#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/assist_debug_reg.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO       	8
#define I2C_MASTER_SDA_IO       	10
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ      	100000
#define SHTC3_ADDR              	0x70

#define TRIGGER_GPIO 1
#define ECHO_GPIO 0
#define MAX_DISTANCE_CM 400
#define TIMEOUT_US (MAX_DISTANCE_CM * 58)

void i2c_master_init(void){
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

float read_humidity_and_weather(void){
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
                i2c_master_read_byte(cmd, &data[i], I2C_MASTER_ACK);
        }
        i2c_master_read_byte(cmd, &data[5], I2C_MASTER_NACK);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        float t = (float)((uint16_t)data[0] << 8 | (uint16_t)data[1]) / 65535.0*175.0-45.0;
	return t;
}


void getL(int gp, int v){
    while (gpio_get_level(ECHO_GPIO) == gp && esp_timer_get_time() - v < TIMEOUT_US);
}

void app_main(){

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TRIGGER_GPIO,
    };

    gpio_config(&io_conf);
    io_conf.intr_type = GPIO_INTR_POSEDGE, io_conf.mode = GPIO_MODE_INPUT,
    io_conf.pin_bit_mask = 1ULL << ECHO_GPIO, gpio_config(&io_conf);
    i2c_master_init();

    while (1){
        gpio_set_level(TRIGGER_GPIO, 0);
        esp_rom_delay_us(2);

        gpio_set_level(TRIGGER_GPIO, 1);  
	esp_rom_delay_us(10);

	gpio_set_level(TRIGGER_GPIO, 0);

	uint32_t v = esp_timer_get_time();
	int temp = 58;

	getL(0, v);
        v = esp_timer_get_time();
	getL(1, v);

        uint32_t d = (esp_timer_get_time() - v) / temp;
	int dis = (int)d;
	vTaskDelay(100 / portTICK_PERIOD_MS);
	float t = read_humidity_and_weather();
        
	printf("Distance: %d cm at %.1fC \n", (int)dis, t);
        
	gpio_set_level(TRIGGER_GPIO, 0);
        esp_rom_delay_us(2);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

