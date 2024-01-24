#ifndef __USER_UART_INC_
#define __USER_UART_INC_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

bool user_uart1_receive_data(uint8_t *data, size_t expected_len);

#endif /* __USER_UART_INC_ */
