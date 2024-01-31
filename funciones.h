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
#define MIN_VELOCIDAD 2
#define BARRA_LONGITUD 5 

struct termios t_old, t_new;

int key;

// control de contraseña
int controlpassword(void){

    char contrasenaIngresada[LONGITUD_CONTRASENA + 1]; // +1 para el carácter nulo
    char caracter = 0;
    int intentos = 0;

    // Terminal no bloqueante inicial
    tcgetattr(FD_STDIN, &t_old); // lee atributos del teclado
    t_new = t_old;
    t_new.c_lflag &= ~(ECHO | ICANON); // anula entrada canónica y eco
    t_new.c_cc[VMIN] = 1;              // setea el minimo numero de caracteres que espera read()
    t_new.c_cc[VTIME] = 0;             // setea tiempo maximo de espera de caracteres que lee read()
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
            return 1; // Contraseña correcta
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
    
    return 0; // Contraseña incorrecta o demasiados intentos
}



// menu principal
char printmenu(void)
{
    system("clear");
    puts("=========================================\n");
    puts("          MENÚ PRINCIPAL\n");
    puts("=========================================\n");
    puts("(1) Elegir Secuencia");
    puts("(2) Definir Velocidad Inicial");
    puts("(3) Modo Remoto");
    puts("(4) Modo Local");
    puts("(5) Salir");

    char choice;
    do
    {
        read(FD_STDIN, &choice, 1);
        choice -= 48;
    } while (choice < 1 || choice > 4);

    return choice;
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
    puts("(1)\t\t El Auto Fantastico");
    puts("(2)\t\t El Choque");
    puts("(3)\t\t La Apilada");
    puts("(4)\t\t La Carrera");
    puts("(5)\t\t Voy 2 vuelvo 1");
    puts("(6)\t\t Juntos a la par");
    puts("(7)\t\t Carga");
    puts("(8)\t\t Brincos largos");

    opcion = getc(stdin);

    system("clear");
        
    return opcion;
}


// changePause: Modifica la velocidad de las secuencias
// Valor de retorno: Entero que representa la tecla interpretada (1: Flecha arriba, 2: Flecha abajo, 0: Otra tecla)
int changePause()
{
    switch (aux)
    {
        case 0x415b1b: // ARROW UP
             return 10;

        case 0x425b1b: // ARROW DOWN
             return -10;

        default:
             return 0; // OTHER
    }
}

int barraVeloc(int pausa) {
    int modificacion, resultado;
    
    modificacion = changePause();
    resultado = pausa + modificacion;
    
    pausa = (resultado < MIN_VELOCIDAD) ? MIN_VELOCIDAD : (resultado > MAX_VELOCIDAD) ? MAX_VELOCIDAD : resultado;
    
    fprintf(stdout, "Velocidad : \t\t\t");
    
    for (int i = 0; i < BARRA_LONGITUD; i += 2) {
        if (pausa >= (MIN_VELOCIDAD + i)) {
            printf("▒");
        } else {
            printf(" ");  // Espacio en blanco para borrar bloques anteriores
        }
    }
    
    return pausa;
}

#endif
