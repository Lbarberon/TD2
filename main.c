#include <wiringPi.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include "funciones.h"

//Constantes importantes
#define AD_BASE 120
#define ADDRESS 0x48

//Declaracion de pines de salida	
int vecOutput[8] = {26, 21, 20, 16, 12, 25, 24, 23};

//Declaracion de tablas de secuencias de luces
unsigned char LaCarrera[] = {0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x88, 0x48, 0x24, 0x14, 0x0A, 0x06, 0x03, 0x01, 0x00};

unsigned char AutoFantastico[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00};

unsigned char ElChoque[] = {0x81, 0x42, 0x24, 0x18, 0x00, 0x18, 0x24, 0x42, 0x81, 0x00};

unsigned char LaApilada[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x81, 0x41, 0x21, 0x11, 0x9, 0x05, 0x03, 0x83, 0x43, 0x23, 0x13, 0x0B, 0x07, 0x87, 0x47, 0x27, 0x11, 0x0F,0x8F, 0x4F, 0x2f, 0x1F, 0x9F, 0x5F, 0x3F, 0xBF, 0x7F, 0xFF, 0x00};

unsigned char Voy2Vuelvo1[] = {0x80, 0x40, 0x20, 0x40, 0x20, 0x10, 0x20, 0x10, 0x08, 0x10, 0x08, 0x04, 0x08, 0x04, 0x02, 0x04, 0x02, 0x01, 0x02, 0x01, 0x00, 0x01, 0x00};

unsigned char JuntosALaPar[] = {0x80, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00, 0x01, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00};

int controlpassword(void);
char printmenu(void);
char printSecuencia(void);
int getKey(unsigned int );
void barraVeloc(int );
void Secuencias(unsigned char * , int , float );
void Despliegue();
void Carga(float);
void VoyDosVuelvoUno(float);
void config0(void);		//Configura el modo de lectura de la funcion read
void waitms(void);		//Genera un delay

struct termios old, new;	//Estructuras termios para guardar las configuraciones del teclado

char aux = 'E';

int main(void){

  if(wiringPiSetup() == -1){
    fprintf(stdout, "No se puede iniciar wiringPi");
    return 1;
  }

  for(int i = 0 ; i < 8 ; i++){
    pinMode(vecOutput[i], OUTPUT);
    digitalWrite(vecOutput[i], 0);
  }

  pinMode(17, INPUT);

  Despliegue();

  return 0;
}

void Despliegue()
{
  int longitud, habilitacion = 0;
  char opcion = 0;
  float pausa = 0.5;

  for(int i = 0; i < 8 ; i++){
    digitalWrite(vecOutput[i], 0);
  }

  habilitacion = controlpassword();

  if(habilitacion == 1){
      opcion = printSecuencia();

      fflush(stdin);

      config0();
      aux = 'A';

      switch(opcion){
          case '1': longitud = sizeof(AutoFantastico)/sizeof(unsigned int);
                  puts("Auto Fantastico (Enter para Salir)");
                  Secuencias(AutoFantastico, longitud, pausa);
                  break;

          case '2': longitud = sizeof(ElChoque)/sizeof(unsigned int);
                  puts("El Choque (Enter para Salir)");
                  Secuencias(ElChoque, longitud, pausa);
                  break;

          case '3': longitud = sizeof(LaApilada)/sizeof(unsigned int);
                  puts("La Apilada (Enter para Salir)");
                  Secuencias(LaApilada, longitud, pausa);
                  break;

          case '4': longitud = sizeof(LaCarrera)/sizeof(unsigned int);
                  puts("La Carrera (Enter para Salir)");
                  Secuencias(LaCarrera, longitud, pausa);
                  break;

          case '5': longitud = sizeof(Voy2Vuelvo1)/sizeof(unsigned int);
                  puts("Voy2Vuelvo1 (Enter para Salir)");
                  Secuencias(Voy2Vuelvo1, longitud, pausa);
                  break;

          case '6': longitud = sizeof(JuntosALaPar)/sizeof(unsigned int);
                  puts("Juntos a la par (Enter para Salir)");
                  Secuencias(JuntosALaPar, longitud, pausa);
                  break;

          case '7': 
                  puts("Carga (Enter para Salir)");
                  Carga(pausa);
                  break;

          case '8': 
                  puts("Brincos largos (Enter para Salir)");
                  VoyDosVuelvoUno(pausa);
                  break;

          default:
      fprintf(stdout, "%d, no es una opcion valida", opcion);
      sleep(5);
                  break;
      }

    tcsetattr (0 , TCSANOW , &old);
  }
}

void config0(void){
  tcgetattr(0,&old);
  new = old;
  new.c_lflag &= ~(ECHO | ICANON);	//elimina eco y configura modo no canonico
  new.c_cc[VMIN]=0;			//setea el minimo numero de caracteres que espera read()
  new.c_cc[VTIME] = 0;			//setea tiempo maximo de espera de caracteres que lee read()
  tcsetattr(0,TCSANOW,&new);
}

void waitms(void){
  int n = 0;
  while(n<1000 && (aux!='\n')){
    read(0, &aux, 1);		// lectura de teclado recibe el fd del teclado.
    usleep(1000);
    n++;
  }
}

void Secuencias(unsigned char *Secuencia, int length, float pausa)
{       
        int mascara = 0;
        const unsigned char constante = 0x01;
        unsigned char resultado = 0;
  
        while(1){
            for(int j = 0 ; j < length ; j++){
              waitms();
              if(aux == '\n') //Enter
                break;

              if(aux == 65) //Flecha hacia arriba
                pausa -= 0.1;

              if(aux == 66) //Flecha hacia abajo
                pausa += 0.1;

              aux = 'E';

              for(unsigned int offset=0;offset<8;offset++){
                resultado = constante & (*(Secuencia + j) >> offset);
                
                if(resultado)
                  mascara = 1;
                else
                  mascara = 0;
                
                digitalWrite(vecOutput[offset], mascara);
              }
              sleep(pausa);
            }
          if(aux == '\n')
            break;
      }
}


void Carga(float pausa)
{
  while(1){
    for(int i = 0; i < 8; i++){
      waitms();
      if(aux == '\n')
        break;

      if(aux == 65) //Flecha hacia arriba
        pausa -= 0.1;

      if(aux == 66) //Flecha hacia abajo
        pausa += 0.1;

      aux = 'E';

      digitalWrite(vecOutput[i], 1);
      sleep(pausa);
    }
    for(int i =0; i < 8; i++){
      digitalWrite(vecOutput[i], 0);
    }

    if(aux == '\n')
      break;

  }
}

void VoyDosVuelvoUno(float pausa)
{
  while(1){
    digitalWrite(vecOutput[0], 1);
    sleep(pausa);
    digitalWrite(vecOutput[0], 0);

    for(int i = 0; i < 8; i++){
      waitms();
      if(aux == '\n')
        break;

      if(aux == 65) //Flecha hacia arriba
        pausa -= 0.1;

      if(aux == 66) //Flecha hacia abajo
        pausa += 0.1;

      aux = 'E';

      if((i % 2) == 0){
        digitalWrite(vecOutput[i], 1);
        sleep(pausa);
        digitalWrite(vecOutput[i], 0);
      }else{
        digitalWrite(vecOutput[i + 4], 1);
        sleep(pausa);
        digitalWrite(vecOutput[i + 4], 0);
      }
    }
    if(aux == '\n')
      break;
  }
}
