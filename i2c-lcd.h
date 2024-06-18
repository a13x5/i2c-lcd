#ifndef I2C_LCD_H
#define I2C_LCD_H

/*
 * Pin layout as it for PCF8574T i2c expander card.
 */
#define PIN_DB7 0x80
#define PIN_DB6 0x40
#define PIN_DB5 0x20
#define PIN_DB4 0x10
#define PIN_BL 0x08
#define PIN_E 0x04
#define PIN_RW 0x02
#define PIN_RS 0x01

/*
 * Data send modes.
 * Its either instruction/command or data.
 */
#define LCD_MODE_CMD 0x00
#define LCD_MODE_DATA PIN_RS

/*
 * Display commands
 */
#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_HOME 0x02

/*
 * Backlight control
 */
#define LCD_BACKLIGHT_ON PIN_BL
#define LCD_BACKLIGHT_OFF 0x00

/*
 * Display control instructions.
 * Ref. HD44780 manual pages 23-28.
 */
#define LCD_FUNCTION_SET 0x20
#define LCD_FUNCTION_4BIT 0x00
#define LCD_FUNCTION_2LINES 0x08
#define LCD_FUNCTION_5x8DOTS 0x00
#define LCD_FUNCTION_5x10DOTS 0x04

#define LCD_CONTROL_SET 0x08
#define LCD_CONTROL_DISPLAY_ON 0x04
#define LCD_CONTROL_DISPLAY_OFF 0x00
#define LCD_CONTROL_CURSOR_ON 0x02
#define LCD_CONTROL_CURSOR_OFF 0x00
#define LCD_CONTROL_BLINK_ON 0x01
#define LCD_CONTROL_BLINK_OFF 0x00

#define LCD_ENTRYMODE_SET 0x04
#define LCD_ENTRYMODE_LEFT 0x02
#define LCD_ENTRYMODE_DDRAM_DECREMENT 0x00

#define LCD_SHIFT_SET 0x10
#define LCD_SHIFT_CURSOR 0x00
#define LCD_SHIFT_DISPLAY 0x08
#define LCD_SHIFT_RIGHT 0x04
#define LCD_SHIFT_LEFT 0x00

#define LCD_DDRAM_ADDR_SET 0x80
#define LCD_DDRAM_2ND_ROW_ADDR 0x40
#define LCD_CGRAM_ADDR_SET 0x40

typedef struct {
    int i2c_fd;
    unsigned char backlight;
    unsigned char lcd_function;
    unsigned char lcd_control;
    unsigned char lcd_entrymode;
} lcd_config;

void general_write(lcd_config *cfg, unsigned char byte, unsigned char mode);
void clk_write(int fd, unsigned char byte);
void init_lcd(lcd_config *cfg);
void string_write(lcd_config *cfg, char str[]);
void clear_display(lcd_config *cfg);
void return_home(lcd_config *cfg);
void move_screen(lcd_config *cfg, unsigned int chars);
void set_cursor(lcd_config *cfg, unsigned int col, unsigned int row);
void apply_lcd_control(lcd_config *cfg);
void cgram_write(lcd_config *cfg, unsigned int char_addr, unsigned char char_map[8]);
lcd_config configure(char i2c_filename[], unsigned char i2c_addr);

#endif
