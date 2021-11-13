/*
 * ControlePID.h
 *
 *  Criado: 04/06/2021
 *
 *  Autor: Bruno Barbosa do Amaral (amaral_bruno@id.uff.br)
 *	Grupo de trabalho: NITEE - UFF
 *
 *  Última atualização: 04/06/2021
 *
 *  Descrição da atualização:
 *
 *  Foi usada como base a versão ControlePID_v11.
 *  Esta versão se propõe a modificar o mecanismo de escolha
 *  do tipo de controlador. Foi incluído um switch case para determinar,
 *  com base no novo parâmetro "tipo_ctrl" da função ControlePID_criar, o tipo do controle
 *  a ser executado.
 *
 *	Descrição:
 *	Realiza a malha de controle do controlador proporcional-integral-derivativo
 *
 *	Uso:
 *       A função Controlador_PID_criar deve ser chamada somente uma vez para configurar
 *       o controlador e retornar um ponteiro do tipo ControladorPID com as configurações do controle.
 *       Uma vez feita a configuração, a função ControladorPID_computa deve ser chamada a cada iteração da
 *       simulação, ou interrupção, para execução do algoritmo de controle.
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

//PROTÓTIPOS DAS FUNÇÕES
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
*   Descrição: Retorna endereço de memória de uma variável do tipo ControladorPID com as configurações de controle
*    informadas pelo usuário através de seus argumentos.
*
*    Argumentos:
*               double *sinal_e: Endereço de memória da variável que contém o valor de entrada do controle PID.
*                Ela deve ter espaço alocado para uma variável.
*
*               double *sinal_s: Endereço de memória da variável que contém o valor de saída do controle PID.
*                Ela deve ter espaço alocado para uma variável.
*
*               double Kp: Constante do termo proporcional do controle PID.
*               double Ki: Constante do termo integrativo do controle PID.
*               double Kd: Constante do termo derivativo do controle PID.
*               double pamostragem: Perído de amostragem dos sinais.
*               double v_maximo: Valor máximo da saída do controle PID.
*               double v_mínimo: Valor mínimo da saída do controle PID.
*               int tipo_ctrl: seleciona o tipo de controlador. Ex: P, PI ...
*               P: 0; I: 1; PD: 2; PI: 3; PID: 4.
*               OBS: utilize as macros fornecidas pelo arquivo de cabeçalho para melhorar a legibilidade do código
*
*/

void
ControladorPID_executar(ControladorPID* controlePID);

/*
*   Descrição: Realiza os cálculos do controle PID.
*
*    Argumentos:
*               ControladorPID *controlePID : Endereço de memória da variável responsável pelo controle PID.
*                Ela deve ter espaço alocado para uma variável do tipo ControladorPID.
*
*/


void
ControladorPID_destruir(ControladorPID *controlePID);

/*
*   Descrição: Desaloca memória da variável responsável pelo controle PID
*
*    Argumentos:
*               ControladorPID *controlePID : Endereço de memória da variável responsável pelo controle PID.
*                Ela deve ter espaço alocado para uma variável do tipo ControladorPID.
*
*/

void
ControladorPID_resetar(ControladorPID *controlePID);
/*
*   Descrição: Zera todas as variáveis de saída e internas utilzadas nos cálculos
*
*    Argumentos:
*               ControladorPID *controlePID : Endereço de memória da variável responsável pelo controle PID.
*                Ela deve ter espaço alocado para uma variável do tipo ControladorPID.
*
*/

#endif //CONTROLADOR_PID



