#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Definições do display
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  -1  // Como o pino RST não está disponível na simulação, definimos como -1

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Configurações da rede Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Endpoint da API do Mercado Pago
const char* api_url = "https://api.mercadopago.com/pos/qr-code";
const char* access_token = "TEST-b1333d5b-1e03-4716-ad4e-2886f5c38783";

// Configuração do botão
const int buttonPin = 16;
bool buttonPressed = false;

// Função para inicializar o display, o botão e a conexão Wi-Fi
void setup() {
  Serial.begin(115200);

  // Inicializar display
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  
  // Exibir mensagem inicial
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Aperte o botao para pagar");

  // Debug: verificar se o display foi inicializado corretamente
  Serial.println("Display inicializado");

  // Configurar pino do botão como entrada
  pinMode(buttonPin, INPUT_PULLUP);

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

// Loop principal para verificar o estado do botão e o status do pagamento
void loop() {
  // Verificar o estado do botão
  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    buttonPressed = true;
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.println("Gerando QR Code...");
    createQR();
  }

  // Verificar notificações de pagamento
  checkPaymentStatus();
  delay(10000); // Espera de 10 segundos
}

// Função para criar o QR Code usando a API do Mercado Pago
void createQR() {
  HTTPClient http;
  http.begin(api_url);  // URL da API do Mercado Pago
  http.addHeader("Authorization", String("Bearer ") + access_token);  // Adiciona o token de autorização
  http.addHeader("Content-Type", "application/json");  // Define o tipo de conteúdo como JSON
  
  // Corpo da requisição em formato JSON
  String jsonBody = "{\"transaction_amount\": 100.00, \"description\": \"Payment for Order 123\", \"payment_method_id\": \"pix\", \"payer\": {\"email\": \"customer@example.com\", \"identification\": {\"type\": \"CPF\", \"number\": \"12345678900\"}}}";
  
  int httpResponseCode = http.POST(jsonBody);  // Envia a requisição POST
  
  if (httpResponseCode > 0) {
    String response = http.getString();  // Recebe a resposta da API
    Serial.println(response);  // Exibe a resposta no console serial
    // Aqui você pode processar a resposta para obter o QR Code
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println("QR Code gerado");
    // Adicionar código para desenhar o QR Code no display
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);  // Exibe o código de erro se a requisição falhar
  }
  http.end();  // Finaliza a requisição
}

// Função para verificar o status do pagamento
void checkPaymentStatus() {
  HTTPClient http;
  http.begin("https://api.mercadopago.com/users/test");  // URL do servidor que verifica o status do pagamento
  int httpCode = http.GET();  // Envia a requisição GET
  
  if (httpCode > 0) {
    String payload = http.getString();  // Recebe a resposta do servidor
    Serial.println(payload);  // Exibe a resposta no console serial
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println(payload);  // Exibe a resposta no display
  } else {
    Serial.println("Error on HTTP request");  // Exibe uma mensagem de erro se a requisição falhar
  }
  http.end();  // Finaliza a requisição
}
