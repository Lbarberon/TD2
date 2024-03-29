#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include "funciones.h"
#include <pcf8591.h>

//Constantes importantes
#define AD_BASE 120
#define Address 0x48
#define BASE 64

char aux = 'E';
int ind_veloc = 2;
unsigned int velocidad[] = {1, 10, 1000, 800000, 1000000000};

//Declaracion de pines de salida -- Modificar a los pines de placa	
int vecOutput[8] = {4, 5, 6, 26,  27, 28, 29, 25}; // Placa RP1 en lab

//Declaracion de tablas de secuencias de luces
unsigned char LaCarrera[] = {0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x88, 0x48, 0x24, 0x14, 0x0A, 0x06, 0x03, 0x01, 0x00};
unsigned char AutoFantastico[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00};
unsigned char ElChoque[] = {0x81, 0x42, 0x24, 0x18, 0x00, 0x18, 0x24, 0x42, 0x81, 0x00};
unsigned char LaApilada[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x81, 0x41, 0x21, 0x11, 0x9, 0x05, 0x03, 0x83, 0x43, 0x23, 0x13, 0x0B, 0x07, 0x87, 0x47, 0x27, 0x11, 0x0F,0x8F, 0x4F, 0x2f, 0x1F, 0x9F, 0x5F, 0x3F, 0xBF, 0x7F, 0xFF, 0x00};
unsigned char Voy2Vuelvo1[] = {0x80, 0x40, 0x20, 0x40, 0x20, 0x10, 0x20, 0x10, 0x08, 0x10, 0x08, 0x04, 0x08, 0x04, 0x02, 0x04, 0x02, 0x01, 0x02, 0x01, 0x00, 0x01, 0x00};
unsigned char JuntosALaPar[] = {0x80, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00, 0x01, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00};

extern struct termios t_old, t_new, t_new1;	//Estructuras termios para guardar las configuraciones del teclado

char printMenu(void);
char printSecuencia(void);
int seteoVelocidad(void);
char controlpassword(void);
int cambiarPausa(void);
void controlVeloc(int , char );
void Secuencias(int , unsigned char * , int , char);
void Mod(int , char );
void Carga(int , char);
void BrincosLargos(int , char );
void apagarLeds(void);
void config0(void);
void lectura(void);

extern void retardo(unsigned long int Op);

int main(void){
  char opcion = 'A';
  int fd;
  
  // Inicializa la configuracion de WiringPi
  if(wiringPiSetup() == -1) {
    printf("No se puede iniciar WiringPi\n");
    return -1;
  }
  pcf8591Setup(BASE, Address);

  config0(); // Obtenemos las configuraciones de teclado

  for(int i = 0 ; i < 8 ; i++){
    pinMode(vecOutput[i], OUTPUT);
    digitalWrite(vecOutput[i], 0);
  }
  if(controlpassword() == 'A')
    while(opcion != '5'){
      system("clear");
      opcion = printMenu();

      switch(opcion){
        case '1': Mod(-1, 'L');
                  break;
        case '2': fd = serialOpen("/dev/ttyS0", 9600); //configuracion de puerto serial
                  if(fd < 0) {
                    printf("No se puede abrir el dispositivo serial\n");
                    return 1;
                  }
                  Mod(fd, 'R'); 
                  serialClose(fd);
                  break;
	case '3': ind_veloc = seteoVelocidad();  
                  break;
        case '4': for(int i = 0; i < 3; i++){
                    for(int j = 0;j< 8; j++)
                      digitalWrite(vecOutput[j], 1);

                    sleep(1);
                    apagarLeds();
                    sleep(1);

                  }
                  break;
	case '5': break;
        default:  printf("Ingresar opcion valida");
                  break;
      }
     aux = 'E'; 
    }
  system("clear");
  printf("\nEl programa ha finalizado\n");
  tcsetattr (0 , TCSANOW , &t_old);

  return 0;
}

void Mod(int fd, char modo)
{
  int longitud;
  char opcion;

    while(opcion != '9'){
	opcion = 'A';
    	if(modo == 'L')
      		opcion = printSecuencia();
    	else 
	  	while(opcion == 'A'){
          		if(serialDataAvail(fd))
              			opcion = serialGetchar(fd);
          }
          
    aux = 'E';

    tcsetattr (0 , TCSANOW , &t_new1);

    switch(opcion){
      case '1': longitud = sizeof(AutoFantastico);
                printf("Auto Fantastico (Enter para Salir)");
                fflush(stdout);
		Secuencias(fd, AutoFantastico, longitud, modo);
                break;

      case '2': longitud = sizeof(ElChoque);
                printf("El Choque (Enter para Salir)");
                fflush(stdout);
                Secuencias(fd, ElChoque, longitud, modo);
                break;

      case '3': longitud = sizeof(LaApilada);
                fflush(stdout);
                printf("La Apilada (Enter para Salir)");
                Secuencias(fd, LaApilada, longitud, modo);
                break;

      case '4': longitud = sizeof(LaCarrera);
                printf("La Carrera (Enter para Salir)");
                fflush(stdout);
                Secuencias(fd, LaCarrera, longitud, modo);
                break;

      case '5': longitud = sizeof(Voy2Vuelvo1);
                printf("Voy2Vuelvo1 (Enter para Salir)");
                fflush(stdout);
                Secuencias(fd, Voy2Vuelvo1, longitud, modo);
                break;

      case '6': longitud = sizeof(JuntosALaPar);
                printf("Juntos a la par (Enter para Salir)");
                fflush(stdout);
                Secuencias(fd, JuntosALaPar, longitud, modo);
                break;

      case '7': printf("Carga (Enter para Salir)");
                fflush(stdout);
                tcsetattr(FD_STDIN, TCSANOW, &t_new1);
                Carga(fd, modo);
                break;

      case '8': printf("Brincos largos (Enter para Salir)");
                fflush(stdout);
                tcsetattr(FD_STDIN, TCSANOW, &t_new1);
                BrincosLargos(fd, modo);
                break;

      case '9': break;

      default:  printf("%d no es una opcion valida", opcion);
                fflush(stdout);
                retardo(velocidad[ind_veloc]);
                break;
    }
    printf("\n");
    apagarLeds();
}
    tcsetattr(0 , TCSANOW , &t_old);
}


void Secuencias(int fd, unsigned char *Secuencia, int longitud, char modo)
{       
          const unsigned char constante = 0x01;
          unsigned char resultado = 0;

          tcsetattr(FD_STDIN, TCSANOW, &t_new1);
          while(1){
              for(int j = 0 ; j < longitud ; j++){
                controlVeloc(fd, modo);      
                if(aux == '\n') //Enter
                    break;
                aux = 'E';

                 for(int offset = 0 ; offset < 8 ; offset++){
                    resultado = constante & (Secuencia[j] >> offset);
                    (resultado) ? digitalWrite(vecOutput[offset], 1) : digitalWrite(vecOutput[offset], 0);
                 }       
                 retardo(velocidad[ind_veloc]);
              }
              if(aux == '\n')
                break;
          }
}

void Carga(int fd, char modo)
{
  while(1){
    for(int i = 0; i < 8; i++){
      controlVeloc(fd, modo);      
      if(aux == '\n') //Enter
          break;
      aux = 'E';

      digitalWrite(vecOutput[i], 1);
      retardo(velocidad[ind_veloc]);
    }
    apagarLeds();
    if(aux == '\n')
      break;
  }
}

void BrincosLargos(int fd, char modo)
{
   while(1){
    digitalWrite(vecOutput[0], 1);
    retardo(velocidad[ind_veloc]);
     
    digitalWrite(vecOutput[0], 0);

    for(int i = 0; i < 8; i++){
      controlVeloc(fd, modo);      
      if(aux == '\n') //Enter
          break;
      aux = 'E';

      if((i % 2) == 0){
        digitalWrite(vecOutput[i], 1);
        retardo(velocidad[ind_veloc]);
        
        digitalWrite(vecOutput[i], 0);
      }else{
        digitalWrite(vecOutput[i + 4], 1);
        retardo(velocidad[ind_veloc]);
        
        digitalWrite(vecOutput[i + 4], 0);
      }
    }
    if(aux == '\n')
        break;
    }
}

void apagarLeds(void)
{
    for(int i =0; i < 8; i++)
      digitalWrite(vecOutput[i], 0);
}

