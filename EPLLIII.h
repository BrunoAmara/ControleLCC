/*
    Arquivo: EPLLII.h

    Criado: 21/05/2021

    Autor(es):

    Bruno Amaral (amaral_bruno@id.uff.br)
    C�digo destinado ao trabalho de conclus�o de curso

    �ltima atualiza��o: 21/05/2021 - Feita por: Bruno Amaral (amaral_bruno@id.uff.br)

    Descri��o da atualiza��o:

	Descri��o do c�digo:

	Implementa��o do EPLL III descrito no livro do Ghartemani.

	Uso:

	1) Declarar ponteiro para a estrutura EPLL
	2) Alocar mem�ria para a estrutura por meio da fun��a EPLL_criar
	3) Invocar fun��o EPLL_executar a cada amostragem para executar
	a malha do EPLL III
*/

#ifndef MOD_EPLLIII
#define MOD_EPLLIII

//Defini��o de tipo vis�vel ao usu�rio
typedef struct eplliii EPLLIII;

//Prot�tipos das fun��es da biblioteca

/*
    Nome da fun��o: EPLLIII_criar

    Descri��o:

    Aloca mem�ria para um ponteiro do tipo EPLLII*

    Tipo de retorno: EPLLIII*

    Argumentos:

    double *sinais_entrada: sinais el�tricos de entrada
    double *amplitudes_estimadas_saida: ponteiro para amplitudes estimadas pelo EPLL. 3 posi��es
    double *fases_estimadas_saida: ponteiro para fases angulares estimadas. 3 posi��es
    double *frequencia_estimada_saida: ponteiro para frequ�ncia angular estimada pelo EPLLIII. 1 posi��o
    double *sinais_estimados_saida: ponteiro para componentes fundamentais estimadas dos sinais de entrada. 3 posi��es
    double amplitude_base: amplitude nominal de fase (pico) dos sinais de entrada
    double frequencia_base: frequ�ncia nominal do sistema em Hz
    double frequencia_amostragem: frequ�ncia de amostragem do c�digo em Hz
    double ganho_um: ganho mi 1 para o n�cloe do EPLL
    double ganho_dois: ganho mi 2 para a estima��o de frequ�ncia principal
    double ganho_tres: ganho mi 3 para o n�cleo do EPLL
*/

EPLLIII*
EPLLIII_criar (double *sinais_entrada,
               double *amplitudes_estimadas_saida,
               double *fases_estimadas_saida,
               double *frequencia_estimada_saida,
               double *sinais_estimados_saida,
               double amplitude_base,
               double frequencia_base,
               double frequencia_amostragem,
               double ganho_um,
               double ganho_dois,
               double ganho_tres);

/*
    Nome da fun��o: EPLL_executar

    Descri��o:

    Executa o algoritmo do EPLL

    Tipo de retorno: void

    Argumentos:

    NucleoEPLL *meuEPLL: ponteiro para uma estrutura EPLL
*/

void
EPLLIII_executar (EPLLIII *meuEPLL);

/*
    Nome da fun��o: EPLL_reparametrizar

    Descri��o:

    Reparametriza o algoritmo do EPLL

    Tipo de retorno: EPLLIII*

    Argumentos:

    NucleoEPLL *meuEPLL: ponteiro para uma estrutura EPLL
    double amplitude_base: amplitude nominal de fase (pico) dos sinais de entrada
    double frequencia_base: frequ�ncia nominal do sistema em Hz
    double frequencia_amostragem: frequ�ncia de amostragem do c�digo em Hz
    double ganho_um: ganho mi 1 para o n�cloe do EPLL
    double ganho_dois: ganho mi 2 para a estima��o de frequ�ncia principal
    double ganho_tres: ganho mi 3 para o n�cleo do EPLL
*/

void
EPLLIII_reparametrizar(double amplitude_base,
                       double frequencia_base,
                       double frequencia_amostragem,
                       double ganho_um,
                       double ganho_dois,
                       double ganho_tres,
                       EPLLIII *meuEPLL);

/*
    Nome da fun��o: EPLL_destruir

    Descri��o:

    Desaloca mem�ria de uma estrutura do tipo EPLL*

    Tipo de retorno: void

    Argumentos:

    NucleoEPLL *meuEPLL: ponteiro para uma estrutura do tipo EPLL*
*/

void
EPLLIII_destruir (EPLLIII *meuEPLL);

#endif // MOD_EPLLIII
