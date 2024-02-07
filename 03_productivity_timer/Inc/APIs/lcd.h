#ifndef __LCD_INC_
#define __LCD_INC_

#include <stdint.h>

#define SIMULATE_LCD_TROUGH_CONSOLE    (1)

void display_init(void);
void display_clear(void);
void display_show(void);
void display_time(uint32_t time);
void display_message(const char *message);

#endif /* __LCD_INC_ */
