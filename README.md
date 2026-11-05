# Sintetizador digital polifónico con LPC1769

TP integrador de Electrónica Digital III (FCEFyN-UNC). Especificación en
[`docs/especificacion_sintetizador_lpc1769.md`](docs/especificacion_sintetizador_lpc1769.md).

> **Versión mínima (Bloque 0).** Cubre lo necesario para reconstruir y probar el firmware.
> Los pasos de CachyOS todavía no están verificados.

## Placa

- LPCXpresso LPC1769 (Cortex-M3), con sonda de depuración on-board (NXP LPC11U3x CMSIS-DAP).
- Reloj del sistema: 100 MHz. LED de prueba en P0.22.

## Herramientas y versiones (PC de escritorio, Fedora 44)

| Herramienta | Versión |
|---|---|
| MCUXpresso IDE | 25.6.136 (dentro del distrobox `ubuntu-mcuxpresso`, Ubuntu 22.04) |
| LinkServer | 25.6.131 |
| Compilador del firmware | Arm GNU Toolchain 14.2.Rel1 (GCC 14.2.1), el que trae MCUXpresso |
| Tests de lógica | gcc 16.2.1 y GNU Make 4.4.1 |
| Aplicación | Python 3.14.8 y pytest 9.2.0 (`app/requirements.txt`) |

Para que LinkServer abra la sonda en Linux, el host necesita las reglas udev de NXP
(`85-lpcxpresso.rules`, con la regla `KERNEL=="hidraw*"` para `1fc9:0132`). Si aparece
*Probe not open*, faltan.

## Origen de los drivers

Los drivers no están en este repositorio. Vienen de
[`David-A-T-M/LPC17xx-CMSIS-Driver-Enhancement`](https://github.com/David-A-T-M/LPC17xx-CMSIS-Driver-Enhancement),
commit `511893e` (2026-03-05). El proyecto de MCUXpresso es `CMSISv2p00_LPC17xx`, dentro de ese repo.

⚠️ **Se usó con dos cambios locales** que no están en ese commit: en `Drivers/inc/lpc17xx_clkpwr.h` y
`Drivers/inc/lpc17xx_pinsel.h`, `#include "lpc17xx.h"` pasa a `#include "LPC17xx.h"`. En Linux el nombre
del archivo distingue mayúsculas y minúsculas. Además, el `.cproject` de la biblioteca tiene cambios
locales sin revisar. Hasta que el repo de drivers los incluya, hay que aplicarlos a mano.

## Reconstruir y ver el firmware en la placa

1. Clonar este repo y el de los drivers. La ubicación en disco da igual: el IDE resuelve la biblioteca
   por nombre de proyecto dentro del workspace (paso 3).
2. Aplicar el cambio de mayúsculas en los dos headers:
   ```bash
   cd LPC17xx-CMSIS-Driver-Enhancement/CMSISv2p00_LPC17xx/Drivers/inc
   sed -i 's/#include "lpc17xx.h"/#include "LPC17xx.h"/' lpc17xx_clkpwr.h lpc17xx_pinsel.h
   ```
3. En MCUXpresso, importar los dos proyectos (`CMSISv2p00_LPC17xx` y `sintetizador`) al mismo workspace.
4. Compilar `CMSISv2p00_LPC17xx` en Debug y después `sintetizador` en Debug (*Project → Clean* y build).
   Solo se soporta la configuración Debug. Tiene que dar 0 errores y 0 warnings.
5. Descargar a la placa con *Debug* y ejecutar.

Resultado esperado:
- El LED (P0.22) alterna con un período de 1 s.
- La consola de semihosting muestra `Sintetizador LPC1769: reloj de 100000000 Hz`.
- Con la placa reiniciada sin depurador (desenchufar y volver a enchufar el USB), el LED alterna igual.

## Tests

```bash
# Lógica del firmware en la PC (gcc + make)
make -C firmware/tests test

# Aplicación (pytest)
python3 -m venv app/.venv
app/.venv/bin/pip install -r app/requirements.txt
cd app && .venv/bin/pytest
```

Los dos comandos terminan con código distinto de 0 si un test falla.

## Estructura

```text
firmware/   src/ inc/ tests/ tools/   firmware y sus tests de lógica en la PC
app/        tests/                    aplicación de escritorio
docs/       constitución, spec, backlog, stories/ y evidencia/
```

La evidencia de cada story está en `docs/evidencia/`.
