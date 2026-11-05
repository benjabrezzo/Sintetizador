# Backlog — Sintetizador LPC1769

## Contexto

Sintetizador digital polifónico de cuatro voces sobre LPC1769, con app de escritorio para diseñar ondas y
configurar el ecualizador, y espectro en matrices LED. Es el TP integrador de EDIII: lo usa quien toca y lo
evalúa la cátedra.

**Stack:** firmware en C, CMSIS, Drivers y MCUXpresso, sin RTOS. App en Python, PySide6, numpy, pyserial y
pyqtgraph (principio 7).

**Decisiones caras:** ninguna.

## Backlog

Cada story indica su bloque (`B`) y las secciones de la spec que implementa.

1. **B0** — Como evaluador de la cátedra, quiero reconstruir el firmware desde cero y verlo correr en la placa, para comprobar que el proyecto es reproducible.
2. **B1 · §3.1** — Como evaluador, quiero medir una senoide estable a la salida del DAC, para comprobar que su frecuencia es la esperada. Requiere: #1
3. **B2 · §7.1, §7.4** — Como evaluador, quiero que esa señal se mantenga durante minutos sin pausas ni discontinuidades, para confiar en que el audio no se corta. Requiere: #2
4. **B3 · §3.7** — Como músico, quiero escuchar en auriculares la señal de la placa con volumen controlable, para oírla sin distorsión y con un nivel seguro. Requiere: #2
5. **B4 · §3.1** — Como evaluador, quiero seleccionar distintas frecuencias, compararlas con las teóricas y escucharlas, para comprobar que las notas van a estar afinadas. Requiere: #3
6. **B5 · §3.2** — Como evaluador, quiero medir y escuchar cuatro notas simultáneas en una sola salida, para comprobar que se mezclan sin desborde. Requiere: #5
7. **B6 · §3.2, §3.8, §3.11** — Como músico, quiero tocar hasta cuatro de las ocho notas a la vez y rotar la forma de onda con un pulsador, para elegir qué suena. Requiere: #6
8. **B7 · §3.4** — Como músico, quiero ajustar con cuatro potenciómetros el ataque, decaimiento, sostenido y liberación de las notas, para dar forma a cada sonido. Requiere: #7
9. **B8 · §3.3** — Como músico, quiero diseñar una onda por armónicos en la app, o partir de guitarra, piano, órgano o flauta, y ver su forma y espectro, para crear un timbre propio.
10. **B9 · §3.8, §3.14** — Como músico, quiero enviar mi onda a la placa y seleccionarla como quinta forma de onda con el pulsador de rotación, para tocar mi timbre sin cortar lo que suena ni perder la onda anterior si el envío falla. Requiere: #7, #9
11. **B10 · §3.5** — Como músico, quiero ajustar graves, medios y agudos con perillas en la app mientras suena, para balancear el sonido. Requiere: #10
12. **B11 · §3.6** — Como evaluador, quiero comprobar con señales conocidas que el análisis ubica cada frecuencia en su banda sin interrumpir el audio, para confiar en lo que va a mostrar el visualizador. Requiere: #11
13. **B12 · §3.6, §3.15** — Como evaluador, quiero ver patrones y colores correctos en las dos matrices como una superficie de 16×8 mientras suena el audio, para comprobar que funcionan sin afectarlo y con un consumo seguro. Requiere: #3
14. **B13 · §3.6** — Como músico, quiero ver en las matrices 16 barras con el espectro de lo que toco, para notar los cambios de timbre y ecualización. Requiere: #12, #13
15. **B14 · §7, §10** — Como evaluador, quiero ver todas las funciones operando juntas en una prueba prolongada y cada periférico demostrado en placa, para dar el proyecto por terminado. Requiere: #4, #8, #10, #11, #14

## Notas

- **Orden:** es secuencial por la regla de avance de la spec. `Requiere` marca solo las dependencias reales.
- **Una story por bloque:** el principio 2 pide stories dentro de un bloque. Por eso no hay un walking skeleton que cruce varios bloques.
- **Actor evaluador:** se usa en las stories donde todavía no hay nada para tocar o ver.
- **Bloque 15:** sigue en la spec como ampliación y queda fuera de este backlog.
- **Quinta onda:** cualquier onda nueva se sube desde Python y se selecciona rotando con EINT1. La app no selecciona ondas.
- **Componentes:** antes de cada prueba con hardware se indica qué tiene que estar preparado y armado.
