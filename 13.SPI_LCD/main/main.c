#include "main.h"

static const char *TAG = "main";

/**
 * @brief LCD API测试函数 - 演示所有LCD功能
 */
void app_main(void)
{
    ESP_LOGI(TAG, "LCD API测试程序启动...");

    /* ========== 测试1：初始化LCD ========== */
    ESP_LOGI(TAG, "测试1: 初始化LCD");
    lcd_init(); // 初始化LCD（必须首先调用）
    vTaskDelay(pdMS_TO_TICKS(500));

    /* ========== 测试2：清屏功能 ========== */
    ESP_LOGI(TAG, "测试2: 清屏功能");
    lcd_clear(BLACK); // 清屏为黑色
    vTaskDelay(pdMS_TO_TICKS(500));

    /* ========== 测试3：显示标题和系统信息 ========== */
    ESP_LOGI(TAG, "测试3: 显示标题");
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(1, 1, "LCD API TEST", CYAN, BLACK);

    lcd_set_font_size(FONT_SIZE_16);
    lcd_show_string(3, 1, "ESP32-S3 Board", GREEN, BLACK);
    lcd_show_string(4, 1, "240x320 Display", GREEN, BLACK);
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* ========== 测试4：字体大小演示 ========== */
    ESP_LOGI(TAG, "测试4: 字体大小演示");
    lcd_clear(BLACK);

    // 12号字体 (12x6)
    lcd_set_font_size(FONT_SIZE_12);
    lcd_show_string(1, 1, "12x6: Small font", WHITE, BLACK);

    // 16号字体 (16x8)
    lcd_set_font_size(FONT_SIZE_16);
    lcd_show_string(3, 1, "16x8: Normal font", YELLOW, BLACK);

    // 24号字体 (24x12)
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(5, 1, "24x12: Large", CYAN, BLACK);

    // 32号字体 (32x16)
    lcd_set_font_size(FONT_SIZE_32);
    lcd_show_string(8, 1, "32x16 XL", MAGENTA, BLACK);

    vTaskDelay(pdMS_TO_TICKS(3000));

    /* ========== 测试5：显示单个字符 ========== */
    ESP_LOGI(TAG, "测试5: 显示单个字符");
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(1, 1, "Single Chars:", WHITE, BLACK);

    lcd_set_font_size(FONT_SIZE_32);
    lcd_show_char(3, 1, 'A', RED, BLACK);
    lcd_show_char(3, 3, 'B', GREEN, BLACK);
    lcd_show_char(3, 5, 'C', BLUE, BLACK);
    lcd_show_char(3, 7, 'D', YELLOW, BLACK);
    lcd_show_char(3, 9, 'E', MAGENTA, BLACK);

    vTaskDelay(pdMS_TO_TICKS(2000));

    /* ========== 测试6：数字显示 ========== */
    ESP_LOGI(TAG, "测试6: 数字显示");
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(1, 1, "Number Display:", CYAN, BLACK);

    lcd_set_font_size(FONT_SIZE_16);
    lcd_show_string(3, 1, "Dec:", WHITE, BLACK);
    lcd_show_num(3, 6, 12345, 5, GREEN, BLACK);

    lcd_show_string(4, 1, "Hex:", WHITE, BLACK);
    lcd_show_hexnum(4, 6, 0xABCD, 4, YELLOW, BLACK);

    lcd_show_string(5, 1, "Float:", WHITE, BLACK);
    lcd_show_float(5, 7, 25.67, 5, RED, BLACK);

    vTaskDelay(pdMS_TO_TICKS(3000));

    /* ========== 测试7：动态数字更新 ========== */
    ESP_LOGI(TAG, "测试7: 动态数字更新");
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(1, 1, "Counter Test", MAGENTA, BLACK);

    lcd_set_font_size(FONT_SIZE_16);
    lcd_show_string(3, 1, "Count:", WHITE, BLACK);

    // 计数显示
    for (int i = 0; i <= 100; i += 5)
    {
        lcd_show_num(3, 8, i, 3, CYAN, BLACK);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    /* ========== 测试8：温度传感器模拟显示 ========== */
    ESP_LOGI(TAG, "测试8: 模拟传感器数据");
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(1, 1, "Sensor Data", YELLOW, BLACK);

    lcd_set_font_size(FONT_SIZE_16);
    lcd_show_string(3, 1, "Temp:", GREEN, BLACK);
    lcd_show_float(3, 7, 23.45, 5, GREEN, BLACK);
    lcd_show_char(3, 13, 'C', GREEN, BLACK);

    lcd_show_string(4, 1, "Humi:", CYAN, BLACK);
    lcd_show_float(4, 7, 65.20, 5, CYAN, BLACK);
    lcd_show_char(4, 13, '%', CYAN, BLACK);

    lcd_show_string(5, 1, "Press:", MAGENTA, BLACK);
    lcd_show_num(5, 8, 1013, 4, MAGENTA, BLACK);
    lcd_show_string(5, 13, "hPa", MAGENTA, BLACK);

    vTaskDelay(pdMS_TO_TICKS(3000));

    /* ========== 测试9：像素点绘制 ========== */
    ESP_LOGI(TAG, "测试9: 像素点绘制");
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(1, 1, "Pixel Draw", WHITE, BLACK);

    // 绘制一些像素点形成图案
    for (int i = 0; i < 50; i++)
    {
        lcd_draw_pixel(120 + i, 100, RED);
        lcd_draw_pixel(120 - i, 100, RED);
        lcd_draw_pixel(120, 100 + i, RED);
        lcd_draw_pixel(120, 100 - i, RED);
    }

    // 绘制对角线
    for (int i = 0; i < 100; i++)
    {
        lcd_draw_pixel(70 + i, 120 + i, GREEN);
        lcd_draw_pixel(170 - i, 120 + i, BLUE);
    }

    vTaskDelay(pdMS_TO_TICKS(3000));

    /* ========== 测试10：颜色显示 ========== */
    ESP_LOGI(TAG, "测试10: 颜色显示");
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_16);

    lcd_show_string(1, 1, "Color Palette:", WHITE, BLACK);
    lcd_show_string(3, 1, "RED", RED, BLACK);
    lcd_show_string(4, 1, "GREEN", GREEN, BLACK);
    lcd_show_string(5, 1, "BLUE", BLUE, BLACK);
    lcd_show_string(6, 1, "YELLOW", YELLOW, BLACK);
    lcd_show_string(7, 1, "CYAN", CYAN, BLACK);
    lcd_show_string(8, 1, "MAGENTA", MAGENTA, BLACK);
    lcd_show_string(9, 1, "WHITE", WHITE, BLACK);

    vTaskDelay(pdMS_TO_TICKS(3000));

    /* ========== 测试11：屏幕方向切换（可选） ========== */
    ESP_LOGI(TAG, "测试11: 屏幕方向切换");

    // 横屏模式90度
    lcd_set_direction(LCD_LANDSCAPE_90);
    lcd_clear(BLUE);
    lcd_set_font_size(FONT_SIZE_24);
    lcd_show_string(1, 1, "Landscape 90", YELLOW, BLUE);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 竖屏模式180度
    lcd_set_direction(LCD_PORTRAIT_180);
    lcd_clear(RED);
    lcd_show_string(1, 1, "Portrait 180", WHITE, RED);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 恢复默认竖屏模式
    lcd_set_direction(LCD_PORTRAIT_0);
    lcd_clear(BLACK);
    lcd_show_string(1, 1, "Portrait 0", GREEN, BLACK);
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* ========== 测试完成 ========== */
    ESP_LOGI(TAG, "所有测试完成！");
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_32);
    lcd_show_string(4, 1, "All Tests", GREEN, BLACK);
    lcd_show_string(5, 1, "Completed!", GREEN, BLACK);

    lcd_set_font_size(FONT_SIZE_16);
    lcd_show_string(8, 1, "LCD is ready!", CYAN, BLACK);

    /* ========== 主循环：显示动态时间 ========== */
    uint32_t counter = 0;
    lcd_clear(BLACK);
    lcd_set_font_size(FONT_SIZE_32);
    lcd_set_direction(LCD_LANDSCAPE_90);
    lcd_show_string(10, 1, "Runtime:", WHITE, BLACK);

    while (1)
    {
        // 每秒更新一次计数
        lcd_show_num(10, 10, counter, 5, YELLOW, BLACK);
        lcd_show_string(10, 16, "s", YELLOW, BLACK);
        counter++;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
