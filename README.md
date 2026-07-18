# Sistema de Monitoramento e Controle Volumétrico Inteligente com ESP32-S3
Este projeto consiste em um sistema automatizado e residencial/industrial de monitoramento de nível de líquido e automação de bombeamento utilizando o microcontrolador ESP32-S3. O sistema conta com sinalização visual por LEDs, alertas sonoros por bipes em frequências distintas para estados críticos, acionamento seguro de bomba periférica via módulo relé e um painel web dinâmico e responsivo acessível via IP local.
## 🚀 Funcionalidades
*   **Monitoramento em Tempo Real:** Leitura de sensores de nível configurados com resistores de pulldown internos.
*   **Controle Dinâmico de LEDs:** 
    *   **Nível Mínimo (25%):** O LED Verde acende por 1s, pisca em sincronia com o alarme acústico de escassez e retorna a ficar aceso fixo em loop.
    *   **Nível Médio (50%):** LEDs Verde e Amarelo acesos fixos simultaneamente.
    *   **Nível Máximo (100%):** LEDs Verde e Amarelo acesos fixos; LED Vermelho acende por 1s, pisca em sincronia com a sirene rápida de transbordo e retorna a ficar aceso fixo em loop.
*   **Automação de Bombeamento:** A bomba liga automaticamente ao detectar nível crítico baixo ou tanque vazio, continua ligada no nível médio e desliga imediatamente ao atingir o nível máximo.
*   **Painel Web Premium:** Interface moderna em Português (BR) com efeito visual de ondulação da água, status dinâmico colorido/piscante, monitoramento de tensão (5.1V fictício) e botão para controle manual da bomba.
*   **Título Editável via Web:** Ícone de lápis integrado ao cabeçalho que permite renomear o painel diretamente pelo navegador, salvando a string em tempo real na memória do ESP32-S3.
## 🛠️ Materiais Utilizados

| Componente | Quantidade | Descrição |
| :--- | :--- | :--- |
| **ESP32-S3 Dev Module** | 1 | Microcontrolador principal com Wi-Fi nativo e suporte a USB CDC. |
| **Módulo Relé (4 canais)** | 1 | Utilizado para isolar e chavear a potência da bomba com segurança. |
| **Mini Bomba d'água de Diafragma** | 1 | Atuador responsável pelo enchimento do recipiente. |
| **Bateria de Lítio (18650)** | 1 | Fonte de alimentação externa dedicada para a bomba de água. |
| **Alto-falante / Buzzer** | 1 | Emissor acústico conectado ao pino correspondente para alertas sonoros. |
| **LED Difuso Verde (5mm)** | 1 | Indicador visual de Nível Mínimo / Funcionamento básico. |
| **LED Difuso Amarelo (5mm)** | 1 | Indicador visual de Nível Médio. |
| **LED Difuso Vermelho (5mm)** | 1 | Indicador visual de Nível Máximo / Alerta Crítico. |
| **Sensores de Contato Mecânico** | 3 | Hastes/fios para fechamento de contato elétrico com o fluido. |
| **Cabos Jumpers e Protoboard** | 1 | Estrutura para conexões e testes de bancada. |

## 🔧 Configuração da IDE do Arduino
Para garantir o correto funcionamento do monitor serial e gravação estável no ESP32-S3, utilize as seguintes configurações no menu **Ferramentas**:
*   **Placa:** "ESP32S3 Dev Module"
*   **USB CDC On Boot:** "Enabled"