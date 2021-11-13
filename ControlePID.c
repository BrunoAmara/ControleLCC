#include "ControlePID.h"

//DEFININDO STRUCT CONTROLADORPID
struct ControladorPID{

    //PARÂMETROS DO CONTROLE E VETORES DE ENTRADA E SAÍDA DE SINAL
    double kp;
    double ki;
    double kd;
    double period_amost;
    double somador; //INTEGRAL
    double *sinal_entrada;
    double *sinal_saida;
    double *entradas_antigas; //DEVIDO AO BLOCO INTEGRATIVO E  AO DERIVATIVO

    //VALORES LIMITANTES PARA A SAÍDA DO CONTROLADOR
    double valor_max;
    double valor_min;
    int anti_windup;

    //VARIÁVEIS AUXILIARES
    double sinal_saida_nao_limitada;

    //CONSTANTES PARA AUXILIAR NOS CÁLCULOS
    double kd_sobre_pamost;
    double ki_vezes_pamost_sobre_dois;

    //PONTEIRO RESPONSÁVEL POR IMPLEMENTAR O CONTROLE EM TEMPO DISCRETO
    void (*func_transf_controle)(ControladorPID*);
};

//DEFINIÇÕES DAS FUNÇÕES
void
func_transf_p(ControladorPID *controlePID)
{

    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada);

    //LIMITANDO AMPLITUDE DA SAÍDA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

}

void
func_transf_i(ControladorPID *controlePID)
{

    //ATUALIZANDO VALOR DO SOMADOR(CALCULANDO A INTEGRAL PELO MÉTODO TRAPEZOIDAL)
    if(controlePID->anti_windup!=1){
        controlePID->somador=controlePID->somador
        +controlePID->ki_vezes_pamost_sobre_dois*((*controlePID->sinal_entrada)+(*controlePID->entradas_antigas));
    }

    //CALCULANDO SAÍDA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->somador;

    //VARIÁVEL AUXILIAR RECEBE VALOR DA SAÍDA SEM A LIMITAÇÃO
    controlePID->sinal_saida_nao_limitada=*controlePID->sinal_saida;

    //LIMITANDO AMPLITUDE DA SAÍDA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

    /*SE AUX1 E AUX2 FOREM IGUAIS, A SAÍDA DO PID ATIGIU SATURAÇÃO
    SE O SINAL DE ERRO (SINAL_ENTRADA) E A SAÍDA SEM LIMITAÇÃO(AUX1) FOREM POSITIVAS...
    O VALOR DO INTEGRADOR CONTINUARÁ A SUBIR MESMO EM SATURAÇÃO
    ENTÃO, CANCELE A INTEGRAÇÃO
    */
    if(((*controlePID->sinal_entrada>0.0)&&(controlePID->sinal_saida_nao_limitada>controlePID->valor_max))||
    ((*controlePID->sinal_entrada<0.0)&&(controlePID->sinal_saida_nao_limitada<controlePID->valor_min))){
        controlePID->anti_windup=1;
    }else{
        controlePID->anti_windup=0;
    }

    //ATUALIZANDO VALOR ANTIGO DA ENTRADA
    *controlePID->entradas_antigas=*controlePID->sinal_entrada;

}

void
func_transf_pi(ControladorPID *controlePID)
{

    //ATUALIZANDO VALOR DO SOMADOR(CALCULANDO A INTEGRAL PELO MÉTODO TRAPEZOIDAL)
    if(controlePID->anti_windup!=1){
        controlePID->somador=controlePID->somador
        +controlePID->ki_vezes_pamost_sobre_dois*((*controlePID->sinal_entrada)+(*controlePID->entradas_antigas));
    }

    //CALCULANDO SAÍDA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada)+controlePID->somador;

    //VARIÁVEL AUXILIAR RECEBE VALOR DA SAÍDA SEM A LIMITAÇÃO
    controlePID->sinal_saida_nao_limitada=*controlePID->sinal_saida;

    //LIMITANDO AMPLITUDE DA SAÍDA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

    /*SE AUX1 E AUX2 FOREM IGUAIS, A SAÍDA DO PID ATIGIU SATURAÇÃO
    SE O SINAL DE ERRO (SINAL_ENTRADA) E A SAÍDA SEM LIMITAÇÃO(AUX1) FOREM POSITIVAS...
    O VALOR DO INTEGRADOR CONTINUARÁ A SUBIR MESMO EM SATURAÇÃO
    ENTÃO, CANCELE A INTEGRAÇÃO
    */
    if(((*controlePID->sinal_entrada>0.0)&&(controlePID->sinal_saida_nao_limitada>controlePID->valor_max))||
    ((*controlePID->sinal_entrada<0.0)&&(controlePID->sinal_saida_nao_limitada<controlePID->valor_min))){
        controlePID->anti_windup=1;
    }else{
        controlePID->anti_windup=0;
    }

    //ATUALIZANDO VALOR ANTIGO DA ENTRADA
    *controlePID->entradas_antigas=*controlePID->sinal_entrada;

}

void
func_transf_pd(ControladorPID *controlePID)
{

    //CALCULANDO SAÍDA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada)
    +controlePID->kd_sobre_pamost*((*controlePID->sinal_entrada)-(*controlePID->entradas_antigas));

    //LIMITANDO AMPLITUDE DA SAÍDA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

    //ATUALIZANDO VALOR ANTIGO DA ENTRADA
    *controlePID->entradas_antigas=*controlePID->sinal_entrada;
}

void
func_transf_pid(ControladorPID *controlePID)
{

    //ATUALIZANDO VALOR DO SOMADOR(CALCULANDO A INTEGRAL PELO MÉTODO TRAPEZOIDAL)
    if(controlePID->anti_windup!=1){
        controlePID->somador=controlePID->somador
        +controlePID->ki_vezes_pamost_sobre_dois*((*controlePID->sinal_entrada)+(*controlePID->entradas_antigas));
    }

    //CALCULANDO SAÍDA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada)
    +controlePID->kd_sobre_pamost*((*controlePID->sinal_entrada)-(*controlePID->entradas_antigas))
    +controlePID->somador;

    //VARIÁVEL AUXILIAR RECEBE VALOR DA SAÍDA SEM A LIMITAÇÃO
    controlePID->sinal_saida_nao_limitada=*controlePID->sinal_saida;

    //LIMITANDO AMPLITUDE DA SAÍDA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

    /*SE AUX1 E AUX2 FOREM IGUAIS, A SAÍDA DO PID ATIGIU SATURAÇÃO
    SE O SINAL DE ERRO (SINAL_ENTRADA) E A SAÍDA SEM LIMITAÇÃO(AUX1) FOREM POSITIVAS...
    O VALOR DO INTEGRADOR CONTINUARÁ A SUBIR MESMO EM SATURAÇÃO
    ENTÃO, CANCELE A INTEGRAÇÃO
    */
    if(((*controlePID->sinal_entrada>0.0)&&(controlePID->sinal_saida_nao_limitada>controlePID->valor_max))||
    ((*controlePID->sinal_entrada<0.0)&&(controlePID->sinal_saida_nao_limitada<controlePID->valor_min))){
        controlePID->anti_windup=1;
    }else{
        controlePID->anti_windup=0;
    }

    //ATUALIZANDO VALOR ANTIGO DA ENTRADA
    *controlePID->entradas_antigas=*controlePID->sinal_entrada;

}

ControladorPID*
Controlador_PID_criar(double *sinal_e,
                      double *sinal_s,
                      double Kp,
                      double Ki,
                      double Kd,
                      double pamostragem,
                      double v_maximo,
                      double v_minimo,
                      int tipo_ctrl)
{


    ControladorPID* controlePID;

    //ALOCANDO MEMÓRIA PARA PONTEIRO DA STRUCT CONTROLADORPID E SUAS VARIÁVEIS
    controlePID=malloc(sizeof(ControladorPID));
    controlePID->entradas_antigas=calloc(ORDEM_APROX,sizeof(double)); //ORDEM_APROX: ORDEM DA APROXIMAÇÃO POR SÉRIE DE TAYLOR

    //ATRIBUINDO ENDEREÇOS DOS SINAIS DE ENTRADA E SAÍDA DO SISTEMA ÀS VARIÁVEIS INTERNAS DA ESTRUTURA
    controlePID->sinal_entrada=sinal_e;
    controlePID->sinal_saida=sinal_s;

    //DEFININDO FUNÇÃO DO CONTROLE COM BASE NAS CONSTANTES NÃO NULAS INFORMADAS
    switch(tipo_ctrl)
    {
        case P:

            //P
            controlePID->func_transf_controle=func_transf_p;

            break;

        case I:

            //I

            //CONSIDERANDO SISTEMA INICIALMENTE RELAXADO
            controlePID->somador=0.0;

            //INICIALIZAÇÃO DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
            controlePID->anti_windup=0;

            break;

        case PD:

            //PD
            controlePID->func_transf_controle=func_transf_pd;

            break;

        case PI:

            //PI
            controlePID->func_transf_controle=func_transf_pi;

            //CONSIDERANDO SISTEMA INICIALMENTE RELAXADO
            controlePID->somador=0.0;

            //INICIALIZAÇÃO DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
            controlePID->anti_windup=0;

            break;

        case PID:

            //PID
            controlePID->func_transf_controle=func_transf_pid;

            //CONSIDERANDO SISTEMA INICIALMENTE RELAXADO
            controlePID->somador=0.0;

            //INICIALIZAÇÃO DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
            controlePID->anti_windup=0;

            break;

        default:

            //Controlador padrão, caso haja erro no valor de entrada: PID

            //PID
            controlePID->func_transf_controle=func_transf_pid;

            //CONSIDERANDO SISTEMA INICIALMENTE RELAXADO
            controlePID->somador=0.0;

            //INICIALIZAÇÃO DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
            controlePID->anti_windup=0;


    }

    //CONFIGURANDO CONTROLE
    controlePID->kp=Kp;
    controlePID->ki=Ki;
    controlePID->kd=Kd;
    controlePID->period_amost=pamostragem;
    controlePID->valor_max=v_maximo;
    controlePID->valor_min=v_minimo;

    //CONSTANTES AUXILIARES
    controlePID->kd_sobre_pamost=controlePID->kd/controlePID->period_amost;
    controlePID->ki_vezes_pamost_sobre_dois=controlePID->ki*controlePID->period_amost*0.5;

    //VARIÁVEIS AUXILIARES
    controlePID->sinal_saida_nao_limitada=0.0;

    return controlePID;

}

void
ControladorPID_executar(ControladorPID* controlePID)
{
    //REALIZANDO CÁLCULOS
    controlePID->func_transf_controle(controlePID);
}

void
ControladorPID_destruir(ControladorPID *controlePID)
{
    //DESALOCANDO MEMÓRIA
    free(controlePID);
}

void
ControladorPID_resetar(ControladorPID *controlePID)
{
    //Zerando variáveis de saída
    *controlePID->sinal_saida=0.0;

    //Zerando entradas antigas
    *controlePID->entradas_antigas=0.0;

    //Zerando somador
    controlePID->somador=0.0;
}

