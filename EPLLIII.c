#include "EPLLIII.h"
#include "stdlib.h"
#include "math.h"

// *** Macros

//N�mero de fases
#define N_FASES 3

//Toler�ncia de amplitude m�nima
#define TOL_AMPLITUDE_MIN 0.01 // 1% da amplitude base

//Posi��es de vetores de vari�veis de estado
#define N_POS_ESTADOS 2
#define N 0
#define N1 1

//Constante matem�tica
#define DOIS_PI 6.283185307179586

//Acesso de endere�os de vari�veis de cada fase
#define EDR_FASE_A 0
#define EDR_FASE_B 1
#define EDR_FASE_C 2


//Defini��o de tipo n�o vis�vel ao usu�rios
typedef struct nucleo_eplliii NucleoEPLLIII;

//Defini��o da estrutura nucleo_eplliii
struct nucleo_eplliii
{
    // *** Definindo atributos

    //Entrada
    double *sinal_erro;
    double *freq_angular;

    //Sa�das
    double *ampld_est;
    double *fase_est;
    double *sinal_est;
    double *x;

    //Par�metros
    double freq_base;
    double tol_amptd_min;
    double periodo_amost;
    double ganho_um;
    double ganho_tres;

    // --- Vari�veis internas ---

    //Modelagem em espa�o de estados
    double amptd_ee[N_POS_ESTADOS];
    double fase_ee[N_POS_ESTADOS];

    // ---------------------------

    //Constantes para otimiza��o de c�lculos
    double period_amost_vezes_ganho_um;
};

//Defini��o da estrutura eplliii
struct eplliii
{
    // *** Definindo atributos

    //Entrada
    double *sinais_entrada;

    //Sa�das
    double *amptd_est;
    double *fases_est;
    double *freq_est;
    double *sinais_est;

    //Par�metros
    double amptd_base;
    double tol_amptd_min;
    double freq_base;
    double periodo_amost;
    double ganho_um;
    double ganho_dois;
    double ganho_tres;

    // --- Vari�veis internas ---

    //N�cleos
    double freq_angular;
    double erros[N_FASES];
    double x[N_FASES];

    //Modelagem em espa�o de estados
    double x_ef[N_POS_ESTADOS]; //Estima��o de frequ�ncia (EF)

    // --------------------------

    //Estruturas para os n�clos
    NucleoEPLLIII *meusNucleos[N_FASES];

    //Constantes para otimiza��o de c�lculos
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

//Defini��o das fun��es

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
    //Vari�vel para itera��o
    int i;

    //Alocando mem�ria para estrutura NucleoEPLLIII
    NucleoEPLLIII *meuNucleo=malloc(sizeof(NucleoEPLLIII));

    // *** Inicializando atributos

    //Entradas e sa�das
    meuNucleo->sinal_erro=sinal_erro;
    meuNucleo->freq_angular=frequencia_angular;
    meuNucleo->ampld_est=amptd_est;
    meuNucleo->fase_est=fase_est;
    meuNucleo->sinal_est=sinal_est;
    meuNucleo->x=x;

    //Par�metros
    meuNucleo->freq_base=freq_base;
    meuNucleo->periodo_amost=periodo_amost;
    meuNucleo->tol_amptd_min=tol_amptd_min;
    meuNucleo->ganho_um=ganho_um;
    meuNucleo->ganho_tres=ganho_tres;

    //Vari�veis internas
    for (i=0; i<N_POS_ESTADOS; i++)
    {
        meuNucleo->amptd_ee[i]=0.1; // Para evitar divis�o por zero
        meuNucleo->fase_ee[i]=0.0;
    }

    //Calculando constantes de otimiza��o
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
    //Vari�vel de itera��o
    int i;

    //Alocando mem�ria para estrutura EPLLIII
    EPLLIII *meuEPLL=malloc(sizeof(EPLLIII));

    // *** Inicializando atributos

    //Entradas e sa�das
    meuEPLL->sinais_entrada=sinais_entrada;
    meuEPLL->amptd_est=amplitudes_estimadas_saida;
    meuEPLL->fases_est=fases_estimadas_saida;
    meuEPLL->freq_est=frequencia_estimada_saida;
    meuEPLL->sinais_est=sinais_estimados_saida;

    //Par�metros
    meuEPLL->amptd_base=amplitude_base;
    meuEPLL->freq_base=frequencia_base*DOIS_PI;
    meuEPLL->periodo_amost=1.0/(frequencia_amostragem);
    meuEPLL->ganho_um=ganho_um;
    meuEPLL->ganho_dois=ganho_dois;
    meuEPLL->ganho_tres=ganho_tres;
    meuEPLL->tol_amptd_min=meuEPLL->amptd_base*TOL_AMPLITUDE_MIN; // 1% da amplitude base

    //Calculando constantes de otimiza��o
    meuEPLL->period_amost_vezes_ganho_dois=meuEPLL->periodo_amost*meuEPLL->ganho_dois;

    //Vari�veis de estima��o de frequ�ncia
    meuEPLL->freq_angular=meuEPLL->freq_base;
    for (i=0; i<N_POS_ESTADOS; i++)
    {
        meuEPLL->x_ef[i]=0.0;
    }

    for (i=0; i<N_FASES; i++)
    {
        //Vari�veis de E/S
        meuEPLL->sinais_entrada[i]=0.0;
        meuEPLL->amptd_est[i]=0.1; //Evitar divis�o por zero
        meuEPLL->fases_est[i]=0.0;
        meuEPLL->freq_est[i]=meuEPLL->freq_base; // Valor inicial: frequ�ncia nominal da rede
        meuEPLL->sinais_est[i]=0.0;

        //Vari�veis internas
        meuEPLL->erros[i]=0.0;
        meuEPLL->x[i]=0.0;

        //Estruturas dos n�cleos de cada fase
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
    //Vari�vel de itera��o
    int i;

    // *** Redefinindo par�metros do algoritmo

    //Par�metros
    meuEPLL->amptd_base=amplitude_base;
    meuEPLL->freq_base=frequencia_base*DOIS_PI;
    meuEPLL->periodo_amost=1.0/(frequencia_amostragem);
    meuEPLL->ganho_um=ganho_um;
    meuEPLL->ganho_dois=ganho_dois;
    meuEPLL->ganho_tres=ganho_tres;
    meuEPLL->tol_amptd_min=meuEPLL->amptd_base*TOL_AMPLITUDE_MIN; // 1% da amplitude base

    //Calculando constantes de otimiza��o
    meuEPLL->period_amost_vezes_ganho_dois=meuEPLL->periodo_amost*meuEPLL->ganho_dois;

    // *** Reinicializando vari�veis

    //Vari�veis de estima��o de frequ�ncia
    meuEPLL->freq_angular=meuEPLL->freq_base;
    for (i=0; i<N_POS_ESTADOS; i++)
    {
        meuEPLL->x_ef[i]=0.0;
    }

    for (i=0; i<N_FASES; i++)
    {
        //Vari�veis de E/S
        meuEPLL->sinais_entrada[i]=0.0;
        meuEPLL->amptd_est[i]=0.1; //Evitar divis�o por zero
        meuEPLL->fases_est[i]=0.0;
        meuEPLL->freq_est[i]=meuEPLL->freq_base; // Valor inicial: frequ�ncia nominal da rede
        meuEPLL->sinais_est[i]=0.0;

        //Vari�veis internas
        meuEPLL->erros[i]=0.0;
        meuEPLL->x[i]=0.0;

        //Estruturas dos n�cleos de cada fase
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
    //Atualizando sa�das deste instante
    *meuNucleo->ampld_est=EPLLIII_valor_abs(meuNucleo->amptd_ee[N]);
    *meuNucleo->fase_est=meuNucleo->fase_ee[N];
    *meuNucleo->sinal_est=(*meuNucleo->ampld_est)*sin(*meuNucleo->fase_est);

    // *** Calculando vari�veis de estado do pr�ximo instate

    //Amplitude
    meuNucleo->amptd_ee[N1]=meuNucleo->amptd_ee[N]+
    meuNucleo->period_amost_vezes_ganho_um*(*meuNucleo->sinal_erro)*sin(meuNucleo->fase_ee[N]);

    //Para evitar divis�o por zero
    if (*meuNucleo->ampld_est>meuNucleo->tol_amptd_min)
    {
        //Atualizando vari�vel para estima��o de frequ�ncia deste instante
        *meuNucleo->x=*meuNucleo->sinal_erro*cos(meuNucleo->fase_ee[N])/meuNucleo->amptd_ee[N];

        //Fase
        meuNucleo->fase_ee[N1]=meuNucleo->fase_ee[N]+meuNucleo->periodo_amost*(*meuNucleo->freq_angular+
        meuNucleo->ganho_tres*(*meuNucleo->sinal_erro)*cos(meuNucleo->fase_ee[N])/meuNucleo->amptd_ee[N]);
    } else //Se amplitude estimada do sinal for menor que 1% da amplitude base, ent�o fa�a:
    {
        //Atualizando vari�vel para estima��o de frequ�ncia deste instante
        *meuNucleo->x=0.0;

        //Fase com corre��o
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

    //Atualizando vari�veis
    meuNucleo->amptd_ee[N]=meuNucleo->amptd_ee[N1];
    meuNucleo->fase_ee[N]=meuNucleo->fase_ee[N1];
}

void
EPLLIII_executar(EPLLIII *meuEPLL)
{
    //Vari�vel auxiliar para itera��o
    static int i;

    //Atualizando sa�das deste instante
    meuEPLL->freq_angular=meuEPLL->freq_base+meuEPLL->x_ef[N];
    *meuEPLL->freq_est=EPLLIII_valor_abs(meuEPLL->freq_angular);

    //Caso n�o haja sinais de entrada, preciso manter algoritmo rodando
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

        //Executando n�cleo de cada fase
        NucleoEPLLIII_executar(meuEPLL->meusNucleos[i]);
    }

    //Estimando x_ef do pr�ximo instante
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
