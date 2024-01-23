#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "funciones.h"
#include <termios.h>

#define FD_STDIN 0

int key;


// control de contraseña

int controlpassword(void){
  
    char contrasenaIngresada[LONGITUD_CONTRASENA + 1]; // +1 para el carácter nulo
    char caracter = 0;
    int intentos = 0;

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
  
    return 0; // Contraseña incorrecta o demasiados intentos
}



// menu principal
int printmenu(void)
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
int printSecuencia(void)
{
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

    char choice;
    scanf("%c", &choice);

    system("clear");
  
    return choice;
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
