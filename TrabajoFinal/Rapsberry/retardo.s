.text
.arm
.global retardo
 
retardo:
  loop: subs r0, r0, #1  // Decrementa el contador
        bne loop      // Si no se ha alcanzado el final vuelve al bucle    
  mov pc, lr     //Devuelve el control al llamador