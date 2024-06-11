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
     * Printing simple strings and move them
     */
    char str1[] = "First line, which is longer than second";
    char str2[] = "Second line";
    set_cursor(&cfg, 0, 1);
    string_write(&cfg, str2);
    set_cursor(&cfg, 0, 0);
    sleep(1);
    string_write(&cfg, str1);
    sleep(1);
    move_screen(&cfg, strlen(str1) - 16);
    return_home(&cfg);
    sleep(1);
    clear_display(&cfg);
    /*
     * Low level commands to pring characters and move display
     * Ref. HD44780 manual page 17.
     */
    general_write(&cfg, 0xBF, LCD_MODE_DATA);
    general_write(&cfg, (LCD_SHIFT_SET | LCD_SHIFT_CURSOR | LCD_SHIFT_RIGHT), LCD_MODE_CMD);
    general_write(&cfg, 0xFF, LCD_MODE_DATA);
    general_write(&cfg, (LCD_SHIFT_SET | LCD_SHIFT_CURSOR | LCD_SHIFT_RIGHT), LCD_MODE_CMD);
    general_write(&cfg, 0xFF, LCD_MODE_DATA);
    general_write(&cfg, (LCD_SHIFT_SET | LCD_SHIFT_CURSOR | LCD_SHIFT_RIGHT), LCD_MODE_CMD);
    general_write(&cfg, 0xFF, LCD_MODE_DATA);
    sleep(3);
    clear_display(&cfg);
    /*
     * Funny pattern and blinking
     */
    for (int i = 0; i < 16; i+=2) {
	set_cursor(&cfg, i, 0);
	general_write(&cfg, 0xFF, LCD_MODE_DATA);
    }
    for (int i = 1; i < 16; i+=2) {
	set_cursor(&cfg, i, 1);
	general_write(&cfg, 0xFF, LCD_MODE_DATA);
    }
    for (int i = 0; i < 5; i++) {
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
