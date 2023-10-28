# Guarda_Chuva
**Felipe Cortez dos Santos**
*28/10/2023*

## Objetivo
![Logo (1)](https://github.com/Grupo-Arthemis/Sprint_3_Entrega_2_ECCS/assets/84590776/53732870-e1d1-4142-92f1-b8d91198a28c)

Este projeto consiste em um código desenvolvido para o **Pé D’Água**, uma iniciativa que participou do evento **NEXT 2023** em parceria com a **IBM** e a **FIAP**. O objetivo é medir a quantidade de chuva em uma determinada região e enviar os dados para uma plataforma online.

## Projeto
O projeto **Pé D’Água** adota uma abordagem integrada na obtenção e análise de dados de chuva, visando monitorar padrões de precipitação em São Paulo. A implementação abrange a coleta de informações provenientes do sensor Guarda_Chuva e sua integração a um banco de dados centralizado, permitindo o registro e a análise de padrões de chuva em diversas localizações da cidade. A avaliação dos dados se baseia na consideração da quantidade de chuva em milímetros e sua duração, o que contribui para a avaliação do potencial de alagamento.

## Componentes necessários para a funcionalidade do código
Para executar este código, você vai precisar dos seguintes componentes:

- 1 sensor de temperatura e umidade DHT11
- 1 sensor magnético Reed Switch
- 1 placa de desenvolvimento ESP32
- 1 pluviômetro de báscula, que é um dispositivo que mede a precipitação pluviométrica por meio de um pêndulo que se inclina a cada volume de água acumulado.

![image](http://www.c2o.pro.br/proj/pluviometro/figuras/animacao_pluviometro_bascula.gif).

- 1 imã, que é usado para acionar o Reed Switch a cada movimento do pêndulo.

## Funcionalidade
O código é um sistema de contagem de milímetros de chuva, onde cada pêndulo realizado pelo pluviômetro tem um valor X que é adicionado a uma variável que armazena o total de chuva acumulada em um período definido pelo usuário.

O código também conecta o ESP32 à internet e envia todas as informações necessárias para o Tago.io, uma plataforma que permite visualizar e analisar os dados coletados pelo sensor.

*O código está comentado e pronto para ser modificado de acordo com a preferência do usuário*
