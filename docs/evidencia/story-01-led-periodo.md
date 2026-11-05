# Story 1 · M2 · Período del LED (P0.22)

**Criterio:** el LED alterna con un período medido de 1 s ± 1 %.

## Procedimiento
- Firmware: `firmware/src/sintetizador.c`, SysTick a 1 ms y alternancia de P0.22 cada 500 ticks.
- Placa: LPCXpresso LPC1769, firmware descargado desde MCUXpresso (Debug).
- Instrumento: cronómetro de un celular, a mano. No hay osciloscopio ni analizador lógico.
- Se contaron períodos completos (de encendido a encendido, dos cambios de estado cada uno).

## Resultado

| Dato | Valor |
|---|---|
| Períodos contados | 120 |
| Tiempo medido | 120,4 s |
| Período medio | 120,4 / 120 = 1,0033 s |
| Desviación respecto de 1 s | +0,33 % |

## Incertidumbre
- Error de arranque y parada del cronómetro a mano: ±0,3 s en total (estimado, no medido).
- Sobre 120 s equivale a ±0,25 %, así que el período está entre 1,0008 s y 1,0058 s (+0,08 % a +0,58 %).
- Todo ese rango cae dentro de ±1 %. Se contaron 120 períodos para que la incertidumbre del método
  fuera menor que la tolerancia.

## Conclusión
El período medido cumple 1 s ± 1 %.

## Pendiente
La desviación se mide con el celular. La frecuencia real de `SystemCoreClock` se confirma con el mensaje
de arranque (M3). El reinicio sin depurador se verifica aparte (M4).
