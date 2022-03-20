/*! Printing on stdout/stderr */
#pragma once

int printf(char *format, ...);
int puts(char *format, ...);
void warn(char *format, ...);

int stdio_init();
