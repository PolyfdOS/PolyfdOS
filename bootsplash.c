#include "bootsplash.h"
#include "fb.h"
#include "io.h"

/** Simple delay function */
static void delay(unsigned int milliseconds)
{
    volatile unsigned int i, j;
    for (i = 0; i < milliseconds; i++) {
        for (j = 0; j < 10000; j++) {
            /* Busy wait */
        }
    }
}

/** Clear screen with black background */
static void clear_screen(void)
{
    int i;
    for (i = 0; i < 80 * 25; i++) {
        fb_write_cell(i * 2, ' ', FB_WHITE, FB_BLACK);
    }
}

/** Draw progress bar */
static void draw_progress_bar(int percent)
{
    int bar_width = 40;
    int filled = (bar_width * percent) / 100;
    int i;
    int start_col = (80 - bar_width - 2) / 2;
    int bar_line = 20;
    
    /* Draw [ */
    fb_write_cell((bar_line * 80 + start_col) * 2, '[', FB_LIGHT_CYAN, FB_BLACK);
    
    /* Draw progress */
    for (i = 0; i < bar_width; i++) {
        char c = (i < filled) ? '=' : ' ';
        unsigned char color = (i < filled) ? FB_LIGHT_GREEN : FB_DARK_GREY;
        fb_write_cell((bar_line * 80 + start_col + 1 + i) * 2, c, color, FB_BLACK);
    }
    
    /* Draw ] */
    fb_write_cell((bar_line * 80 + start_col + bar_width + 1) * 2, ']', FB_LIGHT_CYAN, FB_BLACK);
    
    /* Draw percentage */
    char percent_str[5];
    percent_str[0] = ' ';
    percent_str[1] = '0' + (percent / 10);
    percent_str[2] = '0' + (percent % 10);
    percent_str[3] = '%';
    percent_str[4] = '\0';
    
    int percent_col = start_col + bar_width + 3;
    for (i = 0; i < 4; i++) {
        fb_write_cell((bar_line * 80 + percent_col + i) * 2, percent_str[i], FB_WHITE, FB_BLACK);
    }
}

/** Draw loading message */
static void draw_loading_message(const char *msg)
{
    int i;
    int len = 0;
    const char *p = msg;
    
    while (*p++) len++;
    
    int start_col = (80 - len) / 2;
    int msg_line = 18;
    
    /* Clear previous message */
    for (i = 0; i < 80; i++) {
        fb_write_cell((msg_line * 80 + i) * 2, ' ', FB_WHITE, FB_BLACK);
    }
    
    /* Draw new message */
    p = msg;
    i = 0;
    while (*p) {
        fb_write_cell((msg_line * 80 + start_col + i) * 2, *p, FB_LIGHT_CYAN, FB_BLACK);
        p++;
        i++;
    }
}

/** bootsplash_show */
void bootsplash_show(void)
{
    int i;
    
    /* Clear screen */
    clear_screen();
    
    /* ASCII Art Logo */
    const char *logo[] = {
        "",
        "",
        "        ____       _        __     _  ___  ____  ",
        "       |  _ \\ ___ | |_   _ / _| __| |/ _ \\/ ___| ",
        "       | |_) / _ \\| | | | | |_ / _` | | | \\___ \\ ",
        "       |  __/ (_) | | |_| |  _| (_| | |_| |___) |",
        "       |_|   \\___/|_|\\__, |_|  \\__,_|\\___/|____/ ",
        "                     |___/                       ",
        "",
        "        =======================================",
        "           Moroccan x86 Operating System",
        "                  Version 1.4",
        "        =======================================",
        "",
        0
    };
    
    /* Draw logo centered */
    int line = 2;
    for (i = 0; logo[i] != 0; i++) {
        int len = 0;
        const char *p = logo[i];
        while (*p++) len++;
        
        int start_col = (80 - len) / 2;
        
        int col = start_col;
        p = logo[i];
        while (*p) {
            unsigned char color = (i >= 2 && i <= 7) ? FB_LIGHT_CYAN : FB_WHITE;
            fb_write_cell((line * 80 + col) * 2, *p, color, FB_BLACK);
            p++;
            col++;
        }
        line++;
    }
    
    /* Boot sequence with progress bar */
    const char *messages[] = {
        "Initializing hardware...",
        "Loading kernel modules...",
        "Mounting filesystem...",
        "Starting services...",
        "Preparing shell...",
        "Ready!",
        0
    };
    
    int msg_index = 0;
    int percent = 0;
    
    /* Animate loading */
    while (percent <= 100) {
        /* Update message */
        if (percent % 20 == 0 && messages[msg_index] != 0) {
            draw_loading_message(messages[msg_index]);
            msg_index++;
        }
        
        /* Update progress bar */
        draw_progress_bar(percent);
        
        delay(50);  /* 50ms per update */
        percent += 2;  /* 2% per frame = 50 frames = 2.5 seconds */
    }
    
    /* Show final message */
    draw_loading_message("Boot complete!");
    delay(500);
    
    /* Clear screen for OS */
    clear_screen();
}
