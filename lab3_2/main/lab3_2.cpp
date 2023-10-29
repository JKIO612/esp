#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h" // Include the I2C driver library
#include "DFRobot_LCD.h" // Include your LCD library
#include <string.h>
#include <cstdint>
#include <stdio.h>


DFRobot_LCD lcd(16, 2);
extern "C" void app_main() {
    // Adjust LCD settings accordingly
    lcd.init();
    lcd.setRGB(0, 255, 0);

   
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printstr("Hello CSE121!");

        lcd.setCursor(0, 1);
        lcd.printstr("Yue");
   
}
