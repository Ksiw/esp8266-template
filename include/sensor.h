#pragma once

void sensor_init();
void process_sensor();
float get_temperature();
float get_humidity();
bool get_new_data();
void set_temperature_delta(float d);