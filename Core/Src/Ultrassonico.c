/* DATA: 12/10/2022
 * UNIVERSIDADE FEDERAL DE MINAS GERAIS
 * AUTOR: Caio Teraoka de Menezes Câmara e Cristovão Eustaquio da Silva
 * VERSÃO 2.0
 * API: Ultrassonico
 * DESCRIÇÃO: API desenvolvida para uso do módulo Sensor Ultrassônico HC-SR04.
 * REQUISITOS de HARDWARE: Módulo Sensor Ultrassônico HC-SR04, Microcontrolador STM32F103RBT6, LEDs, Resistores de 220 Ohms e Jumpers
 * REQUISITOS DE SOFTWARE: STM32CubeIDE; Timer 1 deve ser configurado como "Input capture direct mode", ajustado para ter 1Mhz de clock, counter period no valor máximo (0xffff-1) e habilitar a opção "TIM capture compare interrupt"
 *
 * Esta API foi desenvolvida como trabalho da disciplina de Programação de Sistemas Embarcados da UFMG –Prof. Ricardo de Oliveira Duarte –Departamento de Engenharia Eletrônica
 */

#include "main.h"
#include "Ultrassonico.h"

extern int funcao; 										// Variavel que diz qual função o projeto quer
extern TIM_HandleTypeDef htim1;
uint32_t Time1 = 0; 									//Variável que marca o momento de subida do Echo
uint32_t Time2 = 0; 									//Variável que marca o momento de descida do Echo
uint32_t Diferenca = 0; 								//Variável que marca o tempo que o Echo ficou em nivel alto
uint8_t Primeira_Captura = 0; 							//Para saber se quando a interrupção for chamada o sinal está em subida(0) ou descida(1)
uint32_t Distancia  = 0;								//Variável que indica a distância
uint32_t Distancia_Real  = 0;
uint32_t a = 0;
uint32_t b = 0;											//Variável que nos diz o quanto o sensor está errando

//Vai retornar a distância medida em centímetros
uint32_t Medir_Distancia_CM(void){
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 1); 			// Para acionar o sensor se deve gerar um pulso de duração de 10uS no pino Trigger
	for(int x = 0; x < 40; ++x){} 						//delay de 10uS (O clock funciona a 40Mhz)
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 0);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);			//Habilita a interrupção para o timer 1, irá permitir a leitura da subida do Echo
	Distancia = kallman(Distancia);						//Filtragem do sinal
	Distancia_Real = (Distancia - a)/(1+b);
	return Distancia_Real; 								//Retorna a distância em centímetros

}

//Vai retornar a distância em polegadas
uint32_t Medir_Distancia_INCH(void){
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 1); 			// Para acionar o sensor se deve gerar um pulso de duração de 10uS no pino Trigger
	for(int x = 0; x < 40; ++x){} 						//delay de 10uS (O clock funciona a 40Mhz)
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 0);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1); 			//Habilita a interrupção para o timer 1, irá permitir a leitura da subida do Echo
	Distancia = kallman(Distancia);						//Filtragem do sinal
	Distancia_Real = (Distancia - a)/(1+b);
	return Distancia_Real/2.54; 						//Retorna a distância em polegadas

}

//Função para calibração do sensor
void Calibracao(void){
	HAL_GPIO_WritePin(GPIOA, LED_1_Pin, 0);
	HAL_GPIO_WritePin(GPIOA, LED_1_Pin, 1);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, LED_1_Pin, 0);		//O LED1 irá piscar uma vez para indicar o começo da amostragem
	HAL_Delay(1000);
	uint32_t medicao[10];
	uint32_t dist = 10;
	uint32_t somaY = 0 , somaX = 100 , somaQX = 1000, somaXY ;
    for(int a = 0; a < 10; ++a){				//o loop serve para realizar 10 amostras da distância
        medicao[a] = Medir_Distancia_CM() - dist;
        somaY += medicao[a];
        somaXY = medicao[a]*10;
        HAL_Delay(60);
    }
    a = ((somaY*somaQX)-(somaX*somaXY))/(10*somaQX - (somaX*somaX)); //Calculo dos parâmetros para definição da distância correta
    b = (10*somaXY-(somaX*somaY))/(10*somaQX - (somaX*somaX));
	HAL_GPIO_WritePin(GPIOA, LED_1_Pin, 1);							//LED1 acende para indicar a finalização da calibração
}

//Recebe um valor de distância como parâmetro e quando o objeto estiver em uma distância menor um led irá acender como alerta
void Alerta_Distancia(uint32_t dist){ 					//A função vai acontecer enquanto a variável funcao estiver em 2
	uint32_t dist_atual = 0;
	while(funcao == 2){
	dist_atual = Medir_Distancia_CM(); 					//Mede a distancia atual
	if (dist_atual < dist) HAL_GPIO_WritePin(GPIOA, LED_2_Pin, 1); //Condicional para decisao se acende o led ou não
	else HAL_GPIO_WritePin(GPIOA, LED_2_Pin, 0);
	HAL_Delay(60); 										//delay de 60ms indicado pelo fabricante do sensor entre uma medição e outra
	}
} 														//Recebe um valor de distância como parâmetro e quando o objeto estiver em uma distância menor um led irá acender como alerta


//Esta função serve para capturar o tempo de que Echo fica em nível lógico alto
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if (Primeira_Captura==0) 						//Se a variavel for igual a zero significa que o sinal subiu para nivel lógico alto
		{
			Time1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); //O momento de subida é armazenado na variavel time1
			Primeira_Captura = 1;


			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING); //Muda a configuração para ler o momento em que o sinal esteja descendo
		}

		else if (Primeira_Captura==1) 					//Se a variavel for igual a um significa que o sinal desceu para nivel lógico baixo
		{
			Time2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);// //O momento de descida é armazenado na variavel time1
			__HAL_TIM_SET_COUNTER(htim, 0); 			//Reinicia o contador do timer1 para zero

			if (Time2 > Time1)							// Se time2 for maior que time1 o resultado do tempo é igual a diferenca
			{
				Diferenca = Time2-Time1;
			}

			else if (Time1 > Time2)						// Se time1 for maior que time2 significa que o contador ultrapassou seu limite e reiniciou a contagem
														//por isso, o resultado do tempo é igual a expressao abaixo
			{
				Diferenca = (0xffff - Time1) + Time2;
			}

			Distancia = Diferenca * 0.034/2;
			Primeira_Captura = 0;


			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING); //Muda a configuração para ler o momento em que o sinal esteja subindo
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1); //A interrupção é desabilitada para ser chamada apenas quando necessário
		}
	}
}

//Filtro de Kallman para tratamento do sinal
uint32_t kallman(uint32_t U){
	  uint32_t R = 40;
	  uint32_t H = 1.00;
	  uint32_t Q = 10;
	  uint32_t P = 0;
	  uint32_t U_hat = 0;
	  uint32_t K = 0;
	  K = P*H/(H*P*H+R);
	  U_hat += + K*(U-H*U_hat);
	  P = (1-K*H)*P+Q;
	  return U_hat;
}


