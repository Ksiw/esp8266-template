#pragma once
#include <arduino.h>

void main_init();
void main_process();
void parce_incoming_command(char *topic, byte *payload, unsigned int length);