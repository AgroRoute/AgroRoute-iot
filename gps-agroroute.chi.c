#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SECOND 1000000  // microsegundos

typedef struct {
    uart_dev_t uart;
    double     lat;
    double     lon;
} chip_state_t;

// Esta función se llama cada segundo
static void chip_timer_event(void *user_data) {
    chip_state_t *chip = (chip_state_t *)user_data;

    // Incrementamos posición (simula movimiento)
    chip->lat += 0.0001;
    chip->lon += 0.0001;

    // Preparamos la línea GPGGA con la lat/lon actuales
    char nmea[128];
    snprintf(nmea, sizeof(nmea),
      "$GPGGA,123519,%.4f,N,%.4f,W,1,08,0.9,545.4,M,46.9,M,,*47\r\n",
      fabs(chip->lat), fabs(chip->lon)
    );

    // La enviamos por UART (pin TX)
    uart_write(chip->uart, (uint8_t *)nmea, strlen(nmea));
}

void chip_init(void) {
    // Para poder ver estos printf en la consola de Wokwi
    setvbuf(stdout, NULL, _IOLBF, 1024);

    // Reservamos e inicializamos el estado del chip
    chip_state_t *chip = calloc(1, sizeof(chip_state_t));
    if (!chip) return;

    // Valores iniciales de lat/lon (por ejemplo Lima)
    chip->lat = -12.0464;
    chip->lon = -77.0428;

    // Configuramos los pines TX/RX según tu .chip.json
    int tx_pin = pin_init("TX", INPUT);
    int rx_pin = pin_init("RX", INPUT);

    // Inicializamos UART a 9600bps en esos pines
    const uart_config_t uart_cfg = {
        .tx        = tx_pin,
        .rx        = rx_pin,
        .baud_rate = 9600,
        .rx_data   = NULL,
        .write_done= NULL,
        .user_data = chip
      };
    chip->uart = uart_init(&uart_cfg);

    // Creamos y arrancamos un timer periódico cada 1s
    const timer_config_t timer_cfg = {
        .callback  = chip_timer_event,
        .user_data = chip
      };
    timer_t timer = timer_init(&timer_cfg);
    timer_start(timer, SECOND, true);

    printf("GPS AgroRoute custom chip initialized\n");
}
