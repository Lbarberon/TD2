#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "termset.h"
#include "funciones.h"

#define FD_STDIN 0

char printSecuencia(void);
void lecturaLocal(void);
void config0(void);

struct termios t_old, t_new, t_new1;

char aux = 'E';

int main(void) {
    int fd;
    char opcion;
	
	config0();
	
    // Apertura del puerto serie
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        printf("ERROR: No se pudo abrir el dispositivo.\n");
        return -1;
    }

    // Configuracion del TTY
    if (termset(fd, 9600, &t_old, &t_new) == -1) {
        printf("ERROR: No se pudo configurar el TTY.\n");
        return 1;
    }

    while(opcion != '9'){ // opcion 9 es volver al menu principal
	  
      opcion = printSecuencia();
	
      switch(opcion){
          case '1': printf("Auto Fantastico (Enter para Salir)\n");
                    break;
          case '2': printf("El Choque (Enter para Salir)\n");
                    break;
          case '3': printf("La Apilada (Enter para Salir)\n");
                    break;
          case '4': printf("La Carrera (Enter para Salir)\n");
                    break;
          case '5': printf("Voy2Vuelvo1 (Enter para Salir)\n");
                    break;
          case '6': printf("Juntos a la par (Enter para Salir)\n");
                    break;
          case '7': printf("Carga (Enter para Salir)\n");
                    break;
          case '8': printf("Brincos largos (Enter para Salir)\n");
                    break;
          case '9': break;
          default:  printf("No es una opcion valida\n");
                    sleep(3);
                    break;
      }
      printf("\n");

      tcflush(fd, TCIOFLUSH);  // Limpieza del bufer de entrada/salida
      write(fd, &opcion, 1);   // envia opcion a Rapberry
      tcdrain(fd);   // Espera hasta que se haya transmitido toda la salida escrita

	  tcsetattr(FD_STDIN, TCSANOW, &t_new1);
      if((opcion >= 1) && (opcion < 9))
        while(aux != '\n'){ // enter finaliza la secuencia elegida
          aux = 'E'; // actualiza el valor de aux para un correcto funcionamiento de la funcion lecturaLocal

          lecturaLocal();  //actualiza el valor de aux segun las teclas presionadas(flechas y enter)

          tcflush(fd, TCIOFLUSH);
          write(fd, &aux, 1); // envia aux a Rapberry
          tcdrain(fd);  

          sleep(1);
        }
    }
    close(fd);
	tcsetattr(FD_STDIN, TCSANOW, &t_old);

    return 0;
}
