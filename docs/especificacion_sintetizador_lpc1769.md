# Sintetizador digital polifónico con LPC1769

**Especificación funcional, decisiones de diseño y plan de
implementación incremental**

  -----------------------------------------------------------------------
  Campo                               Definición
  ----------------------------------- -----------------------------------
  Proyecto                            Sintetizador digital polifónico
                                      basado en LPC1769

  Plataforma de desarrollo            LPC1769 / Cortex-M3

  Aplicación auxiliar                 Editor y configurador en Python

  Salida de audio                     DAC del LPC1769, filtro de
                                      reconstrucción, amplificador y jack
                                      de 3,5 mm

  Visualización                       Dos matrices LED CJMCU-64 de 8×8,
                                      conectadas en cascada

  Estado del documento                Especificación inicial acordada

  Principios del proyecto             `docs/constitution.md`
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 1. Descripción del proyecto

El proyecto consiste en desarrollar un sintetizador digital polifónico
de hasta cuatro voces, implementado sobre un microcontrolador NXP
LPC1769.

El sintetizador permitirá tocar ocho notas musicales mediante ocho
pulsadores. Cada voz tendrá un oscilador digital independiente, una
frecuencia, una fase y una envolvente ADSR propias. Las señales
generadas por las voces se combinarán mediante un mezclador digital y se
procesarán con un ecualizador de tres bandas antes de enviarse al DAC.

Una aplicación de escritorio desarrollada en Python permitirá diseñar
formas de onda mediante síntesis aditiva, visualizar sus características
y transferir las tablas de muestras al microcontrolador mediante UART.
También ofrecerá configuraciones precargadas que imitan instrumentos y
perillas para configurar el ecualizador, cuyo procesamiento se realiza en
el LPC1769.

El sistema también contará con un visualizador de espectro de audio.
Este analizará mediante FFT la señal generada por el sintetizador y
mostrará sus componentes frecuenciales en dos matrices LED CJMCU-64 de
8×8, colocadas horizontalmente para formar una superficie de 16×8
píxeles.

El desarrollo se realizará de manera incremental. Cada bloque deberá
implementarse, probarse y aprobarse de forma independiente antes de
comenzar el siguiente. No se avanzará si el bloque actual no cumple sus
criterios de aceptación.

## 2. Objetivos funcionales

El sistema deberá permitir:

-   Reproducir hasta cuatro notas simultáneamente.
-   Detectar ocho pulsadores, correspondientes a ocho notas musicales.
-   Generar señales periódicas mediante tablas de onda.
-   Cambiar entre cinco formas de onda (cuadrada, diente de sierra,
    triangular, sinusoidal y la generada en Python) mediante un pulsador
    conectado a EINT1.
-   Controlar la frecuencia de cada voz de manera independiente.
-   Controlar la amplitud temporal de cada voz mediante una envolvente
    ADSR.
-   Mezclar las voces en una única señal digital.
-   Aplicar ecualización digital de graves, medios y agudos.
-   Convertir la señal digital a analógica mediante el DAC del LPC1769.
-   Entregar una señal apta para auriculares mediante una etapa
    analógica externa.
-   Diseñar formas de onda desde una aplicación Python.
-   Transferir tablas de onda y parámetros al LPC1769 mediante UART.
-   Visualizar el espectro de la señal de audio en dos matrices LED.
-   Mantener la reproducción de audio estable mientras se actualizan las
    matrices y se realizan otras tareas.

## 3. Decisiones de diseño acordadas

Las siguientes decisiones se consideran requisitos de diseño. Cualquier
modificación posterior deberá registrarse y justificarse.

  -----------------------------------------------------------------------
  Área                                Decisión
  ----------------------------------- -----------------------------------
  Arquitectura de síntesis            DDS (Direct Digital Synthesis) con
                                      acumulador de fase

  Frecuencia de muestreo              32 kHz para empezar, marcados por
                                      `DACCNTVAL`; fija durante la
                                      reproducción, se podrán probar
                                      otras

  Polifonía                           Cuatro osciladores independientes y
                                      mezclador digital

  Entradas musicales                  Ocho pulsadores, para ocho notas,
                                      leídos por GPIO desde SysTick

  Formas de onda                      Cuatro precargadas (cuadrada, diente
                                      de sierra, triangular y sinusoidal)
                                      y una quinta generada en Python

  Tablas de onda                      256 muestras por ciclo; las cuatro
                                      precargadas en Flash, la quinta en
                                      RAM

  Selección de forma de onda          Un pulsador en EINT1; cada pulsación
                                      avanza a la onda siguiente, que se
                                      aplica desde la próxima nota

  Grabación de voz                    No se implementa

  Programación de periféricos         Drivers de
                                      `CMSISv2p00_LPC17xx/Drivers`,
                                      incluida la UART

  Periféricos utilizados              SysTick, Timer, ADC, DAC, GPDMA,
                                      UART, GPIO y EINT; no se usa SPI

  Envolventes                         ADSR independiente por voz,
                                      actualizada cada 1 ms por SysTick

  Controles ADSR                      Cuatro potenciómetros leídos
                                      mediante ADC

  Diseño de formas de onda            Síntesis aditiva en Python

  Aplicación de escritorio            Python con PySide6, numpy, pyserial
                                      y pyqtgraph

  Parámetros armónicos                Solo la amplitud de cada armónico;
                                      la cantidad se carga sola al elegir
                                      la frecuencia

  Instrumentos                        Guitarra, piano, órgano y flauta:
                                      configuraciones precargadas de
                                      armónicos, con indicación de cómo
                                      ajustar la ADSR

  Frecuencias armónicas               Múltiplos enteros de la frecuencia
                                      fundamental

  Transferencia de formas de onda     Python genera y cuantiza las
                                      muestras; el LPC1769 recibe la
                                      tabla mediante UART

  Evolución del timbre                Wavetable variable en el tiempo,
                                      como ampliación posterior

  Ecualizador                         Digital, ejecutado en el LPC1769;
                                      la aplicación solo lo configura

  Tipo de ecualizador                 Tres bandas: low-shelf, peaking EQ
                                      y high-shelf

  Implementación de filtros           Filtros IIR Biquad

  Aritmética DSP                      Punto fijo: muestras y envolventes
                                      en Q15; coeficientes Biquad en
                                      Q2.30, con acumulador de 64 bits

  Transferencia de audio              DMA con doble buffering

  Generación de muestras              En la ISR del DMA

  Organización del firmware           Loop principal simple, sin RTOS

  Salida digital-analógica            DAC del LPC1769

  Salida física                       Filtro Sallen-Key con LM358,
                                      amplificador LM386 y jack de 3,5 mm

  Visualizador                        Espectro de frecuencias calculado
                                      mediante FFT propia de 512 puntos

  Matrices LED                        Dos CJMCU-64 de 8×8 conectadas en
                                      cascada, con fuente externa

  Transmisión a las matrices          Timer + DMA

  Adaptación de nivel de datos        MOSFET BSS138 con pull-up de
                                      2,2 kΩ a 5 V

  Disposición física                  Horizontal, formando 16 columnas
                                      por 8 filas

  Actualización visual                Objetivo inicial de 20--30 FPS,
                                      sujeto a pruebas

  Comunicación PC-microcontrolador    UART

  Tests                               Lógica del firmware en la PC (gcc),
                                      app con pytest y hardware con
                                      mediciones en placa

  Forma de trabajo                    De a una story por vez, dentro de
                                      cada bloque
  -----------------------------------------------------------------------

### 3.1. Arquitectura DDS

Cada voz tendrá su propio acumulador de fase. La frecuencia se
determinará mediante el incremento aplicado al acumulador en cada
muestra.

Para un acumulador de fase de (M) bits:

\[ P\[n+1\] = (P\[n\] + `\Delta `{=tex}P) `\bmod 2`{=tex}\^M \]

\[ f = `\frac{\Delta P \cdot f_s}{2^M}`{=tex} \]

Donde:

-   \(P\) es la fase acumulada.
-   (`\Delta `{=tex}P) es el incremento de fase.
-   (f_s) es la frecuencia de muestreo.
-   \(f\) es la frecuencia de salida.

La fase de cada voz será independiente. La frecuencia de muestreo del
sistema, en cambio, permanecerá fija mientras se reproduce audio.

Se comenzará con (f_s) = 32 kHz, marcados por el contador del DAC:
`DACCNTVAL` = 25 MHz / 32 kHz ≈ 781. Se podrán probar otras frecuencias.

La posición de cada voz se guardará en formato Q16.16. Como las tablas
tienen 256 muestras, la posición da la vuelta en 2\^24 (M = 24) y el índice
de tabla son los bits 23:16. Los incrementos de las ocho notas se
precalculan en una tabla `const`.

### 3.2. Polifonía y mezcla

El sistema tendrá cuatro voces simultáneas. Cada voz mantendrá, como
mínimo:

-   Estado de activación.
-   Nota asignada.
-   Frecuencia o incremento de fase.
-   Acumulador de fase.
-   Identificador de tabla de onda.
-   Estado de la envolvente ADSR.
-   Nivel de amplitud.

Las cuatro señales se combinarán digitalmente antes de ingresar al
ecualizador. El control de niveles será el siguiente:

1.  La suma de voces se calculará en 32 bits, de modo que nunca desborde.
2.  La mezcla se escalará con una ganancia fija de 1/2.
3.  Después del ecualizador, la señal se saturará al rango de 10 bits del
    DAC (0 a 1023). Nunca habrá wrap-around.
4.  Un contador registrará las saturaciones para medirlas en las pruebas.

Con cuatro voces fuertes o un refuerzo alto del ecualizador se podrá
recortar la señal de vez en cuando. Esto se acepta a cambio de que una
nota sola use la mitad del rango del DAC, en lugar de una cuarta parte.

Cuando se presiona una nota, se le asigna una voz con este criterio:

1.  Una voz libre.
2.  Si no hay, la voz en Release con menor nivel.
3.  Si tampoco hay, la voz más antigua.

Si la tecla vuelve a sonar mientras su voz sigue activa, se reutiliza esa
misma voz.

### 3.3. Editor de formas de onda

La aplicación se desarrollará en Python con PySide6 (interfaz), numpy
(cálculo), pyserial (UART) y pyqtgraph (gráficos). Permitirá configurar
la amplitud de la fundamental y de sus armónicos:

\[ x(t) = `\sum`{=tex}\_{k=1}\^{N} A_k `\sin`{=tex}(2`\pi `{=tex}k f_0
t) \]

Donde (f_0) es la frecuencia fundamental y (A_k) la amplitud del
armónico (k).

La aplicación generará una tabla periódica de muestras, la normalizará y
la cuantizará al formato acordado con el firmware. La tabla se
transferirá al LPC1769 mediante UART.

El usuario solo editará amplitudes; la fase de los armónicos no es
configurable. Al seleccionar la frecuencia fundamental, la aplicación
cargará automáticamente los armónicos y mostrará la frecuencia de cada uno.

La misma tabla suena en las ocho teclas, de modo que el límite lo fija la
nota más aguda: para evitar aliasing en Do5 (523,25 Hz) a 32 kHz, se usarán
como máximo 30 armónicos (30 × 523,25 Hz = 15,7 kHz \< 16 kHz). Si cambia la
frecuencia de muestreo, el máximo se recalcula con la misma regla.

La cantidad de armónicos utilizables deberá respetar las limitaciones de
muestreo y aliasing. En una etapa posterior se podrá incorporar un
conjunto de tablas limitadas en banda para diferentes rangos de notas.

La tabla generada será la quinta forma de onda del sintetizador. No
reemplaza a las cuatro precargadas.

**Instrumentos.** La aplicación incluirá configuraciones precargadas de
guitarra, piano, órgano y flauta, que cargan los armónicos con las
amplitudes correspondientes para que la onda se asemeje a ese instrumento.
Al elegir una, mostrará un mensaje que indica cómo ajustar los
potenciómetros de la ADSR para acercarse más a su sonido. Las amplitudes y
los mensajes se definirán al implementar el Bloque 8.

### 3.4. Envolvente ADSR

Cada voz tendrá una envolvente propia con cuatro etapas:

-   **Attack:** tiempo de crecimiento desde el nivel inicial hasta el
    máximo.
-   **Decay:** transición desde el máximo hasta el nivel de sustain.
-   **Sustain:** nivel mantenido mientras la tecla continúe presionada.
-   **Release:** caída desde el nivel actual hasta cero al soltar la
    tecla.

Los cuatro potenciómetros controlarán los parámetros globales de Attack,
Decay, Sustain y Release. Cada voz mantendrá su propio estado temporal
de envolvente.

SysTick actualizará las envolventes cada 1 ms. Attack, Decay y Release se
mapearán a un rango de 2 a 2000 ms, y Sustain a un nivel de 0 a 100 %.

El comportamiento ante las teclas será el siguiente:

-   Al soltar la tecla, en cualquier etapa, se pasa a Release desde el
    nivel actual.
-   Al volver a tocarla, se pasa a Attack desde el nivel actual, para evitar
    clicks.

El ADC funcionará en modo burst sobre los cuatro canales de los
potenciómetros, y SysTick leerá sus resultados.

### 3.5. Ecualizador

El ecualizador se ejecutará en el LPC1769, después de la mezcla de voces
y antes del DAC.

Tendrá tres bandas:

-   Low-shelf: graves, a 200 Hz.
-   Peaking EQ: medios, a 1 kHz.
-   High-shelf: agudos, a 4 kHz.

La ganancia de cada banda irá de −12 a +6 dB, porque por encima de +6 dB una
nota sola ya saturaría con la ganancia de mezcla de 1/2. Al iniciar, las
tres bandas estarán en 0 dB.

Cada banda se implementará mediante un filtro IIR Biquad. La aplicación
Python solo configurará los parámetros, mediante perillas de frecuencias
bajas, medias y altas. La aplicación calculará los coeficientes de los
Biquad y los enviará por UART, de modo que el LPC1769 no necesite cálculo en
punto flotante; el procesamiento de las muestras se realizará en el
microcontrolador. Los coeficientes nuevos se aplicarán entre bloques de
audio.

La aplicación no analizará sonidos existentes.

### 3.6. Visualizador LED

Las dos matrices CJMCU-64 formarán una superficie de 16 columnas por 8
filas.

El visualizador analizará la señal digital posterior al ecualizador y
anterior al DAC:

1.  Obtener un bloque de muestras.
2.  Aplicar una ventana Hann.
3.  Calcular la FFT.
4.  Agrupar los bins en 16 bandas de frecuencia.
5.  Calcular la magnitud de cada banda.
6.  Aplicar escala, normalización y suavizado.
7.  Convertir la magnitud en una altura de entre 0 y 8 píxeles.
8.  Actualizar las matrices.

La actualización del visualizador será una tarea secundaria. Nunca
deberá bloquear la generación de audio ni retrasar la transferencia DMA
al DAC.

La FFT será de implementación propia: radix-2 de 512 puntos en Q15, con
tablas de factores de giro y de ventana precalculadas en Flash. Con 32 kHz,
cada bin mide 62,5 Hz; con 256 puntos, las fundamentales de las ocho notas
(262 a 523 Hz) caerían en muy pocas columnas. Las 16 bandas tendrán
distribución logarítmica.

La ISR de audio copiará las muestras a un buffer de captura separado. Cuando
ese buffer se llene, dejará de escribirlo hasta que el loop principal
termine de procesarlo.

La transmisión a las matrices se hará con Timer + DMA, sin SPI. El Timer
genera las peticiones de DMA, y el DMA copia un buffer ya codificado al
registro del pin de datos (P0.0). Las matrices se alimentarán con una fuente
externa regulable, ajustada a 5 V, con masa común con el LPC1769.

La fuente no define el límite de consumo: 128 LEDs en blanco al máximo
consumirían unos 7,7 A (a 60 mA por LED), una corriente excesiva para el
cableado y las pistas de las matrices. El firmware limitará el brillo al
25 %, lo que deja el peor caso en unos 1,9 A. La tensión de la fuente se
comprobará con un multímetro antes de conectar las matrices.

### 3.7. Salida de audio

La salida se organizará de la siguiente manera:

\[ `\text{DAC}`{=tex}
`\rightarrow `{=tex}`\text{Filtro de reconstrucción}`{=tex}
`\rightarrow `{=tex}`\text{Amplificador de auriculares}`{=tex}
`\rightarrow `{=tex}`\text{Jack 3,5 mm}`{=tex} \]

El DAC no se conectará directamente a los auriculares. La etapa
analógica deberá contemplar el nivel de continua, el acoplamiento, la
impedancia de carga y los niveles seguros de salida.

El diseño inicial, a validar en el Bloque 3, es el siguiente:

``` text
AOUT --- 8,2 kΩ --+-- 8,2 kΩ --+-- LM358 (+)   salida ---+--- (-) LM358
                  |            |                         |
                2,2 nF        1 nF                       |
          (a salida LM358)   (a GND)                     |
                                                         |
  salida LM358 --- 1 µF --- 150 kΩ --- pote 10 kΩ log --- LM386 (pin 3)

  LM386 (pin 5) --- 220 µF --+-- 100 Ω --- L
                             +-- 100 Ω --- R
```

  Etapa            Componentes                Función
  ---------------- -------------------------- --------------------------------
  Filtro           LM358, 2 × 8,2 kΩ, 2,2 nF  Sallen-Key de 2.º orden, con
                   y 1 nF                     fc ≈ 13 kHz y ganancia unitaria
  Acoplamiento     1 µF                       Quita los 1,65 V de continua
  Atenuación y     150 kΩ y potenciómetro de  Baja los 3,3 Vpp del DAC a unos
  volumen          10 kΩ logarítmico          170 mVpp como máximo
  Amplificador     LM386 con ganancia 20,     Maneja la carga de los
                   100 nF + 100 µF en         auriculares, con unos 3,4 Vpp
                   alimentación               como máximo
  Red de salida    10 Ω + 47 nF de pin 5 a    Estabilidad, según la aplicación
                   GND                        típica del LM386
  Salida           220 µF y 100 Ω por canal   Bloquea la continua; deja unos
                                              0,8 Vpp en auriculares de 32 Ω

**Por qué se hace así:**

-   **LM358 a 5 V:** su entrada y su salida funcionan desde 0 V hasta unos
    3,5 V, lo que cubre el rango del DAC sin una fuente negativa.
-   **Carga del DAC:** la primera resistencia de 8,2 kΩ respeta el mínimo de
    1 kΩ de la hoja de datos.
-   **LM386:** a diferencia del LM358, está pensado para manejar cargas de
    baja impedancia y se consigue con facilidad.
-   **100 Ω por canal:** los auriculares son muy sensibles. Estas resistencias
    limitan el nivel a unos 2,6 mW y separan los dos canales, que reciben la
    misma señal.

**Alimentación.** El LM358 y el LM386 usarán 5 V de una línea distinta de la
de las matrices (por ejemplo, el USB de la placa), porque los LED generan
ruido en su alimentación. El retorno de corriente de las matrices irá
directamente a la fuente, y no por la masa de la etapa de audio; ambas se
unirán en un solo punto.

### 3.8. Selección de forma de onda

El sintetizador contará con cinco formas de onda. Para cambiar entre
ellas se utilizará un pulsador conectado a EINT1. Cada pulsación avanzará a
la onda siguiente, en este orden cíclico:

1.  Cuadrada.
2.  Diente de sierra.
3.  Triangular.
4.  Sinusoidal.
5.  Onda generada en Python.

Después de la última se vuelve a la cuadrada. Mientras no se haya recibido
una onda desde Python, la rotación saltea la quinta posición.

El cambio se aplicará desde la próxima nota: las voces que ya están
sonando conservan su onda hasta terminar.

Hay una excepción: cuando llega una tabla nueva desde Python, se valida en un
buffer separado y el puntero de la quinta onda cambia entre bloques de audio.
Las voces que están usando esa onda pasan a la tabla nueva, y se acepta que
pueda oírse un click. Así, nunca se escribe sobre la tabla que está sonando.

No se implementará la grabación de voz.

### 3.9. Uso de drivers

Los periféricos del LPC1769 se programarán mediante los drivers ubicados
en `CMSISv2p00_LPC17xx/Drivers`. Esto incluye la UART, que cuenta con su
driver en `lpc17xx_uart.c`.

Solo se usarán SysTick, Timer, ADC, DAC, GPDMA, UART, GPIO y EINT (PINSEL,
NVIC y CLKPWR, como configuración). Se accederá directamente a un registro
únicamente donde el driver no cubra lo necesario, con un comentario que lo
justifique.

### 3.10. Distribución de memoria

  Región                  Contenido                     Motivo
  ----------------------- ----------------------------- -----------------------
  Flash                   Las cuatro tablas precargadas Son `const`: no ocupan
                          (2 KB, `int16_t`), tablas de  RAM y no se pierden al
                          incrementos, ventana Hann     reiniciar
  `RamLoc32` (32 KB)      Pila, voces, filtros, FFT,    Acceso directo de la
                          buffer UART y quinta onda     CPU, sin competir con
                          (activa + recepción, 1 KB)    el DMA
  AHB SRAM banco 0        Buffers de audio al DAC       Lo lee el canal DMA del
  (16 KB)                 (512 B)                       audio
  AHB SRAM banco 1        Buffer de las matrices        Puerto separado del
  (16 KB)                 (\~9,2 KB)                    banco 0: los dos canales
                                                        DMA no compiten

La quinta onda está en RAM porque llega en funcionamiento y escribir la
Flash detendría la ejecución; por eso se pierde al reiniciar. El proyecto
ve los dos bancos como una sola región (`RAM2`), de modo que ubicar cada
buffer en su banco requiere ajustar la configuración de memoria.

La FFT usará unos 3 KB de `RamLoc32`: 1 KB de captura y 2 KB de trabajo
(parte real e imaginaria). Sus tablas de factores de giro y de ventana
ocuparán unos 2 KB de Flash.

Las tablas de Flash (ondas, incrementos, ventana y factores de giro) se
generarán con `firmware/tools/gen_tables.py` y no se editarán a mano.

### 3.11. Lectura de pulsadores

Las ocho teclas se leerán por GPIO desde SysTick, cada 1 ms, sin
interrupciones de GPIO. Así, los rebotes no generan ráfagas de
interrupciones que compitan con el audio. Una tecla cambia de estado tras
10 ms estable, y ese cambio genera la pulsación o la liberación.

Las notas son la escala de Do mayor, de Do4 a Do5:

  Tecla   1        2        3        4        5        6        7        8
  ------- -------- -------- -------- -------- -------- -------- -------- --------
  Nota    Do4      Re4      Mi4      Fa4      Sol4     La4      Si4      Do5
  Hz      261,63   293,66   329,63   349,23   392,00   440,00   493,88   523,25

El pulsador de EINT1 ignora nuevos disparos durante 150 ms.

### 3.12. Prioridades de interrupción

Un número menor indica mayor prioridad.

  Interrupción        Prioridad   Motivo
  ------------------- ----------- --------------------------------------------
  `DMA_IRQn`          0           Si se atrasa, se produce un corte audible
  `SysTick_IRQn`      1           Envolventes, potenciómetros y teclas cada 1 ms
  `UART0_IRQn`        2           La FIFO es de 16 B
  `EINT1_IRQn`        4           Evento humano, sin urgencia

El Timer de las matrices no usa interrupción: solo genera peticiones de
DMA. El canal DMA 0 es del audio y el canal 1, de las matrices, ya que el
canal de menor número tiene prioridad.

La ISR del DMA generará cada bloque de audio: voces, mezcla, ecualizador y
copia de muestras para la FFT. Es la ISR más larga del sistema (se estiman
unos 20 mil ciclos cada 2 ms, cerca del 10 % de la CPU), pero su duración está
acotada, y así el audio no depende de las tareas del loop principal. La
duración real se medirá en los Bloques 5 y 10.

El loop principal, sin RTOS, atenderá en este orden el parser de UART, la FFT
y la codificación de las matrices.

### 3.13. Asignación de pines

  Función                      Pin
  ---------------------------- --------------------------------------------
  Teclas Do4 a Do5             P2.0 a P2.7, con pull-up, activas en bajo
  Selección de onda (EINT1)    P2.11, flanco descendente, con pull-up
  Attack, Decay, Sustain       AD0.0 P0.23, AD0.1 P0.24, AD0.2 P0.25
  Release                      AD0.4 P1.30 (AD0.3 comparte pin con el DAC)
  Salida del DAC               AOUT P0.26
  UART0                        TXD0 P0.2, RXD0 P0.3, adaptador USB-TTL de 3,3 V
  Datos de las matrices        P0.0, con BSS138 (`FIOPIN0` y `FIOMASK`)
  LED de la placa              P0.22, salida (prueba de vida del Bloque 0)

### 3.14. Protocolo UART

La comunicación usará UART0 a 115200 8N1, con recepción por interrupción
hacia un buffer circular de 256 B. Los paquetes se procesarán fuera de la
ISR. El detalle se documentará en `docs/protocolo.md`.

Formato de trama: `0xA5 | CMD | LEN_L | LEN_H | payload | CHK`, donde CHK
es la suma de 8 bits de CMD a payload, en complemento a dos.

  CMD      Mensaje         Dirección   Contenido
  -------- --------------- ----------- -------------------------------------
  `0x01`   UPLOAD_WAVE     PC a LPC    256 muestras `int16_t` (512 B)
  `0x03`   PING            PC a LPC    Vacío; responde `0x83` con la versión
  `0x04`   SET_EQ          PC a LPC    5 coeficientes Q2.30 por banda (60 B)
  `0x06`   ACK             LPC a PC    Trama aceptada
  `0x15`   NACK            LPC a PC    Código de error

### 3.15. Adaptación de nivel de la línea de datos

Los WS2812B alimentados con 5 V exigen en DIN un nivel alto de
aproximadamente 3,5 V (0,7 × VDD), mientras que un GPIO del LPC1769 entrega
como máximo 3,3 V. Conectarlos directamente deja la señal por debajo del
umbral garantizado: podría funcionar en algunas condiciones y fallar en
otras. Por eso, entre el GPIO y la matriz se usará un MOSFET de canal N
BSS138 con una resistencia de pull-up de 2,2 kΩ.

``` text
                 3,3 V                 5 V (fuente de las matrices)
                   |                    |
                 Gate                 2,2 kΩ
                   |                    |
GPIO LPC1769 --- Source  [BSS138]  Drain +---- DIN de la matriz
```

**Conexiones.** El gate va fijo a 3,3 V, el source al GPIO y el drain a DIN.
La resistencia va del drain a los 5 V de la misma fuente que alimenta las
matrices. La masa es común entre la fuente, el LPC1769 y las matrices.

**Funcionamiento.** El MOSFET conduce según la tensión entre gate y source
(VGS), y como el gate está fijo, esa tensión la determina el GPIO:

  GPIO          VGS        MOSFET   Quién fija DIN                  DIN
  ------------- ---------- -------- ------------------------------- ------
  Bajo (0 V)    \~3,3 V    Conduce  El GPIO, a través del canal     \~0 V
  Alto (3,3 V)  0 V        Cortado  La resistencia de pull-up       5 V

El circuito no invierte la señal: un nivel lógico en el GPIO es el mismo
nivel en DIN, de modo que el buffer del DMA se codifica sin cambios.

**Por qué se hace así:**

-   **BSS138:** su umbral de VGS es de 1,5 V como máximo, de modo que con
    3,3 V conduce con seguridad. Un 2N7000 puede tener un umbral de hasta
    3 V, demasiado cerca de la tensión disponible.
-   **Gate a 3,3 V:** así el GPIO nunca ve los 5 V. Con el MOSFET cortado,
    el diodo interno queda en inversa, porque su ánodo está del lado del
    source (3,3 V) y su cátodo, del lado del drain (5 V).
-   **MOSFET en lugar de un NPN:** un NPN en emisor común invierte la señal
    y, al salir de saturación, puede demorar hasta unos 200 ns. Eso deforma
    pulsos de 400 ns.
-   **Pull-up a la fuente de las matrices:** DIN no supera la tensión de
    alimentación de los LED durante el funcionamiento normal. Si esa fuente
    está apagada con el GPIO en alto, el diodo interno conduce y lleva DIN a
    unos 2,7 V. Por eso, las matrices se alimentan antes de transmitir y el
    pin queda en bajo cuando no hay tramas.
-   **Valor de 2,2 kΩ:** resulta de un compromiso entre dos límites:
    -   Corriente: con el GPIO en bajo, el pin absorbe (5 V − VOL) / R ≈
        2,1 mA, dentro de los 4 mA garantizados por la hoja de datos para
        VOL ≤ 0,4 V. Con 1 kΩ serían unos 4,6 mA, por encima de ese valor.
    -   Velocidad: el flanco de subida lo genera la resistencia al cargar
        la capacidad de la línea, y tarda t = R C ln(5 / 1,5) ≈ 1,2 R C
        hasta llegar a 3,5 V. Con unos 30 pF estimados, son unos 80 ns. Una
        resistencia mayor acorta demasiado los pulsos altos.

**Efecto en la temporización.** El flanco de bajada lo fuerza el canal del
MOSFET y es rápido, mientras que el de subida se atrasa unos 80 ns. Los
pulsos altos se acortan: el "0" pasa de 400 a unos 320 ns y el "1", de 800 a
unos 720 ns, sin cambiar la duración total del bit. Estos márgenes deben
contrastarse con la hoja de datos del WS2812B y medirse con osciloscopio. La
conexión entre el circuito y DIN debe ser corta, porque más capacidad
atrasa más el flanco.

**Estado de reposo.** Los WS2812B toman el nivel bajo prolongado como señal
de reinicio. Tras un reset, el GPIO queda como entrada con pull-up y DIN se
mantiene en alto; el firmware debe poner el pin como salida en bajo antes de
la primera transmisión y mantenerlo así entre tramas.

------------------------------------------------------------------------

## 4. Arquitectura general del sistema

``` text
                         COMPUTADORA
                 +--------------------------+
                 | Aplicación Python        |
                 |                          |
                 | - Editor de armónicos    |
                 | - Generador de tablas    |
                 | - Visualización de onda  |
                 | - Instrumentos           |
                 | - Configuración del EQ   |
                 +------------+-------------+
                              |
                         UART bidireccional
                              |
                              v
+-----------------------------------------------------------+
|                         LPC1769                           |
|                                                           |
|  Entradas                                                  |
|  +----------------+  +----------------+                    |
|  | 8 pulsadores   |  | 4 potenciómetros|                    |
|  | GPIO + EINT1   |  | ADC             |                    |
|  +--------+-------+  +--------+-------+                    |
|           |                   |                            |
|           v                   v                            |
|  +-----------------------------------------------------+  |
|  | Control de voces y parámetros                       |  |
|  +---------------------------+-------------------------+  |
|                              |                            |
|  +---------------------------v-------------------------+  |
|  | Motor de síntesis                                    |  |
|  | 4 DDS -> 4 ADSR -> Mezclador                         |  |
|  +---------------------------+-------------------------+  |
|                              |                            |
|  +---------------------------v-------------------------+  |
|  | Ecualizador digital: Low-shelf / Peaking / High-shelf|  |
|  +---------------------------+-------------------------+  |
|                              |                            |
|               +--------------+--------------+             |
|               |                             |             |
|               v                             v             |
|  +-------------------------+  +-------------------------+ |
|  | Doble buffer de audio   |  | Análisis espectral      | |
|  | DMA -> DAC              |  | Ventana -> FFT -> bandas | |
|  +------------+------------+  +------------+------------+ |
|               |                            |              |
+---------------|----------------------------|--------------+
                |                            |
                v                            v
       Filtro + amplificador          Control de matrices
                |                     LED (Timer + DMA)
                v                            |
          Jack 3,5 mm                        v
             Auriculares              2 matrices 8x8
                                      (16x8 píxeles)
```

## 5. Organización propuesta del firmware

La implementación se dividirá en módulos. Los nombres son orientativos y
podrán ajustarse al organizar el código.

  Módulo            Capa       Responsabilidad
  ----------------- ---------- -------------------------------------------
  `main`            Hardware   Inicialización y coordinación general
  `board`           Hardware   Configuración de pines y recursos de la placa
  `audio_engine`    Hardware   Coordinación del flujo de audio (ISR del DMA)
  `audio_dma`       Hardware   Buffers, DMA y DAC
  `adc_input`       Hardware   Lectura de potenciómetros
  `button_input`    Hardware   Lectura de pulsadores y EINT1
  `uart_link`       Hardware   UART y buffer de recepción
  `led_matrix`      Hardware   Timer + DMA hacia los WS2812B
  `oscillator`      Lógica     DDS y lectura de tablas
  `voice`           Lógica     Asignación y estado de las cuatro voces
  `envelope`        Lógica     Máquinas de estado ADSR y mapeo de parámetros
  `mixer`           Lógica     Mezcla y control de niveles
  `equalizer`       Lógica     Filtros Biquad de tres bandas
  `debounce`        Lógica     Antirrebote de las teclas
  `uart_protocol`   Lógica     Validación y respuesta de paquetes
  `wavetable`       Lógica     Almacenamiento y administración de tablas
  `spectrum`        Lógica     Ventana, FFT y agrupación de bandas
  `led_encoder`     Lógica     Mapeo de píxeles y codificación del buffer

Los módulos de lógica no acceden a registros ni a drivers, de modo que se
pueden compilar y probar en la PC. Reciben sus datos por parámetro, y los
módulos de hardware son los únicos que tocan periféricos. En la aplicación
rige la misma separación: la lógica no depende de PySide6, que solo se usa en
`app/ui/`.

La lógica de audio deberá tener prioridad sobre las tareas de
visualización, comunicación y controles. Las interrupciones deberán ser
breves: se evitará realizar una FFT, transmitir una trama LED o procesar
paquetes UART completos dentro de una ISR.

------------------------------------------------------------------------

# 6. Plan de implementación incremental

## Regla de avance

**No se comienza un bloque nuevo hasta que el bloque actual esté
implementado, probado y aprobado.**

Cada bloque deberá terminar con:

1.  Implementación funcional.
2.  Pruebas reproducibles.
3.  Evidencia del resultado (medición, captura, registro o
    demostración).
4.  Criterios de aceptación cumplidos.
5.  Código integrado y estable en el repositorio.

Si una prueba falla, se corrige el bloque actual. No se continúa
agregando funcionalidades sobre una base que todavía no funciona.

Los bloques se dividen internamente en stories, pero el criterio de
aprobación se aplica al bloque completo.

## Stories

Se trabaja en una sola story por vez. Cada story es un archivo en
`docs/stories/` que indica:

-   Bloque al que pertenece y sección de esta especificación que implementa.
-   Estado: pendiente, en curso o terminada.
-   Criterios de aceptación propios.

Una story se cierra cuando sus criterios se cumplen y sus tests pasan.

## Evidencia

La evidencia de cada bloque (mediciones, capturas, registros) se guarda en
`docs/evidencia/`. Un bloque queda aprobado cuando sus criterios están marcados
en esta especificación y su evidencia está guardada.

------------------------------------------------------------------------

## Bloque 0 --- Preparación del entorno y estructura del proyecto

**Objetivo:** disponer de un proyecto base reproducible y una estructura
de trabajo organizada.

### Implementación

-   Crear el proyecto para LPC1769 en MCUXpresso.
-   Verificar la configuración del reloj y la frecuencia del
    microcontrolador.
-   Verificar la compilación y descarga del firmware.
-   Crear la estructura inicial de carpetas; los módulos de la sección 5 nacen con su bloque.
-   Configurar el repositorio Git.
-   Crear `docs/stories/` y `docs/evidencia/`.
-   Preparar los tests de lógica en la PC (`firmware/tests/`, con gcc y make) y
    los de la aplicación (`app/tests/`, con pytest).
-   Agregar al proyecto el include path de `Drivers/inc`.
-   Documentar la placa, herramientas y versiones utilizadas.
-   Crear un mecanismo básico de diagnóstico, por ejemplo mediante UART
    o semihosting.

### Pruebas

-   Compilar el proyecto sin errores.
-   Programar el LPC1769.
-   Ejecutar una prueba de vida, como alternar un LED GPIO.
-   Verificar que el diagnóstico funcione.
-   Comprobar que el proyecto puede reconstruirse desde un estado
    limpio.
-   Ejecutar un test de ejemplo en la PC y otro con pytest.

### Criterios de aceptación

-   [x] El proyecto compila correctamente.
-   [x] El firmware se descarga y ejecuta.
-   [x] La prueba de vida funciona.
-   [x] El repositorio contiene el proyecto base.
-   [x] El entorno de desarrollo queda documentado.
-   [x] Los tests de ejemplo se ejecutan en la PC y con pytest.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 1 --- Temporización, DAC y generación de audio de prueba

**Objetivo:** producir una señal analógica estable utilizando el DAC del
LPC1769.

### Implementación

-   Configurar el DAC y su contador (`DACCNTVAL`) para establecer la
    frecuencia de muestreo de 32 kHz.
-   Generar una tabla de prueba sencilla, inicialmente una senoide.
-   Reproducir la tabla mediante el mecanismo de actualización elegido.
-   Medir la frecuencia de muestreo y la frecuencia de la señal de
    salida.

En este bloque se puede comenzar sin DMA si facilita la puesta en
marcha. Sin embargo, antes de aprobarlo debe quedar definida y probada
la ruta de reproducción que se utilizará en el sistema final.

### Pruebas

-   Medir la salida del DAC con osciloscopio o instrumento equivalente.
-   Comprobar la frecuencia de la señal.
-   Verificar que la frecuencia de muestreo sea estable.
-   Observar la forma de onda y el nivel de salida.
-   Comprobar que no existan pausas o reinicios inesperados.

### Criterios de aceptación

-   [ ] El DAC produce una señal periódica.
-   [ ] La frecuencia medida coincide con la esperada dentro de la
    tolerancia definida.
-   [ ] La frecuencia de muestreo es estable.
-   [ ] La señal se mantiene durante un período prolongado sin
    interrupciones.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 2 --- DMA y doble buffering de audio

**Objetivo:** reproducir audio de manera continua sin que la CPU tenga
que atender cada muestra individualmente.

### Implementación

-   Configurar el DMA para alimentar el DAC.
-   Reservar dos buffers de audio.
-   Configurar la alternancia entre buffers.
-   Generar el contenido de un buffer, en la ISR del DMA, mientras el DMA
    reproduce el otro.
-   Implementar la sincronización entre finalización de transferencia y
    generación del siguiente bloque.
-   Implementar un contador de underruns (sección 7.1).

### Pruebas

-   Reproducir una senoide durante varios minutos.
-   Alternar el contenido de los buffers.
-   Comprobar que no existan discontinuidades entre bloques.
-   Medir el tiempo de generación disponible por bloque.
-   Verificar que la CPU no modifique un buffer mientras el DMA lo
    utiliza.

### Criterios de aceptación

-   [ ] El DAC recibe muestras mediante DMA.
-   [ ] Los dos buffers se alternan correctamente.
-   [ ] No aparecen pausas ni discontinuidades atribuibles al cambio de
    buffer.
-   [ ] No se sobrescribe un buffer en uso.
-   [ ] El sistema mantiene la reproducción durante una prueba
    prolongada, con el contador de underruns en 0.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 3 --- Etapa analógica de salida

**Objetivo:** obtener una salida adecuada para auriculares desde el DAC.

Este bloque se adelantó respecto del plan inicial, donde estaba después del
ecualizador, para poder escuchar el resultado de los bloques siguientes y no
validarlos solo con osciloscopio. Se prueba con la senoide de los Bloques 1 y 2.

### Implementación

-   Conseguir los componentes de la sección 3.7 (LM358, LM386 y pasivos).
-   Construir el filtro de reconstrucción y comprobar su respuesta.
-   Construir la etapa de acoplamiento, volumen y amplificación.
-   Conectar el jack de 3,5 mm.
-   Validar los niveles de salida y ajustar los valores si hace falta.

### Pruebas

-   Medir la señal a la salida del filtro.
-   Verificar atenuación de componentes no deseadas.
-   Medir offset de continua.
-   Probar con una carga de prueba apropiada.
-   Comprobar ausencia de clipping analógico.
-   Probar con auriculares y verificar que el nivel sea controlable.

### Criterios de aceptación

-   [ ] El circuito reproduce correctamente el audio del DAC.
-   [ ] El filtro atenúa las imágenes de muestreo de manera adecuada.
-   [ ] El amplificador puede manejar la carga prevista.
-   [ ] No existe una componente continua peligrosa en la salida.
-   [ ] El nivel de audio es utilizable y no presenta distorsión
    evidente.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 4 --- DDS y reproducción de una voz

**Objetivo:** generar distintas frecuencias mediante un acumulador de
fase sin modificar la frecuencia de muestreo.

### Implementación

-   Implementar el acumulador de fase en Q16.16 (sección 3.1).
-   Implementar el incremento de fase.
-   Implementar la conversión de fase a índice de tabla.
-   Reproducir una tabla de onda mediante DDS.
-   Implementar el cálculo del incremento de fase a partir de la
    frecuencia deseada.
-   Usar muestras en Q15 y documentar las escalas de punto fijo.

### Pruebas

-   Generar varias frecuencias con la misma tabla.
-   Medir las frecuencias obtenidas.
-   Comparar valores teóricos y medidos.
-   Comprobar cambios de frecuencia sin alterar la frecuencia de
    muestreo.
-   Verificar el comportamiento al pasar por el límite del acumulador.

### Criterios de aceptación

-   [ ] Una voz reproduce una tabla mediante DDS.
-   [ ] Se pueden seleccionar distintas frecuencias.
-   [ ] La frecuencia de muestreo permanece fija.
-   [ ] La frecuencia generada coincide con la esperada dentro de la
    tolerancia definida.
-   [ ] No aparecen errores numéricos ni interrupciones.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 5 --- Cuatro voces y mezclador digital

**Objetivo:** reproducir hasta cuatro notas simultáneamente.

### Implementación

-   Crear la estructura de datos de una voz.
-   Mantener un acumulador de fase independiente por voz.
-   Permitir asignar una frecuencia y una amplitud a cada voz.
-   Implementar la suma de las cuatro voces.
-   Aplicar el escalado y la saturación de la sección 3.2, con su contador.
-   Mantener la fase de las voces activas entre bloques de audio.

### Pruebas

-   Reproducir una voz.
-   Reproducir dos, tres y cuatro voces simultáneas.
-   Verificar las frecuencias individuales.
-   Comparar la señal mezclada con la suma esperada.
-   Probar combinaciones de amplitudes máximas.
-   Comprobar que no haya desborde ni wrap-around, y medir las saturaciones
    con el contador.

### Criterios de aceptación

-   [ ] Las cuatro voces funcionan independientemente.
-   [ ] Cada voz puede tener una frecuencia distinta.
-   [ ] La mezcla contiene las cuatro señales.
-   [ ] La suma no produce overflow y la salida se satura sin wrap-around.
-   [ ] La reproducción continúa sin interrupciones.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 6 --- Pulsadores y asignación de notas

**Objetivo:** controlar el sintetizador mediante ocho pulsadores
físicos.

### Implementación

-   Configurar ocho entradas GPIO.
-   Implementar antirrebote.
-   Implementar la correspondencia entre pulsadores y notas (sección 3.11).
-   Detectar eventos de pulsación y liberación.
-   Implementar asignación de notas a las cuatro voces, con la política de
    la sección 3.2.
-   Configurar el pulsador de EINT1 para rotar entre las formas de onda
    (sección 3.8).

Las teclas se leerán por GPIO desde SysTick, cada 1 ms. El pulsador de
selección de onda es el único que usa una interrupción externa.

### Pruebas

-   Pulsar cada tecla individualmente.
-   Mantener varias teclas presionadas.
-   Probar combinaciones de hasta cuatro notas.
-   Presionar más de cuatro teclas.
-   Verificar el antirrebote.
-   Comprobar que las liberaciones se detecten correctamente.
-   Rotar por todas las formas de onda con EINT1.
-   Cambiar de onda mientras suena una nota y comprobar que el cambio se
    aplique recién en la siguiente.

### Criterios de aceptación

-   [ ] Los ocho pulsadores se detectan correctamente.
-   [ ] Cada pulsador corresponde a la nota definida.
-   [ ] Se pueden tocar cuatro notas simultáneas.
-   [ ] La asignación de voces funciona cuando se supera la polifonía.
-   [ ] No se producen activaciones falsas por rebote.
-   [ ] EINT1 rota las ondas y el cambio se aplica desde la próxima nota.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 7 --- Envolventes ADSR y potenciómetros

**Objetivo:** controlar la evolución de amplitud de cada nota.

### Implementación

-   Implementar una máquina de estados ADSR por voz, actualizada cada
    1 ms por SysTick.
-   Implementar las etapas Attack, Decay, Sustain y Release.
-   Configurar cuatro canales ADC en modo burst para los potenciómetros, y
    leerlos desde SysTick.
-   Asignar un potenciómetro a cada parámetro ADSR.
-   Filtrar o suavizar las lecturas ADC para evitar variaciones bruscas.
-   Convertir los valores ADC en tiempos y niveles utilizables.
-   Implementar el comportamiento ante las teclas de la sección 3.4.

### Pruebas

-   Probar cada etapa por separado.
-   Medir tiempos de Attack, Decay y Release.
-   Verificar el nivel de Sustain.
-   Cambiar los potenciómetros durante la reproducción.
-   Probar varias voces con envolventes simultáneas.
-   Comprobar que una voz liberada llegue a amplitud cero y quede
    disponible.

### Criterios de aceptación

-   [ ] Cada voz mantiene su propia envolvente.
-   [ ] Las cuatro etapas ADSR funcionan.
-   [ ] Los potenciómetros modifican los parámetros correspondientes.
-   [ ] Las envolventes no generan saltos o valores fuera de rango.
-   [ ] Las voces quedan disponibles al finalizar Release.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 8 --- Aplicación Python y editor de armónicos

**Objetivo:** diseñar formas de onda desde una computadora.

### Implementación

-   Crear la aplicación de escritorio.
-   Crearla con PySide6, numpy y pyqtgraph.
-   Permitir seleccionar la frecuencia fundamental de referencia.
-   Cargar automáticamente los armónicos, hasta el máximo de la sección 3.3.
-   Permitir configurar la amplitud de la fundamental.
-   Permitir configurar la amplitud de cada armónico.
-   Definir e incorporar las configuraciones de guitarra, piano, órgano y
    flauta, con su mensaje de ajuste de la ADSR.
-   Generar la señal mediante síntesis aditiva.
-   Mostrar la forma de onda en el dominio temporal.
-   Mostrar el espectro de amplitud.
-   Normalizar la señal.
-   Cuantizar las muestras al formato definido para el LPC1769.
-   Exportar la tabla a un archivo para facilitar las pruebas.

En esta etapa todavía no es necesario transferir la tabla por UART.
Primero debe verificarse que el generador produzca resultados correctos.

### Pruebas

-   Generar una senoide.
-   Generar una señal con fundamental y segundo armónico.
-   Generar una señal con varios armónicos.
-   Comprobar las frecuencias presentes en el espectro.
-   Verificar que la señal sea periódica.
-   Comprobar que la cuantización respete el rango permitido.
-   Cambiar la frecuencia y comprobar que se carguen los armónicos
    correctos.
-   Cargar cada instrumento y comprobar sus amplitudes y su mensaje de ADSR.

### Criterios de aceptación

-   [ ] Se pueden configurar las amplitudes armónicas.
-   [ ] La aplicación genera una tabla de muestras.
-   [ ] La forma de onda y el espectro se visualizan correctamente.
-   [ ] La tabla está normalizada y cuantizada.
-   [ ] La cantidad de armónicos respeta los límites definidos.
-   [ ] Los instrumentos cargan sus armónicos y muestran cómo ajustar la
    ADSR.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 9 --- Protocolo UART y transferencia de tablas

**Objetivo:** transferir formas de onda desde Python al LPC1769 de
manera confiable.

### Implementación

-   Configurar UART en el LPC1769 mediante su driver.
-   Implementar recepción sin bloquear el motor de audio.
-   Implementar el formato de paquete de la sección 3.14 y documentarlo en
    `docs/protocolo.md`.
-   Implementar mensajes de confirmación y error.
-   Transferir una tabla completa.
-   Validar longitud, formato y rango de muestras.
-   Almacenar la tabla recibida sin modificar una tabla que esté en uso.
-   Activar una tabla nueva solamente después de validar su recepción,
    cambiando el puntero de la quinta onda entre bloques de audio.

### Pruebas

-   Enviar una tabla de 256 muestras y tramas de longitud incorrecta.
-   Comprobar que los datos recibidos coincidan con los enviados.
-   Interrumpir una transferencia intencionalmente.
-   Enviar paquetes inválidos.
-   Verificar que el firmware rechace datos corruptos.
-   Reproducir una tabla recibida.
-   Probar la recepción mientras el audio está funcionando.

### Criterios de aceptación

-   [ ] Python y el LPC1769 intercambian paquetes.
-   [ ] Las tablas se reciben completas.
-   [ ] Los errores de transferencia se detectan.
-   [ ] Una tabla inválida no reemplaza una tabla funcional.
-   [ ] La recepción no interrumpe la reproducción de audio.
-   [ ] Una tabla recibida queda disponible como quinta onda.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 10 --- Ecualizador digital de tres bandas

**Objetivo:** modificar el balance frecuencial del audio antes de
enviarlo al DAC.

### Implementación

-   Implementar un filtro Biquad en punto fijo, con coeficientes Q2.30 y
    acumulador de 64 bits.
-   Verificar su estabilidad y respuesta.
-   Implementar low-shelf.
-   Implementar peaking EQ.
-   Implementar high-shelf.
-   Conectar los tres filtros en cascada.
-   Usar las frecuencias y ganancias de la sección 3.5.
-   Permitir modificar los coeficientes entre bloques, sin producir saltos
    audibles.
-   Calcular los coeficientes en Python y validar que entren en Q2.30.
-   Incorporar configuración desde Python, inicialmente mediante valores
    de prueba y luego por UART, con perillas para frecuencias bajas, medias
    y altas.

### Pruebas

-   Aplicar señales de frecuencia conocida.
-   Medir la respuesta de cada filtro.
-   Verificar el comportamiento de graves, medios y agudos.
-   Comprobar estabilidad para los rangos de parámetros permitidos.
-   Probar cambios de parámetros durante la reproducción.
-   Medir el costo de procesamiento por bloque.

### Criterios de aceptación

-   [ ] Los tres filtros funcionan individualmente.
-   [ ] Los tres filtros funcionan en cascada.
-   [ ] La respuesta frecuencial coincide con la configuración.
-   [ ] No aparecen inestabilidades ni overflow.
-   [ ] El procesamiento cumple el plazo de generación de audio.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 11 --- FFT y cálculo de las 16 bandas

**Objetivo:** analizar el espectro de la señal de audio en tiempo real.

### Implementación

-   Implementar una FFT propia radix-2 de 512 puntos en Q15, con
    escalado por etapa para evitar desbordes.
-   Medir su costo computacional.
-   Generar las tablas de factores de giro y de ventana en Flash.
-   Implementar una ventana Hann.
-   Calcular la FFT de bloques de audio.
-   Calcular magnitudes espectrales.
-   Agrupar los bins en 16 bandas de frecuencia con distribución
    logarítmica.
-   Aplicar escala y normalización.
-   Convertir las magnitudes en niveles de 0 a 8.
-   Incorporar suavizado temporal y caída gradual de las barras.

El análisis utilizará la señal posterior al ecualizador y anterior al
DAC, tomada del buffer de captura de la sección 3.6, sin interferir con los
buffers que están siendo reproducidos o generados.

### Pruebas

-   Analizar una senoide de frecuencia conocida.
-   Verificar que el pico aparezca en la banda esperada.
-   Analizar señales con varios armónicos.
-   Probar distintas frecuencias y amplitudes.
-   Verificar la agrupación de los bins.
-   Medir el tiempo de ejecución de la FFT.
-   Comprobar que el análisis no provoque underruns de audio.

### Criterios de aceptación

-   [ ] La FFT identifica correctamente las componentes de prueba.
-   [ ] Las 16 bandas se calculan correctamente.
-   [ ] La conversión a niveles de 0 a 8 es estable.
-   [ ] El procesamiento cumple los plazos establecidos.
-   [ ] La reproducción de audio no se interrumpe.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 12 --- Control de matrices CJMCU-64

**Objetivo:** controlar las dos matrices LED conectadas en cascada.

### Implementación

-   Verificar el modelo exacto de controlador y la disposición de
    píxeles de las matrices.
-   Conectar una matriz y comprobar su funcionamiento.
-   Construir la adaptación de nivel con BSS138 y pull-up de 2,2 kΩ
    (sección 3.15).
-   Comprobar en la placa que P0.0 esté libre y accesible.
-   Configurar P0.0 como salida en bajo antes de la primera transmisión y
    mantenerlo así entre tramas.
-   Implementar el protocolo de comunicación para los WS2812B con Timer +
    DMA, según el esquema inicial siguiente:
    -   Timer2 con MR0 y reinicio al coincidir, para pedir una transferencia
        cada 400 ns (MAT2.0, con PCLK de 25 MHz y MR0 = 9).
    -   Tres transferencias de 1 byte por bit: alto, dato y bajo. Da un bit
        de 1,2 µs.
    -   Canal DMA 1, desde el buffer al registro `FIOPIN0`, con `FIOMASK`
        para que solo cambie P0.0.
    -   Tres elementos LLI, porque 9216 transferencias superan el máximo de
        4095 por elemento.
-   Fijar a mano la dirección de destino del DMA: en M2P con conexión
    `GPDMA_MAT2_0`, `GPDMA_SetupChannel` la apunta a `MR0`, no al GPIO.
-   Limpiar la petición de DMA pendiente del Timer antes de habilitar el
    canal.
-   Implementar el envío de los 128 píxeles.
-   Conectar la segunda matriz en cascada.
-   Crear una función de representación de una matriz lógica de 16×8.
-   Implementar la limitación de brillo al 25 %.
-   Evitar transmisiones bloqueantes y conflictos con el audio.
-   Incorporar alimentación externa de 5 V y masa común con el LPC1769,
    comprobando la tensión de la fuente antes de conectar las matrices.

### Pruebas

-   Medir en DIN los niveles alto y bajo con osciloscopio.
-   Medir en DIN la duración de los pulsos "0" y "1", y compararla con la
    hoja de datos del WS2812B.
-   Encender un píxel individual.
-   Recorrer filas y columnas.
-   Comprobar la orientación de ambas matrices.
-   Mostrar patrones de prueba.
-   Verificar la transmisión de 128 píxeles.
-   Medir el tiempo de actualización.
-   Comprobar que el envío no interrumpa el audio.
-   Verificar alimentación y temperatura en condiciones de uso.

### Criterios de aceptación

-   [ ] DIN cumple los niveles y tiempos del WS2812B, medidos con
    osciloscopio.
-   [ ] Las dos matrices funcionan.
-   [ ] Se controlan como una superficie lógica de 16×8.
-   [ ] El mapeo físico de píxeles es correcto.
-   [ ] Se pueden mostrar patrones y colores.
-   [ ] La actualización no bloquea el audio.
-   [ ] El consumo y la alimentación se mantienen dentro de límites
    seguros.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 13 --- Integración del visualizador de espectro

**Objetivo:** mostrar en las matrices el espectro calculado en el bloque
11.

### Implementación

-   Conectar la salida de las 16 bandas con el controlador LED.
-   Asignar una columna a cada banda.
-   Convertir cada nivel en una altura de 0 a 8 píxeles.
-   Definir una paleta de colores según intensidad.
-   Implementar suavizado y caída de las barras.
-   Limitar la frecuencia de actualización a un objetivo inicial de
    20--30 FPS.
-   Coordinar la actualización LED con la reproducción de audio.

### Pruebas

-   Reproducir una senoide y observar la banda correspondiente.
-   Reproducir señales con varios armónicos.
-   Tocar varias notas simultáneamente.
-   Cambiar el ecualizador y observar el cambio del espectro.
-   Verificar la estabilidad de las barras.
-   Ejecutar el sintetizador durante varios minutos con el visualizador
    activo.
-   Comprobar que no aparezcan underruns de audio.

### Criterios de aceptación

-   [ ] Las barras representan las 16 bandas calculadas.
-   [ ] La altura refleja la magnitud de cada banda.
-   [ ] Los cambios de timbre y ecualización se reflejan visualmente.
-   [ ] La actualización es fluida.
-   [ ] El audio permanece estable durante la visualización.

**No avanzar hasta cumplir todos los criterios.**

------------------------------------------------------------------------

## Bloque 14 --- Integración completa y pruebas de sistema

**Objetivo:** verificar el funcionamiento conjunto del sintetizador.

### Implementación

-   Integrar entradas, voces, ADSR, tablas, ecualizador, DAC, FFT y
    matrices.
-   Integrar la aplicación Python y la comunicación UART.
-   Revisar prioridades de interrupciones y tareas.
-   Medir uso de CPU y memoria.
-   Revisar todos los buffers y sus propietarios.
-   Incorporar manejo de errores.
-   Documentar el procedimiento de puesta en marcha.

### Pruebas

-   Tocar una nota y variar sus parámetros.
-   Tocar cuatro notas simultáneamente.
-   Subir una onda nueva desde Python y seleccionarla rotando con EINT1.
-   Cambiar parámetros del ecualizador.
-   Observar el visualizador durante la reproducción.
-   Transferir una tabla mientras el sistema reproduce audio.
-   Ejecutar pruebas prolongadas.
-   Comprobar recuperación ante paquetes UART inválidos.
-   Medir carga de CPU, uso de SRAM y estabilidad de audio.
-   Demostrar en placa la función de cada periférico: SysTick (ADSR y
    teclas), Timer (matrices), ADC (potenciómetros), DAC y GPDMA (audio), UART
    (aplicación), GPIO (teclas y datos) y EINT1 (onda).

### Criterios de aceptación

-   [ ] Todas las funciones principales operan simultáneamente.
-   [ ] No existen interrupciones audibles durante el uso normal.
-   [ ] El contador de underruns permanece en 0 durante la prueba
    prolongada.
-   [ ] Cada periférico cumple su función, demostrada en placa.
-   [ ] No se producen errores de memoria ni overflow.
-   [ ] La comunicación UART no afecta la reproducción.
-   [ ] El visualizador no afecta la reproducción.
-   [ ] Los controles responden correctamente.
-   [ ] El sistema supera una prueba prolongada.

**El proyecto principal se considera terminado cuando se cumplen todos
los criterios.**

------------------------------------------------------------------------

## Bloque 15 --- Ampliación: wavetable variable en el tiempo

**Objetivo:** enriquecer el timbre de los instrumentos mediante varias
tablas que evolucionan durante una nota.

### Implementación

-   Definir un instrumento como un conjunto ordenado de tablas.
-   Definir en qué etapa o intervalo temporal se utiliza cada tabla.
-   Implementar transición entre tablas.
-   Evaluar interpolación entre tablas para evitar saltos.
-   Ampliar el editor Python para crear y organizar las tablas.
-   Ampliar el protocolo UART para transferir instrumentos completos.
-   Gestionar memoria y activación segura de los nuevos datos.

### Pruebas

-   Comparar una nota con tabla fija y con tablas variables.
-   Verificar que las transiciones no produzcan clicks.
-   Medir memoria utilizada.
-   Medir costo de procesamiento.
-   Probar varias voces con instrumentos variables.

### Criterios de aceptación

-   [ ] Un instrumento puede contener varias tablas.
-   [ ] Las tablas evolucionan durante la nota.
-   [ ] Las transiciones son audiblemente correctas.
-   [ ] No se exceden los límites de memoria.
-   [ ] La reproducción mantiene sus plazos.

Este bloque es una ampliación y no condiciona la aprobación del
sintetizador principal.

------------------------------------------------------------------------

# 7. Reglas técnicas transversales

Estas reglas se aplican durante todos los bloques.

## 7.1. Tiempo real de audio

-   La generación de audio tiene prioridad sobre la visualización, UART
    y controles.
-   Las ISR deben ser breves. La ISR del DMA es la excepción, porque genera
    el bloque de audio, pero su duración debe estar acotada y medida.
-   No ejecutar FFT dentro de la interrupción de audio.
-   No transmitir matrices LED desde una ISR.
-   No realizar operaciones bloqueantes dentro del camino crítico.
-   Medir el tiempo de procesamiento por bloque y conservar margen antes
    del próximo vencimiento.
-   Mantener un contador de underruns: al terminar de generar un bloque, la
    ISR comprueba si el canal de audio ya terminó el buffer siguiente y, en
    ese caso, incrementa el contador.

## 7.2. Memoria

El LPC1769 dispone de recursos limitados de SRAM. Antes de fijar tamaños
definitivos se deberá realizar un presupuesto de memoria que contemple:

-   Buffers de audio.
-   Tablas de ondas.
-   Estados de las voces.
-   Estados de filtros.
-   Buffers de FFT.
-   Buffers de transmisión LED.
-   Buffers UART y estructuras auxiliares.

No se deben asignar buffers grandes sin justificar su necesidad.

## 7.3. Punto fijo

-   Documentar el formato numérico de cada señal.
-   Documentar los rangos permitidos.
-   Controlar el crecimiento de la suma de voces.
-   Evitar overflow en multiplicaciones y acumulaciones.
-   Verificar la estabilidad de los Biquad.
-   Medir el error numérico con señales de referencia.

## 7.4. Propiedad de los buffers

Cada buffer debe tener un propietario y un estado claramente definido.

-   El DMA es el único consumidor del buffer de audio que está
    reproduciendo.
-   La CPU genera muestras en el buffer que no está siendo reproducido.
-   Las tablas activas no se modifican durante su utilización.
-   Los datos recibidos por UART se validan en un espacio separado antes
    de activarse.
-   El análisis FFT debe utilizar una copia o una región de muestras
    cuyo acceso esté coordinado.

## 7.5. Integración incremental

-   Mantener una versión funcional al finalizar cada bloque.
-   No mezclar cambios de varios bloques sin pruebas intermedias.
-   Registrar problemas conocidos.
-   Conservar pruebas que puedan repetirse.
-   Si una integración rompe un bloque aprobado, se corrige antes de
    continuar.

## 7.6. Tests

  Qué se prueba                 Dónde         Cómo
  ----------------------------- ------------- --------------------------------
  Módulos de lógica del         PC            `firmware/tests/`, con gcc y make,
  firmware                                    sin frameworks externos
  Lógica de la aplicación       PC            `app/tests/`, con pytest
  Módulos de hardware           Placa         Mediciones con evidencia en
                                              `docs/evidencia/`

-   Los tests deben pasar antes de cerrar una story.
-   Los módulos de lógica usan tipos de `stdint.h`, para comportarse igual en
    la PC y en el LPC1769.
-   Los módulos de lógica se prueban con señales de referencia: frecuencias
    del DDS, tiempos de la ADSR, respuesta de los Biquad, picos de la FFT y
    tramas válidas e inválidas.

------------------------------------------------------------------------

# 8. Riesgos técnicos que deben vigilarse

  -----------------------------------------------------------------------
  Riesgo                  Consecuencia            Mitigación
  ----------------------- ----------------------- -----------------------
  Sobrecarga de CPU       Underrun de audio       Medir tiempo por bloque
                                                  y reducir costo de DSP

  Overflow en mezcla      Distorsión digital      Escalado, acumuladores
                                                  adecuados y pruebas de
                                                  rango

  Alias en tablas         Timbre incorrecto a     Limitar armónicos y
                          ciertas notas           evaluar tablas
                                                  band-limited

  Cambios abruptos de     Clicks audibles         Doble tabla y cambio
  tabla                                           seguro entre bloques

  Inestabilidad de Biquad Señal incorrecta o      Limitar parámetros y
                          saturación              validar coeficientes

  FFT demasiado costosa   Interrupciones de audio FFT por bloques y
                                                  ejecución de menor
                                                  prioridad

  Transmisión LED         Pérdida de muestras     Temporizador con DMA
  bloqueante                                      

  Consumo elevado de LEDs Caída de tensión o      Fuente externa y
                          calentamiento           limitación de brillo

  Conflictos de DMA       Transferencias          Asignar canales y
                          incorrectas             prioridades desde el
                                                  diseño

  SRAM insuficiente       Corrupción o            Presupuesto de memoria
                          imposibilidad de        temprano
                          integrar                
  -----------------------------------------------------------------------

------------------------------------------------------------------------

# 9. Definiciones pendientes para la etapa de diseño detallado

Las siguientes decisiones no impiden iniciar los primeros bloques, pero
deben resolverse antes de implementar los bloques afectados.

-   Frecuencia de muestreo definitiva (se comienza con 32 kHz).
-   Amplitudes y mensajes de ADSR de los cuatro instrumentos (Bloque 8).
-   Factor Q de cada banda del ecualizador (Bloque 10).
-   Límites exactos de las 16 bandas (Bloque 11).
-   Confirmación en placa del esquema de Timer + DMA para los WS2812B y de
    que P0.0 esté disponible (Bloque 12).
-   Validación de la etapa analógica y del nivel de salida (Bloque 3).

Cada decisión deberá documentarse antes de comenzar el bloque que
dependa de ella.

------------------------------------------------------------------------

# 10. Definición de finalización

El sintetizador principal estará terminado cuando:

1.  Pueda reproducir cuatro notas simultáneamente.
2.  Cada voz tenga su propio DDS y ADSR.
3.  Se puedan generar y transferir tablas desde Python.
4.  El ecualizador de tres bandas funcione en tiempo real.
5.  El audio se reproduzca mediante DMA y DAC sin interrupciones, con el
    contador de underruns en 0.
6.  La salida analógica sea adecuada para auriculares.
7.  Las dos matrices muestren el espectro de audio mediante FFT.
8.  El sistema completo supere las pruebas de integración y estabilidad.

La evolución del timbre mediante múltiples wavetables se considera una
ampliación posterior.

**Principio rector del desarrollo: un bloque funcional y probado es la
base del siguiente. No se agregan funcionalidades para compensar o
esconder fallas de bloques anteriores.**
