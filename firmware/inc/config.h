/* Configuración del sintetizador: pines, tiempos y prioridades (constitución, principio 9).
 * Solo constantes: lo puede incluir cualquier módulo, también los de lógica. */
#ifndef CONFIG_H
#define CONFIG_H

/* LED de la placa (prueba de vida del Bloque 0, spec 3.13) */
#define LED_PORT            0        /* P0.22 */
#define LED_PIN             22
#define LED_HALF_PERIOD_MS  500      /* alterna cada 500 ms: período de 1 s */

#endif /* CONFIG_H */
