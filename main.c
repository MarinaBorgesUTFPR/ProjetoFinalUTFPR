
// LED vermelho - PF1 - Lampada UV ligada
// LED azul - PF2 - Sinaliza��o
// LED verde - PF3 - Sinaliza��o
// Chave fim de curso SW2 - PF0
// Bot�o SW1 - PF4
// Rel� - PB4

#include <stdio.h>
#include <stdint.h> // C99 variable types
#include "ADCSWTrigger.h"
#include "portb.h"
#include "portf.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "UART.h"
#include "Timer0.h"

void Output_Init(void);

unsigned char estado = 0; // variavel que representa o estado que est� sendo executado
char inf_serial; // Informa��o recebida pelo serial
unsigned int flash_Red = 0;
unsigned int flash_All = 0;
unsigned long OutGreen;	 	// Visor para LED green
unsigned long OutBlue; 		// Visor para LED blue
unsigned long OutRed; 		// Visor para LED red
unsigned long Rele; 		// Visor para LED red


void Timer0A_Handler(void){
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;
	Timer0_off();	// Fun��o para delisgar o timer, Timer0.h
	GPIO_PORTB_DATA_R = 0x0; // Desliga o rel�
}



int main(void){ 
	int32_t SW2, SW1; // Variavel para receber a informa��o da chave SW2 - PF0
	//int32_t data;	//Pega o valor do potenciometro 
  Output_Init();              // initialize output device
  PortF_Init();
	PortB_Init();
	SysTick_Init();							// inicializa o systick
	//ADC0_InitSWTriggerSeq3_Ch9();		// Sequencer 3, canal 9 (PE4)
	printf("Projeto final de sistemas microcontrolados\n"); // Informa��es do projeto para aparecerem durante a execu��o 
	printf("Tema Caixa de Desinfeccao\n");
	printf("Autora Marina Borges\n");
	printf(" \n");
	while(1){				
		switch(estado){
			case(0):
				GPIO_PORTF_DATA_R = 0x0; // Os oito bits da porta F s�o zerados, logo PF1,PF2,PF3 est�o em zero (LEDs desligados)
				GPIO_PORTB_DATA_R = 0x0; // Os oito bits da porta B s�o zerados, logo garante o PB4 com sinal zero (Rel� desligado)
				
				OutBlue = GPIO_PORTF_DATA_R&0x04; //Sa�da para o Led Azul
				OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green
				OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar n�vel no LED red
				Rele = GPIO_PORTB_DATA_R&0x10; // Indicar nivel do Rel�
			
				SW2 = GPIO_PORTF_DATA_R & 0x01; // A variavel recebe a informa��o do pino PF0 por meio de um mascara
				if(SW2){ // Entra no if caso o valor seja diferente de 0, ou seja, SW2 = 0x01
						estado = 0;  	 // A caixa esta aberta novamente, portanto n�o podemos dar sequencia, voltamos ao inicio da maquina de estados
				}else{  
					estado = 1; // A caixa esta fechada, seguimos a maquina de estados.
				}
				break;
			
			case(1):
				GPIO_PORTF_DATA_R = 0x0; // Apaga o LED verde, retorno do case 3
			  OutGreen = GPIO_PORTF_DATA_R&0x0; //Sa�da para o Led Verde, retorno
				printf("Estado 1: Caixa fechada, Led Azul pisca para confirmar sinal\n");
				GPIO_PORTF_DATA_R = 0x04; // Acende o LED azul
				OutBlue = GPIO_PORTF_DATA_R&0x04; //Sa�da para o Led Azul
				SysTick_Wait(3200000); // Fun��o para esperar 200 ms
				GPIO_PORTF_DATA_R = 0x0; // Apaga o LED azul
				OutBlue = GPIO_PORTF_DATA_R&0x04; //Sa�da para o Led Azul
				SysTick_Wait(3200000); // Fun��o para esperar 200 ms
				estado = 2; // No pr�ximo loop seguimos o programa
				break;
			
			case(2):
				SW1 = GPIO_PORTF_DATA_R & 0x10; // A variavel recebe a informa��o do pino PF0 por meio de um mascara
			
				if(SW1 == 0x0){		// Testa se o valor � zero, se sim o bot�o est� pressionado e executa um comando
					printf("Estado 2: SW1 pressionada, seguimos para o estado 4\n");
					estado = 4; // Neste primeiro caso, n�s somos levados ao quinto estado dessa m�quina
					
				} else{		// Caso o valor seja 1 o bot�o n�o est� pressionado e executa outro comando
					printf("Estado 2: SW1 n�o pressionada, seguimos para o estado 3\n");	
					estado = 3; // Caso a fim de curso n�o esteja ativado seguimos para o estado 3
				}
				break;
				
			case(3):
				GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED acende Verde
				OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green
			  printf("Caixa Aberta: Retorna ao processo inicial.\n");
			  SysTick_Wait(6400000); // Fun��o para esperar 400 ms
				estado = 1;
		   	break;
			
			case(4):
				printf("Caixa fechada: Em opera��o, objeto ser� desinfectado.\n"); // Avisa que est� tudo certo para iniciar o processo de desinfec��o
				GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED is red
				OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar n�vel no LED red
				GPIO_PORTB_DATA_R = 0x10; // Liga o rel�
				Rele = GPIO_PORTB_DATA_R = 0x10; // Indicar nivel do Rel�
				SysTick_Wait(12800000); // Fun��o para esperar 800 ms
				estado = 5; // Seguimos para o pr�ximo passo da m�quina de estados
				break;
			
			case(5):
				GPIO_PORTB_DATA_R = 0x0; // Desliga o rel�
				Rele = GPIO_PORTB_DATA_R&0x10; // Indicar nivel do Rel�
				GPIO_PORTF_DATA_R = 0x0;  // 0b0000_0000 LED 
				OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar n�vel no LED red
			  printf("Estado 5 - Caixa fechada: Em opera��o, objeto esta finalizando a desinfetacao.\n");
				while (flash_Red < 6) 
				{
					GPIO_PORTF_DATA_R = 0x02; // 0b0000_0010 LED is red
					OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar n�vel no LED red
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					GPIO_PORTF_DATA_R = 0x0; // Apaga o LED vermelho
					OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar n�vel no LED red
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					flash_Red = flash_Red + 1;
				}
				estado = 6; // No pr�ximo loop seguimos o programa
				break;
			
			case(6):
				SW1 = GPIO_PORTF_DATA_R & 0x10; // A variavel recebe a informa��o do pino PF4 por meio de um mascara
				
				if(SW1){ // A caixa continua fechada, ent�o damos sequencia as a��es
					printf("Estado 6 - Caixa Aberta \n");
					estado = 1; // A caixa esta aberta novamente, portanto n�o podemos dar sequencia, voltamos ao inicio da maquina de estados 
					
				}else{  // A caixa continua fechada, ent�o damos sequencia as a��es	// Entra no if caso o valor seja diferente de 0, ou seja, SW2 = 0x01	
					printf("Estado 6 - Caixa Fechada: Leds de sinaliza��o s�o acionados\n");
					GPIO_PORTF_DATA_R = 0x04;  // 0b0000_0100 LED acende Azul
					OutBlue = GPIO_PORTF_DATA_R&0x04; //Sa�da para o Led Azul
					GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED acende Verde
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green
					estado = 7;
				}
				break;
				
			case(7):
				SW1 = GPIO_PORTF_DATA_R & 0x10; // A variavel recebe a informa��o do pino PF4 por meio de um mascara
				if(SW1)
				{ 
					printf("Estado 7 - Retornar para saber se a caixa esta fechada\n");
					GPIO_PORTF_DATA_R = 0x00;  // Apaga os Leds
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green
					OutBlue = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green
					GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED Verde
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green	
					estado = 2; // seguimos para o pr�ximo estado 
					
				} else {
					estado = 8; 
					printf("Estado 7 - Finaliza o processo, retorno ao estado inicial\n");
				}
				break;
			
			case(8):
				printf("Estado 8 - Finaliza o processo, retorno ao estado inicial\n");
				while (flash_All < 4){
					GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED Vermelho
					OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar n�vel no LED red
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar n�vel no LED red
					
					
					GPIO_PORTF_DATA_R = 0x04;  // 0b0000_0100 LED Azul
					OutBlue = GPIO_PORTF_DATA_R&0x04; // Indicar n�vel no LED blue
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					OutBlue = GPIO_PORTF_DATA_R&0x04; // Indicar n�vel no LED red
					
					GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED Verde
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
					SysTick_Wait(3200000); // Fun��o para esperar 200 ms
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar n�vel no LED green

					flash_All = flash_All + 1;
				}
				estado = 0; // Seguimos para o pr�ximo estado
				break;
		}			
  }
}
