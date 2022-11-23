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

#ifndef ULTRASSONICO_H
#define ULTRASSONICO_H
#include <stdint.h>

uint32_t Medir_Distancia_CM(void); //Vai retornar a distância medida em centimétros
uint32_t Medir_Distancia_INCH(void); //Vai retornar a distância em polegadas
void Alerta_Distancia(uint32_t dist); //Recebe um valor de distância como parâmetro e quando o objeto estiver em uma distância menor um led irá acender como alerta
void Calibracao(void); // Esta função tem como objetivo calibrar o sensor para uma medição mais fidedígna
uint32_t kallman(uint32_t U); //Filtro de Kallman para tratamento do sinal

#endif
