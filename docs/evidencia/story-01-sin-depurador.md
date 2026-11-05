# Story 1 · M4 · Reinicio sin depurador

**Criterio:** al reiniciar la placa sin el depurador, el LED alterna igual.

## Procedimiento
- Firmware: el de `story-01-arranque.md` (mensaje por semihosting solo si `C_DEBUGEN` está activo).
- Se terminó la sesión de depuración, se desenchufó el USB y se volvió a enchufar, para que `DHCSR`
  se limpie con el power-on. El IDE no estaba controlando la placa.

## Resultado
- El LED alterna solo, sin depurador (semihosting no cuelga el arranque).
- Benja observó que el período sigue siendo de aproximadamente 1 s. Esta observación es cualitativa:
  no se repitió el conteo de períodos con el celular. El período medido con rigor es el de
  `story-01-led-periodo.md`.

## Conclusión
El firmware arranca y alterna el LED sin depurador conectado.
