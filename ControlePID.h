/*
 * ControlePID.h
 *
 *  Criado: 04/06/2021
 *
 *  Autor: Bruno Barbosa do Amaral (amaral_bruno@id.uff.br)
 *	Grupo de trabalho: NITEE - UFF
 *
 *  �ltima atualiza��o: 04/06/2021
 *
 *  Descri��o da atualiza��o:
 *
 *  Foi usada como base a vers�o ControlePID_v11.
 *  Esta vers�o se prop�e a modificar o mecanismo de escolha
 *  do tipo de controlador. Foi inclu�do um switch case para determinar,
 *  com base no novo par�metro "tipo_ctrl" da fun��o ControlePID_criar, o tipo do controle
 *  a ser executado.
 *
 *	Descri��o:
 *	Realiza a malha de controle do controlador proporcional-integral-derivativo
 *
 *	Uso:
 *       A fun��o Controlador_PID_criar deve ser chamada somente uma vez para configurar
 *       o controlador e retornar um ponteiro do tipo ControladorPID com as configura��es do controle.
 *       Uma vez feita a configura��o, a fun��o ControladorPID_computa deve ser chamada a cada itera��o da
 *       simula��o, ou interrup��o, para execu��o do algoritmo de controle.
 *       A subrotina ControladorPID_destroi deve ser chamada em caso de erro, ou fim de atividade do controle PID.
 *
 *
 *
 */

#ifndef CONTROLADOR_PID
#define CONTROLADOR_PID

#include <stdlib.h>
#include <stdio.h>

// Macros para escolha do tipo de controle
#define P 0
#define I 1
#define PD 2
#define PI 3
#define PID 4

#define ORDEM_APROX 1 //ORDEM DO SISTEMA

//DEFININDO TIPO BASEADO NA STRUCT CONTROLADORPID
typedef struct ControladorPID ControladorPID;

//PROT�TIPOS DAS FUN��ES
ControladorPID*
Controlador_PID_criar(double *sinal_e,
                      double *sinal_s,
                      double Kp,
                      double Ki,
                      double Kd,
                      double pamostragem,
                      double v_maximo,
                      double v_minimo,
                      int tipo_ctrl);

/*
*   Descri��o: Retorna endere�o de mem�ria de uma vari�vel do tipo ControladorPID com as configura��es de controle
*    informadas pelo usu�rio atrav�s de seus argumentos.
*
*    Argumentos:
*               double *sinal_e: Endere�o de mem�ria da vari�vel que cont�m o valor de entrada do controle PID.
*                Ela deve ter espa�o alocado para uma vari�vel.
*
*               double *sinal_s: Endere�o de mem�ria da vari�vel que cont�m o valor de sa�da do controle PID.
*                Ela deve ter espa�o alocado para uma vari�vel.
*
*               double Kp: Constante do termo proporcional do controle PID.
*               double Ki: Constante do termo integrativo do controle PID.
*               double Kd: Constante do termo derivativo do controle PID.
*               double pamostragem: Per�do de amostragem dos sinais.
*               double v_maximo: Valor m�ximo da sa�da do controle PID.
*               double v_m�nimo: Valor m�nimo da sa�da do controle PID.
*               int tipo_ctrl: seleciona o tipo de controlador. Ex: P, PI ...
*               P: 0; I: 1; PD: 2; PI: 3; PID: 4.
*               OBS: utilize as macros fornecidas pelo arquivo de cabe�alho para melhorar a legibilidade do c�digo
*
*/

void
ControladorPID_executar(ControladorPID* controlePID);

/*
*   Descri��o: Realiza os c�lculos do controle PID.
*
*    Argumentos:
*               ControladorPID *controlePID : Endere�o de mem�ria da vari�vel respons�vel pelo controle PID.
*                Ela deve ter espa�o alocado para uma vari�vel do tipo ControladorPID.
*
*/


void
ControladorPID_destruir(ControladorPID *controlePID);

/*
*   Descri��o: Desaloca mem�ria da vari�vel respons�vel pelo controle PID
*
*    Argumentos:
*               ControladorPID *controlePID : Endere�o de mem�ria da vari�vel respons�vel pelo controle PID.
*                Ela deve ter espa�o alocado para uma vari�vel do tipo ControladorPID.
*
*/

void
ControladorPID_resetar(ControladorPID *controlePID);
/*
*   Descri��o: Zera todas as vari�veis de sa�da e internas utilzadas nos c�lculos
*
*    Argumentos:
*               ControladorPID *controlePID : Endere�o de mem�ria da vari�vel respons�vel pelo controle PID.
*                Ela deve ter espa�o alocado para uma vari�vel do tipo ControladorPID.
*
*/

#endif //CONTROLADOR_PID



