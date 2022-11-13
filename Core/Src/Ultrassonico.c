/* DATA: 12/10/2022
 * UNIVERSIDADE FEDERAL DE MINAS GERAIS
 * AUTOR: Caio Teraoka de Menezes Câmara e Cristovão Eustaquio da Silva
 * VERSÃO 1.0
 * API: Ultrassonico
 * DESCRIÇÃO: API desenvolvida para uso do módulo Sensor Ultrassônico HC-SR04. Suas funções contam com
 * REQUISITOS de HARDWARE: Módulo Sensor Ultrassônico HC-SR04, Microcontrolador STM32F103RBT6, LEDs, Resistores de 220 Ohms e Jumpers
 * REQUISITOS DE SOFTWARE: STM32CubeIDE; Timer 1 deve ser configurado como "Input capture direct mode", ajustado para ter 1Mhz de clock, counter period no valor máximo (0xffff-1) e habilitar a opção "TIM capture compare interrupt"
 *
 * Esta API foi desenvolvida como trabalho da disciplina de Programação de Sistemas Embarcados da UFMG –Prof. Ricardo de Oliveira Duarte –Departamento de Engenharia Eletrônica
 */

#include "main.h"
#include "Ultrassonico.h"


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
uint32_t Time1 = 0; //Variavel que marca o momento de subida do Echo
uint32_t Time2 = 0; //Variavel que marca o momento de descida do Echo
uint32_t Diferenca = 0; //Variavel que marca o tempo que o Echo ficou em nivel alto
uint8_t Primeira_Captura = 0;
uint32_t Distancia  = 0;
extern int funcao; // Variavel que diz qual função o projeto quer

uint32_t Medir_Distancia_CM(void){
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 1); // Para acionar o sensor se deve gerar um pulso de duração de 10uS no pino Trigger
	for(int x = 0; x < 40; ++x){} //delay de 10uS (O clock funciona a 40Mhz)
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 0);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1); //Habilita a interrupção para o timer 1, irá permitir a leitura da subida do Echo
	return Distancia; //Retorna a distância em centímetros

} //Vai retornar a distância medida em centímetros


uint32_t Medir_Distancia_INCH(void){
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 1); // Para acionar o sensor se deve gerar um pulso de duração de 10uS no pino Trigger
	for(int x = 0; x < 40; ++x){} //delay de 10uS (O clock funciona a 40Mhz)
	HAL_GPIO_WritePin(GPIOA, Trigger_Pin, 0);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1); //Habilita a interrupção para o timer 1, irá permitir a leitura da subida do Echo
	return Distancia/2.54; //Retorna a distância em polegadas

} //Vai retornar a distância em polegadas


void Aproximacao(void){
	uint32_t dist = 0;
	while(funcao == 1){ //A função vai acontecer enquanto a variável funcao estiver em 1
		dist = Medir_Distancia_CM(); //Mede a distancia atual do objeto
		HAL_GPIO_WritePin(GPIOA, LED_1_Pin, 1); //Acender o led
		HAL_Delay(10*dist); // delay de acordo com a distancia
		dist = Medir_Distancia_CM();
		HAL_GPIO_WritePin(GPIOA, LED_1_Pin, 0); // Desliga o led
		HAL_Delay(10*dist);
	}

} //Irá funcionar como um sensor de ré, o LED irá piscar com uma frequência inversamente proporcional a distância

void Alerta_Distancia(uint32_t dist){ //A função vai acontecer enquanto a variável funcao estiver em 2
	uint32_t dist_atual = 0;
	while(funcao == 2){
	dist_atual = Medir_Distancia_CM(); //Mede a distancia atual
	if (dist_atual < dist) HAL_GPIO_WritePin(GPIOA, LED_2_Pin, 1); //Condicional para decisao se acende o led ou não
	else HAL_GPIO_WritePin(GPIOA, LED_2_Pin, 0);
	HAL_Delay(60); //delay de 60ms indicado pelo fabricante do sensor entre uma medição e outra
	}
} //Recebe um valor de distância como parâmetro e quando o objeto estiver em uma distância menor um led irá acender como alerta


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) //Esta função serve para capturar o tempo de que Echo fica em nível lógico alto
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if (Primeira_Captura==0) //Se a variavel for igual a zero significa que o sinal subiu para nivel lógico alto
		{
			Time1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); //O momento de subida é armazenado na variavel time1
			Primeira_Captura = 1;


			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING); //Muda a configuração para ler o momento em que o sinal esteja descendo
		}

		else if (Primeira_Captura==1) ////Se a variavel for igual a um significa que o sinal desceu para nivel lógico baixo
		{
			Time2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);// //O momento de descida é armazenado na variavel time1
			__HAL_TIM_SET_COUNTER(htim, 0); //Reinicia o contador do timer1 para zero

			if (Time2 > Time1)// Se time2 for maior que time1 o resultado do tempo é igual a diferenca
			{
				Diferenca = Time2-Time1;
			}

			else if (Time1 > Time2)// Se time1 for maior que time2 significa que o contador ultrapassou seu limite e reiniciou a contagem
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
