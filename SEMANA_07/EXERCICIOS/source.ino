// Bibliotecas para conexão wifi
#include <WiFi.h>
#include <WiFiMulti.h>

// Biblioteca para rodar o servidor embarcado
#include <WebServer.h>

// Biblioteca para serialização de string para JSON
#include <ArduinoJson.h>

// Definição da estrutura base para serialização JSON
StaticJsonDocument<250> jsonDocument;
char buffer[250];

// Definição dos pins dos atuadores
#define BLUE_LED 17
#define RED_LED 18
#define RESET_BUTTON 42


// Instância das classes WifiMulti e WebServer
WiFiMulti WiFiMulti;
WebServer server(80);

// Controle de estado do botão de reset
int buttonState = 0;

// Variáveis que guardam informações e pontuação dos players
int player_one_points = 0;
int player_two_points = 0;

int player_one_seconds = 0;
int player_two_seconds = 0;

bool player_one_finished = false;
bool player_two_finished = false;

// Função responsável por comparar os pontos e tempo de jogo de cada jogador
// Para então escolher um player Vencedor
void finish() {
  Serial.println("[+] Finish function called!!");
  if (player_one_points == player_two_points) {
    if (player_one_seconds < player_two_seconds) {
      digitalWrite(BLUE_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    } else if (player_one_seconds == player_two_seconds) {
      digitalWrite(BLUE_LED, HIGH);
      digitalWrite(RED_LED, HIGH);
    } else {
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(RED_LED, HIGH);
    }
  } else if (player_one_points > player_two_points) {
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
}

// Função responsável por guardar os pontos e tempo de jogo
// de um player, quando este termina o quiz
void playerFinish(int playerId, int points, int timeInSeconds) {
  if (playerId == 0) {
    player_one_points = points;
    player_one_seconds = timeInSeconds;
    player_one_finished = true;
  } else if (playerId == 1) {
    player_two_points = points;
    player_two_seconds = timeInSeconds;
    player_two_finished = true;
  }

  // Se os dois players finalizaram, chama a função finish
  if (player_one_finished && player_two_finished) {
    finish();
  }
}

// Função que cuida da requisição POST para o endpoint "/finishPlayer"
// Recebe as informações do corpo da requisição, e chama as funções
// responsáveis
void postFinishPlayer() {
  if (server.hasArg("plain") == false) {
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  // Parsing do JSON do corpo da requisição
  Serial.println("[+] Post recebido com sucesso!");
  int playerId = int(jsonDocument["playerId"]);
  int points = int(jsonDocument["points"]);
  int timeInSeconds = int(jsonDocument["timeInSeconds"]);
  

  // Chama a função playerFInish
  playerFinish(playerId, points, timeInSeconds);
  server.send(200, "application/json", "{\"success\":true}");
  delay(3000);
}

// Função utilizada para retornar "OK" para
// os métodos OPTIONS das requisições CORS
void sendCrossOriginHeader() {
  server.send(204);
}


// Função que reseta a pontuação e estado de jogo dos players
void resetGame() {
  player_one_points = 0;
  player_two_points = 0;
  player_one_seconds = 0;
  player_two_seconds = 0;
  player_one_finished = false;
  player_two_finished = false;

  // Apaga todos os LEDs
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

// Função responsável por fazer o setup das rotas e endpoints
// do webserver embarcado
void routing_setup() {
  Serial.println("[+] Routing Setup!");

  // ativa cors
  server.enableCORS();
  
  // define as rotas de POST e OPTIONS (para cors)
  server.on("/finishPlayer", HTTP_POST, postFinishPlayer);
  server.on("/finishPlayer", HTTP_OPTIONS, sendCrossOriginHeader);
  server.begin();
}

// Função responsável por definir o pinMode dos atuadores
void pin_setup() {
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(RESET_BUTTON, INPUT);
}

// Função responsável por conectar o ESP32 ao Wifi
void connectWifi() {
  WiFiMulti.addAP("minha-rede", "minha-senha");

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Chama as funções encapsuladas
  Serial.begin(115200);
  delay(10);
  pin_setup();
  connectWifi()
  routing_setup();
  delay(500);
}


void loop() {
  delay(200);
  // Altera o estado do botão de reset
  buttonState = digitalRead(RESET_BUTTON);
  
  // Se o botão for pressionado, um reset total das informações de jogo
  // é feito
  if (buttonState == HIGH) {
   resetGame(); 
  }

  // Cuida das requisições do webserver
  server.handleClient();
}
