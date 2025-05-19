#include "oled_ui.h"

#include <U8g2lib.h>

#include "ble_mijia.h"
#include "network.h"
#include "status.h"

// 屏幕配置
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void initUI() {
    u8g2.begin();
}

void drawUI() {
    u8g2.clearBuffer();

    // === 中框体 ===
    u8g2.drawFrame(18, 0, 68, 64);
    u8g2.drawFrame(18, 0, 53, 23);

    // 龙猫
    u8g2.setFont(u8g2_font_unifont_t_animals);
    u8g2.drawGlyph(40, 62, 0x004d);

    // === 温湿度 ===
    // temp1 & temp2
    u8g2.setFont(u8g2_font_profont17_tr);
    u8g2.setCursor(21, 19);
    u8g2.printf("%.1f", temp1);
    u8g2.setCursor(21, 41);
    u8g2.printf("%.1f", temp2);
    // temp1 & temp2 ℃
    u8g2.setFont(u8g2_font_9x15_t_symbols);
    u8g2.drawGlyph(59, 19, 0x2103);
    u8g2.drawGlyph(59, 41, 0x2103);
    // temp3
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(92, 61);
    u8g2.printf("%.1f", temp3);
    // temp3 ℃
    u8g2.setFont(u8g2_font_6x12_t_symbols);
    u8g2.drawGlyph(116, 61, 0x2103);
    // ambient temp & humidity
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(92, 30);
    u8g2.printf("%.1f", ambient_temp);
    u8g2.setCursor(92, 42);
    u8g2.printf("%d%%", ambient_humidity);
    // ambient temp ℃
    u8g2.setFont(u8g2_font_6x12_t_symbols);
    u8g2.drawGlyph(116, 30, 0x2103);

    // === 图标 ===
    // 制冷 & 风扇 & PWM
    if (coolingStatus) {
        u8g2.setFont(u8g2_font_unifont_t_78_79); // 16x16
        u8g2.drawGlyph(0, 15, 0x2744); // cold

        u8g2.setFont(u8g2_font_waffle_t_all); // 12x12
        u8g2.drawGlyph(2, 33, 0xe1c0);
        // u8g2.setFont(u8g2_font_waffle_t_all); // 12x12
        // u8g2.drawGlyph(2, 33, 0xe235);

        u8g2.setFont(u8g2_font_profont12_tr);
        u8g2.setCursor(0, 47);
        u8g2.print(pwmValue);
    }

    // WiFi
    if (isWiFiConnected()) {
        u8g2.setFont(u8g2_font_waffle_t_all); // 12x12
        u8g2.drawGlyph(88, 15, 0xe2b5);
    }

    // MQTT
    if (isMQTTConnected()) {
        u8g2.setFont(u8g2_font_siji_t_6x10); // 12x12
        u8g2.drawGlyph(102, 15, 0xe02d);
    }

    // 蓝牙
    if (isBLEConnected()) {
        u8g2.setFont(u8g2_font_waffle_t_all); // 12x12
        u8g2.drawGlyph(116, 15, 0xe00b);
        // u8g2.setFont(u8g2_font_siji_t_6x10); // 12x12
        // u8g2.drawGlyph(116, 15, 0xe00b);
    }

    u8g2.sendBuffer();
}
