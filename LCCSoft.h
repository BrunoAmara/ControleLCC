/*
    Arquivo: LCCSoft.h

    Criado: 03/06/2021

    Autor(es):

    Bruno Amaral (amaral_bruno@id.uff.br)

    �ltima atualiza��o: 03/06/2021 - Feita por: Bruno Amaral (amaral_bruno@id.uff.br)

    Descri��o da atualiza��o:

    Mudan�a na fun��o de execu��o da l�gica de controle. Agora, os c�lculos das 3 fases
    est�o resumidos em um loop de 3 itera��es, uma para cada fase.

	Descri��o do c�digo:

	Este c�digo � um teste para a nova estrutura de c�digo proposta por Bruno Amaral
	para o software do projeto de P&D da Light. Constitui a estrutura interna do c�digo de controle
	do limitador do projeto.

	C�digo recebe as entradas normalizadas da estrutura LCCHardware, verifica se h� curto
	na rede e realiza o controle de imped�ncia do equipamento.

	Uso:

	1) Chamar fun��o LCCSoft_criar para inicializar a estrutura com os par�metros desejados
    2) Chamar fun��o LCCSoft_executar para executar o algoritmo
    3) Ao fim da execu��o do programa, ou em caso de erro, chamar LCCSoft_destruir e reinicializar a
    estrutura

*/

#ifndef LIMITADOR_SOFT
#define LIMITADOR_SOFT

//Macros para modos de opera��o
#define LCC_SOFT_LIM_PLEN 0
#define LCC_SOFT_MOD_I 1

//Definindo novo tipo para estrutura
typedef struct limitador_cc LCCSoft;

/*
    Nome da fun��o: LCCSoft_criar

    Descri��o:

    Aloca mem�ria para um ponteiro do tipo LCCSoft*

    Tipo de retorno: LimitadroCC*

    Argumentos:

    double *tensoes_fase: ponteiro para tens�es de fase das 3 fases do sistema (recomenda-se em pu)
    double *correntes_linha: ponteiro para correntes de linha das 3 fases do sistema (recomenda-se em pu)
    double *ref_corrente_linha: ponteiro para refer�ncia de corrente de linha para
    controle de modula��o de imped�ncia
    double *ref_pwm: ponteiro para valor de refer�ncia do PWM
    double frequencia_nominal: frequ�ncia nominal do sistema
    double limite_sup_corrente: limite superior de corrente para detec��o em pu
    double limite_inf_corrente: limite inferior de corrente para sa�da do curto em pu
    double lambda: fator de sensibilidade. Deve ser um valor entre 0 e 1 
    double freq_amostragem: frequ�ncia de amostragem do sistema em Hz
    int modo_operacao: modo de opera��o do limitador; ; 0- Limita��o plena; 1- Controle de Corrente.
*/

//Prot�tipos de fun��es
LCCSoft*
LCCSoft_criar(double *tensoes_fase_norm,
                  double *correntes_linha_norm,
                  double *ref_corrente_linha_norm,
                  double *ref_pwm,
                  double frequencia_nominal,
                  double limite_sup_corrente,
                  double limite_inf_corrente,
                  double lambda,
                  double freq_amostragem,
                  int modo_operacao);
/*
    Nome da fun��o: LCCSoft_destruir

    Descri��o:

    Desaloca mem�ria para um ponteiro do tipo LCCSoft*

    Tipo de retorno: void

    Argumentos:

    LCCSoftHardware *LCC: ponteiro para uma estrutura do tipo LCCSoft*
*/

void
LCCSoft_destruir(LCCSoft *LCC);

/*
    Nome da fun��o: LCCSoft_executar

    Descri��o:

    Executa o c�digo de controle do limitador

    Tipo de retorno: void

    Argumentos:

    LCCSoft *LCC: ponteiro para uma estrutura do tipo LCCSoft*
*/

void
LCCSoft_executar(LCCSoft *LCC);

/*
    Nome da fun��o: LCCSoft_obter_var_internas

    Descri��o:

    Obt�m vari�veis internas da estrutura do LCCSoft

    Tipo de retorno: void

    Argumentos:

    double obter_dados: ponteiro para receber os valores internos
    LCCSoft *LCC: ponteiro para uma estrutura do tipo LCCSoft*
*/

void
LCCSoft_obter_var_internas(double *obter_dados,
                                    LCCSoft *LCC);

#endif // LIMITADOR_SOFT



