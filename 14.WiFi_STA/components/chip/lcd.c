#include "lcd.h"

uint8_t lcd_buf[153600]; // 240*320*2 = 153600 bytes for 2.4" LCD
static uint16_t lcd_width = LCD_WIDTH;   // 当前LCD宽度
static uint16_t lcd_height = LCD_HEIGHT; // 当前LCD高度
static lcd_font_size_t lcd_current_font_size = FONT_SIZE_32; // 当前字体大小，默认32x16

/**
 * @brief 设置LCD字体大小
 * @param size 字体大小枚举值
 */
void lcd_set_font_size(lcd_font_size_t size)
{
    lcd_current_font_size = size;
}

/**
 * @brief 获取当前LCD字体大小
 * @return 当前字体大小枚举值
 */
lcd_font_size_t lcd_get_font_size(void)
{
    return lcd_current_font_size;
}

/**
 * @brief 获取字体的宽度（像素）
 * @param size 字体大小枚举值
 * @return 字体宽度
 */
static uint8_t lcd_get_font_width(lcd_font_size_t size)
{
    switch (size)
    {
        case FONT_SIZE_12: return 6;   // 12x6
        case FONT_SIZE_16: return 8;   // 16x8
        case FONT_SIZE_24: return 12;  // 24x12
        case FONT_SIZE_32: return 16;  // 32x16
        default: return 16;
    }
}

/**
 * @brief 获取字体的高度（像素）
 * @param size 字体大小枚举值
 * @return 字体高度
 */
static uint8_t lcd_get_font_height(lcd_font_size_t size)
{
    switch (size)
    {
        case FONT_SIZE_12: return 12;  // 12x6
        case FONT_SIZE_16: return 16;  // 16x8
        case FONT_SIZE_24: return 24;  // 24x12
        case FONT_SIZE_32: return 32;  // 32x16
        default: return 32;
    }
}

/**
 * @brief 获取字体数据的字节数
 * @param size 字体大小枚举值
 * @return 字体数据字节数
 */
static uint8_t lcd_get_font_bytes(lcd_font_size_t size)
{
    switch (size)
    {
        case FONT_SIZE_12: return 12;  // 12x6 = 72 bits = 12 bytes (每行6像素，12行)
        case FONT_SIZE_16: return 16;  // 16x8 = 128 bits = 16 bytes (每行8像素，16行)
        case FONT_SIZE_24: return 48;  // 24x12 = 288 bits = 48 bytes (每行12像素，24行)
        case FONT_SIZE_32: return 64;  // 32x16 = 512 bits = 64 bytes (每行16像素，32行)
        default: return 64;
    }
}

static void lcd_write_cmd(uint8_t cmd)
{
    LCD_DC(0);
    spi2_write_data(&cmd, 1);
}

static void lcd_write_data(uint8_t data)
{
    LCD_DC(1);
    spi2_write_data(&data, 1);
}

static void lcd_write_data16(uint16_t data)
{
    uint8_t databuf[2] = {0, 0};
    databuf[0] = data >> 8;
    databuf[1] = data & 0xFF;
    LCD_DC(1);
    spi2_write_data(databuf, 2);
}

static void lcd_write_datan(uint8_t *data, uint16_t length)
{
    LCD_DC(1);
    spi2_write_data(data, length);
}

static void lcd_hard_reset(void)
{
    LCD_RST(0);
    vTaskDelay(100);
    LCD_RST(1);
    vTaskDelay(100);
}

void lcd_on(void)
{
    LCD_BLK(1);
    vTaskDelay(10);
}

void lcd_off(void)
{
    LCD_BLK(0);
    vTaskDelay(10);
}

static void lcd_set_window(uint16_t xstar, uint16_t ystar, uint16_t xend, uint16_t yend)
{
    lcd_write_cmd(0x2a);
    lcd_write_data16(xstar);
    lcd_write_data16(xend);
    lcd_write_cmd(0x2b);
    lcd_write_data16(ystar);
    lcd_write_data16(yend);
    lcd_write_cmd(0x2c);
}

void lcd_clear(uint16_t color)
{
    uint16_t i;
    uint32_t j;
    uint8_t data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    lcd_set_window(0, 0, lcd_width - 1, lcd_height - 1);

    // 填充缓冲区：240*320*2/10 = 15360 字节
    for (j = 0; j < 15360 / 2; j++)
    {
        lcd_buf[j * 2] = data[0];
        lcd_buf[j * 2 + 1] = data[1];
    }

    // 分10次发送，每次15360字节，总共153600字节
    for (i = 0; i < 10; i++)
    {
        lcd_write_datan(lcd_buf, 15360);
    }
}

void lcd_set_direction(uint8_t direction)
{
    lcd_write_cmd(0x36);
    switch (direction)
    {
        case LCD_PORTRAIT_0:      // 竖屏模式0：0度
            lcd_write_data(0x00);
            lcd_width = LCD_WIDTH;
            lcd_height = LCD_HEIGHT;
            break;
        case LCD_LANDSCAPE_90:    // 横屏模式1：90度
            lcd_write_data(0x60);
            lcd_width = LCD_HEIGHT;
            lcd_height = LCD_WIDTH;
            break;
        case LCD_PORTRAIT_180:    // 竖屏模式2：180度
            lcd_write_data(0xC0);
            lcd_width = LCD_WIDTH;
            lcd_height = LCD_HEIGHT;
            break;
        case LCD_LANDSCAPE_270:   // 横屏模式3：270度
            lcd_write_data(0xA0);
            lcd_width = LCD_HEIGHT;
            lcd_height = LCD_WIDTH;
            break;
        default:
            lcd_write_data(0x00);
            lcd_width = LCD_WIDTH;
            lcd_height = LCD_HEIGHT;
            break;
    }
}

void lcd_init(void)
{
    spi2_init();
    xl9555_init();

    /* 配置XL9555的P1.0和P1.2为输出模式 */
    xl9555_set_pin_mode(1, 0, 0); /* P1.0 (LCD_BLK) 设为输出模式 */
    xl9555_set_pin_mode(1, 2, 0); /* P1.2 (LCD_RST) 设为输出模式 */

    spi_device_interface_config_t spidevice_structure = {0};
    spidevice_structure.clock_source = SPI_CLK_SRC_DEFAULT;
    spidevice_structure.clock_speed_hz = 60000000;
    spidevice_structure.mode = 0;
    spidevice_structure.queue_size = 7;
    spidevice_structure.spics_io_num = GPIO_NUM_21;
    spi_bus_add_device(SPI2_HOST, &spidevice_structure, &spi2_handle);

    gpio_config_t gpio_init_struct;

    /* LCD_DC管脚 (GPIO_NUM_40) */
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;        /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;              /* 配置输出模式 */
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_NUM_40;   /* 配置引脚位掩码 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE; /* 失能下拉 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;      /* 使能上拉 */
    gpio_config(&gpio_init_struct);                        /* 引脚配置 */

    lcd_hard_reset();
    lcd_on();
    vTaskDelay(100);

    lcd_write_cmd(0x11);
    vTaskDelay(120);

    lcd_write_cmd(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    lcd_write_cmd(0x35);
    lcd_write_data(0x00);

    lcd_write_cmd(0x36);
    lcd_write_data(0x70);

    lcd_write_cmd(0x3A);
    lcd_write_data(0x05);

    lcd_write_cmd(0xB7);
    lcd_write_data(0x35);

    lcd_write_cmd(0xBB);
    lcd_write_data(0x2D);

    lcd_write_cmd(0xC0);
    lcd_write_data(0x2C);

    lcd_write_cmd(0xC2);
    lcd_write_data(0x01);

    lcd_write_cmd(0xC3);
    lcd_write_data(0x15);

    lcd_write_cmd(0xC4);
    lcd_write_data(0x20);

    lcd_write_cmd(0xC6);
    lcd_write_data(0x0F);

    lcd_write_cmd(0xD0);
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xD6);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xE0);
    lcd_write_data(0x70);
    lcd_write_data(0x05);
    lcd_write_data(0x0A);
    lcd_write_data(0x0B);
    lcd_write_data(0x0A);
    lcd_write_data(0x27);
    lcd_write_data(0x2F);
    lcd_write_data(0x44);
    lcd_write_data(0x47);
    lcd_write_data(0x37);
    lcd_write_data(0x14);
    lcd_write_data(0x14);
    lcd_write_data(0x29);
    lcd_write_data(0x2F);

    lcd_write_cmd(0xE1);
    lcd_write_data(0x70);
    lcd_write_data(0x07);
    lcd_write_data(0x0C);
    lcd_write_data(0x08);
    lcd_write_data(0x08);
    lcd_write_data(0x04);
    lcd_write_data(0x2F);
    lcd_write_data(0x33);
    lcd_write_data(0x46);
    lcd_write_data(0x18);
    lcd_write_data(0x15);
    lcd_write_data(0x15);
    lcd_write_data(0x2B);
    lcd_write_data(0x2D);

    lcd_write_cmd(0x21);
    lcd_write_cmd(0x29);
    lcd_write_cmd(0x2C);

    lcd_set_direction(LCD_PORTRAIT_0);
    lcd_clear(BLACK);
}

static void lcd_set_cursor(uint16_t xpos, uint16_t ypos)
{
    lcd_set_window(xpos, ypos, xpos, ypos);
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_cursor(x, y);
    lcd_write_data16(color);
}

void lcd_show_char(uint8_t line, uint8_t column, uint8_t chr, uint16_t fontcolor, uint16_t backgroundcolor)
{
    uint8_t i, j = 0;
    uint8_t chr_temp = 0;
    uint8_t font_width = lcd_get_font_width(lcd_current_font_size);
    uint8_t font_height = lcd_get_font_height(lcd_current_font_size);
    uint8_t font_bytes = lcd_get_font_bytes(lcd_current_font_size);
    const unsigned char *font_data = NULL;
    
    // 根据当前字体大小选择对应的字体数据
    switch (lcd_current_font_size)
    {
        case FONT_SIZE_12:
            font_data = asc2_1206[chr - ' '];
            break;
        case FONT_SIZE_16:
            font_data = asc2_1608[chr - ' '];
            break;
        case FONT_SIZE_24:
            font_data = asc2_2412[chr - ' '];
            break;
        case FONT_SIZE_32:
            font_data = asc2_3216[chr - ' '];
            break;
        default:
            font_data = asc2_3216[chr - ' '];
            break;
    }
    
    // 设置字符显示窗口
    lcd_set_window((column - 1) * font_width, (line - 1) * font_height, 
                   column * font_width - 1, line * font_height - 1);
    
    // 根据字体大小选择不同的扫描方式
    if (lcd_current_font_size == FONT_SIZE_32)
    {
        // 32x16字体：数据按小端序存储，每2字节为一行的16位数据
        // 共32行，每行16位，共64字节
        for (i = 0; i < 32; i++)  // 32行
        {
            uint16_t row_data = (font_data[i * 2 + 1] << 8) | font_data[i * 2];  // 交换字节序
            for (j = 0; j < 16; j++)  // 16列
            {
                if (row_data & (0x01 << j))
                {
                    lcd_write_data16(fontcolor);
                }
                else
                {
                    lcd_write_data16(backgroundcolor);
                }
            }
        }
    }
    else if (lcd_current_font_size == FONT_SIZE_24)
    {
        // 24x12字体：数据按大端序存储，每2字节为一行的16位数据（但只用高12位）
        // 共24行，每行12位（存储在16位中），共48字节
        for (i = 0; i < 24; i++)  // 24行
        {
            uint16_t row_data = (font_data[i * 2] << 8) | font_data[i * 2 + 1];  // 大端序：高字节在前
            for (j = 0; j < 12; j++)  // 12列
            {
                if (row_data & (0x8000 >> j))  // 从高位开始扫描（MSB first）
                {
                    lcd_write_data16(fontcolor);
                }
                else
                {
                    lcd_write_data16(backgroundcolor);
                }
            }
        }
    }
    else if (lcd_current_font_size == FONT_SIZE_16)
    {
        // 16x8字体：数据按每字节存储一行的8位数据
        // 共16行，每行8位，共16字节
        for (i = 0; i < 16; i++)  // 16行
        {
            chr_temp = font_data[i];
            for (j = 0; j < 8; j++)  // 8列
            {
                if (chr_temp & (0x80 >> j))  // 从高位到低位扫描
                {
                    lcd_write_data16(fontcolor);
                }
                else
                {
                    lcd_write_data16(backgroundcolor);
                }
            }
        }
    }
    else  // FONT_SIZE_12
    {
        // 12x6字体：数据按每字节存储一行的8位数据（但只用高6位）
        // 共12行，每行6位（存储在8位中），共12字节
        for (i = 0; i < 12; i++)  // 12行
        {
            chr_temp = font_data[i];
            for (j = 0; j < 6; j++)  // 6列
            {
                if (chr_temp & (0x80 >> j))  // 从高位到低位扫描
                {
                    lcd_write_data16(fontcolor);
                }
                else
                {
                    lcd_write_data16(backgroundcolor);
                }
            }
        }
    }
}

void lcd_show_string(uint8_t line, uint8_t column, char *string, uint16_t fontcolor, uint16_t backgroundcolor)
{
    uint8_t i = 0;
    for (i = 0; string[i] != '\0'; i++)
    {
        lcd_show_char(line, column + i, string[i], fontcolor, backgroundcolor);
    }
}

uint32_t lcd_pow(uint32_t x, uint32_t y)
{
    uint32_t Result = 1;
    while (y--)
    {
        Result *= x;
    }
    return Result;
}

void lcd_show_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length, uint16_t fontcolor, uint16_t backgroundcolor)
{
    uint8_t i;
    for (i = 0; i < length; i++)
    {
        lcd_show_char(line, column + i, number / lcd_pow(10, length - i - 1) % 10 + '0', fontcolor, backgroundcolor);
    }
}

void lcd_show_hexnum(uint8_t line, uint8_t column, uint32_t number, uint8_t length, uint16_t fontcolor, uint16_t backgroundcolor)
{
    uint8_t i, singlenumber;
    for (i = 0; i < length; i++)
    {
        singlenumber = number / lcd_pow(16, length - i - 1) % 16;
        if (singlenumber < 10)
        {
            lcd_show_char(line, column + i, singlenumber + '0', fontcolor, backgroundcolor);
        }
        else
        {
            lcd_show_char(line, column + i, singlenumber - 10 + 'A', fontcolor, backgroundcolor);
        }
    }
}

void lcd_show_float(uint8_t line, uint8_t column, float number, uint8_t length, uint16_t fontcolor, uint16_t backgroundcolor)
{
    uint8_t i;
    uint32_t temp;
    uint32_t number1 = number * 100;
    for (i = 0; i < length; i++)
    {
        temp = (number1 / lcd_pow(10, length - i - 1)) % 10;
        if (i == (length - 2))
        {
            lcd_show_char(line, column + length - 2, '.', fontcolor, backgroundcolor);
            i++;
            length += 1;
        }
        lcd_show_num(line, column + i, temp, 1, fontcolor, backgroundcolor);
    }
}

void lcd_show_picture(uint8_t *img)
{
    unsigned long i = 0;
    unsigned long j = 0;
    lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    /* lcd_buf存储摄像头整一帧RGB数据 */
    for (j = 0; j < LCD_WIDTH * LCD_HEIGHT; j++)
    {
        lcd_buf[2 * j] = img[2 * i];
        lcd_buf[2 * j + 1] = img[2 * i + 1];
        i++;
    }

    /* 例如：240*320*2/15360 = 10;分10次发送RGB数据 */
    for (j = 0; j < (LCD_WIDTH * LCD_HEIGHT * 2 / 15360); j++)
    {
        /* &lcd_buf[j * LCD_BUF_SIZE] 偏移地址发送数据 */
        lcd_write_datan(&lcd_buf[j * 15360], 15360);
    }
}