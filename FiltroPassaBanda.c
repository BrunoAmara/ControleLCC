/*
 * FiltroPassaBanda.c
 *
 *  Criado: 11/03/2020
 *      Autor: Guilherme Scofano 
 *		Grupo de trabalho: Nitee - UFF
 *	Descrição:
 *		Contém a definição de tipos, variáveis e funções de NucleoEPLL.
 *		Este arquivo é propriedade intelectual do Nitee e não deve ser
 *		divulgado em texto pleno a terceiros. Detalhes confidenciais 
 *		de projeto devem ficar neste arquivo, bem como variáveis e
 *		funções que não devem ser acessadas pelo usuário.
 *
 *	Última atulização: 11/03/2020
 */

//Ideia: criar um núcleo principal, que estima componentes fundamentais de sequência positiva e, caso necessário, chama outros núcleos, conforme definido

//Inclui somente o cabeçalho de NucleoEPLL
#include "FiltroPassaBanda.h"
#include <stdlib.h>

//Definindo os tipos declarados em EPLL.h
struct FiltroPassaBanda
{
	double *entrada;
	double *saida;
	double y[3];		//y[0] é a saída mais atual, y[2] a mais antiga
	double x[3];		//x[0] é a entrada mais atual, x[2] a mais antiga
	double coefs_y[3];	//Coeficientes de y -- coefs_y[2] é o coeficiente de y[2]
	double coefs_x[3];	//Coeficientes de x -- coefs_x[2] é o coeficiente de x[2]
};

FiltroPassaBanda*
FiltroPassaBanda_criar(
	double *entrada,
	double *saida,
	double psi,
	double w0,
	double f_amostragem)
{

	FiltroPassaBanda *fpb = (FiltroPassaBanda*) malloc(sizeof(FiltroPassaBanda));

	if( fpb==NULL )
	{
	    return NULL;
	}

	fpb->entrada = entrada;
	fpb->saida = saida;

	double a = 4*psi*w0/f_amostragem;
	double b = w0*w0/f_amostragem/f_amostragem;

	fpb->coefs_x[0] = a;
	fpb->coefs_x[1] = 0;
	fpb->coefs_x[2] = a;
	fpb->coefs_y[0] = (4+a+b);
	fpb->coefs_y[1] = (-8+2*b);
	fpb->coefs_y[2] = (4-a+b);

	int k;
	for(k=0; k<3; k++)
	{
		fpb->x[k]=0;
		fpb->y[k]=0;
	}

	return fpb;

}

//Algoritmo de controle
void 
FiltroPassaBanda_computa(FiltroPassaBanda *fpb)
{
	fpb->x[0] = *(fpb->entrada);
	fpb->y[0] = (fpb->coefs_x[0]*fpb->x[0] 
		- fpb->coefs_x[2]*fpb->x[2] 
		- fpb->coefs_y[1]*fpb->y[1] 
		- fpb->coefs_y[2]*fpb->y[2])/fpb->coefs_y[0];

	fpb->x[2] = fpb->x[1];
	fpb->x[1] = fpb->x[0];
	fpb->y[2] = fpb->y[1];
	fpb->y[1] = fpb->y[0];

	*(fpb->saida) = fpb->y[0];
}

void 
FiltroPassaBanda_destroi(FiltroPassaBanda *fpb)
{
	free(fpb);
}
