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

#ifndef ULTRASSONICO_H
#define ULTRASSONICO_H
#include <stdint.h>

uint32_t Medir_Distancia_CM(void); //Vai retornar a distância medida em centimétros
uint32_t Medir_Distancia_INCH(void); //Vai retornar a distância em polegadas
void Aproximacao(void); //Irá funcionar como um sensor de ré, o LED irá piscar com uma frequência inversamente proporcional a distância
void Alerta_Distancia(uint32_t dist); //Recebe um valor de distância como parâmetro e quando o objeto estiver em uma distância menor um led irá acender como alerta
void delay_us(uint16_t us);

#endif
