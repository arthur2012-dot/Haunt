/**
 * Haunt Firmware v0.2 — Independent CYD System
 * Gengar-themed · Built for Batista
 * Hardware: ESP32-2432S028R (Cheap Yellow Display)
 *
 * Core apps: Home, Clock, Calculator, Notes, Light, Sensor, Wi-Fi, Game Hub
 * Original mini-games (no ROMs / no third-party characters)
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <WiFi.h>
#include <Preferences.h>

// ── Hardware pins (CYD ESP32-2432S028R) ──────────────────────────────────────
constexpr int TFT_BACKLIGHT_PIN = 21;
constexpr int TOUCH_CS_PIN      = 33;
constexpr int TOUCH_IRQ_PIN     = 36;
constexpr int TOUCH_SCK_PIN     = 25;
constexpr int TOUCH_MISO_PIN    = 39;
constexpr int TOUCH_MOSI_PIN    = 32;
constexpr int LDR_PIN           = 34;
constexpr int LED_R_PIN         = 4;
constexpr int LED_G_PIN         = 16;
constexpr int LED_B_PIN         = 17;
constexpr int SPEAKER_PIN       = 26;

constexpr uint8_t MAX_NETWORKS  = 7;

// ── Palette ──────────────────────────────────────────────────────────────────
const uint16_t NAVY   = 0x0861;
const uint16_t PANEL  = 0x18E3;
const uint16_t CYAN   = 0x07FF;
const uint16_t WHITE  = 0xFFFF;
const uint16_t GREEN  = 0x07E0;
const uint16_t ORANGE = 0xFD20;
const uint16_t RED    = 0xF800;
const uint16_t YELLOW = 0xFFE0;
const uint16_t BLACK  = 0x0000;
const uint16_t PURPLE = 0x780F;

// ── Pages ────────────────────────────────────────────────────────────────────
enum Page {
  HOME, CLOCK_PAGE, CALC_PAGE, NOTES_PAGE, LIGHT_PAGE, SENSOR_PAGE,
  WIFI_PAGE, WIFI_LIST_PAGE, WIFI_PASSWORD_PAGE, GAMES_PAGE,
  MAZE_PAGE, JUMP_PAGE, REACT_PAGE
};

// ── Forward declarations ─────────────────────────────────────────────────────
void button(int x, int y, int w, int h, const String &s, uint16_t c, uint8_t z = 2);
void beep(uint16_t hz = 900, uint16_t ms = 45);
void header(const String &s);
void setLamp(bool on);
void statusBar();
void drawHome();
void drawClock();
float calculate(const String &s, bool &ok);
void drawCalc();
void drawNotes();
void drawLight();
void drawSensor();
void drawWifi();
void drawWifiList();
void drawWifiPassword();
void drawGames();
void drawMaze();
void drawJump();
void drawReact();
void drawPage();
void scanWifi();
void connectNetwork(bool saved);
bool readTouch(int &x, int &y);
void mazeMove(int dx, int dy);
void handleTap(int x, int y);
void updateJump();

// ── Global state ─────────────────────────────────────────────────────────────
TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen touch(TOUCH_CS_PIN, TOUCH_IRQ_PIN);
Preferences store;

Page page = HOME;
String noteText, calcText, calcAnswer, wifiStatus, selectedSsid, passwordText;
String foundSsid[MAX_NETWORKS];
int foundRssi[MAX_NETWORKS];
uint8_t foundCount = 0;
bool passwordUpper = true, lightOn = false;
int clockHour = 12, clockMinute = 0;
uint32_t clockBaseMs = 0, lastClockDraw = 0, lastSensorDraw = 0;

// Mini-games (original — no external assets)
int mazeX = 1, mazeY = 1, mazeScore = 0, mazeBest = 0;
const char *mazeMap[11] = {
  "###############",
  "#.............#",
  "#.###.###.###.#",
  "#.#...#...#...#",
  "#.#.#####.#.###",
  "#.....#.......#",
  "###.#.#.#####.#",
  "#...#...#.....#",
  "#.#####.#.###.#",
  "#.............#",
  "###############"
};
int jumpX = 35, jumpY = 185, jumpVel = 0, jumpScore = 0, jumpBest = 0, obstacleX = 235;
uint32_t lastJump = 0;
bool reactWaiting = false, reactReady = false;
uint32_t reactStart = 0;
int reactBest = 0;

// ── UI helpers ───────────────────────────────────────────────────────────────
void button(int x, int y, int w, int h, const String &s, uint16_t c, uint8_t z) {
  tft.fillRoundRect(x, y, w, h, 8, c);
  tft.setTextColor(WHITE, c);
  tft.setTextSize(z);
  int16_t bx, by;
  uint16_t bw, bh;
  tft.getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);
  tft.setCursor(x + (w - bw) / 2, y + (h - bh) / 2);
  tft.print(s);
}

void beep(uint16_t hz, uint16_t ms) {
  ledcAttachPin(SPEAKER_PIN, 0);
  ledcWriteTone(0, hz);
  delay(ms);
  ledcWriteTone(0, 0);
}

void header(const String &s) {
  tft.fillScreen(NAVY);
  tft.fillRect(0, 0, 240, 28, PANEL);
  tft.setTextColor(CYAN, PANEL);
  tft.setTextSize(2);
  tft.setCursor(7, 7);
  tft.print(s);
  button(184, 3, 52, 22, "Inicio", ORANGE, 1);
}

void setLamp(bool on) {
  lightOn = on;
  digitalWrite(LED_R_PIN, on ? LOW : HIGH);
  digitalWrite(LED_G_PIN, on ? LOW : HIGH);
  digitalWrite(LED_B_PIN, on ? LOW : HIGH);
}

void statusBar() {
  int mins = (clockHour * 60 + clockMinute + (millis() - clockBaseMs) / 60000UL) % 1440;
  char tm[6];
  snprintf(tm, sizeof(tm), "%02d:%02d", mins / 60, mins % 60);
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(187, 35);
  tft.print(tm);
  tft.setCursor(8, 35);
  tft.print(WiFi.status() == WL_CONNECTED ? "Wi-Fi conectado" : "Modo local");
}

// ── Screens ──────────────────────────────────────────────────────────────────
void drawHome() {
  header("Haunt");
  statusBar();
  button(7, 53, 72, 49, "RELOGIO", CYAN, 1);
  button(84, 53, 72, 49, "CALC", GREEN, 2);
  button(161, 53, 72, 49, "NOTAS", ORANGE, 1);
  button(7, 110, 72, 49, "LUZ", 0x7BEF, 2);
  button(84, 110, 72, 49, "SENSOR", PURPLE, 1);
  button(161, 110, 72, 49, "WI-FI", 0x001F, 1);
  button(39, 174, 162, 53, "GAME HUB", 0x001F, 2);
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(23, 241);
  tft.print("Toque em um app para abrir");
}

void drawClock() {
  header("Relogio");
  int total = (clockHour * 60 + clockMinute + (millis() - clockBaseMs) / 60000UL) % 1440;
  char tm[6];
  snprintf(tm, sizeof(tm), "%02d:%02d", total / 60, total % 60);
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(6);
  tft.setCursor(30, 60);
  tft.print(tm);
  tft.setTextSize(1);
  tft.setCursor(20, 135);
  tft.print("Ajuste; o horario fica salvo neste aparelho.");
  button(10, 165, 48, 44, "H-", PANEL);
  button(64, 165, 48, 44, "H+", PANEL);
  button(128, 165, 48, 44, "M-", PANEL);
  button(182, 165, 48, 44, "M+", PANEL);
}

float calculate(const String &s, bool &ok) {
  int p = -1;
  char op = 0;
  for (int i = 1; i < (int)s.length(); i++) {
    if (String("+-*/").indexOf(s[i]) >= 0) {
      p = i;
      op = s[i];
      break;
    }
  }
  if (p < 0) {
    ok = false;
    return 0;
  }
  float a = s.substring(0, p).toFloat();
  float b = s.substring(p + 1).toFloat();
  if (op == '/' && b == 0) {
    ok = false;
    return 0;
  }
  ok = true;
  if (op == '+') return a + b;
  if (op == '-') return a - b;
  if (op == '*') return a * b;
  return a / b;
}

void drawCalc() {
  header("Calculadora");
  tft.fillRoundRect(8, 38, 224, 42, 6, PANEL);
  tft.setTextColor(WHITE, PANEL);
  tft.setTextSize(2);
  tft.setCursor(12, 48);
  tft.print(calcText);
  tft.setTextSize(1);
  tft.setCursor(12, 67);
  tft.print(calcAnswer);
  const char *k[] = {"7", "8", "9", "/", "4", "5", "6", "*", "1", "2", "3", "-", "C", "0", "=", "+"};
  for (int i = 0; i < 16; i++) {
    button(10 + (i % 4) * 56, 92 + (i / 4) * 35, 50, 29, k[i], i % 4 == 3 ? ORANGE : PANEL);
  }
}

void drawNotes() {
  header("Notas locais");
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(8, 34);
  tft.print("A nota fica guardada na memoria interna.");
  tft.fillRoundRect(8, 48, 224, 48, 6, PANEL);
  tft.setTextColor(WHITE, PANEL);
  tft.setCursor(13, 57);
  tft.print((noteText.length() ? noteText : "Toque nas letras para escrever").substring(0, 58));
  const char *rows[] = {"ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ0123", "456789 .,-"};
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 10; c++) {
      int x = 5 + c * 23, y = 108 + r * 29;
      tft.drawRoundRect(x, y, 20, 25, 3, CYAN);
      tft.setTextColor(WHITE, NAVY);
      tft.setCursor(x + 7, y + 8);
      tft.print(rows[r][c]);
    }
  }
  button(8, 226, 68, 30, "APAGAR", RED, 1);
  button(84, 226, 68, 30, "ESPACO", PANEL, 1);
  button(160, 226, 72, 30, "SALVAR", GREEN, 1);
}

void drawLight() {
  if (lightOn) {
    tft.fillScreen(WHITE);
    tft.setTextColor(BLACK, WHITE);
    tft.setTextSize(2);
    tft.setCursor(42, 115);
    tft.print("Toque para desligar");
    return;
  }
  header("Luz");
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(2);
  tft.setCursor(44, 70);
  tft.print("Luz desligada");
  tft.setTextSize(1);
  tft.setCursor(20, 105);
  tft.print("A tela e o LED viram uma luz fraca.");
  button(45, 145, 150, 55, "LIGAR", ORANGE, 3);
}

void drawSensor() {
  header("Sensor de luz");
  int v = analogRead(LDR_PIN);
  tft.fillRect(0, 32, 240, 200, NAVY);
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(2);
  tft.setCursor(31, 70);
  tft.printf("Luz: %d", v);
  tft.setTextSize(1);
  tft.setCursor(16, 114);
  tft.print("Numero maior significa mais luz no sensor.");
  tft.fillRoundRect(20, 145, 200, 20, 8, PANEL);
  tft.fillRoundRect(20, 145, map(constrain(v, 0, 4095), 0, 4095, 0, 200), 20, 8, YELLOW);
}

void drawWifi() {
  header("Redes Wi-Fi");
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(8, 38);
  tft.print(WiFi.status() == WL_CONNECTED ? "Conectado: " + WiFi.SSID() : "Escolha sua rede e informe a senha.");
  tft.setCursor(8, 55);
  tft.print(wifiStatus.length() ? wifiStatus.substring(0, 38) : "Nenhuma rede foi procurada ainda.");
  button(42, 94, 156, 42, "BUSCAR REDES", GREEN);
  button(42, 148, 156, 42, "RECONECTAR", 0x001F);
  button(42, 202, 156, 42, "ESQUECER REDE", RED, 1);
}

void drawWifiList() {
  header("Escolha a rede");
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(8, 35);
  tft.print("Toque no nome da sua rede Wi-Fi.");
  if (!foundCount) {
    tft.setCursor(8, 58);
    tft.print("Nenhuma rede encontrada.");
    return;
  }
  for (uint8_t i = 0; i < foundCount; i++) {
    int y = 50 + i * 30;
    tft.fillRoundRect(7, y, 226, 25, 5, PANEL);
    tft.setTextColor(WHITE, PANEL);
    tft.setCursor(12, y + 8);
    tft.print(foundSsid[i].substring(0, 22));
    tft.setCursor(185, y + 8);
    tft.printf("%d", foundRssi[i]);
  }
}

void drawWifiPassword() {
  header("Senha do Wi-Fi");
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(7, 34);
  tft.print(selectedSsid.substring(0, 30));
  tft.fillRoundRect(7, 48, 226, 30, 5, PANEL);
  tft.setTextColor(WHITE, PANEL);
  tft.setCursor(12, 59);
  for (uint8_t i = 0; i < passwordText.length(); i++) tft.print('*');
  const char *up[] = {"ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ .,-"};
  const char *lo[] = {"abcdefghij", "klmnopqrst", "uvwxyz .,-"};
  const char **rows = passwordUpper ? up : lo;
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 10; c++) {
      char ch = rows[r][c];
      if (!ch) continue;
      int x = 5 + c * 23, y = 88 + r * 29;
      tft.drawRoundRect(x, y, 20, 25, 3, CYAN);
      tft.setTextColor(WHITE, NAVY);
      tft.setCursor(x + 7, y + 8);
      tft.print(ch);
    }
  }
  button(5, 179, 54, 30, passwordUpper ? "abc" : "ABC", PANEL, 1);
  button(64, 179, 54, 30, "123", PANEL, 1);
  button(123, 179, 54, 30, "APAGAR", RED, 1);
  button(182, 179, 53, 30, "ESPACO", PANEL, 1);
  button(33, 220, 174, 35, "CONECTAR E SALVAR", GREEN, 1);
}

void drawGames() {
  header("Game Hub");
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(13, 35);
  tft.print("Jogos originais pequenos, feitos para esta tela.");
  button(22, 59, 196, 45, "CACA PONTOS", 0x001F, 2);
  button(22, 114, 196, 45, "PULO RAPIDO", GREEN, 2);
  button(22, 169, 196, 45, "TESTE DE REFLEXO", PURPLE, 2);
  tft.setTextColor(WHITE, NAVY);
  tft.setCursor(20, 240);
  tft.print("Sem ROMs baixadas e sem personagens de terceiros.");
}

void drawMaze() {
  header("Caca pontos");
  const int s = 15, ox = 7, oy = 48;
  for (int r = 0; r < 11; r++) {
    for (int c = 0; c < 15; c++) {
      char cell = mazeMap[r][c];
      tft.fillRect(ox + c * s, oy + r * s, s - 1, s - 1, cell == '#' ? PANEL : BLACK);
      if (cell == '.') tft.fillCircle(ox + c * s + 7, oy + r * s + 7, 2, YELLOW);
    }
  }
  tft.fillCircle(ox + mazeX * s + 7, oy + mazeY * s + 7, 5, CYAN);
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(8, 220);
  tft.printf("Pontos: %d   Recorde: %d", mazeScore, mazeBest);
  button(10, 240, 45, 35, "<", PANEL, 2);
  button(65, 240, 45, 35, "^", PANEL, 2);
  button(120, 240, 45, 35, "v", PANEL, 2);
  button(175, 240, 45, 35, ">", PANEL, 2);
}

void drawJump() {
  header("Pulo rapido");
  tft.fillRect(0, 55, 240, 160, BLACK);
  tft.fillRect(0, 204, 240, 6, GREEN);
  tft.fillRect(jumpX, jumpY, 16, 16, CYAN);
  tft.fillRect(obstacleX, 188, 12, 16, RED);
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(9, 222);
  tft.printf("Pontos: %d   Recorde: %d", jumpScore, jumpBest);
  button(45, 242, 150, 34, "TOQUE PARA PULAR", ORANGE, 1);
}

void drawReact() {
  header("Teste de reflexo");
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(1);
  tft.setCursor(16, 52);
  if (!reactWaiting && !reactReady) {
    tft.print("Toque em COMECAR. Espere o verde.");
    button(35, 105, 170, 55, "COMECAR", ORANGE, 2);
  } else if (reactWaiting) {
    tft.print("Espere... nao toque ainda.");
    button(35, 105, 170, 55, "ESPERE", RED, 2);
  } else {
    tft.print("TOQUE AGORA!");
    button(35, 105, 170, 55, "AGORA!", GREEN, 3);
  }
  tft.setCursor(16, 200);
  tft.printf("Melhor tempo: %d ms", reactBest);
}

void drawPage() {
  if (page == HOME) drawHome();
  else if (page == CLOCK_PAGE) drawClock();
  else if (page == CALC_PAGE) drawCalc();
  else if (page == NOTES_PAGE) drawNotes();
  else if (page == LIGHT_PAGE) drawLight();
  else if (page == SENSOR_PAGE) drawSensor();
  else if (page == WIFI_PAGE) drawWifi();
  else if (page == WIFI_LIST_PAGE) drawWifiList();
  else if (page == WIFI_PASSWORD_PAGE) drawWifiPassword();
  else if (page == GAMES_PAGE) drawGames();
  else if (page == MAZE_PAGE) drawMaze();
  else if (page == JUMP_PAGE) drawJump();
  else drawReact();
}

// ── Logic ────────────────────────────────────────────────────────────────────
void scanWifi() {
  wifiStatus = "Buscando redes...";
  page = WIFI_PAGE;
  drawWifi();
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks(false, true);
  foundCount = 0;
  for (int i = 0; i < n && foundCount < MAX_NETWORKS; i++) {
    if (WiFi.SSID(i).length()) {
      foundSsid[foundCount] = WiFi.SSID(i);
      foundRssi[foundCount++] = WiFi.RSSI(i);
    }
  }
  WiFi.scanDelete();
  wifiStatus = foundCount ? "Escolha uma rede na lista." : "Nenhuma rede encontrada.";
  page = WIFI_LIST_PAGE;
  drawWifiList();
}

void connectNetwork(bool saved) {
  String ssid = saved ? store.getString("wifi_ssid", "") : selectedSsid;
  String pass = saved ? store.getString("wifi_pass", "") : passwordText;
  if (!ssid.length()) {
    wifiStatus = "Primeiro procure e escolha uma rede.";
    drawWifi();
    return;
  }
  tft.fillScreen(NAVY);
  tft.setTextColor(WHITE, NAVY);
  tft.setTextSize(2);
  tft.setCursor(28, 120);
  tft.print("Conectando...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  uint32_t began = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - began < 15000) delay(100);
  if (WiFi.status() == WL_CONNECTED) {
    if (!saved) {
      store.putString("wifi_ssid", ssid);
      store.putString("wifi_pass", pass);
    }
    wifiStatus = "Conectado. IP: " + WiFi.localIP().toString();
    beep(1200, 80);
  } else {
    wifiStatus = "Nao conectou. Confira a senha.";
  }
  page = WIFI_PAGE;
  drawWifi();
}

bool readTouch(int &x, int &y) {
  if (!touch.touched()) return false;
  TS_Point p = touch.getPoint();
  x = constrain(map(p.x, 200, 3800, 0, 240), 0, 239);
  y = constrain(map(p.y, 240, 3800, 0, 320), 0, 319);
  delay(140);
  return true;
}

void mazeMove(int dx, int dy) {
  int nx = mazeX + dx, ny = mazeY + dy;
  if (nx < 0 || nx >= 15 || ny < 0 || ny >= 11 || mazeMap[ny][nx] == '#') return;
  mazeX = nx;
  mazeY = ny;
  mazeScore++;
  if (mazeScore > mazeBest) {
    mazeBest = mazeScore;
    store.putInt("mazeBest", mazeBest);
  }
  beep(700, 18);
  drawMaze();
}

void handleTap(int x, int y) {
  if (lightOn) {
    setLamp(false);
    drawLight();
    return;
  }
  if (x > 180 && y < 30 && page != HOME) {
    page = HOME;
    drawHome();
    return;
  }

  if (page == HOME) {
    if (y >= 53 && y < 102) page = x < 80 ? CLOCK_PAGE : (x < 158 ? CALC_PAGE : NOTES_PAGE);
    else if (y >= 110 && y < 159) page = x < 80 ? LIGHT_PAGE : (x < 158 ? SENSOR_PAGE : WIFI_PAGE);
    else if (y >= 174 && y < 230) page = GAMES_PAGE;
    drawPage();
    return;
  }

  if (page == CLOCK_PAGE && y >= 165 && y <= 209) {
    if (x < 60) clockHour = (clockHour + 23) % 24;
    else if (x < 120) clockHour = (clockHour + 1) % 24;
    else if (x < 180) clockMinute = (clockMinute + 59) % 60;
    else clockMinute = (clockMinute + 1) % 60;
    clockBaseMs = millis();
    store.putInt("hour", clockHour);
    store.putInt("minute", clockMinute);
    drawClock();
    return;
  }

  if (page == CALC_PAGE && y >= 92 && y < 232) {
    int col = constrain((x - 10) / 56, 0, 3);
    int row = constrain((y - 92) / 35, 0, 3);
    const char *k[] = {"7", "8", "9", "/", "4", "5", "6", "*", "1", "2", "3", "-", "C", "0", "=", "+"};
    String key = k[row * 4 + col];
    if (key == "C") {
      calcText = "";
      calcAnswer = "";
    } else if (key == "=") {
      bool ok;
      float a = calculate(calcText, ok);
      calcAnswer = ok ? String(a, 4) : "Exemplo: 12+3";
    } else {
      calcText += key;
    }
    drawCalc();
    return;
  }

  if (page == NOTES_PAGE) {
    if (y >= 108 && y < 224) {
      const char *rows[] = {"ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ0123", "456789 .,-"};
      int r = (y - 108) / 29, c = constrain((x - 5) / 23, 0, 9);
      if (noteText.length() < 58) noteText += rows[r][c];
    } else if (y >= 226) {
      if (x < 76) noteText = "";
      else if (x < 152 && noteText.length() < 58) noteText += " ";
      else {
        store.putString("note", noteText);
        beep();
      }
    }
    drawNotes();
    return;
  }

  if (page == LIGHT_PAGE && y > 130) {
    setLamp(true);
    drawLight();
    return;
  }

  if (page == WIFI_PAGE) {
    if (y >= 94 && y < 140) scanWifi();
    else if (y >= 148 && y < 195) connectNetwork(true);
    else if (y >= 202 && y < 250) {
      store.remove("wifi_ssid");
      store.remove("wifi_pass");
      WiFi.disconnect(true);
      wifiStatus = "Rede salva apagada.";
      drawWifi();
    }
    return;
  }

  if (page == WIFI_LIST_PAGE && y >= 50 && y < 50 + foundCount * 30) {
    selectedSsid = foundSsid[(y - 50) / 30];
    passwordText = "";
    page = WIFI_PASSWORD_PAGE;
    drawWifiPassword();
    return;
  }

  if (page == WIFI_PASSWORD_PAGE) {
    if (y >= 88 && y < 175) {
      const char *up[] = {"ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ .,-"};
      const char *lo[] = {"abcdefghij", "klmnopqrst", "uvwxyz .,-"};
      const char **rows = passwordUpper ? up : lo;
      int r = (y - 88) / 29, c = constrain((x - 5) / 23, 0, 9);
      char ch = rows[r][c];
      if (ch && passwordText.length() < 63) passwordText += ch;
    } else if (y >= 179 && y < 210) {
      if (x < 59) passwordUpper = !passwordUpper;
      else if (x < 118 && passwordText.length() < 63) passwordText += "0";
      else if (x < 177 && passwordText.length()) passwordText.remove(passwordText.length() - 1);
      else if (passwordText.length() < 63) passwordText += " ";
    } else if (y >= 220 && y < 260) {
      connectNetwork(false);
      return;
    }
    drawWifiPassword();
    return;
  }

  if (page == GAMES_PAGE) {
    if (y >= 59 && y < 104) {
      page = MAZE_PAGE;
      mazeX = 1;
      mazeY = 1;
      mazeScore = 0;
    } else if (y >= 114 && y < 159) {
      page = JUMP_PAGE;
      jumpX = 35;
      jumpY = 185;
      jumpVel = 0;
      jumpScore = 0;
      obstacleX = 235;
      lastJump = millis();
    } else if (y >= 169 && y < 214) {
      page = REACT_PAGE;
      reactWaiting = false;
      reactReady = false;
    }
    drawPage();
    return;
  }

  if (page == MAZE_PAGE && y >= 240) {
    if (x < 55) mazeMove(-1, 0);
    else if (x < 110) mazeMove(0, -1);
    else if (x < 165) mazeMove(0, 1);
    else mazeMove(1, 0);
    return;
  }

  if (page == JUMP_PAGE && y >= 230) {
    if (jumpY >= 185) {
      jumpVel = -11;
      beep(1000, 30);
    }
    return;
  }

  if (page == REACT_PAGE) {
    if (!reactWaiting && !reactReady) {
      reactWaiting = true;
      reactStart = millis() + random(1600, 4200);
      drawReact();
    } else if (reactReady) {
      int ms = millis() - reactStart;
      if (!reactBest || ms < reactBest) {
        reactBest = ms;
        store.putInt("reactBest", reactBest);
      }
      reactWaiting = false;
      reactReady = false;
      beep(1400, 70);
      drawReact();
    } else {
      reactWaiting = false;
      drawReact();
    }
    return;
  }
}

void updateJump() {
  if (page != JUMP_PAGE || millis() - lastJump < 50) return;
  lastJump = millis();
  jumpVel += 1;
  jumpY += jumpVel;
  if (jumpY >= 185) {
    jumpY = 185;
    jumpVel = 0;
  }
  obstacleX -= 7;
  if (obstacleX < -15) {
    obstacleX = 240;
    jumpScore++;
    if (jumpScore > jumpBest) {
      jumpBest = jumpScore;
      store.putInt("jumpBest", jumpBest);
    }
  }
  if (obstacleX < jumpX + 16 && obstacleX + 12 > jumpX && jumpY + 16 > 188) {
    jumpScore = 0;
    obstacleX = 240;
    beep(180, 150);
  }
  drawJump();
}

// ── Setup & Loop ─────────────────────────────────────────────────────────────
void setup() {
  pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(TFT_BACKLIGHT_PIN, HIGH);
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  setLamp(false);
  analogReadResolution(12);

  tft.init();
  tft.setRotation(0);

  touchSPI.begin(TOUCH_SCK_PIN, TOUCH_MISO_PIN, TOUCH_MOSI_PIN, TOUCH_CS_PIN);
  touch.begin(touchSPI);
  touch.setRotation(0);

  store.begin("haunt", false);
  noteText   = store.getString("note", "");
  clockHour  = store.getInt("hour", 12);
  clockMinute = store.getInt("minute", 0);
  mazeBest   = store.getInt("mazeBest", 0);
  jumpBest   = store.getInt("jumpBest", 0);
  reactBest  = store.getInt("reactBest", 0);
  clockBaseMs = millis();
  randomSeed(esp_random());

  drawHome();
}

void loop() {
  int x, y;
  if (readTouch(x, y)) handleTap(x, y);

  if (page == CLOCK_PAGE && millis() - lastClockDraw >= 1000) {
    lastClockDraw = millis();
    drawClock();
  }
  if (page == SENSOR_PAGE && millis() - lastSensorDraw >= 300) {
    lastSensorDraw = millis();
    drawSensor();
  }
  if (page == REACT_PAGE && reactWaiting && millis() >= reactStart) {
    reactReady = true;
    beep(1100, 50);
    drawReact();
  }
  updateJump();
}
