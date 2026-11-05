# Story 1: Reconstruir el firmware y verlo correr en placa

**Bloque:** 0 · **Spec:** §6 Bloque 0, §7.6 · **Estado:** terminada

## Story
Como evaluador de la cátedra, quiero reconstruir el firmware desde cero y verlo correr en la placa, para comprobar que el proyecto es reproducible.

## Criterios de aceptación
Se comprueban primero en esta PC y después en la notebook con CachyOS, adonde el proyecto llega por GitHub.

- [x] Dado el proyecto en su workspace con los drivers ya cargados, cuando hago clean y compilo en Debug, entonces compila sin errores ni warnings en el código del proyecto.
- [x] Dado ese firmware descargado en la LPC1769, cuando lo ejecuto con el depurador, entonces el LED alterna con un período medido de 1 s ± 1 % y la consola muestra un mensaje de arranque que informa un reloj de 100 MHz; y cuando reinicio la placa sin el depurador, el LED alterna igual.
- [x] Dado el proyecto, cuando ejecuto los tests de ejemplo de lógica en la PC y los de la aplicación, entonces ambos pasan, y si rompo un test el comando termina con error.

## Plan
- Actualizar las rutas en la spec, la constitución, el backlog y las instrucciones del proyecto; después, reorganizar las carpetas y apuntar el IDE a las nuevas.
- Crear el repositorio Git, sin artefactos de compilación, y agregar como remota `git@github.com:benjabrezzo/Sintetizador.git`. Benja crea el repo en GitHub y hace los commits.
- Agregar la prueba de vida y el diagnóstico: el LED se suma a la asignación de pines de la spec y alterna con SysTick, y el mensaje de arranque sale por semihosting solo si hay un depurador conectado. Solo se crean los módulos que esta story usa.
- Agregar los tests de ejemplo: uno con gcc y make, sin frameworks, y otro con pytest, en un venv que por ahora solo instala pytest.
- Guardar la evidencia, repetir en CachyOS y marcar los criterios en la spec.

## Fuera de alcance
- Copiar los drivers al repositorio: quien lo pruebe los tiene cargados en su workspace.
- UART: configuración, recepción y protocolo de §3.14. El diagnóstico pasa a UART en el Bloque 9.
- Módulos de §5 que esta story no usa: nacen con su bloque, sin archivos vacíos.
- README (placa, herramientas, versiones, origen de los drivers y pasos): lo escribe Benja al terminar el proyecto.
- Configuración Release: solo se soporta Debug.
- PySide6, numpy, pyqtgraph y pyserial (Bloque 8).

## Estructura acordada

```text
sintetizador/            raíz del repo y del proyecto de MCUXpresso
├── firmware/
│   ├── src/             .c, incluidos el startup y tables.c
│   ├── inc/             .h, incluidos config.h y tables.h
│   ├── tests/           tests de lógica en la PC (antes tests/host/)
│   └── tools/           gen_tables.py
├── app/                 aplicación de escritorio, con ui/ y tests/
├── hardware/            esquemáticos y BOM; vacía hasta el Bloque 3
├── docs/                constitución, spec, backlog, stories/ y evidencia/
└── README.md            entorno y pasos para reconstruir
```
