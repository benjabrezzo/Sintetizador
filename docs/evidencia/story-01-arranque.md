# Story 1 · M3 · Mensaje de arranque por semihosting

**Criterio:** con el depurador, la consola muestra un mensaje de arranque que informa un reloj de 100 MHz.

## Procedimiento
- Firmware: `firmware/src/sintetizador.c`. Imprime `SystemCoreClock` por `printf` solo si
  `CoreDebug->DHCSR` indica que hay un depurador conectado (`C_DEBUGEN`).
- Biblioteca de C: Redlib (semihost).
- Se ejecutó desde MCUXpresso con la configuración `sintetizador LinkServer Debug`.

## Salida de la consola (semihosting)

```text
[MCUXpresso Semihosting Telnet console for 'sintetizador LinkServer Debug' started on port 50724 @ 127.0.0.1]

Sintetizador LPC1769: reloj de 100000000 Hz
```

## Conclusión
El firmware informa `SystemCoreClock` = 100 000 000 Hz = 100 MHz.

## Pendiente
El comportamiento sin depurador (M4) se verifica aparte.
