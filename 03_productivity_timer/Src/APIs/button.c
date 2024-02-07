#include "button.h"
#include "main.h"

#include <stdint.h>
#include <stdio.h>

/** Truth table of the buttons:
 * B1 | B2 | B3 | Value | Signal
 *  0 |  0 |  1 |   1   | START_PAUSE 
 *  0 |  1 |  0 |   2   | DEC_TIME
 *  1 |  0 |  0 |   4   | INC_TIME
 *  1 |  1 |  0 |   6   | ABORT 
*/

#if SIMULATE_BUTTONS_THROUGH_CONSOLE

extern UART_HandleTypeDef huart1;

button_id_t button_get_pressed(void)
{
    uint8_t rx_value;
    HAL_StatusTypeDef error_code = HAL_UART_Receive(&huart1, &rx_value, 1, 50);

    if (error_code != HAL_OK) {return BUTTON_ID_NONE;}

    printf("[BUTTON] Received: %c\r\n", rx_value);

    switch (rx_value) {
        case '+': { return BUTTON_ID_PLUS;  }
        case '-': { return BUTTON_ID_MINUS; }
        case 'b': { return BUTTON_ID_BOTH;  }
        case 's': { return BUTTON_ID_START_PAUSE; }
        default:  { return BUTTON_ID_NONE;  }
    }
}

#endif /* SIMULATE_BUTTONS_THROUGH_CONSOLE */