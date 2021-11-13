/*
    Arquivo: LCCSoft.h

    Criado: 03/06/2021

    Autor(es):

    Bruno Amaral (amaral_bruno@id.uff.br)

    Última atualização: 03/06/2021 - Feita por: Bruno Amaral (amaral_bruno@id.uff.br)

    Descrição da atualização:

    Mudança na função de execução da lógica de controle. Agora, os cálculos das 3 fases
    estão resumidos em um loop de 3 iterações, uma para cada fase.

	Descrição do código:

	Este código é um teste para a nova estrutura de código proposta por Bruno Amaral
	para o software do projeto de P&D da Light. Constitui a estrutura interna do código de controle
	do limitador do projeto.

	Código recebe as entradas normalizadas da estrutura LCCHardware, verifica se há curto
	na rede e realiza o controle de impedância do equipamento.

	Uso:

	1) Chamar função LCCSoft_criar para inicializar a estrutura com os parâmetros desejados
    2) Chamar função LCCSoft_executar para executar o algoritmo
    3) Ao fim da execução do programa, ou em caso de erro, chamar LCCSoft_destruir e reinicializar a
    estrutura

*/

#ifndef LIMITADOR_SOFT
#define LIMITADOR_SOFT

//Macros para modos de operação
#define LCC_SOFT_LIM_PLEN 0
#define LCC_SOFT_MOD_I 1

//Definindo novo tipo para estrutura
typedef struct limitador_cc LCCSoft;

/*
    Nome da função: LCCSoft_criar

    Descrição:

    Aloca memória para um ponteiro do tipo LCCSoft*

    Tipo de retorno: LimitadroCC*

    Argumentos:

    double *tensoes_fase: ponteiro para tensões de fase das 3 fases do sistema (recomenda-se em pu)
    double *correntes_linha: ponteiro para correntes de linha das 3 fases do sistema (recomenda-se em pu)
    double *ref_corrente_linha: ponteiro para referência de corrente de linha para
    controle de modulação de impedância
    double *ref_pwm: ponteiro para valor de referência do PWM
    double frequencia_nominal: frequência nominal do sistema
    double limite_sup_corrente: limite superior de corrente para detecção em pu
    double limite_inf_corrente: limite inferior de corrente para saída do curto em pu
    double freq_amostragem: frequência de amostragem do sistema em Hz
    int modo_operacao: modo de operação do limitador; ; 0- Limitação plena; 1- Controle de Corrente.
*/

//Protótipos de funções
LCCSoft*
LCCSoft_criar(double *tensoes_fase_norm,
                  double *correntes_linha_norm,
                  double *ref_corrente_linha_norm,
                  double *ref_pwm,
                  double frequencia_nominal,
                  double limite_sup_corrente,
                  double limite_inf_corrente,
                  double freq_amostragem,
                  int modo_operacao);
/*
    Nome da função: LCCSoft_destruir

    Descrição:

    Desaloca memória para um ponteiro do tipo LCCSoft*

    Tipo de retorno: void

    Argumentos:

    LCCSoftHardware *LCC: ponteiro para uma estrutura do tipo LCCSoft*
*/

void
LCCSoft_destruir(LCCSoft *LCC);

/*
    Nome da função: LCCSoft_executar

    Descrição:

    Executa o código de controle do limitador

    Tipo de retorno: void

    Argumentos:

    LCCSoft *LCC: ponteiro para uma estrutura do tipo LCCSoft*
*/

void
LCCSoft_executar(LCCSoft *LCC);

/*
    Nome da função: LCCSoft_obter_var_internas

    Descrição:

    Obtém variáveis internas da estrutura do LCCSoft

    Tipo de retorno: void

    Argumentos:

    double obter_dados: ponteiro para receber os valores internos
    LCCSoft *LCC: ponteiro para uma estrutura do tipo LCCSoft*
*/

void
LCCSoft_obter_var_internas(double *obter_dados,
                                    LCCSoft *LCC);

#endif // LIMITADOR_SOFT



