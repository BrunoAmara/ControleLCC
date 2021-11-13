#include "LCCHard.h"

/*
    Arquivo: subrotina_principal_pscad.c

    Criado por: Bruno Amaral

    C�digo destinado ao trabalho de conclus�o de curso

    Descri��o:

    Rotina de interface entre o c�digo de controle e a plataforma PSCAD.
    Ela ser� chamada pela rotina implementada dentro do bloco que modela
    a UMC na simula��o do PSCAD.


*/

void HAL_PSCAD (int *leitura_tensoes_fase,
                int *leitura_correntes_linha,
                double *leitura_ref_corrente_linha,
                double *saida_ref_pwm,
                double *tensao_linha_base,
                double *corrente_linha_base,
                double *frequencia_nominal,
                double *limite_superior_corrente,
                double *limite_inferior_corrente,
                double *freq_amostragem,
                double *tensao_max_adc,
                int *res_adc,
                double *fator_conv_esc,
                int *modo_op,
                double *dados_internos)
{
    //Declara��o de vari�veis
    static LCCHard *meuLCC;
    static int iteracao=0;

    if(iteracao==0)
    {
        //Alocando mem�ria para estrutura LCCHard
        meuLCC=LCCHard_criar(*tensao_linha_base,
                             *corrente_linha_base,
                             *frequencia_nominal,
                             *limite_superior_corrente,
                             *limite_inferior_corrente,
                             *freq_amostragem,
                             *tensao_max_adc,
                             *res_adc,
                             *fator_conv_esc,
                             *modo_op);

        //Atualizando entradas
        LCCHard_atualizar_entradas(leitura_tensoes_fase,
                                   leitura_correntes_linha,
                                   leitura_ref_corrente_linha,
                                   meuLCC);

        //Executando algoritmo
        LCCHard_executar(meuLCC);

        //Atualizando saida
        LCCHard_atualizar_saidas(saida_ref_pwm,
                                 meuLCC);

        //Lendo vari�veis internas do limitador
        LCCHard_obter_var_internas(dados_internos, meuLCC);

        //REALIZO ALOCA��O SOMENTE UMA VEZ
        iteracao++;
    }else
    {
        //Atualizando entradas
        LCCHard_atualizar_entradas(leitura_tensoes_fase,
                                   leitura_correntes_linha,
                                   leitura_ref_corrente_linha,
                                   meuLCC);

        //Executando algoritmo
        LCCHard_executar(meuLCC);

        //Atualizando saida
        LCCHard_atualizar_saidas(saida_ref_pwm,
                                 meuLCC);

        //Lendo vari�veis internas do limitador
        LCCHard_obter_var_internas(dados_internos, meuLCC);
    }

}
