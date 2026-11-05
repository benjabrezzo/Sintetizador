#include <stdio.h>
#include "config.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

/* Diagnóstico por semihosting: el printf se envía al depurador con un BKPT. Sin depurador
 * ese BKPT cuelga el arranque, así que solo se imprime si C_DEBUGEN está activo.
 * Acceso directo a CoreDebug->DHCSR: ningún driver de la lista lo cubre (spec 3.9). */
static int isDebuggerAttached(void) {
    return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0;
}

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
    if (isDebuggerAttached()) {
        printf("Sintetizador LPC1769: reloj de %lu Hz\n", (unsigned long)SystemCoreClock);
    }

    GPIO_SetDir(LED_PORT, 1u << LED_PIN, GPIO_OUTPUT);

    SYSTICK_InternalInit(1);        /* 1 ms */
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while (1) {
        __WFI();
    }
    return 0;
}
