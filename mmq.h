/*
 * mmq.h
 *
 *  Criado: 01/12/2020
 *      Autor: Yuri Couto
 *		Grupo de trabalho: Nitee - UFF
 *	Descri��o:
 *		Tipos de vari�veis, vari�veis e prot�tipos de fun��es devem ficar
 *		neste arquivo, vis�veis para qualquer usu�rio da ferramenta.
 *		Este arquivo N�O DEVE ter detalhes confidenciais sobre o
 *		projeto nem fun��es ou vari�veis que n�o devem ser acess�veis
 * 		ao usu�rio. Detalhes confidenciais devem se ater � unidade de
 *		compila��o (o arquivo MMQ_src.c).
 *
 *	�ltima atualiza��o: 01/12/2020
 *
 *	Anota��es:
 *
 *		Documenta��o deve trazer o tamanho que deve ser alocado para cada
 *		endere�o de mem�ria.
 *
 */

#ifndef _MMQ_H_INCLUDED_
#define _MMQ_H_INCLUDED_

//Bibliotecas necess�rias devem ficar aqui.
#include <stdlib.h>
#include <math.h>
#include "Constantes.h"

typedef struct MMQ MMQ;

/*
 *	Cria uma vari�vel do tipo MMQ
 *	Argumentos:
 *		i_amostrado: ponteiro para o endere�o do vetor de valores de corrente
 *amostrados
 *  Aloca��o de mem�ria: tr�s vari�veis tipo double
 *
 *      estimativa: ponteiro para o endere�o do vetor de estima��o
 *  Aloca��o de mem�ria: tr�s vari�veis tipo double
 *
 *		f_nominal: frequencia nominal da rede utilizada em calculos dentro da
 *estrutura MMQ_criar
 *
 *      f_amostragem: frequencia de amostragem do LCC utilizada em calculos
 *dentro da estrutura MMQ_criar
 *
 *
 *	Valor de retorno: um ponteiro para uma vari�vel do tipo MMQ
 */
MMQ* MMQ_criar(
	 double* i_amostrado,
	 double* estimativa,
	double f_nominal,
	double f_amostragem,
	int n_amostras
	);
/*
 *	Destroi a vari�vel do tipo MMQ, liberando o espa�o alocado para
 *	ela na mem�ria
 *	Argumentos:
 *	mmq: ponteiro de uma vari�vel do tipo MMQ
 */
void MMQ_destroi(MMQ* mmq);

/*
 *	Executa o algoritmo de controle do MMQ
 *	Argumentos:
 *	mmq: ponteiro de uma vari�vel do tipo MMQ
 */
void MMQ_computa(MMQ* mmq);

#endif
