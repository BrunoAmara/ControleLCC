#include "LCCHard.h"
#include "LCCSoft.h"
#include <stdlib.h>
#include "math.h"

//Macros para acesso de endere�os
#define EDR_FASE_A 0x00 //Endere�o Fase A
#define EDR_FASE_B 0x01 //Endere�o Fase B
#define EDR_FASE_C 0x02 //Endere�o Fase C

#define N_FASES 3

#define FATOR_CONV_TENSAO_BASE_TEMPORAL  8.164965809277261e-01
#define FATOR_CONV_CORRENTE_BASE_TEMPORAL  1.414213562373095
#define DE_kU_PARA_U 1000.0
#define ORDEM_FILTRO 1

//Defini��o da estrutura
struct limitador_cc_hardware
{
    //Vari�veis de entrada
	double tensoes_pu[N_FASES];
	double correntes_pu[N_FASES];
	double ref_corrente_pu;

	//Vari�veis de saida
	double ref_pwm[N_FASES]; //Nova ref_pwm

	//Valores base
	double tensao_fase_temporal_base;
	double corrente_linha_temporal_base;

	//Par�metros da convers�o AD
	double tensao_max_adc;
	int nivel_max_adc; //Ex: ADC de 10 bits: nivel_max_adc=1023
	int offset_adc;
	double ganho_circ_cond; // 3.3 VADC � mapeado no valor fator_conv_escala_adc

	//Constante para convers�o da leitura do adc para pu
	double fator_conv_corrente_adc_para_pu;

	//Estrutura para o LCC software
    LCCSoft *LCCNitee;
};

//Defini��es das fun��es do .h
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
              int modo_operacao)
{
    //Vari�vel para itera��o
    int k;

	//Criando estrutura e iniciando-a
	LCCHard *LCCHardware=malloc(sizeof(LCCHard));

	//Inicializando atributos de entrada e sa�da
	LCCHardware->ref_corrente_pu=0.0;
	for(k=0; k<N_FASES; k++)
    {
        LCCHardware->tensoes_pu[k]=0.0;
        LCCHardware->correntes_pu[k]=0.0;
        LCCHardware->ref_pwm[k]=0.0;
    }

	// *** Atributos de parametriza��o

	//Sistema el�trico de pot�ncia
	LCCHardware->tensao_fase_temporal_base=tensao_linha_base*FATOR_CONV_TENSAO_BASE_TEMPORAL*DE_kU_PARA_U;
    LCCHardware->corrente_linha_temporal_base=corrente_linha_base*FATOR_CONV_CORRENTE_BASE_TEMPORAL*DE_kU_PARA_U;

    //Sistema de convers�o AD
    LCCHardware->tensao_max_adc=tensao_max_adc;
    LCCHardware->nivel_max_adc=((int) pow(2.0, resolucao_bits_adc) - 1.0);
    LCCHardware->offset_adc=LCCHardware->nivel_max_adc/2;
	LCCHardware->ganho_circ_cond=ganho_condicionamento;

	// *** Calculando constante de convers�o de leitura de adc para pu. Esta constante ser� usada para convers�o
	//das leituras do ADC, no formato inteiro, para o formato ponto flutuante em valor por unidade (p.u.)

    LCCHardware->fator_conv_corrente_adc_para_pu=(LCCHardware->tensao_max_adc*LCCHardware->ganho_circ_cond)/
	(LCCHardware->nivel_max_adc*LCCHardware->corrente_linha_temporal_base);

    //Alocando mem�ria para estrutura do limitador
    LCCHardware->LCCNitee=LCCSoft_criar(LCCHardware->tensoes_pu,
                                        LCCHardware->correntes_pu,
                                        &LCCHardware->ref_corrente_pu,
                                        LCCHardware->ref_pwm,
                                        frequencia_nominal,
                                        limite_sup_corrente,
                                        limite_inf_corrente,
                                        lambda,
                                        freq_amostragem,
                                        modo_operacao);

	//Retornando estrutura
	return LCCHardware;
}

void
LCCHard_destruir(LCCHard *LCC)
{
	free(LCC);
}

//Tipo REAL do PSCAD compat�vel com double do C
void
LCCHard_atualizar_entradas(int *leituras_tensoes,
                           int *leituras_correntes,
                           double *ref_corrente,
                           LCCHard *LCC)
{
    //Vari�vel para itera��o
    static int k;

    //Normalizando entradas
    LCC->ref_corrente_pu=*ref_corrente; //Entrada de refer�ncia j� em pu
    for (k=0; k<N_FASES; k++)
    {
        LCC->tensoes_pu[k]=*(leituras_tensoes+k)/LCC->tensao_fase_temporal_base;
        LCC->correntes_pu[k]=(double) (*(leituras_correntes+k)-LCC->offset_adc)*LCC->fator_conv_corrente_adc_para_pu;
    }
}

//Tipo REAL do PSCAD compat�vel com double do C
void
LCCHard_atualizar_saidas(double *referencias_pwm,
                         LCCHard *LCC)
{
    //Vari�vel para itera��o
    register int k=0;

    //Atualizando sa�das
    for (; k<N_FASES; k++)
    {
        *(referencias_pwm+k)=LCC->ref_pwm[k];
    }
}

void
LCCHard_executar(LCCHard *LCC)
{
    LCCSoft_executar(LCC->LCCNitee);
}

void
LCCHard_obter_var_internas(double *obter_dados,
                           LCCHard *LCC)
{
    //Obtendo vari�veis internas
    LCCSoft_obter_var_internas(obter_dados,
                               LCC->LCCNitee);
}




