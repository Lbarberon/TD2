#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "termset.h"
#include "funciones.h"

#define FD_STDIN 0

char printSecuencia(void);
void lecturaLocal(void);

char aux = 'E';

int main(void) {
    int fd;
    struct termios oldtty, newtty;
    char opcion;
  
    // Apertura del puerto serie
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        printf("ERROR: No se pudo abrir el dispositivo.\n");
        return -1;
    }

    // Configuracion del TTY
    if (termset(fd, 9600, &oldtty, &newtty) == -1) {
        printf("ERROR: No se pudo configurar el TTY.\n");
        return 1;
    }

    while(opcion != '9'){ // opcion 9 es volver al menu principal
      opcion = printSecuencia();

      switch(opcion){
          case '1': printf("Auto Fantastico (Enter para Salir)");
                    break;
          case '2': printf("El Choque (Enter para Salir)");
                    break;
          case '3': printf("La Apilada (Enter para Salir)");
                    break;
          case '4': printf("La Carrera (Enter para Salir)");
                    break;
          case '5': printf("Voy2Vuelvo1 (Enter para Salir)");
                    break;
          case '6': printf("Juntos a la par (Enter para Salir)");
                    break;
          case '7': printf("Carga (Enter para Salir)");
                    break;
          case '8': printf("Brincos largos (Enter para Salir)");
                    break;
          case '9': break;
          default:  printf("%d no es una opcion valida", opcion);
                    sleep(3);
                    break;
      }
      printf("\n");
      
      tcflush(fd, TCIOFLUSH);  // Limpieza del bufer de entrada/salida
      write(fd, &opcion, 1);   // envia opcion a Rapberry
      tcdrain(fd);   // Espera hasta que se haya transmitido toda la salida escrita
      
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
    
    return 0;
}
