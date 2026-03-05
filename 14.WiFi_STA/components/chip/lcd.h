#ifndef __LCD_H_
#define __LCD_H_

#include <stdint.h>
#include "lcdfont.h"
#include "spi.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "xl9555.h"

/* xl9555 P1.2 */
#define LCD_RST(x) x ? xl9555_set_pin(1, 2, 1) : xl9555_set_pin(1, 2, 0)

/* GPIO_NUM_40 */
#define LCD_DC(x) x ? gpio_set_level(GPIO_NUM_40, 1) : gpio_set_level(GPIO_NUM_40, 0)

/* GPIO_NUM_21 */
#define LCD_CS(x) x ? gpio_set_level(GPIO_NUM_21, 1) : gpio_set_level(GPIO_NUM_21, 0)

/* xl9555 P1.0 */
#define LCD_BLK(x) x ? xl9555_set_pin(1, 0, 1) : xl9555_set_pin(1, 0, 0)

#define WHITE 0xFFFF      /* 白色 */
#define BLACK 0x0000      /* 黑色 */
#define RED 0xF800        /* 红色 */
#define GREEN 0x07E0      /* 绿色 */
#define BLUE 0x001F       /* 蓝色 */
#define MAGENTA 0XF81F    /* 品红色/紫红色 = BLUE + RED */
#define YELLOW 0XFFE0     /* 黄色 = GREEN + RED */
#define CYAN 0X07FF       /* 青色 = GREEN + BLUE */
#define BROWN 0XBC40      /* 棕色 */
#define BRRED 0XFC07      /* 棕红色 */
#define GRAY 0X8430       /* 灰色 */
#define DARKBLUE 0X01CF   /* 深蓝色 */
#define LIGHTBLUE 0X7D7C  /* 浅蓝色 */
#define GRAYBLUE 0X5458   /* 灰蓝色 */
#define LIGHTGREEN 0X841F /* 浅绿色 */
#define LGRAY 0XC618      /* 浅灰色(PANNEL),窗体背景色 */
#define LGRAYBLUE 0XA651  /* 浅灰蓝色(中间层颜色) */
#define LBBLUE 0X2B12     /* 浅棕蓝色(选择条目的反色) */

/* LCD屏幕尺寸定义 */
#define LCD_WIDTH 240  /* LCD宽度 */
#define LCD_HEIGHT 320 /* LCD高度 */

/* LCD显示方向定义 */
#define LCD_PORTRAIT_0 0    /* 竖屏模式0：0度 */
#define LCD_LANDSCAPE_90 1  /* 横屏模式1：90度 */
#define LCD_PORTRAIT_180 2  /* 竖屏模式2：180度 */
#define LCD_LANDSCAPE_270 3 /* 横屏模式3：270度 */

/* LCD字体大小定义 */
typedef enum
{
    FONT_SIZE_12 = 0, /* 12x6字体 */
    FONT_SIZE_16 = 1, /* 16x8字体 */
    FONT_SIZE_24 = 2, /* 24x12字体 */
    FONT_SIZE_32 = 3  /* 32x16字体 (默认) */
} lcd_font_size_t;

/* 初始化与配置 */
void lcd_init(void);                          /* 初始化LCD */
void lcd_clear(uint16_t color);               /* 清屏为指定颜色 */
void lcd_set_direction(uint8_t direction);    /* 设置显示方向 */
void lcd_set_font_size(lcd_font_size_t size); /* 设置字体大小 */
lcd_font_size_t lcd_get_font_size(void);      /* 获取当前字体大小 */

/* 图形绘制 */
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color); /* 绘制像素点 */

/* 文字显示 */
void lcd_show_char(uint8_t line, uint8_t column, uint8_t chr, uint16_t fontcolor, uint16_t backgroundcolor);                       /* 显示单个字符 */
void lcd_show_string(uint8_t line, uint8_t column, char *string, uint16_t fontcolor, uint16_t backgroundcolor);                    /* 显示字符串 */
void lcd_show_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length, uint16_t fontcolor, uint16_t backgroundcolor);    /* 显示十进制数字 */
void lcd_show_hexnum(uint8_t line, uint8_t column, uint32_t number, uint8_t length, uint16_t fontcolor, uint16_t backgroundcolor); /* 显示十六进制数字 */
void lcd_show_float(uint8_t line, uint8_t column, float number, uint8_t length, uint16_t fontcolor, uint16_t backgroundcolor);     /* 显示浮点数 */

/* 图像显示 */
void lcd_show_picture(uint8_t *img); /* 显示全屏图像 */

#endif
