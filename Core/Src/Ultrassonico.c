/* DATA: 12/10/2022
 * UNIVERSIDADE FEDERAL DE MINAS GERAIS
 * AUTOR: Caio Teraoka de Menezes Câmara e Cristovão Eustaquio da Silva
 * VERSÃO 2.0
 * API: Ultrassonico
 * DESCRIÇÃO: API desenvolvida para uso do módulo Sensor Ultrassônico HC-SR04.
 * REQUISITOS de HARDWARE: Módulo Sensor Ultrassônico HC-SR04, Microcontrolador STM32F103RBT6, LEDs, Resistores de 220 Ohms, Jumpers e Shiel Multifunções Arduino
 * REQUISITOS DE SOFTWARE: STM32CubeIDE; Timer 1 deve ser configurado como "Input capture direct mode", ajustado para ter 1Mhz de clock, counter period no valor máximo (0xffff-1) e habilitar a opção "TIM capture compare interrupt"
 *
 * Esta API foi desenvolvida como trabalho da disciplina de Programação de Sistemas Embarcados da UFMG –Prof. Ricardo de Oliveira Duarte –Departamento de Engenharia Eletrônica
 */

#include "main.h"
#include "Ultrassonico.h"

extern int funcao; 									// Variavel que diz qual função o projeto quer
extern int calibrar;								//Variável que habilita a calibração
extern TIM_HandleTypeDef htim1;
float Time1 = 0; 									//Variável que marca o momento de subida do Echo
float Time2 = 0; 									//Variável que marca o momento de descida do Echo
float Diferenca = 0; 								//Variável que marca o tempo que o Echo ficou em nivel alto
uint8_t Primeira_Captura = 0; 						//Para saber se quando a interrupção for chamada o sinal está em subida(0) ou descida(1)
float Distancia  = 0;								//Variável que indica a distância
float Distancia_Real  = 0;							//Variável que indica a distância após a inclusão do erro de medição
float erro = 0;										//Variável que nos diz o quanto o sensor está errando

//Vai retornar a distância medida em centímetros
float Medir_Distancia_CM(void){
	HAL_GPIO_WritePin(GPIOC, Trigger_Pin, 1); 		// Para acionar o sensor se deve gerar um pulso de duração de 10uS no pino Trigger
	for(int x = 0; x < 40; ++x){} 					//delay de 10uS (O clock funciona a 40Mhz)
	HAL_GPIO_WritePin(GPIOC, Trigger_Pin, 0);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);		//Habilita a interrupção para o timer 1, irá permitir a leitura da subida do Echo
	Distancia_Real = Distancia + erro;
	return Distancia_Real;

}

float retornar_erro(){
	return erro;
}

//Vai retornar a distância em polegadas
float Medir_Distancia_INCH(void){
	HAL_GPIO_WritePin(GPIOC, Trigger_Pin, 1); 			// Para acionar o sensor se deve gerar um pulso de duração de 10uS no pino Trigger
	for(int x = 0; x < 40; ++x){} 						//delay de 10uS (O clock funciona a 40Mhz)
	HAL_GPIO_WritePin(GPIOC, Trigger_Pin, 0);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1); 			//Habilita a interrupção para o timer 1, irá permitir a leitura da subida do Echo
	Distancia_Real = Distancia + erro;
	return Distancia_Real/2.54; 						//Conversão para polegadas

}


//Função para calibração do sensor
void Calibracao(float dist){
	//O LED1 irá piscar para indicar o começo da amostragem
	while(calibrar == 0){
	HAL_GPIO_WritePin(GPIOC, LED_1_Pin, 1);
	HAL_Delay(200);
	HAL_GPIO_WritePin(GPIOC, LED_1_Pin, 0);
	HAL_Delay(200);}
	int amostras = 100;
	float medicao = 0;
	//o loop serve para realizar 100 amostras da distância
    for(int a = 0; a < amostras; ++a){
        medicao += dist - Medir_Distancia_CM();
        HAL_Delay(60);
    }
    erro = medicao/amostras;
    //LED1 acende para indicar a finalização da calibração
	HAL_GPIO_WritePin(GPIOC, LED_1_Pin, 1);
	funcao = 0;
	calibrar = 0;
}

//Recebe um valor de distância como parâmetro e quando o objeto estiver em uma distância menor um led irá acender como alerta
void Alerta_Distancia(float dist){
	float dist_atual = 0;
	dist_atual = Medir_Distancia_CM(); 								//Mede a distancia atual
	if (dist_atual < dist) HAL_GPIO_WritePin(GPIOA, LED_2_Pin, 1);  //Condicional para decisão se acende o led ou não
	else HAL_GPIO_WritePin(GPIOA, LED_2_Pin, 0);
	HAL_Delay(60); 													//delay de 60ms indicado pelo fabricante do sensor entre uma medição e outra
}


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

		else if (Primeira_Captura==1) 					//Se a variavel for igual a 1 significa que o sinal desceu para nivel lógico baixo
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




