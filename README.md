# Trabalho de conclusão de curso - Universidade Federal Fluminense

Este é um repositório para os documentos e código fonte do meu projeto de TCC.

## Descrição do projeto

Este é um repositório para guardar os códigos de controle do Limitador de Corrente de Curto-circuito modelado no
programa PSCAD/EMTDC. A proposta do algoritmo é controlar as chaves do equipamento de forma que torne possível
a modulação da corrente limitada. Para isso, uma malha fechada de controle foi implementada usando a linguagem
C. A malha consiste de um subsistema para estimação das amplitudes das correntes do sistema, chamado de EPLLIII, e um
para a compensação dinâmica, denominado ControlePID, que implementa as equações dinâmicas do controle
 proporcional-integral-derivativo. As estimações de amplitude são utilizadas para a detecção do curto-circuito. Caso ele seja
detectado, uma subrotina para modulação de corrente entra. Nela, a amplitude estimada pelo módulo EPLLIII é comparada com uma
amplitude de referência e o erro gerado é usado como entrada do compensador (controlador PI). O controlador PI é responsável
por gerar um sinal de referência para o PWM, que por sua vez criará os sinais de acionamento para cada conjunto de chaves do 
equipamento.
