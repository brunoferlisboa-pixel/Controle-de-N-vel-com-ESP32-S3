#include <WiFi.h>
#include <WebServer.h>

// Configurações do Wi-Fi
const char* ssid = "nome da sua rede wifi";
const char* password = "senha da rede";

// Inicialização do Servidor Web na porta 80
WebServer server(80);

// Definição dos pinos dos Sensores (Entradas)
const int pinoNivel1 = 4;
const int pinoNivel2 = 5;
const int pinoNivelMax = 6;

// Definição dos pinos das Saídas (LEDs e Buzzer)
const int led1 = 7;             // LED Verde (Nível Baixo)
const int led2 = 15;            // LED Amarelo (Nível Médio)
const int ledMax = 16;          // LED Vermelho (Nível Máximo)
const int pinoAltoFalante = 17; // Pino do alto-falante (fio cinza)

// Variáveis globais para armazenar os estados do painel web
int porcentagemAtual = 0;
float metrosAtual = 0.0;
String statusAtual = "Vazio";
String nomePainel = "Painel Controle de Nível"; // Nome editável

void setup() {
  Serial.begin(115200);

  // Configura os sensores com PULLDOWN interno
  pinMode(pinoNivel1, INPUT_PULLDOWN);
  pinMode(pinoNivel2, INPUT_PULLDOWN);
  pinMode(pinoNivelMax, INPUT_PULLDOWN);

  // Configura as saídas
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(ledMax, OUTPUT);
  pinMode(pinoAltoFalante, OUTPUT);

  // Garante que tudo começa desligado
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(ledMax, LOW);
  noTone(pinoAltoFalante);

  // Conexão ao Wi-Fi
  Serial.println();
  Serial.print("Conectando a rede: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Conectado!");
  Serial.print("Endereço IP para acessar o painel: http://");
  Serial.println(WiFi.localIP());

  // Configuração das rotas do servidor web
  server.on("/", enviarPaginaWeb);
  server.on("/dados", enviarDadosJSON);
  
  // Rota para salvar o novo nome editado pelo lápis
  server.on("/salvarNome", []() {
    if (server.hasArg("valor")) {
      nomePainel = server.arg("valor");
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Erro");
    }
  });

  server.begin();
  Serial.println("Sistema de Monitoramento com Alertas Visuais e Web Ativo!");
}

// Função de atraso inteligente que mantém o servidor web rodando sem travar
void aguardar(unsigned long ms) {
  unsigned long tempoInicio = millis();
  while (millis() - tempoInicio < ms) {
    server.handleClient(); // Processa requisições web continuamente
    delay(1);
  }
}

// Rota principal: Envia a página web premium em Português
void enviarPaginaWeb() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>" + nomePainel + "</title>";
  html += "<style>";
  html += "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; background-color: #f5f8fa; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; color: #2c3e50; }";
  html += ".card { background: white; width: 100%; max-width: 380px; border-radius: 30px; padding: 30px; box-shadow: 0 15px 35px rgba(0,0,0,0.06); text-align: center; position: relative; border: 1px solid #eef2f5; }";
  html += ".header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 25px; font-weight: 600; color: #5a6e7f; font-size: 16px; }";
  html += ".header-title { flex-grow: 1; text-align: center; color: #333; font-weight: bold; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; padding: 0 10px; }";
  html += ".pencil-btn { cursor: pointer; color: #5a6e7f; transition: color 0.2s; font-size: 18px; }";
  html += ".pencil-btn:hover { color: #3b82f6; }";
  html += ".grid { display: flex; justify-content: space-between; align-items: center; margin-top: 15px; min-height: 280px; }";
  html += ".metrics { text-align: left; width: 52%; display: flex; flex-direction: column; justify-content: center; }";
  html += ".label { font-size: 11px; color: #9aa8b6; font-weight: 700; margin-bottom: 4px; text-transform: uppercase; letter-spacing: 0.8px; }";
  html += ".value { font-size: 26px; font-weight: bold; color: #1e293b; margin-bottom: 24px; transition: all 0.3s ease; }";
  html += ".tank-container { width: 44%; display: flex; flex-direction: column; align-items: center; position: relative; }";
  html += ".tank { width: 90px; height: 260px; background: #f1f5f9; border-radius: 25px; position: relative; overflow: hidden; border: 2px solid #e2e8f0; }";
  html += ".water { width: 100%; background: linear-gradient(180deg, #3b82f6, #1d4ed8); position: absolute; bottom: 0; transition: height 0.8s cubic-bezier(0.4, 0, 0.2, 1); border-radius: 0 0 22px 22px; display: flex; align-items: center; justify-content: center; overflow: hidden; }";
  
  // Onda animada realista
  html += ".wave { position: absolute; width: 200px; height: 200px; background-color: rgba(255,255,255,0.25); top: -188px; left: -55px; border-radius: 43%; animation: drift 5s infinite linear; pointer-events: none; }";
  html += ".wave-behind { position: absolute; width: 204px; height: 204px; background-color: rgba(255,255,255,0.15); top: -190px; left: -57px; border-radius: 40%; animation: drift 7s infinite linear; pointer-events: none; }";
  
  // Texto dinâmico de porcentagem dentro da água
  html += ".tank-pct-inside { color: white; font-size: 18px; font-weight: bold; z-index: 5; text-shadow: 1px 1px 3px rgba(0,0,0,0.3); transition: opacity 0.3s ease; }";
  
  // Classes CSS do status do nível
  html += ".status-vermelho { color: #ef4444 !important; animation: blinker 1s linear infinite; font-weight: bold; }";
  html += ".status-amarelo { color: #eab308 !important; font-weight: bold; }";
  html += ".status-verde { color: #10b981 !important; font-weight: bold; }";
  html += ".status-vazio { color: #94a3b8 !important; }";
  
  html += "@keyframes drift { from { transform: rotate(0deg); } to { transform: rotate(360deg); } }";
  html += "@keyframes blinker { 50% { opacity: 0; } }";
  html += "</style>";
  html += "<script>";
  
  // Função para editar o nome via caixa de diálogo
  html += "function editarNome() {";
  html += "  let tituloAtual = document.getElementById('titulo-painel').innerText;";
  html += "  let novoNome = prompt('Digite o novo nome para o painel:', tituloAtual);";
  html += "  if (novoNome && novoNome.trim() !== '') {";
  html += "    fetch('/salvarNome?valor=' + encodeURIComponent(novoNome.trim()))";
  html += "      .then(response => {";
  html += "        if (response.ok) {";
  html += "          document.getElementById('titulo-painel').innerText = novoNome.trim();";
  html += "          document.title = novoNome.trim();";
  html += "        }";
  html += "      }).catch(err => console.error('Erro ao salvar nome:', err));";
  html += "  }";
  html += "}";

  html += "function atualizarDados() {";
  html += "  fetch('/dados').then(response => response.json()).then(data => {";
  html += "    document.getElementById('porcentagem').innerText = data.porcentagem + '%';";
  html += "    document.getElementById('metros').innerText = data.metros.toFixed(2) + 'm';";
  
  // Controle de estilos baseado no nível
  html += "    let statusEl = document.getElementById('status');";
  html += "    statusEl.innerText = data.status;";
  html += "    statusEl.className = 'value';"; 
  html += "    let pctInside = document.getElementById('tank-pct-inside');";
  
  html += "    if(data.porcentagem === 0) {";
  html += "      statusEl.classList.add('status-vazio');";
  html += "      pctInside.style.opacity = '0';";
  html += "    } else if(data.porcentagem === 25) {";
  html += "      statusEl.classList.add('status-vermelho');";
  html += "      pctInside.style.opacity = '1';";
  html += "    } else if(data.porcentagem === 50) {";
  html += "      statusEl.classList.add('status-amarelo');";
  html += "      pctInside.style.opacity = '1';";
  html += "    } else if(data.porcentagem === 100) {";
  html += "      statusEl.classList.add('status-verde');";
  html += "      pctInside.style.opacity = '1';";
  html += "    }";
  
  html += "    pctInside.innerText = data.porcentagem + '%';";
  html += "    document.getElementById('water-level').style.height = data.porcentagem + '%';";
  html += "  }).catch(err => console.error('Erro ao atualizar dados:', err));";
  html += "}";
  html += "setInterval(atualizarDados, 1000);"; 
  html += "</script></head><body onload='atualizarDados()'>";
  html += "<div class='card'>";
  html += "  <div class='header'><span style='cursor:pointer;'>&#10094;</span><span class='header-title' id='titulo-painel'>" + nomePainel + "</span><span class='pencil-btn' onclick='editarNome()'>&#9998;</span></div>";
  html += "  <div class='grid'>";
  html += "    <div class='metrics'>";
  html += "      <div class='label'>Proporção do Nível</div><div class='value' id='porcentagem'>--%</div>";
  html += "      <div class='label'>Profundidade do Nível</div><div class='value' id='metros'>--m</div>";
  html += "      <div class='label'>Status do Nível</div><div class='value' id='status'>--</div>";
  html += "      <div class='label'>Tensão de Alimentação</div><div class='value' style='color:#10b981;'>5.1V</div>";
  html += "    </div>";
  html += "    <div class='tank-container'>";
  html += "      <div class='tank'>";
  html += "        <div class='water' id='water-level' style='height: 0%;'>";
  html += "          <div class='wave-behind'></div>";
  html += "          <div class='wave'></div>";
  html += "          <div class='tank-pct-inside' id='tank-pct-inside'>0%</div>";
  html += "        </div>";
  html += "      </div>";
  html += "    </div>";
  html += "  </div>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

// Rota de dados: Envia as informações em formato JSON
void enviarDadosJSON() {
  String json = "{";
  json += "\"porcentagem\":" + String(porcentagemAtual) + ",";
  json += "\"metros\":" + String(metrosAtual) + ",";
  json += "\"status\":\"" + statusAtual + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void loop() {
  // Mantém o servidor web ativo no início do loop
  server.handleClient();

  // Leitura em tempo real dos sensores
  bool nivel1Ativo = digitalRead(pinoNivel1);
  bool nivel2Ativo = digitalRead(pinoNivel2);
  bool nivelMaxAtivo = digitalRead(pinoNivelMax);

  // --- LÓGICA DO NÍVEL MÁXIMO (VERMELHO EM LOOP: ACESO -> PISCANDO -> ACESO) ---
  if (nivelMaxAtivo) {
    Serial.println("ALERTA: Nível Máximo Atingido!");

    porcentagemAtual = 100;
    metrosAtual = 2.50;
    statusAtual = "Nivel Maximo";

    // LEDs Verde e Amarelo ficam acesos fixos
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    
    // 1. LED Vermelho acende fixo por 1 segundo antes de tocar a sirene
    digitalWrite(ledMax, HIGH);
    aguardar(1000); 
    
    // 2. LED Vermelho pisca em sincronia com a sirene rápida
    for (int i = 0; i < 6; i++) {
      digitalWrite(ledMax, HIGH);      // Liga LED Vermelho
      tone(pinoAltoFalante, 1800); 
      aguardar(100);
      
      digitalWrite(ledMax, LOW);       // Desliga LED Vermelho
      tone(pinoAltoFalante, 2200); 
      aguardar(100);
    }
    noTone(pinoAltoFalante);
    
    // 3. LED Vermelho volta a ficar aceso fixo durante o intervalo antes de repetir o loop
    digitalWrite(ledMax, HIGH);
    aguardar(1500); 
  }
  
  // --- LÓGICA DO NÍVEL MÉDIO (VERDE E AMARELO ACESOS FIXOS) ---
  else if (nivel2Ativo) {
    Serial.println("Info: 50% da capacidade.");

    porcentagemAtual = 50;
    metrosAtual = 1.25;
    statusAtual = "Normal";

    // Mantém Verde e Amarelo acesos fixos e desliga o Vermelho
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(ledMax, LOW);
    
    // Som de notificação calma Ding-Dong
    tone(pinoAltoFalante, 880); aguardar(250);
    tone(pinoAltoFalante, 1100); aguardar(400);
    noTone(pinoAltoFalante);
    
    // Aguarda 6 segundos mantendo os LEDs ligados antes de notificar de novo
    aguardar(6000);
  }
  
  // --- LÓGICA DO NÍVEL MÍNIMO (VERDE EM LOOP: ACESO -> PISCANDO -> ACESO) ---
  else if (nivel1Ativo) {
    Serial.println("ALERTA: Nível Mínimo Crítico!");

    porcentagemAtual = 25;
    metrosAtual = 0.50;
    statusAtual = "Critico Baixo";

    // Garante que o Amarelo e o Vermelho estão apagados
    digitalWrite(led2, LOW);
    digitalWrite(ledMax, LOW);
    
    // 1. LED Verde acende fixo por 1 segundo antes dos bipes
    digitalWrite(led1, HIGH);
    aguardar(1000);
    
    // 2. LED Verde pisca junto com os bipes de alerta
    for (int i = 0; i < 3; i++) {
      digitalWrite(led1, HIGH);        // Liga LED Verde
      tone(pinoAltoFalante, 1200); 
      aguardar(250);
      
      digitalWrite(led1, LOW);         // Desliga LED Verde
      noTone(pinoAltoFalante);     
      aguardar(100);
      
      digitalWrite(led1, HIGH);        // Liga LED Verde
      tone(pinoAltoFalante, 1200); 
      aguardar(250);
      
      digitalWrite(led1, LOW);         // Desliga LED Verde
      noTone(pinoAltoFalante);     
      aguardar(400);
    }
    
    // 3. LED Verde volta a ficar aceso fixo durante a pausa antes de reiniciar o loop
    digitalWrite(led1, HIGH);
    aguardar(1500);
  }
  
  // --- COPO VAZIO ---
  else {
    porcentagemAtual = 0;
    metrosAtual = 0.00;
    statusAtual = "Vazio";

    // Desliga todos os indicadores e som
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(ledMax, LOW);
    noTone(pinoAltoFalante);
    
    aguardar(200); // Filtro para estabilização da leitura
  }
}




















































































































































































//by Bruno Lisboa