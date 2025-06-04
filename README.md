# Sistema Vestível de Laser Tag com IoT

Este projeto implementa um sistema vestível de Laser Tag utilizando microcontroladores ESP8266, sensores infravermelhos (IR), comunicação Wi-Fi e o protocolo MQTT. Ele foi desenvolvido como parte do Projeto Integrador 6 da Faculdade Nova Roma.

## Visão Geral

O sistema é composto por dois dispositivos ESP8266 com funcionalidades semelhantes:

- **ESP 1 e 2- Coletes (Receptor IR)**: detecta disparos, gerencia a "vida" do jogador e publica os dados da partida via MQTT.

Cada colete possui:
- Sensor IR para detecção de disparos
- LEDs indicadores (verde, amarelo e vermelho)
- Display LCD 16x2 (via I2C) para mostrar o status
- Conexão Wi-Fi com envio de estatísticas via MQTT

## Funcionalidades

- Redução de vida ao receber disparos (via sinal IR)
- Indicação de vida restante com LEDs coloridos:
  - Verde: 7 a 10
  - Amarelo: 4 a 6
  - Vermelho: 1 a 3
  - Todos desligados: vida zerada
- Exibição do status atual no LCD
- Publicação dos dados da partida no broker MQTT:
  - Tópico: `vitorias/azul` ou `vitorias/vermelho`
- Reconexão automática ao Wi-Fi e ao broker MQTT

## Diferença entre os dois ESPs

| Característica         | ESP (Time Azul)                   | ESP (Time Vermelho)                |
|------------------------|-----------------------------------|------------------------------------|
| Código IR de Disparo   | `0xFF9867` (seta para baixo)      | `0xFF02FD` (seta para cima)        |
| Publicação MQTT        | `vitorias/vermelho`               | `vitorias/azul`                    |
| Função principal       | Recebe disparos e publica dados   | Recebe disparos e publica dados    |

Essa diferenciação permite que cada ESP identifique corretamente de qual time são os disparos e contabilize as vitórias separadamente.

## Fluxo de Funcionamento

1. Jogador pressiona o botão do controle remoto
2. Luva emite sinal IR
3. Colete adversário detecta o sinal com sensor IR
4. Vida do jogador atingido é reduzida
5. LCD e LEDs atualizam o status
6. Se a vida chegar a 0:
   - Atualiza número de partidas
   - Publica estatísticas via MQTT

## Componentes Utilizados

- ESP8266 NodeMCU
- Display LCD 16x2 com módulo I2C
- Receptor infravermelho 38kHz
- Controle remoto IR
- LEDs (vermelho, amarelo, verde)
- Resistores de 220Ω para LEDs
- Fonte de alimentação 5V

## Bibliotecas Utilizadas

- `ESP8266WiFi.h` – Conexão Wi-Fi
- `AsyncMqttClient.h` – Cliente MQTT assíncrono
- `IRremoteESP8266.h` – Leitura de sinais IR
- `LiquidCrystal_I2C.h` – Controle do display LCD via I2C
- `Ticker.h` – Gerenciamento de timers para reconexão

## Configurações MQTT

- Broker: IP local do servidor MQTT (exemplo: `192.168.66.163`)
- Porta: `1883`
- Usuário: `Nome de usuário da VM`
- Senha: `Senha da VM`
- Tópicos: `vitorias/azul`, `vitorias/vermelho`

## Visualização de Dados com Node-RED

Para monitorar os resultados das partidas em tempo real, utilizamos o Node-RED para montar um fluxo de dados que recebe informações via MQTT e as exibe em um gráfico.

![image](https://github.com/user-attachments/assets/69a89cc2-9069-43b0-89b6-fbf757ecffd6)

## Explicação dos nós

- `Vítórias Vermelho e Vítórias Azul (nós MQTT IN)`:
Recebem dados publicados pelos ESPs nos tópicos vitorias/vermelho e vitorias/azul.

- `split`:
Separa as mensagens para tratamento individual, permitindo que dados de diferentes tópicos sejam processados corretamente.

- `Tratamento (nó function)`:
Processa os dados recebidos (ex: converte em número, agrupa por time, estrutura os dados para o gráfico).

- `Gráfico Vitórias x Partidas`:
Mostra um gráfico em tempo real com o número de vitórias de cada time, comparando com o total de partidas.

- `Limpar gráfico (nó inject ou botão)`:
Permite reiniciar/limpar os dados exibidos no gráfico.

## Melhorias Futuras

- Implementar sistema de munição limitada e recarga
- Adição de modos de jogo diferentes
- Feedback com som ou vibração

## Autores

Projeto desenvolvido por alunos da Faculdade Nova Roma:

- Adrian Modesto Lauzid  
- Artur Vinícius Lima Ramos da Silva  
- Gustavo dos Santos Silva  
- Lucas Pereira de Souza  

Supervisor: Prof. Cláudio Pereira da Silva
