#include <stdio.h>
#include <cstdint>
#include "DFRobot_LCD.h"
#include <iostream>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO               8
#define I2C_MASTER_SDA_IO               10
#define I2C_MASTER_NUM I2C_NUM_0        
#define I2C_MASTER_FREQ_HZ              100000
#define SHTC3_ADDR                      0x70

DFRobot_LCD lcd(16,2);

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
                i2c_master_read_byte(cmd, &data[i], I2C_MASTER_ACK);
        }
        i2c_master_read_byte(cmd, &data[5], I2C_MASTER_NACK);

        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        float h = (float)((uint16_t)data[3] << 8 | (uint16_t)data[4]) / 65535.0*100.0;
        float t = (float)((uint16_t)data[0] << 8 | (uint16_t)data[1]) / 65535.0*175.0-45.0;
	
	int hum = static_cast<int>(h);
	int temp = static_cast<int>(t);
	
	std::string t1 = "Temp:" + std::to_string(temp) + "C";
	std::string h1 = "Hum :" + std::to_string(hum) + "%";
	
	lcd.setCursor(0,0);
	lcd.printstr(t1.c_str());
	lcd.setCursor(0,1);
	lcd.printstr(h1.c_str());
	//lcd.setCursor(0,0);
}


extern "C" void app_main(void){
	lcd.init();
	lcd.setRGB(0,255,0);
	while(true){
                read_humidity_and_weather();
                vTaskDelay(pdMS_TO_TICKS(2000));
        }
}

