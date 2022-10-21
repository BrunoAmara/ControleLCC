/*
 * FiltroPassaBanda.h
 *
 *  Criado: 11/03/2020
 *      Autor: Guilherme Scofano 
 *		Grupo de trabalho: Nitee - UFF
 *	Descrição:
 *		Realiza uma implementação no tempo discreto de um filtro passa-banda. Os 
 *		parâmetros de entrada do filtro são a velocidade angular central w0 e o 
 *		amortecimento do filtro psi, relacionado à banda de passagem. 
 *		
 *		Os parâmetros de entrada são referentes a um filtro no domínio do tempo contínuo e
 *		é aplicada uma discretização baseada na transformada bilinear.
 *
 *	Última atulização: 11/03/2020
 *
 */

#ifndef _FILTRO_PASSA_BANDA_INCLUDED_
#define _FILTRO_PASSA_BANDA_INCLUDED_

#include <stdint.h>

typedef struct FiltroPassaBanda FiltroPassaBanda;

/*
 *	Cria uma variável do tipo FiltroPassaBanda
 *	Argumentos:
 *		entrada: endereço inicial de um vetor de três posições com os valores do sinal de 
 			entrada no quadro alfa, beta e zero
 *		angulo: endereço da variável ângulo
 *		amplitude: endereço da variável amplitude
 *		kp: valor da constante proporcional
 *		ki: valor da constante de integração
 *	Valor de retorno: um ponteiro para uma variável do tipo FiltroPassaBanda
 */
FiltroPassaBanda*
FiltroPassaBanda_criar(
	double *entrada,
	double *saida,
	double psi,
	double w0,
	double f_amostragem);

/*
 *	Destroi a variável do tipo FiltroPassaBanda, liberando o espaço alocado para ela na 
 * 	memória
 *	Argumentos:
 *	fpb 	ponteiro de uma variável do tipo FiltroPassaBanda
 */
void 
FiltroPassaBanda_destroi(FiltroPassaBanda* fpb);

/*
 *	Executa a malha de controle para calcular a saída mais atual do filtro. Usar uma, e
 *	somente uma vez no loop de controle.
 *	Argumentos:
 *		fpb: ponteiro de uma variável do tipo FiltroPassaBanda
 */
void 
FiltroPassaBanda_computa(FiltroPassaBanda *fpb);

#endif //_FILTRO_PASSA_BANDA_INCLUDED_
