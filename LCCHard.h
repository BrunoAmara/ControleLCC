/*
    Arquivo: LCCHard.h

    Criado: 05/06/2021

    Autor(es):

    Bruno Amaral (amaral_bruno@id.uff.br)

    Grupo de trabalho: NITEE
    Código destinado ao Trabalho de Conclusão de Curso

    Última atualização: 05/06/2021 - Feita por: Bruno Amaral (amaral_bruno@id.uff.br)

    Descrição da atualização:

    Foi usada como base a versão LimitadorHard para criação deste conjunto de arquivos

	Descrição do código:

	Uso:

*/

#ifndef LCC_HAL_PSCAD
#define LCC_HAL_PSCAD

//Macros para usuário definir modos de operação
#define LCC_HARD_LIM_PLEN 0
#define LCC_HARD_MOD_I 1

//Definindo novo tipo para estrutura
typedef struct limitador_cc_hardware LCCHard;

/*
    Nome da função: LCCHard_criar

    Descrição:

    Aloca memória para um ponteiro do tipo LCCHard

    Tipo de retorno: LimitadroCCHardware*

    Argumentos:

    double tensao_linha_base: tensão base de linha do sistema em kV
    double corrente_linha_base: corrente base de linha do sistema em kA
    double frequencia_nominal: frequência nominal do sistema em Hz
    double limite_sup_corrente: limite superior de corrente para decisão sobre curto-circuito (pu)
    double limite_inf_corrente: limite inferior de corrente para decisão sobre curto-circuito (pu)
    double freq_amostragem: frequência de amostragem do sistema
    double tensao_max_adc: tensão de máxima (nominal) de operação do ADC. Ex: 3.3V, 5V...
    int resolucao_bits_adc: resuloção do conversor analógico-digital em bits. Ex: 10 bits, 12 bits, 16 bits.
    double ganho_condicionamento: valor na operação sobre o sinal de saída do sensor: leitura_sensor/ganho.
    int modo_operacao: modo de operação do limitador; ; 0- Limitação plena; 1- Controle de Corrente.
*/

//Protótipos de funções
LCCHard*
LCCHard_criar(double tensao_linha_base,
              double corrente_linha_base,
              double frequencia_nominal,
              double limite_sup_corrente,
              double limite_inf_corrente,
              double freq_amostragem,
              double tensao_max_adc,
              int resolucao_bits_adc,
              double ganho_condicionamento,
              int modo_operacao);

/*
    Nome da função: LCCHard_destruir

    Descrição:

    Desaloca memória para um ponteiro do tipo LCCHard*

    Tipo de retorno: void

    Argumentos:

    LCCHard *LCC: ponteiro para uma estrutura do tipo LCCHard*

*/

void
LCCHard_destruir(LCCHard *LCC);

/*
    Nome da função: LCCHard_atualizar_entradas

    Descrição:

    Normaliza e prepara as variáveis de entrada para as camadas superiores do código

    Tipo de retorno: void

    Argumentos:

    int *leituras_tensoes: ponteiro para vetor de 3 posições com valores quantizados de tensão do sistema
    int *leituras_correntes: ponteiro para vetor de 3 posições com valores quantizados de corrente do sistema
    double *ref_corrente: ponteiro para variável com a referência em pu para a corrente limitada
    LCCHard *LCC: ponteiro para uma estrutura do tipo LCCHard*
*/

void
LCCHard_atualizar_entradas(int *leituras_tensoes,
                           int *leituras_correntes,
                           double *ref_corrente,
                           LCCHard *LCC);

/*
    Nome da função: LCCHard_atualizar_saidas

    Descrição:

    Retorna as saídas do código de controle

    Tipo de retorno: void

    Argumentos:

    double *referencias_pwm: referências das 3 fases calculadas para o PWM
    LCCHard *LCC: ponteiro para uma estrutura do tipo LCCHard*
*/

void
LCCHard_atualizar_saidas(double *referencias_pwm,
									 LCCHard *LCC);

/*
    Nome da função: LCCHard_obter_var_internas

    Descrição:

    Obtém variáveis internas da estrutura do LCCHard

    Tipo de retorno: void

    Argumentos:

    double obter_dados: ponteiro para receber os valores internos
    LCCHard *LCC: ponteiro para uma estrutura do tipo LCCHard*

*/

void
LCCHard_executar(LCCHard *LCC);

void
LCCHard_obter_var_internas(double *obter_dados,
                           LCCHard *LCC);

#endif // LCC_HAL_PSCAD
