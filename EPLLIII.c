#include "EPLLIII.h"
#include "stdlib.h"
#include "math.h"

// *** Macros

//Número de fases
#define N_FASES 3

//Tolerância de amplitude mínima
#define TOL_AMPLITUDE_MIN 0.01 // 1% da amplitude base

//Posições de vetores de variáveis de estado
#define N_POS_ESTADOS 2
#define N 0
#define N1 1

//Constante matemática
#define DOIS_PI 6.283185307179586

//Acesso de endereços de variáveis de cada fase
#define EDR_FASE_A 0
#define EDR_FASE_B 1
#define EDR_FASE_C 2


//Definição de tipo não visível ao usuários
typedef struct nucleo_eplliii NucleoEPLLIII;

//Definição da estrutura nucleo_eplliii
struct nucleo_eplliii
{
    // *** Definindo atributos

    //Entrada
    double *sinal_erro;
    double *freq_angular;

    //Saídas
    double *ampld_est;
    double *fase_est;
    double *sinal_est;
    double *x;

    //Parâmetros
    double freq_base;
    double tol_amptd_min;
    double periodo_amost;
    double ganho_um;
    double ganho_tres;

    // --- Variáveis internas ---

    //Modelagem em espaço de estados
    double amptd_ee[N_POS_ESTADOS];
    double fase_ee[N_POS_ESTADOS];

    // ---------------------------

    //Constantes para otimização de cálculos
    double period_amost_vezes_ganho_um;
};

//Definição da estrutura eplliii
struct eplliii
{
    // *** Definindo atributos

    //Entrada
    double *sinais_entrada;

    //Saídas
    double *amptd_est;
    double *fases_est;
    double *freq_est;
    double *sinais_est;

    //Parâmetros
    double amptd_base;
    double tol_amptd_min;
    double freq_base;
    double periodo_amost;
    double ganho_um;
    double ganho_dois;
    double ganho_tres;

    // --- Variáveis internas ---

    //Núcleos
    double freq_angular;
    double erros[N_FASES];
    double x[N_FASES];

    //Modelagem em espaço de estados
    double x_ef[N_POS_ESTADOS]; //Estimação de frequência (EF)

    // --------------------------

    //Estruturas para os núclos
    NucleoEPLLIII *meusNucleos[N_FASES];

    //Constantes para otimização de cálculos
    double period_amost_vezes_ganho_dois;
};

double
EPLLIII_valor_abs (double valor)
{
    if (valor<0.0)
    {
        valor=-valor;
    }

    return valor;
}

//Definição das funções

NucleoEPLLIII*
NucleoEPLLIII_criar(double *sinal_erro,
                    double *frequencia_angular,
                    double *amptd_est,
                    double *fase_est,
                    double *sinal_est,
                    double *x,
                    double freq_base,
                    double periodo_amost,
                    double tol_amptd_min,
                    double ganho_um,
                    double ganho_tres)
{
    //Variável para iteração
    int i;

    //Alocando memória para estrutura NucleoEPLLIII
    NucleoEPLLIII *meuNucleo=malloc(sizeof(NucleoEPLLIII));

    // *** Inicializando atributos

    //Entradas e saídas
    meuNucleo->sinal_erro=sinal_erro;
    meuNucleo->freq_angular=frequencia_angular;
    meuNucleo->ampld_est=amptd_est;
    meuNucleo->fase_est=fase_est;
    meuNucleo->sinal_est=sinal_est;
    meuNucleo->x=x;

    //Parâmetros
    meuNucleo->freq_base=freq_base;
    meuNucleo->periodo_amost=periodo_amost;
    meuNucleo->tol_amptd_min=tol_amptd_min;
    meuNucleo->ganho_um=ganho_um;
    meuNucleo->ganho_tres=ganho_tres;

    //Variáveis internas
    for (i=0; i<N_POS_ESTADOS; i++)
    {
        meuNucleo->amptd_ee[i]=0.1; // Para evitar divisão por zero
        meuNucleo->fase_ee[i]=0.0;
    }

    //Calculando constantes de otimização
    meuNucleo->period_amost_vezes_ganho_um=meuNucleo->periodo_amost*meuNucleo->ganho_um;

    //Retornando estrutura
    return meuNucleo;
}

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
               double ganho_tres)
{
    //Variável de iteração
    int i;

    //Alocando memória para estrutura EPLLIII
    EPLLIII *meuEPLL=malloc(sizeof(EPLLIII));

    // *** Inicializando atributos

    //Entradas e saídas
    meuEPLL->sinais_entrada=sinais_entrada;
    meuEPLL->amptd_est=amplitudes_estimadas_saida;
    meuEPLL->fases_est=fases_estimadas_saida;
    meuEPLL->freq_est=frequencia_estimada_saida;
    meuEPLL->sinais_est=sinais_estimados_saida;

    //Parâmetros
    meuEPLL->amptd_base=amplitude_base;
    meuEPLL->freq_base=frequencia_base*DOIS_PI;
    meuEPLL->periodo_amost=1.0/(frequencia_amostragem);
    meuEPLL->ganho_um=ganho_um;
    meuEPLL->ganho_dois=ganho_dois;
    meuEPLL->ganho_tres=ganho_tres;
    meuEPLL->tol_amptd_min=meuEPLL->amptd_base*TOL_AMPLITUDE_MIN; // 1% da amplitude base

    //Calculando constantes de otimização
    meuEPLL->period_amost_vezes_ganho_dois=meuEPLL->periodo_amost*meuEPLL->ganho_dois;

    //Variáveis de estimação de frequência
    meuEPLL->freq_angular=meuEPLL->freq_base;
    for (i=0; i<N_POS_ESTADOS; i++)
    {
        meuEPLL->x_ef[i]=0.0;
    }

    for (i=0; i<N_FASES; i++)
    {
        //Variáveis de E/S
        meuEPLL->sinais_entrada[i]=0.0;
        meuEPLL->amptd_est[i]=0.1; //Evitar divisão por zero
        meuEPLL->fases_est[i]=0.0;
        meuEPLL->freq_est[i]=meuEPLL->freq_base; // Valor inicial: frequência nominal da rede
        meuEPLL->sinais_est[i]=0.0;

        //Variáveis internas
        meuEPLL->erros[i]=0.0;
        meuEPLL->x[i]=0.0;

        //Estruturas dos núcleos de cada fase
        meuEPLL->meusNucleos[i]=NucleoEPLLIII_criar(meuEPLL->erros+i,
                                                    &meuEPLL->freq_angular,
                                                    meuEPLL->amptd_est+i,
                                                    meuEPLL->fases_est+i,
                                                    meuEPLL->sinais_est+i,
                                                    meuEPLL->x+i,
                                                    meuEPLL->freq_base,
                                                    meuEPLL->periodo_amost,
                                                    meuEPLL->tol_amptd_min,
                                                    meuEPLL->ganho_um,
                                                    meuEPLL->ganho_tres);
    }

    //Retornando estrutura
    return meuEPLL;
}

void
EPLLIII_reparametrizar(double amplitude_base,
                       double frequencia_base,
                       double frequencia_amostragem,
                       double ganho_um,
                       double ganho_dois,
                       double ganho_tres,
                       EPLLIII *meuEPLL)
{
    //Variável de iteração
    int i;

    // *** Redefinindo parâmetros do algoritmo

    //Parâmetros
    meuEPLL->amptd_base=amplitude_base;
    meuEPLL->freq_base=frequencia_base*DOIS_PI;
    meuEPLL->periodo_amost=1.0/(frequencia_amostragem);
    meuEPLL->ganho_um=ganho_um;
    meuEPLL->ganho_dois=ganho_dois;
    meuEPLL->ganho_tres=ganho_tres;
    meuEPLL->tol_amptd_min=meuEPLL->amptd_base*TOL_AMPLITUDE_MIN; // 1% da amplitude base

    //Calculando constantes de otimização
    meuEPLL->period_amost_vezes_ganho_dois=meuEPLL->periodo_amost*meuEPLL->ganho_dois;

    // *** Reinicializando variáveis

    //Variáveis de estimação de frequência
    meuEPLL->freq_angular=meuEPLL->freq_base;
    for (i=0; i<N_POS_ESTADOS; i++)
    {
        meuEPLL->x_ef[i]=0.0;
    }

    for (i=0; i<N_FASES; i++)
    {
        //Variáveis de E/S
        meuEPLL->sinais_entrada[i]=0.0;
        meuEPLL->amptd_est[i]=0.1; //Evitar divisão por zero
        meuEPLL->fases_est[i]=0.0;
        meuEPLL->freq_est[i]=meuEPLL->freq_base; // Valor inicial: frequência nominal da rede
        meuEPLL->sinais_est[i]=0.0;

        //Variáveis internas
        meuEPLL->erros[i]=0.0;
        meuEPLL->x[i]=0.0;

        //Estruturas dos núcleos de cada fase
        meuEPLL->meusNucleos[i]=NucleoEPLLIII_criar(meuEPLL->erros+i,
                                                    &meuEPLL->freq_angular,
                                                    meuEPLL->amptd_est+i,
                                                    meuEPLL->fases_est+i,
                                                    meuEPLL->sinais_est+i,
                                                    meuEPLL->x+i,
                                                    meuEPLL->freq_base,
                                                    meuEPLL->periodo_amost,
                                                    meuEPLL->tol_amptd_min,
                                                    meuEPLL->ganho_um,
                                                    meuEPLL->ganho_tres);
    }

}

void
NucleoEPLLIII_executar(NucleoEPLLIII *meuNucleo)
{
    //Atualizando saídas deste instante
    *meuNucleo->ampld_est=EPLLIII_valor_abs(meuNucleo->amptd_ee[N]);
    *meuNucleo->fase_est=meuNucleo->fase_ee[N];
    *meuNucleo->sinal_est=(*meuNucleo->ampld_est)*sin(*meuNucleo->fase_est);

    // *** Calculando variáveis de estado do próximo instate

    //Amplitude
    meuNucleo->amptd_ee[N1]=meuNucleo->amptd_ee[N]+
    meuNucleo->period_amost_vezes_ganho_um*(*meuNucleo->sinal_erro)*sin(meuNucleo->fase_ee[N]);

    //Para evitar divisão por zero
    if (*meuNucleo->ampld_est>meuNucleo->tol_amptd_min)
    {
        //Atualizando variável para estimação de frequência deste instante
        *meuNucleo->x=*meuNucleo->sinal_erro*cos(meuNucleo->fase_ee[N])/meuNucleo->amptd_ee[N];

        //Fase
        meuNucleo->fase_ee[N1]=meuNucleo->fase_ee[N]+meuNucleo->periodo_amost*(*meuNucleo->freq_angular+
        meuNucleo->ganho_tres*(*meuNucleo->sinal_erro)*cos(meuNucleo->fase_ee[N])/meuNucleo->amptd_ee[N]);
    } else //Se amplitude estimada do sinal for menor que 1% da amplitude base, então faça:
    {
        //Atualizando variável para estimação de frequência deste instante
        *meuNucleo->x=0.0;

        //Fase com correção
        meuNucleo->fase_ee[N1]=meuNucleo->fase_ee[N]+meuNucleo->periodo_amost*(meuNucleo->freq_base);
    }

    //Mantendo fase entre 0 e 2pi
    if (meuNucleo->fase_ee[N1]>=DOIS_PI)
    {
        meuNucleo->fase_ee[N1]-=DOIS_PI;
    }else if (meuNucleo->fase_ee[N1]<=-DOIS_PI)
    {
        meuNucleo->fase_ee[N1]+=DOIS_PI;
    }

    //Atualizando variáveis
    meuNucleo->amptd_ee[N]=meuNucleo->amptd_ee[N1];
    meuNucleo->fase_ee[N]=meuNucleo->fase_ee[N1];
}

void
EPLLIII_executar(EPLLIII *meuEPLL)
{
    //Variável auxiliar para iteração
    static int i;

    //Atualizando saídas deste instante
    meuEPLL->freq_angular=meuEPLL->freq_base+meuEPLL->x_ef[N];
    *meuEPLL->freq_est=EPLLIII_valor_abs(meuEPLL->freq_angular);

    //Caso não haja sinais de entrada, preciso manter algoritmo rodando
    for (i=0; i<N_FASES; i++)
    {
        if (meuEPLL->amptd_est[i]>meuEPLL->tol_amptd_min)
        {
            meuEPLL->erros[i]=meuEPLL->sinais_entrada[i]-meuEPLL->sinais_est[i];
        }else
        {
            //Usando sinais artificiais
            meuEPLL->erros[i]=meuEPLL->amptd_base*sin(meuEPLL->fases_est[i])-
            meuEPLL->sinais_est[i];
        }

        //Executando núcleo de cada fase
        NucleoEPLLIII_executar(meuEPLL->meusNucleos[i]);
    }

    //Estimando x_ef do próximo instante
    meuEPLL->x_ef[N1]=meuEPLL->x_ef[N]+meuEPLL->period_amost_vezes_ganho_dois*
    (meuEPLL->x[EDR_FASE_A]+meuEPLL->x[EDR_FASE_B]+meuEPLL->x[EDR_FASE_C]);

    //Atualizando
    meuEPLL->x_ef[N]=meuEPLL->x_ef[N1];
}

void
NucleoEPLLIII_destruir(NucleoEPLLIII *meuNucleo)
{
    free(meuNucleo);
}

void
EPLLIII_destruir(EPLLIII *meuEPLL)
{
    free(meuEPLL);
}
