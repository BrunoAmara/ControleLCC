#include "LCCSoft.h"
#include "EPLLIII.h"
#include "mmq.h"
#include "FiltroPassaBanda.h"
#include "ControlePID.h"
#include <math.h>
#include <stdlib.h>

#define AMOSTRAS_MMQ            16

//Macros para acesso de endere�os
#define EDR_FASE_A 0x00 //Endere�o Fase A
#define EDR_FASE_B 0x01 //Endere�o Fase B
#define EDR_FASE_C 0x02 //Endere�o Fase C

//Macro para freq angular
#define dois_pi 6.283185307

//Macro para intervalo de frequência
#define TOL_FREQ 5.0

//Macros para auxílio nos cálculos
#define RAIZ_DE_TRES 1.732050808

//Macro para saída de curto
#define NUM_AMOST_SAIDA_CURTO 75

//Número de fases
#define N_FASES 3

//Definição da estrutura
struct limitador_cc
{
    //Atributos

    // *** Entradas e saídas

    //Grandezas elétricas de entrada normalizadas (pu)
    double *tensoes_fase_normalizadas;
    double *correntes_linha_normalizadas;
    double *ref_corrente_linha_normalizada;

    // Grandezas filtradas
    double *i_linha_filt;

    //Vari�vel de sa�da para chaveamento
    double *ref_pwm;

    // *** EPLL ***

    //Grandezas estimadas pelo EPLL III
    double amptd_est_corrente_norm[3]; //Amplitudes pico
    double fase_est_corrente_norm[3];
    double sinais_est_corrente_norm[3];
    double frequencia_angular_estimada; //Frequência angular estimada

    //Parâmetros exclusivos do EPLL III
    double amplitude_base;
    double ganho_um;
    double ganho_dois;
    double ganho_tres;
    double freq_max;
    double freq_min;

    // Filtro
    FiltroPassaBanda *fpb[N_FASES];

    //Estrutura para o EPLL
    MMQ *estimador_amplitude;

    // *** Controle PI ***

    //Variáveis e parâmetros controle PI
    double erro_corrente_fase[3];
    double saida_ctrl_pi_fase[3];
    double ctrl_pi_valor_max;
    double ctrl_pi_valor_min;
    double kp;
    double ki;
    double kd;

    //Controles PI para modulação de impedância
    ControladorPID *ControlePIFase[N_FASES];

    // *** LCC ***

    //Funções e parâmetros do controle
    int modo_operacao;
    double freq_amostragem;
    double frequencia_nominal;
    void (*detectar_e_limitar)(LCCSoft*);

    //Variáveis para detecção de curto em cada fase
    double limite_sup_corrente; //Limite superior de corrente em pu (Critério de amplitude)
    double limite_inf_corrente; //Limite inferior de corrente em pu (Critério de amplitude)
    double limite_sup_dif; //Limite superior de diferença em pu (Critério de derivada)
    double limite_inf_dif; //Limite inferior de diferença em pu (Critério de derivada)
    double dif_est[3]; //Estimativa de diferença
    double correntes_ant[3]; //Correntes do instante anterior
    int const_limite_saida_curto;
    int const_saida_curto[3];
    int estado_curto_fase[3];
};

//Função auxiliar
double
modulo_limitador(double valor)
{
    //Se menor que zero
    if(valor<0.0) valor=-valor;

    //Retornando valor
    return valor;
}

//Funções para detecção e controle de impedância para cada modo de operação
void
detectar_limitar_modo_lim_plen(LCCSoft *LCC)
{
    //Variável para iteração
    static int k;

    //*** detecções e controle de corrente por fase ***

    for (k=0; k<N_FASES; k++)
    {
        //Calculando diferença para cada fase
        LCC->dif_est[k]=*(LCC->correntes_linha_normalizadas+k)-LCC->correntes_ant[k];

        //Realizando a detecção de cada fase
        if(!LCC->estado_curto_fase[k])
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])>LCC->limite_sup_corrente))
            {
                //Mudo para estado de curto
                LCC->estado_curto_fase[k]=1;

                //Abro as chaves
                *(LCC->ref_pwm+k)=-1.0; //Portadora entre 0 e 1. -1 garante chave aberta sem erros

                //Zero variável auxiliar para saída de curto
                LCC->const_saida_curto[k]=0;
            }
        }else
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])<=LCC->limite_inf_corrente))
            {
                //Incremento variável auxiliar
                LCC->const_saida_curto[k]++;
            }

            if(LCC->const_saida_curto[k]==LCC->const_limite_saida_curto)
            {
                //Mudo para estado de não curto
                LCC->estado_curto_fase[k]=0;

                //Fecho as chaves
                *(LCC->ref_pwm+k)=1.1; //Portadora entre 0 e 1. 1.1 garante chave fechada sem erros
            }
        }

    }

}

void
detectar_limitar_modo_mod_i(LCCSoft *LCC)
{
    //Variável para iteração
    static int k;

    //*** detecções e controle de corrente por fase ***

    for (k=0; k<N_FASES; k++)
    {
        //Calculando diferença para cada fase
        LCC->dif_est[k]=*(LCC->correntes_linha_normalizadas+k)-LCC->correntes_ant[k];

        //Realizando a detecção de cada fase
        if(!LCC->estado_curto_fase[k])
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])>LCC->limite_sup_corrente))
            {
                //Mudo para estado de curto
                LCC->estado_curto_fase[k]=1;

                //Zero variável auxiliar para saída de curto
                LCC->const_saida_curto[k]=0;
            }
        }else
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])<=LCC->limite_inf_corrente))
            {
                //Incremento variável auxiliar
                LCC->const_saida_curto[k]++;
            }

            if(LCC->const_saida_curto[k]==LCC->const_limite_saida_curto)
            {
                //Mudo para estado de não curto
                LCC->estado_curto_fase[k]=0;
                *(LCC->ref_pwm+EDR_FASE_A)=1.0;

                //Resetando controle PI
                ControladorPID_resetar(LCC->ControlePIFase[k]);
            }
        }

        //Controle de impedância
        if(LCC->estado_curto_fase[k])
        {
            //Calculando erro do PI
            LCC->erro_corrente_fase[k]=*LCC->ref_corrente_linha_normalizada
            -modulo_limitador(LCC->amptd_est_corrente_norm[k]);

            //Executando controle PI
            ControladorPID_executar(LCC->ControlePIFase[k]);

            //Atribuindo saída do PI a ref de pwm
            *(LCC->ref_pwm+k)=LCC->saida_ctrl_pi_fase[k];
        }else
        {
            *(LCC->ref_pwm+k)=1.0;
        }

    }

}

//Definições das funções do .h
LCCSoft*
LCCSoft_criar(double *tensoes_fase_norm,
                  double *correntes_linha_norm,
                  double *ref_corrente_linha_norm,
                  double *ref_pwm,
                  double frequencia_nominal,
                  double limite_sup_corrente,
                  double limite_inf_corrente,
                  double freq_amostragem,
                  int modo_operacao)
{
    //Variável para iteração
    int k;

    //Criando estrutura e iniciando-a
    LCCSoft *LCC=(LCCSoft*)malloc(sizeof(LCCSoft));

    // *** Inicializando atributos ***

    //Variáveis e parâmetros da lógica do LCCSOFT
    LCC->tensoes_fase_normalizadas=tensoes_fase_norm;
    LCC->correntes_linha_normalizadas=correntes_linha_norm;
    LCC->ref_corrente_linha_normalizada=ref_corrente_linha_norm;
    LCC->i_linha_filt = malloc(sizeof(double)*3);
    LCC->ref_pwm=ref_pwm;
    LCC->freq_amostragem=freq_amostragem;
    LCC->limite_sup_corrente=limite_sup_corrente;
    LCC->limite_inf_corrente=limite_inf_corrente;
    LCC->limite_sup_dif=limite_sup_corrente*dois_pi*frequencia_nominal*(1.0/freq_amostragem);
    LCC->limite_inf_dif=limite_inf_corrente*dois_pi*frequencia_nominal*(1.0/freq_amostragem);
    LCC->frequencia_nominal=frequencia_nominal;
    LCC->frequencia_angular_estimada=0.0;
    LCC->modo_operacao=modo_operacao;
    LCC->freq_max=LCC->frequencia_nominal+TOL_FREQ;
    LCC->freq_min=LCC->frequencia_nominal-TOL_FREQ;
    LCC->const_limite_saida_curto=NUM_AMOST_SAIDA_CURTO;

    //EPLL
    LCC->amplitude_base=1.0; //pu. OBS: assume-se que os valores de base usados sejam os nominais do sistema
    LCC->ganho_um=300.0; //Consultar livro do Ghartemani sobre PLL
    LCC->ganho_dois=(1.0/3.0)*(LCC->ganho_um/2.0)*(LCC->ganho_um/2.0); //Consultar livro do Ghartemani sobre PLL
    LCC->ganho_tres=300.0; //Consultar livro do Ghartemani sobre PLL

    for (k=0; k<N_FASES; k++)
    {
        //Saída
        LCC->ref_pwm[k]=1.0;

        //LCC
        LCC->tensoes_fase_normalizadas[k]=0.0;
        LCC->correntes_linha_normalizadas[k]=0.0;

        //EPLL
        LCC->fase_est_corrente_norm[k]=0.0;
        LCC->amptd_est_corrente_norm[k]=0.0;
        LCC->sinais_est_corrente_norm[k]=0.0;

        //Detecção
        LCC->estado_curto_fase[k]=0;
        LCC->const_saida_curto[k]=0;
        LCC->correntes_ant[k]=0.0;
        LCC->dif_est[k]=0.0;

        LCC->fpb[k] = FiltroPassaBanda_criar(
            LCC->correntes_linha_normalizadas+k,
            LCC->i_linha_filt+k,
            2.0f*sqrtf(2.0f),
            CTE_2PI*frequencia_nominal,
            freq_amostragem);

    }

    //Se modo de operação for limitação plena, não há necessidade de controle PI
    //Definindo modo de operação
    if (LCC->modo_operacao==LCC_SOFT_LIM_PLEN)
    {
        //Limitação plena
        LCC->detectar_e_limitar=detectar_limitar_modo_lim_plen;
    }else
    {
        //Limitação por modulação de corrente
        LCC->detectar_e_limitar=detectar_limitar_modo_mod_i;

        //Inicializando variáveis e estruturas dos controles PI
        LCC->kp=200.0;
        LCC->ki=1000.0;
        LCC->kd=0.0;
        LCC->ctrl_pi_valor_max=1.0;
        LCC->ctrl_pi_valor_min=0.0;

        for (k=0; k<N_FASES; k++)
        {
            //Controle PI
            LCC->erro_corrente_fase[k]=0.0;
            LCC->saida_ctrl_pi_fase[k]=0.0;

            //Criando estrutura para o controle PI
            LCC->ControlePIFase[k]=Controlador_PID_criar(&LCC->erro_corrente_fase[k],
                                                         &LCC->saida_ctrl_pi_fase[k],
                                                         LCC->kp,
                                                         LCC->ki,
                                                         LCC->kd,
                                                         1.0/LCC->freq_amostragem,
                                                         LCC->ctrl_pi_valor_max,
                                                         LCC->ctrl_pi_valor_min,
                                                         PI);
        }

    } //Fim IF modo de operação

    // Criando estrutura para estimador de amplitude (mínimos quadrados)
    LCC->estimador_amplitude = MMQ_criar(
                                          LCC->i_linha_filt,
                                          LCC->amptd_est_corrente_norm,
                                          LCC->frequencia_nominal,
                                          LCC->freq_amostragem,
                                          AMOSTRAS_MMQ);

    //Verificando alocação

    //Retornando estrutura
    return LCC;
}

void
LCCSoft_destruir(LCCSoft *LCC)
{
    free(LCC);
}

void
LCCSoft_executar(LCCSoft *LCC)
{
    // Filtrando
    FiltroPassaBanda_computa(LCC->fpb[EDR_FASE_A]);
    FiltroPassaBanda_computa(LCC->fpb[EDR_FASE_B]);
    FiltroPassaBanda_computa(LCC->fpb[EDR_FASE_C]);

    // Calculando amplitudes
    MMQ_computa(LCC->estimador_amplitude);

    //Detectando e limitando
    LCC->detectar_e_limitar(LCC);

    //Atualizando vetor de correntes anteriores
    LCC->correntes_ant[EDR_FASE_A]=*(LCC->correntes_linha_normalizadas+EDR_FASE_A);
    LCC->correntes_ant[EDR_FASE_B]=*(LCC->correntes_linha_normalizadas+EDR_FASE_B);
    LCC->correntes_ant[EDR_FASE_C]=*(LCC->correntes_linha_normalizadas+EDR_FASE_C);
}

void
LCCSoft_obter_var_internas(double *obter_dados,
                               LCCSoft *LCC)
{
    *(obter_dados+EDR_FASE_A)=*(LCC->tensoes_fase_normalizadas+EDR_FASE_A);
    *(obter_dados+EDR_FASE_B)=*(LCC->tensoes_fase_normalizadas+EDR_FASE_B);
    *(obter_dados+EDR_FASE_C)=*(LCC->tensoes_fase_normalizadas+EDR_FASE_C);
    *(obter_dados+EDR_FASE_A+3)=*(LCC->correntes_linha_normalizadas+EDR_FASE_A);
    *(obter_dados+EDR_FASE_B+3)=*(LCC->correntes_linha_normalizadas+EDR_FASE_B);
    *(obter_dados+EDR_FASE_C+3)=*(LCC->correntes_linha_normalizadas+EDR_FASE_C);
    *(obter_dados+6)=modulo_limitador(LCC->amptd_est_corrente_norm[EDR_FASE_A]);
    *(obter_dados+7)=modulo_limitador(LCC->amptd_est_corrente_norm[EDR_FASE_B]);
    *(obter_dados+8)=modulo_limitador(LCC->amptd_est_corrente_norm[EDR_FASE_C]);
    *(obter_dados+9)=(double)LCC->estado_curto_fase[EDR_FASE_A];
    *(obter_dados+10)=(double)LCC->estado_curto_fase[EDR_FASE_B];
    *(obter_dados+11)=(double)LCC->estado_curto_fase[EDR_FASE_C];
    *(obter_dados+12)=LCC->sinais_est_corrente_norm[EDR_FASE_A];
}


