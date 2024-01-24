#include "user_uart.h"

#include "main.h"

#define RX_TIMEOUT		(100)

extern UART_HandleTypeDef huart1;

bool user_uart1_receive_data(uint8_t *data, size_t expected_len)
{
	uint8_t error_code = HAL_UART_Receive(&huart1, data, expected_len, RX_TIMEOUT);
	return (error_code == HAL_OK);
}
