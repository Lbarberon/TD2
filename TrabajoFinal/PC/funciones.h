#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#ifndef _FUNCIONES
#define _FUNCIONES

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
	tcgetattr(FD_STDIN, &t_old);
	t_new = t_old;
	t_new.c_lflag &= ~(ECHO | ICANON);	//elimina eco y configura modo no canonico
	t_new.c_cc[VMIN]=1;			//setea el minimo numero de caracteres que espera read()
	t_new.c_cc[VTIME] = 0;			//setea tiempo maximo de espera de caracteres que lee read()
	
	t_new1 = t_new;
	t_new.c_cc[VMIN]=0;
}
	

// menuSecuencia: Función para el menú de selección de secuencia
// Valor de retorno: Entero que representa la opción elegida
char printSecuencia(void)
{
    char opcion = 0;

    tcsetattr(FD_STDIN, TCSANOW, &t_new);

    system("clear");

	printf("-----------------------------------------\n");
	printf("               SECUENCIAS \n");
	printf("-----------------------------------------\n");
	printf("(1) El auto fantastico\n");
	printf("(2) El choque\n");
	printf("(3) La apilada\n");
	printf("(4) La carrera\n");
	printf("(5) Voy 2 vuelvo 1\n");
	printf("(6) Juntos a la par\n");
	printf("(7) Carga\n");
	printf("(8) Brincos largos\n");
	printf("(9) Volver a menu principal\n");
  
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
