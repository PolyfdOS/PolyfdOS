#include "bootsplash.h"
#include "fb.h"

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

/** bootsplash_show */
void bootsplash_show(void)
{
    int i;
    
    /* Clear screen */
    fb_clear();
    
    /* ASCII Art Logo - polyfdOS */
    const char *logo[] = {
        "\n\n",
        "        ====================================================\n",
        "        ____       _        __     _  ___  ____  \n",
        "       |  _ \\ ___ | |_   _ / _| __| |/ _ \\/ ___| \n",
        "       | |_) / _ \\| | | | | |_ / _` | | | \\___ \\ \n",
        "       |  __/ (_) | | |_| |  _| (_| | |_| |___) |\n",
        "       |_|   \\___/|_|\\__, |_|  \\__,_|\\___/|____/ \n",
        "                     |___/                       \n",
        "        ====================================================\n",
        "              Moroccan x86 Operating System\n",
        "                     Version 1.4\n",
        "        ====================================================\n",
        "\n\n",
        0
    };
    
    /* Draw logo */
    for (i = 0; logo[i] != 0; i++) {
        fb_puts((char *)logo[i]);
    }
    
    /* Loading animation stages */
    const char *stages[] = {
        "                 [          ] Initializing hardware...\n",
        "                 [==        ] Loading kernel modules...\n",
        "                 [====      ] Mounting filesystem...\n",
        "                 [======    ] Starting services...\n",
        "                 [========  ] Preparing shell...\n",
        "                 [==========] Ready!\n",
        0
    };
    
    /* Animate through stages */
    for (i = 0; stages[i] != 0; i++) {
        fb_puts((char *)stages[i]);
        delay(800);  /* ~800ms per stage = ~4.8 seconds total */
    }
    
    /* Show completion message */
    fb_puts("\n\n              Boot complete!\n");
    delay(500);
    
    /* Clear screen for OS */
    fb_clear();
}