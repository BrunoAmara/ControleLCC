/*
 * mmq.c
 *
 *  Criado: 01/12/2020
 *      Autor: Yuri Couto
 *		Grupo de trabalho: Nitee - UFF
 *	Descri��o:
 *		Cont�m a defini��o de tipos, vari�veis e fun��es do MMQ.
 *		Este arquivo � propriedade intelectual do Nitee e n�o deve ser
 *		divulgado em texto pleno a terceiros. Detalhes confidenciais
 *		de projeto devem ficar neste arquivo, bem como vari�veis e
 *		fun��es que n�o devem ser acessadas pelo usu�rio.
 *
 *	�ltima atualiza��o: 30/05/2020
 */

#include "mmq.h"
#define DIM_MATRIX_AUX 2
//numero de harmonicos estudados
#define COMPS_SINAL 1

//declara��o da fun�ao de multiplica��o
static void matriz_multiplicacao(
	float** matriz_esquerda,
	float** matriz_direita,
	float** matriz_saida,
	int linha_esquerda,
	int linha_direita,
	int coluna_direita
	);

//fun��o para achar o determinante de uma matriz
static float determinante(
	float** matriz_entrada,
	int dimensao,
	float** elem_det
	);

static void cofator(
	float** matriz_entrada,
	int dimensao,
	float** elem_det,
	float** matriz_saida
);

static void transposta(
	float** matriz_entrada,
	int linha,
	int coluna,
	float** matriz_saida
);

static void inversa(
	float** matriz_entrada,
	float** fatorial,
	float** elem_det,
	int dimensao,
	float** matriz_saida
);

/*
	Descricao das variaveis da struct MMQ:
		i_entrada: recebe uma amostra de corrente a cada iteracao
		estimativas: recebe uma estimacao mmq a cada iteracao
		matriz_pinvS: matriz pseudo inversa de S com dimensao n_amost x
2*n_harmonicos
		matriz_pSRS: matriz precalculada da pseudo inversa de S com [R]*[S]
		I_ant: matriz com as estimacoes mmq anteriores
		I_aux: matriz auxiliar para receber a multiplicacao de matriz_pinvS com
I_ant
		I: matriz com
*/
struct MMQ
{
	volatile float* i_entrada;
	volatile float* estimativas;
	float** matriz_pinvS;
	float** matriz_pSRS;
	float** I_aux;
	float*** I_ant;
	float** I;
};

MMQ* MMQ_criar(
	volatile float* i_amost,
	volatile float* estimativa,
	float f_nominal,
	float f_amostragem,
	int n_amostras
)
{
    // Alocacao de memoria para o objeto de struct mmq
	MMQ* mmq = (MMQ*)malloc(sizeof(MMQ));
	//Se nao for possivel alocar memoria, retornar NULL
	if (mmq == NULL) return NULL;

	//Inicializa��o de vari�veis
	// Variaveis de iteracao
	int l=0, k=0;
	// Matrizes utilizados no processo
	float** matriz_R;
	float** matriz_RS;
	float** matriz_transposta;
	float** fatorial;
	float** StxS;
	float** elemento_det;
	float** matriz_inversa;
	// Vetor indicando a ordem das componentes do sinal(1=fundamental)
	float comp_sinal[COMPS_SINAL];
	// Velocidade angular da rede
	float velocidade_ang;

	// Atribuindo valores para as vari�veis
	comp_sinal[0] = 1.0f;
	velocidade_ang = CTE_2PI*f_nominal;

	// Associando os enderecos de memorias das variaveis com ponteiros
	// da struct MMQ
	mmq->estimativas = estimativa;
	mmq->i_entrada = i_amost;

	// Alocacao de memoria para cada linha das matrizes(ponteiros de ponteiro)
    matriz_transposta = (float**)calloc(DIM_MATRIX_AUX, sizeof(float*));
    mmq->matriz_pSRS = (float**)calloc(DIM_MATRIX_AUX, sizeof(float*));
    matriz_inversa = (float**)calloc(DIM_MATRIX_AUX, sizeof(float*));
    mmq->I_aux = (float**)calloc(DIM_MATRIX_AUX, sizeof(float*));
    elemento_det =(float**)calloc(DIM_MATRIX_AUX,sizeof(float*));
    mmq->I = (float**)calloc(DIM_MATRIX_AUX, sizeof(float*));
    fatorial=(float**)calloc(DIM_MATRIX_AUX,sizeof(float*));
    StxS = (float**)calloc(DIM_MATRIX_AUX, sizeof(float*));
    mmq->matriz_pinvS = (float**)calloc(n_amostras, sizeof(float*));
    matriz_RS = (float**)calloc(n_amostras, sizeof(float*));
    matriz_R = (float**)calloc(n_amostras, sizeof(float*));
    mmq->I_ant = (float***)calloc(N_FASES, sizeof(float**));
	// Se nao for possivel alocar memoria, retornar NULL
    if (matriz_transposta == NULL) return NULL;
    if (mmq->matriz_pinvS == NULL) return NULL;
    if (mmq->matriz_pSRS == NULL) return NULL;
    if (matriz_inversa == NULL) return NULL;
    if (elemento_det == NULL) return NULL;
    if (mmq->I_aux == NULL) return NULL;
    if (mmq->I_ant == NULL) return NULL;
    if (matriz_RS == NULL) return NULL;
    if (matriz_R == NULL) return NULL;
    if (fatorial == NULL) return NULL;
    if (mmq->I == NULL) return NULL;
    if (StxS == NULL) return NULL;

	// Alocacao de memoria para cada coluna das matrizes(ponteiros de ponteiro)
    for(l = 0; l < n_amostras; l++)
    {
        mmq->matriz_pinvS[l] = (float*)calloc(DIM_MATRIX_AUX, sizeof(float));
        matriz_R[l] = (float*)calloc( n_amostras, sizeof(float));
        matriz_RS[l] = (float*)calloc(DIM_MATRIX_AUX, sizeof(float));
        //Se nao for possivel alocar memoria, retornar NULL
        if (mmq->matriz_pinvS[l] == NULL) return NULL;
        if (matriz_R[l] == NULL) return NULL;
        if (matriz_RS[l] == NULL) return NULL;
    }

    for(l=0;l<DIM_MATRIX_AUX;l++)
    {
        matriz_inversa[l] = (float*)calloc(DIM_MATRIX_AUX,sizeof(float));
        matriz_transposta[l] = (float*)calloc(n_amostras,sizeof(float));
        StxS[l] = (float*)calloc(DIM_MATRIX_AUX,sizeof(float));
        elemento_det[l] = (float*)calloc(DIM_MATRIX_AUX,sizeof(float));
        fatorial[l] = (float*)calloc(DIM_MATRIX_AUX,sizeof(float));
        mmq->matriz_pSRS[l] = (float*)calloc(DIM_MATRIX_AUX,sizeof(float));
        mmq->I_aux[l] = (float*)calloc(1, sizeof(float));
        mmq->I[l]=(float*)calloc(N_FASES, sizeof(float));
    //Se nao for possivel alocar memoria, retornar NULL
        if (matriz_inversa[l] == NULL) return NULL;
        if (matriz_transposta[l] == NULL) return NULL;
        if (StxS[l] == NULL) return NULL;
        if (elemento_det[l] == NULL) return NULL;
        if (fatorial[l] == NULL) return NULL;
        if (mmq->matriz_pSRS[l] == NULL) return NULL;
        if (mmq->I_aux[l] == NULL) return NULL;
        if (mmq->I[l] == NULL) return NULL;
    }

    for(l=0;l<N_FASES;l++)
    {
        mmq->I_ant[l]=(float**)calloc(DIM_MATRIX_AUX, sizeof(float));
        if (mmq->I_ant[l] == NULL) return NULL;
        for (k = 0; k < DIM_MATRIX_AUX; ++k)
        {
            mmq->I_ant[l][k]=(float*)calloc(1, sizeof(float));
            if (mmq->I_ant[l][k] == NULL) return NULL;
        }
    }

	// Atribuindo valores para outras variaveis
    for(l = 0; l<N_FASES; l++)
    {
        mmq->i_entrada[l] = 0.0f;
        mmq->estimativas[l] = 0.0f;
        mmq->I_ant[l][0][0]=1.0f;
        mmq->I_ant[l][1][0]=0.0f;
    }

	// Modelagem da matriz S (utilizando o nome pinvS para reduzir variaveis
	// utilizadas no processo global)
    for ( l = 1; l < n_amostras + 1; l++ )
    {
        for (k=0;k<COMPS_SINAL;k++)
        {
            mmq->matriz_pinvS[l-1][2*k]=
                cosf(l*comp_sinal[k]*velocidade_ang*(1/f_amostragem));
            mmq->matriz_pinvS[l-1][2*k+1]=
                sinf(l*comp_sinal[k]*velocidade_ang*(1/f_amostragem));
        }
    }

    // Calculo da transposta da matriz S
	transposta(mmq->matriz_pinvS, n_amostras,DIM_MATRIX_AUX, matriz_transposta);

	// Calculo da matriz StxS
	matriz_multiplicacao(matriz_transposta, mmq->matriz_pinvS, StxS,
		DIM_MATRIX_AUX, n_amostras, DIM_MATRIX_AUX);

	// Calculo da inversa de StxS
	inversa(StxS, fatorial, elemento_det,
		DIM_MATRIX_AUX, matriz_inversa);

	// Pre-calculo da matriz RS (matriz de rotacao R)
	for (l=0;l< n_amostras;l++)
	{
	    for (k=0;k< n_amostras;k++)
	    {
	        if (l==k+1)
	        {
	            matriz_R[l][k]=1.0f;
	        }
	    }
	}
	// Multiplicando [R]_nxn por [S]_nx2
	matriz_multiplicacao(matriz_R,mmq->matriz_pinvS, matriz_RS,
		n_amostras, n_amostras,DIM_MATRIX_AUX);

	//Recriando a matriz_S com dimens�o nova para o ultimo calculo da pseudoinversa
	for(l = 0; l <  n_amostras; l++)
	{
		free(mmq->matriz_pinvS[l]);
	}
	free(mmq->matriz_pinvS);

	mmq->matriz_pinvS = (float**)calloc(DIM_MATRIX_AUX, sizeof(float*));
	//Se nao for possivel alocar memoria, retornar NULL
    if (mmq->matriz_pinvS == NULL) return NULL;
	
    for(l=0;l<DIM_MATRIX_AUX;l++)
    {
		mmq->matriz_pinvS[l]=(float*)calloc( n_amostras, sizeof(float));
		//Se nao for possivel alocar memoria, retornar NULL
        if (mmq->matriz_pinvS[l] == NULL) return NULL;
    }
    // Multiplicando [StS]^-1_2x2 por [S]^t_2xn
	matriz_multiplicacao(matriz_inversa, matriz_transposta, mmq->matriz_pinvS,
		DIM_MATRIX_AUX, DIM_MATRIX_AUX, n_amostras);

	// Multiplicando [pinv(S)]_2xn por [RS]_nx2 para obter a matriz modelada
	// do algoritmo MMQ recursivo (pSRS)
	matriz_multiplicacao(mmq->matriz_pinvS,matriz_RS, mmq->matriz_pSRS,
		DIM_MATRIX_AUX, n_amostras,DIM_MATRIX_AUX);

	// Liberando memoria dos ponteiros da estrutura q nao serao utilizados mais
	for(l = 0; l < DIM_MATRIX_AUX; l++)
	{
		free(matriz_inversa[l]);
		free(StxS[l]);
		free(matriz_transposta[l]);
		free(fatorial[l]);
		free(elemento_det[l]);
	}
	free(matriz_inversa);
	free(matriz_transposta);
	free(StxS);
	free(fatorial);
	free(elemento_det);

	for(l=0;l< n_amostras;l++)
	{
		free(matriz_R[l]);
		free(matriz_RS[l]);
	}
	free(matriz_R);
	free(matriz_RS);

	return mmq;
}

// Definicao da funcao de multiplicacao de matrizes
void matriz_multiplicacao(
    float** matriz_esquerda,
    float** matriz_direita,
    float** matriz_saida,
    int linha_esquerda,
    int linha_direita,
    int coluna_direita
    )
{
    //declara��o de vari�veis locais
    int l1=0,l2=0,k=0;
    float soma=0.0f;

    //looping de multiplicacao
    for (l1 = 0; l1 < linha_esquerda; ++l1)
    {
        for (k = 0; k < coluna_direita; ++k)
        {
            soma=0.0f;
            for(l2=0; l2 < linha_direita; ++l2)
            {
                 soma += matriz_esquerda[l1][l2]*(matriz_direita[l2][k]);
            }
            matriz_saida[l1][k]=soma;
        }
    }
}

// Definicao da funcao para calculo de determinante
float determinante(
    float** matriz_entrada,
    int dimensao,
    float** elem_det)
{
    float s = 1.0f, det = 0.0f;
    int l, k, c, linha,coluna;
    float** aux;
    aux=elem_det;

    if (dimensao == 1)
    {
        return (matriz_entrada[0][0]);
    }
    else
    {
        det = 0.0f;
        for (c = 0; c < dimensao; c++)
        {
            linha = 0;
            coluna = 0;
        for (l = 0;l < dimensao; l++)
        {
            for (k = 0 ;k < dimensao; k++)
            {
                elem_det[l][k] = 0.0f;
                if (l != 0 && k != c)
                {
                    elem_det[linha][coluna] = matriz_entrada[l][k];
                    if (coluna < (dimensao - 2))
                    {
                        coluna++;
                    }
                    else
                    {
                        coluna = 0;
                        linha++;
                    }
                }
            }
        }
            det = det + s * (matriz_entrada[0][c] *
                              determinante(elem_det,dimensao-1,aux));
            s = -1.0f * s;
        }
    }
    return (det);
}

// Definicao da funcao para calculo do cofator das matrizes
void cofator(
             float** matriz_entrada,
             int dimensao,
             float** elem_det,
             float** matriz_saida
             )
{
 float** aux=elem_det;
 int c, l1, l2, linha, coluna, k;

    for (l1 = 0;l1 < dimensao; l1++)
    {
        for (c = 0;c < dimensao; c++)
        {
            linha = 0;
            coluna = 0;
            for (l2 = 0;l2 < dimensao; l2++)
            {
                for (k = 0;k < dimensao; k++)
                {
                    if (l2 != l1 && k != c)
                    {
                        elem_det[linha][coluna] = matriz_entrada[l2][k];
                        if (coluna < (dimensao - 2))
                            coluna++;
                        else
                        {
                            coluna = 0;
                            linha++;
                        }
                    }
                }
            }
            matriz_saida[l1][c]=pow(-1.0f,l1+c)*determinante(
                                                        elem_det,
                                                        dimensao-1,
                                                        aux
                                                        );
        }
    }
}

// Definicao da funcao para calculo de transposta de matrizes
void transposta(
                float** matriz_entrada,
                int linha,
                int coluna,
                float** matriz_saida
                )
{
    int l,k;
    //C�lculo da transposta
    for (l=0;l<coluna;l++)
    {
        for (k=0;k<linha;k++)
        {
            matriz_saida[l][k]=matriz_entrada[k][l];
        }
    }
}

// Definicao da funcao para calculo da inversa das matrizes
void inversa(
             float** matriz_entrada,
             float** fatorial,
             float** elem_det,
             int dimensao,
             float** matriz_saida
             )
{
    int l, k;
    float** aux=fatorial;
    float det;

    //c�lculo do determinante
    det = determinante(matriz_entrada,dimensao,elem_det);

    if (det == 0)
        return;
    else
    //calculo da matriz cofator
        cofator(matriz_entrada, dimensao,elem_det,fatorial);
    //calculo da matriz inversa
    transposta(fatorial,dimensao,dimensao,aux);

    for (l = 0;l < dimensao; l++)
    {
        for (k = 0;k < dimensao; k++)
        {
            matriz_saida[l][k] = aux[l][k] / det;
        }
    }
}

// Definicao da funcao que executa o algoritmo mmq
void MMQ_computa(MMQ* mmq)
{
    for(int k = 0; k < N_FASES; k++)
    {
    // Executando a multiplicacao de matrizes [pinv(S)RS]_2x2 por [I_ant]_2x1 e
    //armazenando o resultado em uma matriz auxiliar (I_aux)
        matriz_multiplicacao(mmq->matriz_pSRS, mmq->I_ant[k], mmq->I_aux,
                            DIM_MATRIX_AUX, DIM_MATRIX_AUX, 1);

    // Somando a nova amostra multiplicada por pinv(S) a matriz I_aux
    // para obter Ic e Is estimados
        mmq->I[0][k]=mmq->I_aux[0][0]+mmq->matriz_pinvS[0][0]*(mmq->i_entrada[k]);
        mmq->I[1][k]=mmq->I_aux[1][0]+mmq->matriz_pinvS[1][0]*(mmq->i_entrada[k]);

    // M�dulo entre I_S e I_C para estimar a amplitude de corrente
        mmq->estimativas[k] = sqrtf(mmq->I[0][k] * (mmq->I[0][k]) +
            mmq->I[1][k] * (mmq->I[1][k]));
    // Armazenando as estimacoes anteriores
        mmq->I_ant[k][0][0] = mmq->I[0][k];
        mmq->I_ant[k][1][0] = mmq->I[1][k];
    }
}

// Definicao da funcao que libera espaco das variaveis alocaveis do algoritmo
void MMQ_destroi(MMQ* mmq)
{
    for(int l = 0; l < DIM_MATRIX_AUX; l++)
    {
        for(int k=0;k<N_FASES;k++)
            free(mmq->I_ant[k][l]);

        free(mmq->I_aux[l]);
        free(mmq->matriz_pinvS[l]);
        free(mmq->matriz_pSRS[l]);
        free(mmq->I[l]);
    }

    free(mmq->matriz_pinvS);
    free(mmq->matriz_pSRS);
    free(mmq->I_aux);
    free(mmq->I_ant);
    free(mmq->I);
    free(mmq);
}
