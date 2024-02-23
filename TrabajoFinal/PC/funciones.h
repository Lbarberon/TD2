#ifndef _FUNCIONES
#define _FUNCIONES

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <termios.h>

#define FD_STDIN 0
#define MAX_INTENTOS 3
#define LONGITUD_CONTRASENA 5
#define CONTRASENA "12345"
#define MAX_VELOCIDAD 10
#define MIN_VELOCIDAD 0 

extern struct termios t_old, t_new, t_new1;

extern int pausa;
extern char aux;

void config0(void){
  tcgetattr(FD_STDIN, &t_old); // lee atributos del teclado
  
  t_new1 = t_old;
  t_new.c_lflag &= ~(ECHO | ICANON); // anula entrada canónica y eco
  t_new.c_cc[VMIN] = 1;              // setea el minimo numero de caracteres que espera read()
  t_new.c_cc[VTIME] = 0;             // setea tiempo maximo de espera de caracteres que lee read()
  
  t_new1.c_cc[VMIN]=0;			//setea el minimo numero de caracteres que espera read()
}

// menuSecuencia: Función para el menú de selección de secuencia
// Valor de retorno: Entero que representa la opción elegida
char printSecuencia(void)
{
    char opcion = 0;

    tcsetattr(FD_STDIN, TCSANOW, &t_new);

    system("clear");

    puts("-----------------------------------------\n");
    puts("               SECUENCIAS \n");
    puts("-----------------------------------------\n");
    puts("(1)\t\t El auto fantastico");
    puts("(2)\t\t El choque");
    puts("(3)\t\t La apilada");
    puts("(4)\t\t La carrera");
    puts("(5)\t\t Voy 2 vuelvo 1");
    puts("(6)\t\t Juntos a la par");
    puts("(7)\t\t Carga");
    puts("(8)\t\t Brincos largos");
    puts("(9)\t\t Volver a menu principal");
  
    opcion = getchar();

    system("clear");
    tcsetattr(FD_STDIN, TCSANOW, &t_old);

    return opcion;
}

void lecturaLocal(void){
  int n = 0;
  char ingreso[3] = "aaa";

  while(n<1000 && ((aux!='\n') | ((aux!= 'A') | (aux != 'B')))){
    read(FD_STDIN, &ingreso, 3);

    if(ingreso[0] == 27){
         if(ingreso[1] == '['){
            if(ingreso[2] == 'A')
              aux = 'A';
            else if(ingreso[2] == 'B')
              aux = 'B';
        }
    }
  if(ingreso[0] == '\n')
          aux = '\n';

    usleep(500);
      n++;
  }
}
  
#endif
