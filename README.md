# Sistema de Laser Tag com ESP32
Projeto de um sistema básico de Laser Tag utilizando ESP32, controle remoto IR, LCD 16x2 (I2C) e LEDs indicadores para simular disparos, vida e munição de um jogador.

## Funcionalidades
Comandos via controle remoto IR (atirar e recarregar)

Sistema de vida com LEDs:

 Verde: vida cheia

 Amarelo: vida média

 Vermelho: vida baixa

Display LCD mostra munição e vida

Cooldown entre disparos e tempo de recarga

Mensagem de "Derrota..." ao zerar a vida

## Componentes
ESP32

Receptor IR (38kHz)

Controle remoto IR

Display LCD 16x2 (com módulo I2C)

LEDs (verde, amarelo, vermelho)

Resistores

## Lógica do Código
setup(): Inicializa componentes

loop(): Escuta comandos IR

shoot(): Dispara e atualiza vida/munição

reload(): Recarrega munição

updateLCD() e updateLEDs(): Atualizam o feedback visual

## Testes
Simulado no Wokwi

Testes de IR, cooldown, recarga, LEDs e LCD validados

## Melhorias Futuras
Suporte a múltiplos jogadores

Sistema de pontuação

Efeitos sonoros com buzzer

Modo multiplayer via Wi-Fi

## Autores
Adrian Lauzid Modesto
Artur Vinícius Lima Ramos da Silva
Gustavo dos Santos Silva
Lucas Pereira de Souza
