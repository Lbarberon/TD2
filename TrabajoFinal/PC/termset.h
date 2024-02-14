#ifndef TERMSET_H
#define TERMSET_H

#include <termios.h>

// Definicion de las estructuras para la configuracion del puerto serie
struct termios ttyold, ttynew;

// Prototipo de la funcion termset
// Parametros:
// - fd: descriptor de archivo - dispositivo (por ejemplo, /dev/ttyUSB0)
// - baudrate: velocidad de comunicacion (por ejemplo, 9600, 115200)
// - ttyold: estructura termios actual
// - ttynew: nueva estructura termios
int termset(int fd, int baudrate, struct termios *ttyold, struct termios *ttynew);

#endif