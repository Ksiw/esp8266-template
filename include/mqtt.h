#pragma once

void mqttPrintf(const char* topic, const char* format, ...);
void mqtt_init();
void mqtt_process();