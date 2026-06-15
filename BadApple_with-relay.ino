#include <TFT_eSPI.h>
#include <SPI.h>
#include "bad_apple.h"

#define RELAY_PIN 14

#define INVERT_VIDEO true
#define INVERT_RELAY true // Rơ-le

TFT_eSPI tft = TFT_eSPI();
uint8_t frame_buf[160 * 120];
uint16_t line_buf[320];

void setup()
{
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(INVERT_VIDEO ? TFT_BLACK : TFT_WHITE);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, INVERT_RELAY ? HIGH : LOW);
    delay(500); // Delay khởi tạo chống nhiễu rơ-le
}

void loop()
{
    uint32_t data_idx = 0;
    const uint32_t frame_pixels = 160 * 120;
    const uint32_t frame_delay = 1000 / 15;

    for (uint32_t f = 0; f < bad_apple_frames_count; f++)
    {
        uint32_t next_frame_time = millis() + frame_delay;
        uint32_t pixel_cnt = 0;

        // 1. Giải nén RLE
        while (pixel_cnt < frame_pixels && data_idx < bad_apple_data_size)
        {
            uint8_t b = pgm_read_byte(&bad_apple_data[data_idx++]);
            uint8_t color = (b & 0x80) ? 1 : 0;
            uint8_t len = b & 0x7F;

            for (uint8_t i = 0; i < len && pixel_cnt < frame_pixels; i++)
            {
                frame_buf[pixel_cnt++] = color;
            }
        }

        // 2. Kiểm tra định vị 4 góc màn hình
        uint8_t white_value = INVERT_VIDEO ? 0 : 1;
        uint8_t white_corners = 0;

        if (frame_buf[0] == white_value)
            white_corners++; // Trên - Trái
        if (frame_buf[159] == white_value)
            white_corners++; // Trên - Phải
        if (frame_buf[119 * 160] == white_value)
            white_corners++; // Dưới - Trái
        if (frame_buf[119 * 160 + 159] == white_value)
            white_corners++; // Dưới - Phải

        // 3. Kích rơ-le
        if (white_corners >= 3)
        {
            digitalWrite(RELAY_PIN, INVERT_RELAY ? LOW : HIGH);
        }
        else
        {
            digitalWrite(RELAY_PIN, INVERT_RELAY ? HIGH : LOW);
        }

        // 4. Render hình x2
        for (int y = 0; y < 120; y++)
        {
            for (int x = 0; x < 160; x++)
            {
                uint16_t color = INVERT_VIDEO ? (frame_buf[y * 160 + x] ? TFT_BLACK : TFT_WHITE)
                                              : (frame_buf[y * 160 + x] ? TFT_WHITE : TFT_BLACK);
                line_buf[x * 2] = color;
                line_buf[x * 2 + 1] = color;
            }
            tft.pushImage(0, y * 2, 320, 1, line_buf);
            tft.pushImage(0, y * 2 + 1, 320, 1, line_buf);
        }

        while (millis() < next_frame_time)
        {
            yield();
        }
    }

    digitalWrite(RELAY_PIN, INVERT_RELAY ? HIGH : LOW);
    delay(300); // Delay loop
}