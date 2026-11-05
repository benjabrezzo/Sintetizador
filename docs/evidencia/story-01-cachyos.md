# Story 1 · M5 · Reconstrucción en la notebook (CachyOS)

El proyecto se clonó desde GitHub en la notebook y se importó a MCUXpresso (workspace
`~/Documents/MCUXpresso_25.6.136/`, con `LPC17xx-CMSIS-Driver-Enhancement` y `Sintetizador` como carpetas hermanas).

## Build (criterio 1)
Project > Clean y build de Debug. Resultado: **0 errors, 0 warnings** (12:51:22).

| | Fedora (PC de escritorio) | CachyOS (notebook) |
|---|---|---|
| Flash | 11 300 B (2,16 %) | 11 300 B (2,16 %) |
| RamLoc32 | 500 B (1,53 %) | 500 B (1,53 %) |
| `text / data / bss` | 11296 / 4 / 496 | 11296 / 4 / 496 |

El log completo de Fedora está en `story-01-build-debug.txt`.

## Mensaje de arranque (criterio 2)
Con el depurador, la consola de semihosting muestra:

```text
[MCUXpresso Semihosting Telnet console for 'sintetizador LinkServer Debug' started on port 38570 @ 127.0.0.1]

Sintetizador LPC1769: reloj de 100000000 Hz
```

## LED (criterio 2)
El LED parpadea con un período aproximado de 1 s. Observación cualitativa: no se midió con
cronómetro. El período medido con rigor es el de `story-01-led-periodo.md` (+0,33 %).

## Nota sobre los drivers
El arreglo de mayúsculas de `lpc17xx_clkpwr.h` y `lpc17xx_pinsel.h` ya estaba aplicado en la notebook.
El paso 2 del README no se ejercitó en esta prueba.

## Reinicio sin depurador (criterio 2)
Confirmado por Benja: sin depurador, el LED alterna igual.

## Tests (criterio 3)
Salidas de la notebook, con gcc, make y Python 3.14.9 (en Fedora, 3.14.8):

```text
$ make -C firmware/tests test
mkdir -p build
gcc -std=c99 -Wall -Wextra -Werror -o build/test_ejemplo test_ejemplo.c
== build/test_ejemplo
OK

$ python3 -m venv app/.venv
$ app/.venv/bin/pip install -r app/requirements.txt
Successfully installed iniconfig-2.3.1 packaging-26.3 pluggy-1.7.0 pygments-2.21.0 pytest-9.2.0

$ cd app && .venv/bin/pytest
platform linux -- Python 3.14.9, pytest-9.2.0, pluggy-1.7.0
collected 1 item
tests/test_ejemplo.py .                                                  [100%]
1 passed in 0.03s
```

En la notebook solo se comprobó que los tests **pasan**. El comportamiento "si se rompe un test el comando
termina con error" se verificó en la PC de escritorio con los mismos archivos (T2 y T4: `make test` sale
con código 2 y `pytest` con código 1).
