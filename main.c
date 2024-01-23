#include "EasyPIO.h"
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

//Constantes importantes
#define FD_STDIN 0
#define MAX_INTENTOS 3
#define LONGITUD_CONTRASENA 5
#define CONTRASENA "12345"
#define ENTER 10
#define FD_STDIN 0
#define AD_BASE 120
#define ADDRESS 0 x48
#define NUM_LEDS 8

//Declaracion de pines de salida	
int vecOutput[8] = {26, 21, 20, 16, 12, 25, 24, 23};

//Declaracion de tablas de secuencias de luces
unsigned int LaCarrera[] = {0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x88, 0x48, 0x24, 0x14, 0x0A, 0x06, 0x03, 0x01, 0x00};

unsigned int AutoFantastico[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00};

unsigned int ElChoque[] = {0x81, 0x42, 0x24, 0x18, 0x00, 0x18, 0x24, 0x42, 0x81, 0x00};

unsigned int LaApilada[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x81, 0x41, 0x21, 0x11, 0x9, 0x05, 0x03, 0x83, 0x43, 0x23, 0x13, 0x0B, 0x07, 0x87, 0x47, 0x27, 0x11, 0x0F,0x8F, 0x4F, 0x2f, 0x1F, 0x9F, 0x5F, 0x3F, 0xBF, 0x7F, 0xFF, 0x00};

unsigned int Voy2Vuelvo1[] = {0x80, 0x40, 0x20, 0x40, 0x20, 0x10, 0x20, 0x10, 0x08, 0x10, 0x08, 0x04, 0x08, 0x04, 0x02, 0x04, 0x02, 0x01, 0x02, 0x01, 0x00, 0x01, 0x00};

unsigned int JuntosALaPar[] = {0x80, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00, 0x01, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00};

int controlpassword(void);
int printmenu(void);
int printSecuencia(void);
int getKey(unsigned int );
void barraVeloc(int );
void Secuencia(int *, unsigned int *, int, int);
void Despliegue(int *);
void Carga(int *, int);
void VoyDosVuelvoUno(int *, int);
void config0(void);		//Configura el modo de lectura de la funcion read
void waitms(void);		//Genera un delay

struct termios
{
  tcflag_t c_iflag;           /* input mode flags */
  tcflag_t c_oflag;           /* output mode flags */
  tcflag_t c_cflag;           /* control mode flags */
  tcflag_t c_lflag;           /* local mode flags */
  cc_t c_line;                /* line discipline */
  cc_t c_cc[NCCS];            /* control characters */
  speed_t c_ispeed;           /* input speed */
  speed_t c_ospeed;           /* output speed */
};

struct termios old, new;	//Estructuras termios para guardar las configuraciones del teclado
char aux = 'E';

int main(void){

  pioInit();

  for(int i = 0 ; i < 8 ; i++){
    pinMode(vecOutput[i], OUTPUT);
    digitalWrite(vecOutput[i], 0);
  }

  pinMode(17, INPUT);
  
  Despliegue(vecOutput);
  
  return 0;
}

void Despliegue(*vecOutput)
{
  int habilitacion = 0;
  int opcion = 0;
  int longitud, pausa;

  for(int i = 0; i < 8 ; i++){
    digitalWrite(vecOutput[i], 0);
  }
  
  habilitacion = controlpassword();
  
  if(habilitacion == 1){
      opcion = printSecuencia();
      config0();
      aux = 'A';
    
      switch(opcion){
          case 1: longitud = sizeof(LaCarrera)/sizeof(unsigned int);
                  puts("La Carrera (Enter para Salir)");
                  Secuencia(vecOutput, LaCarrera, longitud, pausa);
                  break;

          case 2: longitud = sizeof(AutoFantastico)/sizeof(unsigned int);
                  puts("Auto Fantastico (Enter para Salir)");
                  Secuencia(vecOutput, AutoFantastico, longitud, pausa);
                  break;

          case 3: longitud = sizeof(ElChoque)/sizeof(unsigned int);
                  puts("El Choque (Enter para Salir)");
                  Secuencia(vecOutput, ElChoque, longitud, pausa);
                  break;

          case 4: longitud = sizeof(LaApilada)/sizeof(unsigned int);
                  puts("La Apilada (Enter para Salir)");
                  Secuencia(vecOutput, LaApilada, longitud, pausa);
                  break;
  
          case 5: longitud = sizeof(Voy2Vuelvo1)/sizeof(unsigned int);
                  puts("Voy2Vuelvo1 (Enter para Salir)");
                  Secuencia(vecOutput, Voy2Vuelvo1, longitud, pausa);
                  break;

          case 6: longitud = sizeof(JuntosALaPar)/sizeof(unsigned int);
                  puts("Juntos a la par (Enter para Salir)");
                  Secuencia(vecOutput, JuntosALaPar, longitud, pausa);
                  break;

          case 7: 
                  puts("Carga (Enter para Salir)");
                  Carga(vecOutput, pausa);
                  break;

          case 8: 
                  puts("Brincos largos (Enter para Salir)");
                  VoyDosVuelvoUno(vecOutput, pausa);
                  break;

          default:
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

void Secuencia(int *vecOutput, unsigned int *Secuencia, int length, int pausa)
{       
        int mascara = 0;

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
                if(1 & (Secuencia[j] >> offset))
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


void Carga(*vecOutput, pausa)
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
    if(aux == '\n')
      break;

  }
}

void VoyDosVuelvoUno(*vecOutput , pausa)
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
