#include "lcd.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if SIMULATE_LCD_TROUGH_CONSOLE
static char current_time_displayed[31];
static char current_message_shown[31];

static void center_text(const char *input, char *output, int output_size) {
    int input_len = (strlen(input) >= (output_size - 1)) ? output_size - 1 : strlen(input);
    int spaces = (output_size - input_len - 1) / 2;
    memset(output, ' ', spaces);
    strncpy(output + spaces, input, input_len);
    output[output_size - 1] = '\0';
}

void display_clear(void)
{
	memset(current_time_displayed, ' ', 30);
	memset(current_message_shown, ' ', 30);
	current_time_displayed[30] = 0;
	current_message_shown[30] = 0;
}

void display_show(void)
{
    printf("\r\n");
    printf("################################\r\n");
    printf("#%s#\r\n", current_message_shown);
    printf("#%s#\r\n", current_time_displayed);
    printf("################################\r\n");
    printf("\r\n");
}

void display_time(uint32_t time)
{
    uint8_t minutes = time/60;
    uint8_t seconds = time%60;

    char time_string[8];
    snprintf(time_string, 8, "%03d:%02d", minutes, seconds);
    center_text(time_string, current_time_displayed, 31);
}

void display_message(const char *message)
{
	center_text(message, current_message_shown, 31);
}

void display_init(void)
{
    display_clear();
}

#endif /* SIMULATE_LCD_TROUGH_CONSOLE */
