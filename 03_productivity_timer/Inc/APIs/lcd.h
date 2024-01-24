#ifndef __LCD_INC_
#define __LCD_INC_

#include <stdint.h>

#define USE_UART1_AS_LCD    (1)

void display_clear(void);
void display_time(uint32_t time);
void display_message(const char *message);

#endif /* __LCD_INC_ */
