#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <termios.h>

#ifndef _FUNCIONES
#define _FUNCIONES

#define FD_STDIN 0
#define MAX_INTENTOS 3
#define LONGITUD_CONTRASENA 5
#define CONTRASENA "12345"
#define MAX_VELOCIDAD 10
#define MIN_VELOCIDAD 0 

struct termios t_old, t_new, t_new1;

extern int pausa;
extern char aux;

void config0(void) // Obtiene configuraciones
{ 
  tcgetattr(FD_STDIN, &t_old); // lee atributos del teclado
  t_new = t_old;               // t_new configura la entrada para controlpassword()
  t_new.c_lflag &= ~(ECHO | ICANON); // anula entrada canónica y eco
  t_new.c_cc[VMIN] = 1;              // setea el minimo numero de caracteres que espera read()
  t_new.c_cc[VTIME] = 0;             // setea tiempo maximo de espera de caracteres que lee read()
    
  t_new1 = t_new;           // t_new1 configura la entrada no bloqueante  
  t_new1.c_cc[VMIN]=0;			// setea el minimo numero de caracteres que espera read()
}

char controlpassword(void){ // control de contrasenia
    char contrasenaIngresada[LONGITUD_CONTRASENA + 1]; // +1 para el carácter nulo
    char caracter = 0;
    int intentos = 0;

    tcsetattr(FD_STDIN, TCSANOW, &t_new);
    system("clear");

    while (intentos < MAX_INTENTOS) {
        int i = 0;
        printf("\nPassword: ");

        while (caracter != 10) { // Mientras no se presione ENTER
            caracter = getchar();

            if (caracter == 127) { // Si se presiona BACKSPACE
                if (i > 0) {
                    printf("\b \b"); // Borra un asterisco de la pantalla
                    i--;
                }
            } else if (caracter != 10) { // Si no se presiona ENTER
                if (i < LONGITUD_CONTRASENA) {
                    contrasenaIngresada[i] = caracter;
                    putchar('*'); // Muestra un asterisco en lugar del carácter
                    i++;
                }
            }
        }
        contrasenaIngresada[i] = '\0'; // Agrega el carácter nulo al final de la cadena

        if (strcmp(contrasenaIngresada, CONTRASENA) == 0) {
            printf("\nPassword correcta!\n");
            return 'A'; // Contraseña correcta
        } else {
            printf("\nPassword incorrecta\n");
            intentos++;
        }
        caracter = 0;
    }
    if (intentos == MAX_INTENTOS) {
        printf("Demasiados intentos fallidos. El programa se aborta.\n");
    }
    system("clear");
    tcsetattr(FD_STDIN, TCSANOW, &t_old); // lee atributos del teclado

    return 'D'; // Contraseña incorrecta o demasiados intentos
}

char printMenu(void) //Menu principal
{
    char opcion;
    system("clear");
    tcsetattr(FD_STDIN, TCSANOW, &t_new);
    
    puts("-----------------------------------------\n");
    puts("          MENU PRINCIPAL\n");
    puts("-----------------------------------------\n");
    puts("(1)\t\t Modo Local");
    puts("(2)\t\t Modo remoto");
    puts("(3)\t\t Setear velocidad inicial");
    puts("(4)\t\t Testeo de leds");
    puts("(5)\t\t Salir");

    read(FD_STDIN, &opcion, 1);
    
    system("clear");
    tcsetattr(FD_STDIN, TCSANOW, &t_old);
  
    return opcion;
}

int seteoVelocidad(void){
  int velocidad_inicial = 5;
  unsigned int velocidad, valorADC;

  system("clear");
  tcsetattr(FD_STDIN, TCSANOW, &t_new);
  
  puts("-------------------------------------\n");
  puts("Seteo de velocidad de las secuencias\n");
  puts("-------------------------------------\n");

  while (aux != '\n'){
      valorADC = analogRead(0);
      if ((velocidad != pausa) || velocidad_inicial){
          system("clear");
          printf("\nVelocidad Inicial (Enter para setear)");
          velocidad = 1 + (valorADC * 9 / 255);
          
          printf("\nVelocidad Inicial: %d ms\n", velocidad);
          velocidad_inicial = 0;
      }
      read(FD_STDIN, &aux, 1);
  }
  system("clear");
  tcsetattr(FD_STDIN, TCSANOW, &t_old);

  return velocidad;
}

char printSecuencia(void)
{
    char opcion = 'A';
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
  
    read(FD_STDIN, &opcion, 1);
    system("clear");
    tcsetattr(FD_STDIN, TCSANOW, &t_old);

    return opcion;
}

int cambiarPausa(void) // Modifica la velocidad de las secuencias
{
    switch (aux)
    {
        case 'A': // ARROW UP
             return -1;
        case 'B': // ARROW DOWN
             return 1;
        default:
             return 0; // OTHER
    }
}
  
void lecturaLocal(void){ // Obtiene la tecla ingresada
  int n = 0;
  char ingreso[3] = "aaa";

  while(n<1000 && ((aux!='\n') | ((aux!= 'A') | (aux != 'B')))){
    read(FD_STDIN, &ingreso, 3);

    if(ingreso[0] == 27){
         if(ingreso[1] == '['){
            if(ingreso[2] == 'A') // Flecha ascendente
              aux = 'A';
            else if(ingreso[2] == 'B') // Flecha descendente
              aux = 'B';
        }
    }
    if(ingreso[0] == '\n') // Enter
          aux = '\n';

    usleep(500);
    n++;
  }
}
  
void controlVeloc(int fd, char modo) // Obtiene el valor de pausa
{
    int modificacion, resultado;

    if(modo == 'L')
      lecturaLocal();
    else if(modo == 'R'){
      if(serialDataAvail(fd))
        aux = serialGetchar(fd);
    }
    modificacion = cambiarPausa();
    resultado = pausa + modificacion;

    pausa = (resultado < MIN_VELOCIDAD) ? MIN_VELOCIDAD : (resultado > MAX_VELOCIDAD) ? MAX_VELOCIDAD : resultado;
}

#endif
