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
#define BARRA_LONGITUD 2

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
    tcgetattr(FD_STDIN, &t_old); // lee atributos del teclado
    
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

    puts("=========================================\n");
    puts("             Elegir secuencia \n");
    puts("=========================================\n");
    puts("(1) El Auto Fantástico");
    puts("(2) El Choque");
    puts("(3) La Apilada");
    puts("(4) La Carrera");
    puts("(5) Voy 2 vuelvo 1");
    puts("(6) Juntos a la par");
    puts("(7) Carga");
    puts("(8) Brincos largos");

    opcion = getc(stdin);

    system("clear");
    tcgetattr(FD_STDIN, &t_old); // lee atributos del teclado
    
    return opcion;
}


// getKey: Función para obtener la tecla presionada
// Parámetros: key - valor de la tecla presionada
// Valor de retorno: Entero que representa la tecla interpretada (1: Flecha arriba, 2: Flecha abajo, 3: Enter, 0: Otra tecla)
int getKey(unsigned int key)
{
    switch (key)
    {
    case 0x415b1b: // ARROW UP
        return 1;

    case 0x425b1b: // ARROW DOWN
        return 2;

    default:
        if ((key & 0x000000FF) == 0x0a) // ENTER
            return 3;
        else
            return 0; // OTHER
    }
}




void barraVeloc(int val) {
    val = (val < 0) ? 0 : (val > MAX_VELOCIDAD) ? MAX_VELOCIDAD : val;
    int bloques = (val * BARRA_LONGITUD) / MAX_VELOCIDAD;


    for (int i = 0; i < BARRA_LONGITUD; i++) {
        if (i < bloques) {
            printf("▒");
        } else {
            printf("  ");  // Espacio en blanco para borrar bloques anteriores
        }
    }

}

#endif
