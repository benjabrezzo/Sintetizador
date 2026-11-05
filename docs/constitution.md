# Constitución — Sintetizador LPC1769 (EDIII)

Principios innegociables. Si el código choca con esta lista, se corrige el código.

## Desarrollo
1. **La spec manda.** `docs/especificacion_sintetizador_lpc1769.md` es la fuente de verdad: todo cambio de comportamiento se registra primero en la spec y después en el código. *Verificación: cada story cita su sección de la spec.*
2. **De a una story.** Se trabaja en una sola story por vez, dentro de un bloque de la spec y con criterios de aceptación propios. *Verificación: `docs/stories/` tiene como máximo una story en curso.*
3. **Sin módulo funcionando no se avanza.** Un bloque se aprueba solo con todos sus criterios cumplidos y evidencia guardada; si algo aprobado se rompe, se arregla antes de seguir. *Verificación: criterios marcados en la spec y evidencia en `docs/evidencia/`.*
4. **Toda lógica tiene tests.** La lógica del firmware se prueba en la PC, la de la app con pytest y el hardware con mediciones en placa. *Verificación: los tests pasan antes de cerrar una story.*

## Arquitectura
5. **Lógica separada de la interfaz.** DDS, ADSR, mezcla, Biquad, FFT y protocolo no acceden a registros ni drivers; la lógica de la app no depende de la GUI. *Verificación: sin `LPC_` ni `lpc17xx_` en esos módulos, y sin `PySide6` fuera de `app/ui/`.*
6. **El audio no se interrumpe.** Las muestras se generan en la ISR del DMA con prioridad máxima; FFT, parser UART y codificación LED corren en el loop principal. *Verificación: contador de underruns en 0 durante la prueba prolongada.*

## Tecnología
7. **El stack no cambia.** Firmware: C, CMSIS, Drivers y MCUXpresso, sin RTOS y con FFT propia. App: Python, PySide6, numpy, pyserial y pyqtgraph. *Verificación: una dependencia nueva exige modificar antes la spec.*
8. **Lenguajes definidos.** Firmware en C y app en Python; identificadores en inglés, comentarios y documentación en español. *Verificación: revisión de cada story.*

## Placa y cátedra
9. **Periféricos cerrados y por driver.** Solo SysTick, Timer, ADC, DAC, GPDMA, UART, GPIO y EINT, con los drivers del repo; un acceso directo a registro solo si el driver no lo cubre, con comentario que lo justifique. Pines y prioridades viven en `firmware/inc/config.h`. *Verificación: sin includes de otros periféricos y sin pines fuera de `config.h`.*
10. **Requisitos de la cátedra.** Cada periférico exigido cumple una función real: SysTick (ADSR y teclas), Timer (matrices), ADC (potenciómetros), DAC y GPDMA (audio), UART (app), GPIO (teclas y datos), EINT1 (onda). El equipo debe poder explicar y defender todo el código. *Verificación: esta tabla se demuestra funcionando en placa.*
