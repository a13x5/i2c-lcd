#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "i2c-lcd.h"

int
main(void)
{
    /*
     * Initializing lcd connected to i2c-1 with address 0x27
     */
    lcd_config cfg = configure("/dev/i2c-1", 0x27);
    init_lcd(&cfg);
    /*
     * Printing simple strings and moving screen
     */
    char str1[] = "First line, which is longer than second";
    char str2[] = "Second line";
    set_cursor(&cfg, 0, 2);
    string_write(&cfg, str2);
    set_cursor(&cfg, 0, 1);
    sleep(1);
    string_write(&cfg, str1);
    sleep(1);
    move_screen(&cfg, strlen(str1) - 16);
    return_home(&cfg);
    sleep(1);
    clear_display(&cfg);
    /*
     * Custom characters
     */
    unsigned char left_arrow_addr = 0x01;
    unsigned char left_arrow[8] = {
	0b00001,
	0b00011,
	0b00111,
	0b01111,
	0b00111,
	0b00011,
	0b00001,
	0b00000
    };
    unsigned char right_arrow_addr = 0x02;
    unsigned char right_arrow[8] = {
        0b01000,
        0b01100,
        0b01110,
        0b01111,
        0b01110,
        0b01100,
        0b01000,
        0b00000
    };
    cgram_write(&cfg, left_arrow_addr, left_arrow);
    cgram_write(&cfg, right_arrow_addr, right_arrow);
    init_lcd(&cfg);
    /*
     * Random pattern and blinking
     */
    for (int i = 0; i < 16; i+=2) {
	set_cursor(&cfg, i, 1);
	general_write(&cfg, left_arrow_addr, LCD_MODE_DATA);
	general_write(&cfg, right_arrow_addr, LCD_MODE_DATA);
	set_cursor(&cfg, i, 2);
	general_write(&cfg, right_arrow_addr, LCD_MODE_DATA);
	general_write(&cfg, left_arrow_addr, LCD_MODE_DATA);
    }
    sleep(3);
    for (int i = 0; i < 2; i++) {
	clk_write(cfg.i2c_fd, LCD_BACKLIGHT_OFF);
	sleep(1);
	clk_write(cfg.i2c_fd, LCD_BACKLIGHT_ON);
	sleep(1);
    }

    /*
     * Clearing display and disabling it completely
     */
    clear_display(&cfg);
    cfg.backlight = LCD_BACKLIGHT_OFF;
    cfg.lcd_control = (LCD_CONTROL_SET |
		       LCD_CONTROL_DISPLAY_OFF |
		       LCD_CONTROL_CURSOR_OFF |
		       LCD_CONTROL_BLINK_OFF);
    apply_lcd_control(&cfg);
    close(cfg.i2c_fd);
}
