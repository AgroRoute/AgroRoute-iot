// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 branco alberto villegas peralta

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  // TODO: Put your chip variables here
  pin_t in_pin;
} chip_state_t;

static void on_input_change(void *user_data, pin_t pin, uint32_t value) {
  if (value == HIGH) printf("💧 Humidifier ON\n");
  else printf("🚱 Humidifier OFF\n");
}

void chip_init() {
  setvbuf(stdout, NULL, _IOLBF, 1024);


  chip_state_t *chip = calloc(1, sizeof(chip_state_t));
  if (!chip) return;

  chip->in_pin = pin_init("IN", INPUT);

  uint32_t initial = pin_read(chip->in_pin);
  if (initial == HIGH)   printf("💧 Humidifier ON (initial)\n");
  else                   printf("🚱 Humidifier OFF (initial)\n");

  const pin_watch_config_t watch_cfg = {
    .edge       = BOTH,
    .pin_change = on_input_change,
    .user_data  = chip,
  };
  pin_watch(chip->in_pin, &watch_cfg);

  printf("🔧 Humidifier chip inicializado\n");

  
}
