/*
 * main.c - by Aftersol - An example project that demonstrates how to set
 * up saving and reading a file from an SD card with libdragon.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org>
 * 
*/

#include <libdragon.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>

int main(void) {

    char text_buffer[512];

    uint32_t bin_buffer[128];

    uint32_t sav_bin[128];

    rdpq_font_t *font;

    uint32_t seed;
    
    /* Init logging */
    debug_init_isviewer();
    debug_init_usblog();
    
    /* Init display and peripherals */
    display_init(
        RESOLUTION_320x240,
        DEPTH_16_BPP,
        3,
        GAMMA_NONE,
        FILTERS_DISABLED
    );

    joypad_init();

    timer_init();

    rdpq_init();

    /* Initialize the random number generator, then call rand() every
    frame so to get random behavior also in emulators. */
    getentropy(&seed, sizeof(seed));
    srand(seed);
    register_VI_handler((void(*)(void))rand);

    /* Loads font and register it to slot 1 */
    font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(1, font);

    /* Main loop */
    while (1) {
        joypad_buttons_t button_port_1;
        joypad_buttons_t button_port_2;
        joypad_buttons_t button_port_3;
        joypad_buttons_t button_port_4;

        joypad_buttons_t button_port_1_held;
        joypad_buttons_t button_port_2_held;
        joypad_buttons_t button_port_3_held;
        joypad_buttons_t button_port_4_held;

        surface_t *disp;

        uint8_t i;

        while(!(disp = display_try_get())) {;}

        rdpq_attach(disp, NULL);

        rdpq_set_mode_fill(RGBA32(0, 0, 0, 255));
        rdpq_fill_rectangle(0, 0, 320, 240);

        rdpq_set_mode_standard();

        rdpq_text_printf(
            &(rdpq_textparms_t) {
                .width = 320-32,
                .align = ALIGN_LEFT,
                .wrap = WRAP_WORD,
            }, 
            1, 
            32, 
            32, 
            "Requires a Real N64 Game Console\n"
            "Don\'t run this on emulators\n"
            "Press A or B to write or read random numbers to the SD card\n"
            "Hold Start and press A or B to write or read example text file.\n"
            "Current text file content: %s", 
            text_buffer
        );
        

        joypad_poll();

        button_port_1 = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        button_port_2 = joypad_get_buttons_pressed(JOYPAD_PORT_2);
        button_port_3 = joypad_get_buttons_pressed(JOYPAD_PORT_3);
        button_port_4 = joypad_get_buttons_pressed(JOYPAD_PORT_4);

        button_port_1_held = joypad_get_buttons_held(JOYPAD_PORT_1);
        button_port_2_held = joypad_get_buttons_held(JOYPAD_PORT_2);
        button_port_3_held = joypad_get_buttons_held(JOYPAD_PORT_3);
        button_port_4_held = joypad_get_buttons_held(JOYPAD_PORT_4);

        /* Randomize numbers */
        for (i = 0; i < 128; i++) {
            bin_buffer[i] = rand();
        }

        if (
            (button_port_1_held.start && button_port_1.a) ||
            (button_port_2_held.start && button_port_2.a) ||
            (button_port_3_held.start && button_port_3.a) ||
            (button_port_4_held.start && button_port_4.a)
        ) {
            bool sd_mounted = debug_init_sdfs("sd:/", -1);

            if (sd_mounted) {
                FILE* txt_file = fopen("sd:/sav.txt", "w");
                if (txt_file) {
                    char txt[512];

                    memset(txt, 0, sizeof(txt));

                    sprintf(txt,
                        "Hello, N64brew community!\n"
                        "Example text file.\n"
                        "The quick brown fox jumps over the lazy dog.\n"
                        "Random number: %u", rand()
                    );

                    fwrite(txt, sizeof(char), 512, txt_file);
                    fclose(txt_file);
                }
            }
            else {
                memset(text_buffer, 0, sizeof(text_buffer));
                sprintf(text_buffer, "Failed to mount SD card.");
            }

            debug_close_sdfs();
        } else if (
            (button_port_1_held.start && button_port_1.b) ||
            (button_port_2_held.start && button_port_2.b) ||
            (button_port_3_held.start && button_port_3.b) ||
            (button_port_4_held.start && button_port_4.b)
        ) {
            bool sd_mounted = debug_init_sdfs("sd:/", -1);

            if (sd_mounted) {
                FILE* txt_file = fopen("sd:/sav.txt", "r");
                if (txt_file) {
                    fread(text_buffer, sizeof(char), 512, txt_file);
                    fclose(txt_file);
                }
            }
            else {
                memset(text_buffer, 0, sizeof(text_buffer));
                sprintf(text_buffer, "Failed to mount SD card.");
            }


            debug_close_sdfs();
        }
        else {
            /* If A is pressed, write random numbers to the SD card */
            if (
                button_port_1.a ||
                button_port_2.a ||
                button_port_3.a ||
                button_port_4.a
            )
                {
                bool sd_mounted = debug_init_sdfs("sd:/", -1);

                if (sd_mounted) {
                    FILE* bin_file = fopen("sd:/sav.bin", "wb");
                    uint8_t scratch[4];
                    unsigned int num;

                    scratch[0] = (bin_buffer[0] >> 24) & 0xFF;
                    scratch[1] = (bin_buffer[0] >> 16) & 0xFF;
                    scratch[2] = (bin_buffer[0] >> 8) & 0xFF;
                    scratch[3] = bin_buffer[0] & 0xFF;

                    num = (scratch[0] << 24) |
                    (scratch[1] << 16) |
                    (scratch[2] << 8) |
                    scratch[3];

                    if (bin_file) {
                        memset(bin_buffer, 0, sizeof(bin_buffer));
                        fwrite(bin_buffer, sizeof(uint32_t), 128, bin_file);
                        memset(text_buffer, 0, sizeof(text_buffer));
                        sprintf(
                            text_buffer, 
                            "Wrote random numbers to SD card.\n"
                            "First number: %u",
                            num
                        );
                        fclose(bin_file);
                    }
                } else {
                    memset(text_buffer, 0, sizeof(text_buffer));
                    sprintf(text_buffer, "Failed to mount SD card.");
                }

                debug_close_sdfs();
            }

            /* 
                If B is pressed, read the numbers back from the SD card and
                store them in sav_bin 
            */
            if (
                button_port_1.b ||
                button_port_2.b ||
                button_port_3.b ||
                button_port_4.b
            ) {
                bool sd_mounted = debug_init_sdfs("sd:/", -1);

                if (sd_mounted) {
                    FILE* bin_file = fopen("sd:/sav.bin", "rb");
                    uint8_t scratch[4];
                    unsigned int num;

                    if (bin_file) {
                        memset(sav_bin, 0, sizeof(sav_bin));
                        memset(text_buffer, 0, sizeof(text_buffer));

                        fread(sav_bin, sizeof(uint32_t), 128, bin_file);

                        scratch[0] = (sav_bin[0] >> 24) & 0xFF;
                        scratch[1] = (sav_bin[0] >> 16) & 0xFF;
                        scratch[2] = (sav_bin[0] >> 8) & 0xFF;
                        scratch[3] = sav_bin[0] & 0xFF;

                        num = (scratch[0] << 24) |
                        (scratch[1] << 16) |
                        (scratch[2] << 8) |
                        scratch[3];
                        
                        sprintf(
                            text_buffer,
                            "Read random numbers from SD card.\n"
                            "First number: %u",
                            num
                        );

                        fclose(bin_file);
                    }
                } else {
                    memset(text_buffer, 0, sizeof(text_buffer));
                    sprintf(text_buffer, "Failed to mount SD card.");
                }

                debug_close_sdfs();
            }
        }

        rdpq_detach_show();

    }
    
    return 0;
}