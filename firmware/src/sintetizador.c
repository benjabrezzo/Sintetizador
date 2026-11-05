#include "config.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

/* Milisegundos transcurridos desde el último cambio del LED. Solo la usa la ISR. */
static uint32_t msSinceToggle = 0;

/* SysTick cada 1 ms: alterna el LED cada LED_HALF_PERIOD_MS */
void SysTick_Handler(void) {
    msSinceToggle++;
    if (msSinceToggle >= LED_HALF_PERIOD_MS) {
        msSinceToggle = 0;
        GPIO_TogglePins(LED_PORT, 1u << LED_PIN);
    }
}

int main(void) {
    GPIO_SetDir(LED_PORT, 1u << LED_PIN, GPIO_OUTPUT);

    SYSTICK_InternalInit(1);        /* 1 ms */
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while (1) {
        __WFI();
    }
    return 0;
}
