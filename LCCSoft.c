#include "LCCSoft.h"
#include "EPLLIII.h"
#include "ControlePID.h"
#include <math.h>
#include <stdlib.h>

//Macros para acesso de endere�os
#define EDR_FASE_A 0x00 //Endere�o Fase A
#define EDR_FASE_B 0x01 //Endere�o Fase B
#define EDR_FASE_C 0x02 //Endere�o Fase C

//Macro para freq angular
#define dois_pi 6.283185307

//Macro para intervalo de frequ�ncia
#define TOL_FREQ 5.0

//Macros para aux�lio nos c�lculos
#define RAIZ_DE_TRES 1.732050808

//Macro para sa�da de curto
#define NUM_AMOST_SAIDA_CURTO 75

//N�mero de fases
#define N_FASES 3

//Defini��o da estrutura
struct limitador_cc
{
    //Atributos

    // *** Entradas e sa�das

    //Grandezas el�tricas de entrada normalizadas (pu)
    double *tensoes_fase_normalizadas;
    double *correntes_linha_normalizadas;
    double *ref_corrente_linha_normalizada;

    //Vari�vel de sa�da para chaveamento
    double *ref_pwm;

    // *** EPLL ***

    //Grandezas estimadas pelo EPLL III
    double amptd_est_corrente_norm[3]; //Amplitudes pico
    double fase_est_corrente_norm[3];
    double sinais_est_corrente_norm[3];
    double frequencia_angular_estimada; //Frequ�ncia angular estimada

    //Par�metros exclusivos do EPLL III
    double amplitude_base;
    double ganho_um;
    double ganho_dois;
    double ganho_tres;
    double freq_max;
    double freq_min;

    //Estrutura para o EPLL
    EPLLIII *meuEPLL;

    // *** Controle PI ***

    //Vari�veis e par�metros controle PI
    double erro_corrente_fase[3];
    double saida_ctrl_pi_fase[3];
    double ctrl_pi_valor_max;
    double ctrl_pi_valor_min;
    double kp;
    double ki;
    double kd;

    //Controles PI para modula��o de imped�ncia
    ControladorPID *ControlePIFase[N_FASES];

    // *** LCC ***

    //Fun��es e par�metros do controle
    int modo_operacao;
    double freq_amostragem;
    double frequencia_nominal;
    void (*detectar_e_limitar)(LCCSoft*);

    //Vari�veis para detec��o de curto em cada fase
    double sensib; // Sensibilidade: sensib = 1/(1-lambda), onde lambda: fator de sensibilidade
    double limite_sup_corrente; //Limite superior de corrente em pu (Crit�rio de amplitude)
    double limite_inf_corrente; //Limite inferior de corrente em pu (Crit�rio de amplitude)
    double limite_sup_dif; //Limite superior de diferen�a em pu (Crit�rio de derivada)
    double limite_inf_dif; //Limite inferior de diferen�a em pu (Crit�rio de derivada)
    double dif_est[3]; //Estimativa de diferen�a
    double amptd_est_ant[3]; //Correntes do instante anterior
    int const_limite_saida_curto;
    int const_saida_curto[3];
    int estado_curto_fase[3];
};

//Fun��o auxiliar
double
modulo_limitador(double valor)
{
    //Se menor que zero
    if(valor<0.0) valor=-valor;

    //Retornando valor
    return valor;
}

//Fun��es para detec��o e controle de imped�ncia para cada modo de opera��o
void
detectar_limitar_modo_lim_plen(LCCSoft *LCC)
{
    //Vari�vel para itera��o
    static int k;

    //*** detec��es e controle de corrente por fase ***

    for (k=0; k<N_FASES; k++)
    {
        //Calculando diferen�a para cada fase
        LCC->dif_est[k]=*(LCC->correntes_linha_normalizadas+k)-LCC->amptd_est_ant[k];

        //Realizando a detec��o de cada fase
        if(!LCC->estado_curto_fase[k])
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])>LCC->limite_sup_corrente))
            {
                //Mudo para estado de curto
                LCC->estado_curto_fase[k]=1;

                //Abro as chaves
                *(LCC->ref_pwm+k)=-1.0; //Portadora entre 0 e 1. -1 garante chave aberta sem erros

                //Zero vari�vel auxiliar para sa�da de curto
                LCC->const_saida_curto[k]=0;
            }
        }else
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])<=LCC->limite_inf_corrente))
            {
                //Incremento vari�vel auxiliar
                LCC->const_saida_curto[k]++;
            }

            if(LCC->const_saida_curto[k]==LCC->const_limite_saida_curto)
            {
                //Mudo para estado de n�o curto
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
    //Vari�vel para itera��o
    static int k;

    //*** detec��es e controle de corrente por fase ***

    for (k=0; k<N_FASES; k++)
    {
        //Calculando diferen�a para cada fase
        LCC->dif_est[k]=*(LCC->amptd_est_corrente_norm+k)-LCC->amptd_est_ant[k];

        //Realizando a detec��o de cada fase
        if(!LCC->estado_curto_fase[k])
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])>LCC->limite_sup_corrente) ||
               (LCC->sensib*(LCC->dif_est[k]))>LCC->limite_sup_dif)
            {
                //Mudo para estado de curto
                LCC->estado_curto_fase[k]=1;

                //Zero vari�vel auxiliar para sa�da de curto
                LCC->const_saida_curto[k]=0;
            }
        }else
        {
            if((modulo_limitador(LCC->amptd_est_corrente_norm[k])<=LCC->limite_inf_corrente))
            {
                //Incremento vari�vel auxiliar
                LCC->const_saida_curto[k]++;
            }

            if(LCC->const_saida_curto[k]==LCC->const_limite_saida_curto)
            {
                //Mudo para estado de n�o curto
                LCC->estado_curto_fase[k]=0;
                *(LCC->ref_pwm+EDR_FASE_A)=1.0;

                //Resetando controle PI
                ControladorPID_resetar(LCC->ControlePIFase[k]);
            }
        }

        //Controle de imped�ncia
        if(LCC->estado_curto_fase[k])
        {
            //Calculando erro do PI
            LCC->erro_corrente_fase[k]=*LCC->ref_corrente_linha_normalizada
            -modulo_limitador(LCC->amptd_est_corrente_norm[k]);

            //Executando controle PI
            ControladorPID_executar(LCC->ControlePIFase[k]);

            //Atribuindo sa�da do PI a ref de pwm
            *(LCC->ref_pwm+k)=LCC->saida_ctrl_pi_fase[k];
        }else
        {
            *(LCC->ref_pwm+k)=1.0;
        }

    }

}

//Defini��es das fun��es do .h
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
                  int modo_operacao)
{
    //Vari�vel para itera��o
    int k;

    //Criando estrutura e iniciando-a
    LCCSoft *LCC=(LCCSoft*)malloc(sizeof(LCCSoft));

    // *** Inicializando atributos ***

    //Vari�veis e par�metros da l�gica do LCCSOFT
    LCC->tensoes_fase_normalizadas=tensoes_fase_norm;
    LCC->correntes_linha_normalizadas=correntes_linha_norm;
    LCC->ref_corrente_linha_normalizada=ref_corrente_linha_norm;
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

    // Fator de sensibilidade lambda e sensibilidade
    if ((lambda < 1.0) && (lambda >= 0.0))
    {
        LCC->sensib = 1.0/(1.0-lambda);
    }else
    {
        LCC->sensib = 1.5; // Valor padrão em caso de erro
    }
    
    //EPLL
    LCC->amplitude_base=1.0; //pu. OBS: assume-se que os valores de base usados sejam os nominais do sistema
    LCC->ganho_um=300.0; //Consultar livro do Ghartemani sobre PLL
    LCC->ganho_dois=(1.0/3.0)*(LCC->ganho_um/2.0)*(LCC->ganho_um/2.0); //Consultar livro do Ghartemani sobre PLL
    LCC->ganho_tres=300.0; //Consultar livro do Ghartemani sobre PLL

    for (k=0; k<N_FASES; k++)
    {
        //Sa�da
        LCC->ref_pwm[k]=1.0;

        //LCC
        LCC->tensoes_fase_normalizadas[k]=0.0;
        LCC->correntes_linha_normalizadas[k]=0.0;

        //EPLL
        LCC->fase_est_corrente_norm[k]=0.0;
        LCC->amptd_est_corrente_norm[k]=0.0;
        LCC->sinais_est_corrente_norm[k]=0.0;

        //Detec��o
        LCC->estado_curto_fase[k]=0;
        LCC->const_saida_curto[k]=0;
        LCC->amptd_est_ant[k]=0.0;
        LCC->dif_est[k]=0.0;
    }

    //Se modo de opera��o for limita��o plena, n�o h� necessidade de controle PI
    //Definindo modo de opera��o
    if (LCC->modo_operacao==LCC_SOFT_LIM_PLEN)
    {
        //Limita��o plena
        LCC->detectar_e_limitar=detectar_limitar_modo_lim_plen;
    }else
    {
        //Limita��o por modula��o de corrente
        LCC->detectar_e_limitar=detectar_limitar_modo_mod_i;

        //Inicializando vari�veis e estruturas dos controles PI
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

    } //Fim IF modo de opera��o

    //Criando estrutura para o EPLL III
    LCC->meuEPLL=EPLLIII_criar(LCC->correntes_linha_normalizadas,
                            LCC->amptd_est_corrente_norm,
                            LCC->fase_est_corrente_norm,
                            &LCC->frequencia_angular_estimada,
                            LCC->sinais_est_corrente_norm,
                            LCC->amplitude_base,
                            LCC->frequencia_nominal,
                            LCC->freq_amostragem,
                            LCC->ganho_um,
                            LCC->ganho_dois,
                            LCC->ganho_tres);

    //Verificando aloca��o

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
    //Executando o PLL
    EPLLIII_executar(LCC->meuEPLL);

    //Detectando e limitando
    LCC->detectar_e_limitar(LCC);

    //Atualizando vetor de correntes anteriores
    LCC->amptd_est_ant[EDR_FASE_A]=*(LCC->amptd_est_corrente_norm+EDR_FASE_A);
    LCC->amptd_est_ant[EDR_FASE_B]=*(LCC->amptd_est_corrente_norm+EDR_FASE_B);
    LCC->amptd_est_ant[EDR_FASE_C]=*(LCC->amptd_est_corrente_norm+EDR_FASE_C);
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
    //*(obter_dados+12)=LCC->sinais_est_corrente_norm[EDR_FASE_A];
    *(obter_dados+12)=LCC->dif_est[EDR_FASE_A];
    //*(obter_dados+12)=LCC->limite_sup_dif;
}


