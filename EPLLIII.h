/*
    Arquivo: EPLLII.h

    Criado: 21/05/2021

    Autor(es):

    Bruno Amaral (amaral_bruno@id.uff.br)
    Código destinado ao trabalho de conclusão de curso

    Última atualização: 21/05/2021 - Feita por: Bruno Amaral (amaral_bruno@id.uff.br)

    Descrição da atualização:

	Descrição do código:

	Implementação do EPLL III descrito no livro do Ghartemani.

	Uso:

	1) Declarar ponteiro para a estrutura EPLL
	2) Alocar memória para a estrutura por meio da funçõa EPLL_criar
	3) Invocar função EPLL_executar a cada amostragem para executar
	a malha do EPLL III
*/

#ifndef MOD_EPLLIII
#define MOD_EPLLIII

//Definição de tipo visível ao usuário
typedef struct eplliii EPLLIII;

//Protótipos das funções da biblioteca

/*
    Nome da função: EPLLIII_criar

    Descrição:

    Aloca memória para um ponteiro do tipo EPLLII*

    Tipo de retorno: EPLLIII*

    Argumentos:

    double *sinais_entrada: sinais elétricos de entrada
    double *amplitudes_estimadas_saida: ponteiro para amplitudes estimadas pelo EPLL. 3 posições
    double *fases_estimadas_saida: ponteiro para fases angulares estimadas. 3 posições
    double *frequencia_estimada_saida: ponteiro para frequência angular estimada pelo EPLLIII. 1 posição
    double *sinais_estimados_saida: ponteiro para componentes fundamentais estimadas dos sinais de entrada. 3 posições
    double amplitude_base: amplitude nominal de fase (pico) dos sinais de entrada
    double frequencia_base: frequência nominal do sistema em Hz
    double frequencia_amostragem: frequência de amostragem do código em Hz
    double ganho_um: ganho mi 1 para o núcloe do EPLL
    double ganho_dois: ganho mi 2 para a estimação de frequência principal
    double ganho_tres: ganho mi 3 para o núcleo do EPLL
*/

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
               double ganho_tres);

/*
    Nome da função: EPLL_executar

    Descrição:

    Executa o algoritmo do EPLL

    Tipo de retorno: void

    Argumentos:

    NucleoEPLL *meuEPLL: ponteiro para uma estrutura EPLL
*/

void
EPLLIII_executar (EPLLIII *meuEPLL);

/*
    Nome da função: EPLL_reparametrizar

    Descrição:

    Reparametriza o algoritmo do EPLL

    Tipo de retorno: EPLLIII*

    Argumentos:

    NucleoEPLL *meuEPLL: ponteiro para uma estrutura EPLL
    double amplitude_base: amplitude nominal de fase (pico) dos sinais de entrada
    double frequencia_base: frequência nominal do sistema em Hz
    double frequencia_amostragem: frequência de amostragem do código em Hz
    double ganho_um: ganho mi 1 para o núcloe do EPLL
    double ganho_dois: ganho mi 2 para a estimação de frequência principal
    double ganho_tres: ganho mi 3 para o núcleo do EPLL
*/

void
EPLLIII_reparametrizar(double amplitude_base,
                       double frequencia_base,
                       double frequencia_amostragem,
                       double ganho_um,
                       double ganho_dois,
                       double ganho_tres,
                       EPLLIII *meuEPLL);

/*
    Nome da função: EPLL_destruir

    Descrição:

    Desaloca memória de uma estrutura do tipo EPLL*

    Tipo de retorno: void

    Argumentos:

    NucleoEPLL *meuEPLL: ponteiro para uma estrutura do tipo EPLL*
*/

void
EPLLIII_destruir (EPLLIII *meuEPLL);

#endif // MOD_EPLLIII
