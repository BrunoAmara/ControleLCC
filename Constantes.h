/*
 * Constantes.h
 *
 *  Criado: 21 de agosto de 2020
 *  Autores: 
 *      Guilherme Scofano (guilhermescofano@id.uff.br)
 *      Bruno Barbosa do Amaral (amaral_bruno@id.uff.br)
 *  Grupo de trabalho: NITEE - UFF
 *
 *  Última atualização: 23/07/2020
 *
 *  Descrição:
 *      Contém definição de constantes pré-calculadas e nomes
 *      padronizados de offsets de endereços.
 *
 *  Uso:
 *      Basta incluir a biblioteca no código e chamar os nomes
 *      definidos. Evitar criar nomes com a mesma funcionalidade,
 *      ou realizar os cálculos novamente sem necessidade. A operação
 *      que resulta em cada constante está comentada ao lado do seu
 *      valor.
 */



/*
 *  Constantes pré-calculadas
 */
#define FALSE   0
#define TRUE    1

#define CTE_PI              3.141592654f    //pi
#define CTE_PI_2            1.570796327f    //pi
#define CTE_2PI             6.283185307f    //2pi
#define CTE_2PI_3           2.094395102f    //(2pi/3)
#define CTE_4PI_3           4.188790205f    //(4pi/3)
#define CTE_1_RAIZ_2        0.707106781f    //1/sqrt(2)
#define CTE_1_RAIZ_3        0.577350269f    //1/(sqrt(3))
#define CTE_1_RAIZ_6        0.408248290f    //1/(sqrt(6))
#define CTE_2_RAIZ_3        1.154700538f    //1/(sqrt(6))
#define CTE_1_SOBRE_2RAIZ3  0.288675134f    // 1/(2*sqrt(3))
#define CTE_1_SOBRE_3       0.333333333f    //1/3
#define CTE_2_SOBRE_3       0.666666667f    //2/3
#define CTE_RAIZ_2          1.414213562f    //sqrt(2)
#define CTE_RAIZ_3          1.732050807f    //sqrt(3)
#define CTE_RAIZ_2_RAIZ_3   0.816496581f    //sqrt(2/3)
#define CTE_RAIZ_2_3        0.471404521f    //sqrt(2)/3

/*
 *  Offsets de memória, para identificar as componentes de um quadro
 *  em um vetor.
 */
#define FASE_A          0x00
#define FASE_B          0x01
#define FASE_C          0x02
#define FASE_AB         0x00
#define FASE_BC         0x01
#define FASE_CA         0x02
#define N_FASES         0x03
#define COMP_SEQ_POS    0x00
#define COMP_SEQ_NEG    0x01
#define COMP_SEQ_Z      0x02
#define EDR_FASE_A      0x00 //Endere�o Fase A
#define EDR_FASE_B      0x01 //Endere�o Fase B
#define EDR_FASE_C      0x02 //Endere�o Fase C
#define A_ADDR_OFFSET   0x00
#define B_ADDR_OFFSET   0x01
#define C_ADDR_OFFSET   0x02
#define AB_ADDR_OFFSET  0x00
#define BC_ADDR_OFFSET  0x01
#define CA_ADDR_OFFSET  0x02
