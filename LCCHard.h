/*
    Arquivo: LCCHard.h

    Criado: 05/06/2021

    Autor(es):

    Bruno Amaral (amaral_bruno@id.uff.br)

    Grupo de trabalho: NITEE
    C�digo destinado ao Trabalho de Conclus�o de Curso

    �ltima atualiza��o: 05/06/2021 - Feita por: Bruno Amaral (amaral_bruno@id.uff.br)

    Descri��o da atualiza��o:

    Foi usada como base a vers�o LimitadorHard para cria��o deste conjunto de arquivos

	Descri��o do c�digo:

	Uso:

*/

#ifndef LCC_HAL_PSCAD
#define LCC_HAL_PSCAD

//Macros para usu�rio definir modos de opera��o
#define LCC_HARD_LIM_PLEN 0
#define LCC_HARD_MOD_I 1

//Definindo novo tipo para estrutura
typedef struct limitador_cc_hardware LCCHard;

/*
    Nome da fun��o: LCCHard_criar

    Descri��o:

    Aloca mem�ria para um ponteiro do tipo LCCHard

    Tipo de retorno: LimitadroCCHardware*

    Argumentos:

    double tensao_linha_base: tens�o base de linha do sistema em kV
    double corrente_linha_base: corrente base de linha do sistema em kA
    double frequencia_nominal: frequ�ncia nominal do sistema em Hz
    double limite_sup_corrente: limite superior de corrente para decis�o sobre curto-circuito (pu)
    double limite_inf_corrente: limite inferior de corrente para decis�o sobre curto-circuito (pu)
    double lambda: fator de sensibilidade. Deve ser um valor entre 0 e 1 
    double freq_amostragem: frequ�ncia de amostragem do sistema
    double tensao_max_adc: tens�o de m�xima (nominal) de opera��o do ADC. Ex: 3.3V, 5V...
    int resolucao_bits_adc: resulo��o do conversor anal�gico-digital em bits. Ex: 10 bits, 12 bits, 16 bits.
    double ganho_condicionamento: valor na opera��o sobre o sinal de sa�da do sensor: leitura_sensor/ganho.
    int modo_operacao: modo de opera��o do limitador; ; 0- Limita��o plena; 1- Controle de Corrente.
*/

//Prot�tipos de fun��es
LCCHard*
LCCHard_criar(double tensao_linha_base,
              double corrente_linha_base,
              double frequencia_nominal,
              double limite_sup_corrente,
              double limite_inf_corrente,
              double lambda,
              double freq_amostragem,
              double tensao_max_adc,
              int resolucao_bits_adc,
              double ganho_condicionamento,
              int modo_operacao);

/*
    Nome da fun��o: LCCHard_destruir

    Descri��o:

    Desaloca mem�ria para um ponteiro do tipo LCCHard*

    Tipo de retorno: void

    Argumentos:

    LCCHard *LCC: ponteiro para uma estrutura do tipo LCCHard*

*/

void
LCCHard_destruir(LCCHard *LCC);

/*
    Nome da fun��o: LCCHard_atualizar_entradas

    Descri��o:

    Normaliza e prepara as vari�veis de entrada para as camadas superiores do c�digo

    Tipo de retorno: void

    Argumentos:

    int *leituras_tensoes: ponteiro para vetor de 3 posi��es com valores quantizados de tens�o do sistema
    int *leituras_correntes: ponteiro para vetor de 3 posi��es com valores quantizados de corrente do sistema
    double *ref_corrente: ponteiro para vari�vel com a refer�ncia em pu para a corrente limitada
    LCCHard *LCC: ponteiro para uma estrutura do tipo LCCHard*
*/

void
LCCHard_atualizar_entradas(int *leituras_tensoes,
                           int *leituras_correntes,
                           double *ref_corrente,
                           LCCHard *LCC);

/*
    Nome da fun��o: LCCHard_atualizar_saidas

    Descri��o:

    Retorna as sa�das do c�digo de controle

    Tipo de retorno: void

    Argumentos:

    double *referencias_pwm: refer�ncias das 3 fases calculadas para o PWM
    LCCHard *LCC: ponteiro para uma estrutura do tipo LCCHard*
*/

void
LCCHard_atualizar_saidas(double *referencias_pwm,
									 LCCHard *LCC);

/*
    Nome da fun��o: LCCHard_obter_var_internas

    Descri��o:

    Obt�m vari�veis internas da estrutura do LCCHard

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
