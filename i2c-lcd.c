#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include "i2c-lcd.h"

/*
 * Low level data write function.
 * Clock pin must be manually triggered each time data is transfered.
 * Data is transfered 3 times. Ref. HD44780 manual page 22
 */
void
clk_write(int fd, unsigned char byte)
{
    write(fd, &byte, 1);
    byte |= PIN_E;
    write(fd, &byte, 1);
    usleep(1);
    byte &= ~PIN_E;
    write(fd, &byte, 1);
    usleep(50);
}

/*
 * Writing data in 4 bit mode. So it should be splitted and send 2 times.
 * Ref. HD44780 manual page 22
 * 0xF0 is used to mask bits that don't contain data. 
 */
void
general_write(lcd_config *cfg, unsigned char byte, unsigned char mode)
{
    unsigned char high_order_bits = (byte & 0xF0) | mode | cfg->backlight;
    unsigned char low_order_bits = ((byte << 4) & 0xF0) | mode | cfg->backlight;
    clk_write(cfg->i2c_fd, high_order_bits);
    clk_write(cfg->i2c_fd, low_order_bits);
}

/*
 * HD44780 init sequence.
 * Ref. HD44780 manual pages 23 and 46.
 */
void
init_lcd(lcd_config *cfg)
{
    unsigned char byte;
    usleep(4000);
    byte = (PIN_DB5 | PIN_DB4 | cfg->backlight);
    clk_write(cfg->i2c_fd, byte);
    usleep(4500);
    clk_write(cfg->i2c_fd, byte);
    usleep(4500);
    clk_write(cfg->i2c_fd, byte);
    usleep(150);
    byte = (PIN_DB5 | cfg->backlight);
    clk_write(cfg->i2c_fd, byte);
    
    general_write(cfg, cfg->lcd_function, LCD_MODE_CMD);
    apply_lcd_control(cfg);
    general_write(cfg, cfg->lcd_entrymode, LCD_MODE_CMD);
    return_home(cfg);
}

void
apply_lcd_control(lcd_config *cfg)
{
    general_write(cfg, cfg->lcd_control, LCD_MODE_CMD);
}

void
string_write(lcd_config *cfg, char str[])
{
    for(int i = 0; i < strlen(str); i++) { 
	general_write(cfg, str[i], LCD_MODE_DATA);
    }
}

void
clear_display(lcd_config *cfg)
{
    general_write(cfg, LCD_CMD_CLEAR, LCD_MODE_CMD);
    usleep(4500);
}

void
return_home(lcd_config *cfg)
{
    general_write(cfg, LCD_CMD_HOME, LCD_MODE_CMD);
    usleep(1600);
}

void
move_screen(lcd_config *cfg, unsigned int chars)
{
    for(int i = 0; i < chars; i++) {
	general_write(cfg, (LCD_SHIFT_SET | LCD_SHIFT_DISPLAY | LCD_SHIFT_LEFT), LCD_MODE_CMD);
	usleep(500 * 1000);
    }
}
/*
 * Cursor is moved by setting specific address in DDRAM memory.
 * Ref. HD44780 manual pages 10-12.
 */
void
set_cursor(lcd_config *cfg, unsigned int col, unsigned int row)
{
    if (col > 40 ) {
	puts("Column number can't be more than 40");
	return;
    }
    
    switch (row) {
    case 1:
	general_write(cfg, (LCD_DDRAM_ADDR_SET | col), LCD_MODE_CMD);
	break;
    case 2:
	general_write(cfg, (LCD_DDRAM_ADDR_SET | LCD_DDRAM_2ND_ROW_ADDR + col), LCD_MODE_CMD);
	break;
    default:
	printf("Row values 0 and 1 supported, received %d\n", row);
	return;
    }
}

void
cgram_write(lcd_config *cfg, unsigned int char_addr, unsigned char char_map[8])
{
    char_addr &= 0x07;
    general_write(cfg, (LCD_CGRAM_ADDR_SET | (char_addr << 3)), LCD_MODE_CMD);
    for (int i = 0; i < 8; i++) {
        general_write(cfg, char_map[i], LCD_MODE_DATA);
    }
}

lcd_config
configure(char i2c_filename[], unsigned char i2c_addr)
{
    int i2c_fd = -1;
    i2c_fd = open(i2c_filename, O_RDWR);
    if (i2c_fd < 0) { 
	puts("Error opening i2c device");
	exit(1);
    }
    
    if (ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) {
	close(i2c_fd);
	puts("Error setting I2C device address");
	exit(1);
    }
    
    lcd_config config = {
	.i2c_fd = i2c_fd,
	.backlight = LCD_BACKLIGHT_ON,
	.lcd_function = (LCD_FUNCTION_SET |
			 LCD_FUNCTION_4BIT |
			 LCD_FUNCTION_2LINES |
			 LCD_FUNCTION_5x8DOTS),
	.lcd_control = (LCD_CONTROL_SET |
			LCD_CONTROL_DISPLAY_ON |
			LCD_CONTROL_CURSOR_ON |
			LCD_CONTROL_BLINK_OFF),
	.lcd_entrymode = (LCD_ENTRYMODE_SET |
			  LCD_ENTRYMODE_LEFT |
			  LCD_ENTRYMODE_DDRAM_DECREMENT),
    };
    
    return config;
}
