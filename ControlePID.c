#include "ControlePID.h"

//DEFININDO STRUCT CONTROLADORPID
struct ControladorPID{

    //PAR�METROS DO CONTROLE E VETORES DE ENTRADA E SA�DA DE SINAL
    double kp;
    double ki;
    double kd;
    double period_amost;
    double somador; //INTEGRAL
    double *sinal_entrada;
    double *sinal_saida;
    double *entradas_antigas; //DEVIDO AO BLOCO INTEGRATIVO E  AO DERIVATIVO

    //VALORES LIMITANTES PARA A SA�DA DO CONTROLADOR
    double valor_max;
    double valor_min;
    int anti_windup;

    //VARI�VEIS AUXILIARES
    double sinal_saida_nao_limitada;

    //CONSTANTES PARA AUXILIAR NOS C�LCULOS
    double kd_sobre_pamost;
    double ki_vezes_pamost_sobre_dois;

    //PONTEIRO RESPONS�VEL POR IMPLEMENTAR O CONTROLE EM TEMPO DISCRETO
    void (*func_transf_controle)(ControladorPID*);
};

//DEFINI��ES DAS FUN��ES
void
func_transf_p(ControladorPID *controlePID)
{

    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada);

    //LIMITANDO AMPLITUDE DA SA�DA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

}

void
func_transf_i(ControladorPID *controlePID)
{

    //ATUALIZANDO VALOR DO SOMADOR(CALCULANDO A INTEGRAL PELO M�TODO TRAPEZOIDAL)
    if(controlePID->anti_windup!=1){
        controlePID->somador=controlePID->somador
        +controlePID->ki_vezes_pamost_sobre_dois*((*controlePID->sinal_entrada)+(*controlePID->entradas_antigas));
    }

    //CALCULANDO SA�DA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->somador;

    //VARI�VEL AUXILIAR RECEBE VALOR DA SA�DA SEM A LIMITA��O
    controlePID->sinal_saida_nao_limitada=*controlePID->sinal_saida;

    //LIMITANDO AMPLITUDE DA SA�DA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

    /*SE AUX1 E AUX2 FOREM IGUAIS, A SA�DA DO PID ATIGIU SATURA��O
    SE O SINAL DE ERRO (SINAL_ENTRADA) E A SA�DA SEM LIMITA��O(AUX1) FOREM POSITIVAS...
    O VALOR DO INTEGRADOR CONTINUAR� A SUBIR MESMO EM SATURA��O
    ENT�O, CANCELE A INTEGRA��O
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

    //ATUALIZANDO VALOR DO SOMADOR(CALCULANDO A INTEGRAL PELO M�TODO TRAPEZOIDAL)
    if(controlePID->anti_windup!=1){
        controlePID->somador=controlePID->somador
        +controlePID->ki_vezes_pamost_sobre_dois*((*controlePID->sinal_entrada)+(*controlePID->entradas_antigas));
    }

    //CALCULANDO SA�DA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada)+controlePID->somador;

    //VARI�VEL AUXILIAR RECEBE VALOR DA SA�DA SEM A LIMITA��O
    controlePID->sinal_saida_nao_limitada=*controlePID->sinal_saida;

    //LIMITANDO AMPLITUDE DA SA�DA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

    /*SE AUX1 E AUX2 FOREM IGUAIS, A SA�DA DO PID ATIGIU SATURA��O
    SE O SINAL DE ERRO (SINAL_ENTRADA) E A SA�DA SEM LIMITA��O(AUX1) FOREM POSITIVAS...
    O VALOR DO INTEGRADOR CONTINUAR� A SUBIR MESMO EM SATURA��O
    ENT�O, CANCELE A INTEGRA��O
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

    //CALCULANDO SA�DA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada)
    +controlePID->kd_sobre_pamost*((*controlePID->sinal_entrada)-(*controlePID->entradas_antigas));

    //LIMITANDO AMPLITUDE DA SA�DA
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

    //ATUALIZANDO VALOR DO SOMADOR(CALCULANDO A INTEGRAL PELO M�TODO TRAPEZOIDAL)
    if(controlePID->anti_windup!=1){
        controlePID->somador=controlePID->somador
        +controlePID->ki_vezes_pamost_sobre_dois*((*controlePID->sinal_entrada)+(*controlePID->entradas_antigas));
    }

    //CALCULANDO SA�DA DA MALHA DE CONTROLE
    *controlePID->sinal_saida=controlePID->kp*(*controlePID->sinal_entrada)
    +controlePID->kd_sobre_pamost*((*controlePID->sinal_entrada)-(*controlePID->entradas_antigas))
    +controlePID->somador;

    //VARI�VEL AUXILIAR RECEBE VALOR DA SA�DA SEM A LIMITA��O
    controlePID->sinal_saida_nao_limitada=*controlePID->sinal_saida;

    //LIMITANDO AMPLITUDE DA SA�DA
    if((*controlePID->sinal_saida)>controlePID->valor_max){
        *controlePID->sinal_saida=controlePID->valor_max;
    }else if((*controlePID->sinal_saida)<controlePID->valor_min){
        *controlePID->sinal_saida=controlePID->valor_min;
    }

    /*SE AUX1 E AUX2 FOREM IGUAIS, A SA�DA DO PID ATIGIU SATURA��O
    SE O SINAL DE ERRO (SINAL_ENTRADA) E A SA�DA SEM LIMITA��O(AUX1) FOREM POSITIVAS...
    O VALOR DO INTEGRADOR CONTINUAR� A SUBIR MESMO EM SATURA��O
    ENT�O, CANCELE A INTEGRA��O
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

    //ALOCANDO MEM�RIA PARA PONTEIRO DA STRUCT CONTROLADORPID E SUAS VARI�VEIS
    controlePID=malloc(sizeof(ControladorPID));
    controlePID->entradas_antigas=calloc(ORDEM_APROX,sizeof(double)); //ORDEM_APROX: ORDEM DA APROXIMA��O POR S�RIE DE TAYLOR

    //ATRIBUINDO ENDERE�OS DOS SINAIS DE ENTRADA E SA�DA DO SISTEMA �S VARI�VEIS INTERNAS DA ESTRUTURA
    controlePID->sinal_entrada=sinal_e;
    controlePID->sinal_saida=sinal_s;

    //DEFININDO FUN��O DO CONTROLE COM BASE NAS CONSTANTES N�O NULAS INFORMADAS
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

            //INICIALIZA��O DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
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

            //INICIALIZA��O DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
            controlePID->anti_windup=0;

            break;

        case PID:

            //PID
            controlePID->func_transf_controle=func_transf_pid;

            //CONSIDERANDO SISTEMA INICIALMENTE RELAXADO
            controlePID->somador=0.0;

            //INICIALIZA��O DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
            controlePID->anti_windup=0;

            break;

        default:

            //Controlador padr�o, caso haja erro no valor de entrada: PID

            //PID
            controlePID->func_transf_controle=func_transf_pid;

            //CONSIDERANDO SISTEMA INICIALMENTE RELAXADO
            controlePID->somador=0.0;

            //INICIALIZA��O DO SISTEMA ANTI-WINDUP DO CONTROLADOR INTEGRAL
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

    //VARI�VEIS AUXILIARES
    controlePID->sinal_saida_nao_limitada=0.0;

    return controlePID;

}

void
ControladorPID_executar(ControladorPID* controlePID)
{
    //REALIZANDO C�LCULOS
    controlePID->func_transf_controle(controlePID);
}

void
ControladorPID_destruir(ControladorPID *controlePID)
{
    //DESALOCANDO MEM�RIA
    free(controlePID);
}

void
ControladorPID_resetar(ControladorPID *controlePID)
{
    //Zerando vari�veis de sa�da
    *controlePID->sinal_saida=0.0;

    //Zerando entradas antigas
    *controlePID->entradas_antigas=0.0;

    //Zerando somador
    controlePID->somador=0.0;
}

