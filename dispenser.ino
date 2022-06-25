#include "Nextion.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <EEPROM.h>
#include "ESP8266.h"
#include <ArduinoJson.h>

ESP8266 wifiSerial(Serial1, 115200);

#define SS 53

#define DEBUG true

//======Pinagem de Saida (liga reles das valvulas)======
#define RELE1_REGENERANT 30
#define RELE2_NUTRITION 31
#define RELE3_ANTIFRISO 32
#define RELE4_BIONEUTRAL 33

#define RELE16_NUTRI 40
#define RELE15_REPAIR 41
#define RELE14_DENSITY 42
#define RELE13_DISCIPLINE 43
#define RELE12_ANTIBREAK 44
#define RELE11_GROWTH 45
#define RELE10_CURLY 46
#define RELE9_BASE 47

#define VOLUME_TESTE 10

#define RELE8_PHOTOACTIVE 48

#define QTDVALVULAS 12

// page0
NexButton btnReles = NexButton(0, 3, "btnReles");
NexButton btnShampoo = NexButton(0, 1, "bt0");
NexButton btnTreatment = NexButton(0, 2, "bt1");
NexButton btnWifi = NexButton(0, 5, "btWifi");
NexButton btnCadastro = NexButton(0, 4, "btnCadastro");

// page1
NexDSButton btRegenerant = NexDSButton(1, 5, "bt1");
NexDSButton btNutrition = NexDSButton(1, 6, "bt2");
NexDSButton btAntiFriso = NexDSButton(1, 7, "bt3");
NexDSButton btBioNeutral = NexDSButton(1, 8, "bt4");
NexText volumeShampoo = NexText(1, 2, "t0");

NexButton btMix = NexButton(1, 4, "bt6");
NexPicture picPhotoactiveSh = NexPicture(1, 9, "p0");
NexButton btPhotoactiveSh = NexButton(1, 1, "bt0");

// page2
NexDSButton btNutriTr = NexDSButton(2, 5, "bt1");
NexDSButton btRepair = NexDSButton(2, 6, "bt2");
NexDSButton btDensity = NexDSButton(2, 7, "bt3");
NexDSButton btDiscipline = NexDSButton(2, 8, "bt4");
NexDSButton btAntibreak = NexDSButton(2, 9, "bt5");
NexDSButton btGrowth = NexDSButton(2, 10, "bt6");
NexDSButton btCurly = NexDSButton(2, 11, "bt7");
NexText volumeTratamento = NexText(2, 4, "t1");
NexPicture picPhotoactiveTr = NexPicture(2, 12, "p0");
NexButton btMixTr = NexButton(2, 3, "bt9");
NexButton btPhotoactiveTr = NexButton(2, 1, "bt0");

NexSlider slNutriTr = NexSlider(2, 13, "h0");
NexSlider slRepair = NexSlider(2, 15, "h1");
NexSlider slDensity = NexSlider(2, 16, "h2");
NexSlider slDiscipline = NexSlider(2, 17, "h3");
NexSlider slAntibreak = NexSlider(2, 18, "h4");
NexSlider slGrowth = NexSlider(2, 19, "h5");
NexSlider slCurly = NexSlider(2, 20, "h6");

// page6
NexText rele1 = NexText(6, 15, "rele1");
NexText rele2 = NexText(6, 16, "rele2");
NexText rele3 = NexText(6, 17, "rele3");
NexText rele4 = NexText(6, 18, "rele4");

NexText rele16 = NexText(6, 19, "rele16");
NexText rele15 = NexText(6, 20, "rele15");
NexText rele14 = NexText(6, 21, "rele14");
NexText rele13 = NexText(6, 22, "rele13");
NexText rele12 = NexText(6, 23, "rele12");
NexText rele11 = NexText(6, 24, "rele11");
NexText rele10 = NexText(6, 25, "rele10");
NexText rele9 = NexText(6, 26, "rele9");

NexButton btnRele1At = NexButton(7, 27, "b1"); //Por algum motivo, usar 6 nao funciona. Funcionou com 7.
NexButton btnRele2At = NexButton(7, 28, "b2");
NexButton btnRele3At = NexButton(7, 29, "b3");
NexButton btnRele4At = NexButton(7, 30, "b4");

NexButton btnRele16At = NexButton(7, 31, "b16");
NexButton btnRele15At = NexButton(7, 32, "b15");
NexButton btnRele14At = NexButton(7, 33, "b14");
NexButton btnRele13At = NexButton(7, 34, "b13");
NexButton btnRele12At = NexButton(7, 35, "b12");
NexButton btnRele11At = NexButton(7, 36, "b11");
NexButton btnRele10At = NexButton(7, 37, "b10");
NexButton btnRele9At = NexButton(7, 38, "b9");

NexText txtMsgAt = NexText(6, 39, "txtMsg");

NexButton btnAtualizar = NexButton(7, 14, "btnAtualizar"); //Por algum motivo, usar 6 nao funciona. Funcionou com 7.

// page7 cadastro de usuarios
NexText cadastroOutput = NexText(8, 3, "t1"); //Funcionou com 8.
NexText nomeProfissional = NexText(8, 9, "t6");
NexText senhaProfissional = NexText(8, 8, "t5");
NexText confirmaSenha = NexText(8, 10, "t7");
NexButton btnCadastrar = NexButton(8, 4, "btnCadastrar");

//page8 conexao com a internet
//NexText txtRede = NexText(9, 4, "txtRede"); //Funcionou com 9.
NexComboBox comboRede = NexComboBox(9, 11, "txtRede");
NexText txtSenha = NexText(9, 4, "txtSenha");
NexText txtConexao = NexText(9, 7, "txtConexao");
NexButton btnConectar = NexButton(9, 6, "btnConectar");

NexText txtIP = NexText(9, 8, "txtIP");
NexButton btnGravar = NexButton(9, 10, "btnGravar");

//page9 popup p/ liberar produtos
NexText txtComanda = NexText(11, 3, "txtComanda");
NexText txtPassword = NexText(11, 4, "txtSenha");
NexText txtMsg = NexText(11, 9, "txtMsg");
NexButton btnIniciar = NexButton(11, 6, "btnIniciar");
NexButton btnFechar = NexButton(11, 7, "b1");

//page10 popup paginas gerenciamento
NexText txtSenhaGerenciar  = NexText(12, 2, "txtSenha");
NexText txtMsgGerenciar  = NexText(12, 6, "txtMsg");
NexButton btnProximo = NexButton(12, 4, "btnProximo");
NexButton btnFecharGerenciar = NexButton(12, 5, "b1");

// paginas
NexPage page0 = NexPage(0, 0, "page0");
NexPage page1 = NexPage(1, 0, "page1");
NexPage page2 = NexPage(2, 0, "page2");
NexPage page4 = NexPage(4, 0, "page4");
NexPage page5 = NexPage(5, 0, "page5");
NexPage page11 = NexPage(11, 0, "page9");
NexPage page7 = NexPage(7, 0, "page6");
NexPage page9 = NexPage(9, 0, "page8");
NexPage page12 = NexPage(12, 0, "page10");
NexPage page8 = NexPage(8, 0, "page7");


//variaveis shampoo
enum shampoo {REGENERANT, NUTRITION, ANTIFRISO, BIONEUTRAL, NENHUM};
shampoo opcaoShampoo;

enum funcionalidade {SHAMPOO, TRATAMENTO, RELES, CADASTRO};
funcionalidade opcao;

//variaveis condicionador
bool condicionador[7];
int fator[7];
int tempoValvulaCondicionador[7];
int tempoValvulaBase;

bool photoactive = true;

int K_valvula_0;
int K_valvula_1;
int K_valvula_2;
int K_valvula_11;

//=======rtc e sensor ========
RtcDS3231<TwoWire> Rtc(Wire);
boolean isSdOk;

//===================wifi===============
bool conectado = false;
String redeConectada = "";

String hostIp = "";
uint32_t port = 9007;
//=======================================
String profissional = "";
String numeroComanda = "";

void btPhotoactiveTrPushCallback(void *ptr) {
  photoactive = !photoactive;
}

void btPhotoactiveShPushCallback(void *ptr) {
  photoactive = !photoactive;
}

//=====================================SHAMPOO==========================================
unsigned long volume = 0;

void btRegenerantPushCallback(void *ptr) {
  uint32_t ds;
  btRegenerant.getValue(&ds);
  if (ds)
    opcaoShampoo = REGENERANT;
  else
    opcaoShampoo = NENHUM;
}

void btNutritionPushCallback(void *ptr) {
  uint32_t ds;
  btNutrition.getValue(&ds);
  if (ds)
    opcaoShampoo = NUTRITION;
  else
    opcaoShampoo = NENHUM;
}

void btAntiFrisoPushCallback(void *ptr) {
  uint32_t ds;
  btAntiFriso.getValue(&ds);
  if (ds)
    opcaoShampoo = ANTIFRISO;
  else
    opcaoShampoo = NENHUM;
}

void btBioNeutralPushCallback(void *ptr) {
  uint32_t ds;
  btBioNeutral.getValue(&ds);
  if (ds)
    opcaoShampoo = BIONEUTRAL;
  else
    opcaoShampoo = NENHUM;
}


bool picPhotoactiveShState() {
  uint32_t ds;
  picPhotoactiveSh.getPic(&ds);
  return ds == 6;
}

void btMixPushCallback(void *ptr) {
  resetarPopupSenha();

  char buff[6] = {0};
  volumeShampoo.getText(buff, sizeof(buff));

  volume = atol(buff);
  if (!validarShampoo()) {
    return;
  }
  opcao = SHAMPOO;
  page11.show();
}

bool validarShampoo() {
  if (opcaoShampoo == NENHUM) {
    return false;
  }

  if (volume <= 0) {
    return false;
  }

  return true;
}

void rodaShampoo() {

  unsigned long volShampoo = 0;
  int rele = 0;
  switch (opcaoShampoo) {
    case REGENERANT:
      rele = RELE1_REGENERANT;
      volShampoo = (unsigned long)volume * K_valvula_0;
      break;
    case NUTRITION:
      rele = RELE2_NUTRITION;
      volShampoo = (unsigned long)volume * K_valvula_1;
      break;
    case ANTIFRISO:
      rele = RELE3_ANTIFRISO;
      volShampoo = (unsigned long)volume * K_valvula_2;
      break;
    case BIONEUTRAL:
      rele = RELE4_BIONEUTRAL;
      volShampoo = (unsigned long)volume * K_valvula_11;
      break;
  }

  unsigned long fim = millis() + volShampoo;

  if (volShampoo > 0) {
    page4.show();
    if (photoactive)
      digitalWrite (RELE8_PHOTOACTIVE, LOW);

    digitalWrite (rele, LOW);
    while (millis() < fim) {
    }
    resetarReles();
    page5.show();//retire seu produto

    gravarSD(prepararDadosSdShampoo(), "sham.txt");

    String dadosWifi = prepararDadosWifiShampoo();
    if (wifiSerial.createTCP(hostIp, port)) {
      reenviarDadosTemporarios();
      delay(500);
      enviarDadosWifi(dadosWifi);
      wifiSerial.releaseTCP();
    } else {
      dadosWifi += "\n";
      gravarSD(dadosWifi, "tmp.txt");
    }
  }
  page0.show();
}

//=====================================TRATAMENTO==========================================
unsigned long volumeTotal = 0;

void btNutriTrPushCallback(void *ptr) {
  uint32_t ds;
  btNutriTr.getValue(&ds);
  if (ds)
    condicionador[0] = true;
  else
    condicionador[0] = false;
}

void btRepairPushCallback(void *ptr) {
  uint32_t ds;
  btRepair.getValue(&ds);
  if (ds)
    condicionador[1] = true;
  else
    condicionador[1] = false;
}

void btDensityPushCallback(void *ptr) {
  uint32_t ds;
  btDensity.getValue(&ds);
  if (ds)
    condicionador[2] = true;
  else
    condicionador[2] = false;
}

void btDisciplinePushCallback(void *ptr) {
  uint32_t ds;
  btDiscipline.getValue(&ds);
  if (ds)
    condicionador[3] = true;
  else
    condicionador[3] = false;
}

void btAntibreakPushCallback(void *ptr) {
  uint32_t ds;
  btAntibreak.getValue(&ds);
  if (ds)
    condicionador[4] = true;
  else
    condicionador[4] = false;
}

void btGrowthPushCallback(void *ptr) {
  uint32_t ds;
  btGrowth.getValue(&ds);
  if (ds)
    condicionador[5] = true;
  else
    condicionador[5] = false;
}

void btCurlyPushCallback(void *ptr) {
  uint32_t ds;
  btCurly.getValue(&ds);
  if (ds)
    condicionador[6] = true;
  else
    condicionador[6] = false;
}

void slNutriTrPopCallback(void *ptr) {
  uint32_t ds;
  slNutriTr.getValue(&ds);
  if (ds > 0)
    condicionador[0] = true;
  else
    condicionador[0] = false;
}

void slRepairPopCallback(void *ptr) {
  uint32_t ds;
  slRepair.getValue(&ds);
  if (ds > 0)
    condicionador[1] = true;
  else
    condicionador[1] = false;
}

void slDensityPopCallback(void *ptr) {
  uint32_t ds;
  slDensity.getValue(&ds);
  if (ds > 0)
    condicionador[2] = true;
  else
    condicionador[2] = false;
}

void slDisciplinePopCallback(void *ptr) {
  uint32_t ds;
  slDiscipline.getValue(&ds);
  if (ds > 0)
    condicionador[3] = true;
  else
    condicionador[3] = false;
}

void slAntibreakPopCallback(void *ptr) {
  uint32_t ds;
  slAntibreak.getValue(&ds);
  if (ds > 0)
    condicionador[4] = true;
  else
    condicionador[4] = false;
}

void slGrowthPopCallback(void *ptr) {
  uint32_t ds;
  slGrowth.getValue(&ds);
  if (ds > 0)
    condicionador[5] = true;
  else
    condicionador[5] = false;
}

void slCurlyPopCallback(void *ptr) {
  uint32_t ds;
  slCurly.getValue(&ds);
  if (ds > 0)
    condicionador[6] = true;
  else
    condicionador[6] = false;
}

bool picPhotoactiveTrState() {
  uint32_t ds;
  picPhotoactiveTr.getPic(&ds);
  Serial.print("photoactivo: ");
  Serial.print(ds);
  return ds == 6;
}

void btMixTrPushCallback(void *ptr) {
  resetarPopupSenha();

  char buff[6] = {0};
  volumeTratamento.getText(buff, sizeof(buff));
  volumeTotal = atol(buff);

  if (!validarTratamento()) {
    return;
  }

  lerFatorCondicionador();
  opcao = TRATAMENTO;
  page11.show();
}

void btnProximoPopCallback(void *ptr) {
  //TODO validar senha
  char buffSenha[11] = {0};
  txtSenha.getText(buffSenha, sizeof(buffSenha));
  String strSenha(buffSenha);

  if (strSenha == NULL || strSenha.equals("")) {
    txtMsgGerenciar.setText("Senha obrigatoria");
    return;
  }

  if (procurarSenha(strSenha)) {
    resetarPopupSenha();
    if (opcao == RELES) {
      page7.show();
      mostrarConfiguracaoValvulas();
    }
    else if (opcao == CADASTRO)
      page8.show();
  } else {
    txtMsgGerenciar.setText("Senha nao encontrada");
  }  
}

void btnRelesPopCallback(void *ptr) {
  resetarPopupSenha();
  opcao = RELES;
  page12.show();
}

void btnAtualizarPushCallback(void *ptr) {
  salvarConfiguracaoValvulas();
  gotoPage0();
}

void btnCadastrarPushCallback(void *ptr) {
  cadastroOutput.Set_font_color_pco(63488);
  char buffNome[31] = {0};
  nomeProfissional.getText(buffNome, sizeof(buffNome));
  String strNomeProfissional(buffNome);

  if (strNomeProfissional == NULL || strNomeProfissional.equals("")) {
    cadastroOutput.setText("Nome obrigatorio");
    return;
  }
  char buffSenha[11] = {0};
  senhaProfissional.getText(buffSenha, sizeof(buffSenha));
  String strSenhaProfissional(buffSenha);

  if (strSenhaProfissional == NULL || strSenhaProfissional.equals("")) {
    cadastroOutput.setText("Senha obrigatoria");
    return;
  }

  char buffConfirma[11] = {0};
  confirmaSenha.getText(buffConfirma, sizeof(buffConfirma));
  String strConfirmaSenha(buffConfirma);

  if (!strSenhaProfissional.equals(strConfirmaSenha)) {
    cadastroOutput.setText("Senhas nao conferem");
    return;
  }

  if (procurarSenha(strSenhaProfissional)) {
    cadastroOutput.setText("Senha invalida.");
    return;
  }

  cadastrarProfissional(strNomeProfissional + ";" + strSenhaProfissional + "\n");
  cadastroOutput.Set_font_color_pco(1024);
  cadastroOutput.setText("Profissional cadastrado(a)");
  nomeProfissional.setText("");
  senhaProfissional.setText("");
  confirmaSenha.setText("");
}

bool procurarSenha(String password) {
  String filename = "bd.txt";
  if (!isSdOk)
    return;

  bool encontrou = false;

  File arquivo = SD.open(filename);
  if (arquivo) {
    while (arquivo.available()) {
      String nomeSenha = arquivo.readStringUntil('\n');
      int idxSeparador = nomeSenha.lastIndexOf(";");
      String senha = nomeSenha.substring(idxSeparador + 1);
      profissional = nomeSenha.substring(0, idxSeparador);
      if (password.equals(senha)) {
        encontrou = true;
        break;
      }
    }
    arquivo.close();
  } else {
    Serial.print(F("error opening "));
    Serial.println(filename);
  }
  if (encontrou == false)
    profissional = "";
  return encontrou;
}

void reenviarDadosTemporarios() {
  String arquivo = "tmp.txt";
  if (SD.exists(arquivo)) {
    File forigem = SD.open(arquivo);

    if (forigem) {
      while (forigem.available()) {
        String linha = forigem.readStringUntil('\n');
        enviarDadosWifi(linha);
      }
      forigem.close();
      SD.remove(arquivo);
    }
  }
}

void gravarSD(String msg, String filename) {
  if (!isSdOk)
    return;

  File file = SD.open(filename, FILE_WRITE);
  if (file) {
    Serial.println(F("Gravando no SD card: "));
    Serial.println(msg);
    file.print(msg);
    delay (1000);
    file.close();
  } else {
    Serial.print(F("Erro ao abrir arquivo "));
    Serial.println(filename);
  }
}

void lerSD(String filename) {
  if (!isSdOk)
    return;

  File myFile = SD.open(filename);
  if (myFile) {
    while (myFile.available()) {
      String nomeSenha = myFile.readStringUntil('\n');
      Serial.println(nomeSenha);
    }
    myFile.close();
  } else {
    Serial.print(F("error opening "));
    Serial.println(filename);
  }
}
void cadastrarProfissional(String str) {
  gravarSD(str, "bd.txt");
}

void btnConectarPushCallback(void *ptr) {
  char buff1[31] = {0};
  comboRede.getText(buff1, sizeof(buff1));
  String nomeRede(buff1);

  char buffSenha[21] = {0};
  txtSenha.getText(buffSenha, sizeof(buffSenha));
  String senha(buffSenha);

  txtConexao.setText("Conectando...");
  conectarWifi(nomeRede, senha);

  if (conectado) {
    redeConectada = nomeRede;
    String msg = "Conectado na rede " + nomeRede;
    char buff2[1024];
    msg.toCharArray(buff2, sizeof(buff2));
    txtConexao.Set_font_color_pco(1024);
    txtConexao.setText(buff2);

    if (SD.exists("wifi.txt")) {
      SD.remove("wifi.txt");
    }
    String nomeSenha = nomeRede + "\n" + senha + "\n";
    gravarSD(nomeSenha, "wifi.txt");
  } else {
    txtConexao.Set_font_color_pco(63488);
    String msg = "Falha ao conectar na rede " + nomeRede;
    char buff3[1024];
    msg.toCharArray(buff3, sizeof(buff3));
    txtConexao.setText(buff3);
    redeConectada = "";
  }
  txtSenha.setText("");
}
//---------Roda Rotina do Tratamento---------//
void lerFatorCondicionador() {
  uint32_t valor;

  slNutriTr.getValue(&valor);
  fator[0] = valor;

  slRepair.getValue(&valor);
  fator[1] = valor;

  slDensity.getValue(&valor);
  fator[2] = valor;

  slDiscipline.getValue(&valor);
  fator[3] = valor;

  slAntibreak.getValue(&valor);
  fator[4] = valor;

  slGrowth.getValue(&valor);
  fator[5] = valor;

  slCurly.getValue(&valor);
  fator[6] = valor;
}

void liberarCondicionador(int rele, unsigned long volume) {
  digitalWrite (rele, LOW);
  unsigned long fim = millis() + volume;
  while (millis() < fim) {
  }
  digitalWrite (rele, HIGH);
}

bool validarTratamento() {
  if (volumeTotal <= 0)
    return false;

  bool escolheuCondicionador = false;
  for (int i = 0; i < 7; i++) {
    if (condicionador[i]) {
      escolheuCondicionador = true;
      break;
    }
  }
  if (escolheuCondicionador == false)
    return false;

  return true;
}

void rodaTratamento() {
  unsigned long volumeCondicionador[7] = {0};
  unsigned long somaVolumeCondicionador = 0;
  unsigned long tempoReleFechado[7] = {0};

  for (int i = 0; i < 7; i++) {
    if (condicionador[i]) {
      volumeCondicionador[i] = volumeTotal * fator[i] * 0.01;
      somaVolumeCondicionador += volumeCondicionador[i];
      tempoReleFechado[i] = volumeCondicionador[i] * tempoValvulaCondicionador[i];
    }
  }

  unsigned long volumeBase = volumeTotal - somaVolumeCondicionador;

  unsigned long tempoReleBaseFechado = (unsigned long)tempoValvulaBase * volumeBase;

  if (tempoReleBaseFechado > 0) {
    page4.show();
    if (photoactive)
      digitalWrite (RELE8_PHOTOACTIVE, LOW);
    liberarCondicionador(RELE9_BASE, tempoReleBaseFechado);

    int n = 0;
    for (int i = 0, j = 40; i < 7; i++, j++) { // nutrition
      if (condicionador[i] && tempoReleFechado[i] > 0) {
        liberarCondicionador(j, tempoReleFechado[i]);
        n++;
      }
    }
    resetarRelesTratamento();
    page5.show();//retire seu produto

    gravarSD(prepararDadosSdTratamento(volumeBase, volumeCondicionador), "trat.txt");

    String dadosWifi = prepararDadosWifiTratamento(n, volumeCondicionador);
    if (wifiSerial.createTCP(hostIp, port)) {
      reenviarDadosTemporarios();
      delay(500);
      enviarDadosWifi(dadosWifi);
      wifiSerial.releaseTCP();
    } else {
      dadosWifi += "\n";
      gravarSD(dadosWifi, "tmp.txt");
    }
  }
  page0.show();
}

void resetarReles() {
  digitalWrite (RELE1_REGENERANT, HIGH);
  digitalWrite (RELE2_NUTRITION, HIGH);
  digitalWrite (RELE3_ANTIFRISO, HIGH);
  digitalWrite (RELE4_BIONEUTRAL, HIGH);
  digitalWrite (RELE8_PHOTOACTIVE, HIGH);
}

void resetarRelesTratamento() {
  digitalWrite (RELE16_NUTRI, HIGH);
  digitalWrite (RELE14_DENSITY, HIGH);
  digitalWrite (RELE15_REPAIR, HIGH);
  digitalWrite (RELE13_DISCIPLINE, HIGH);
  digitalWrite (RELE12_ANTIBREAK, HIGH);
  digitalWrite (RELE11_GROWTH, HIGH);
  digitalWrite (RELE10_CURLY, HIGH);
  digitalWrite (RELE9_BASE, HIGH);
  digitalWrite (RELE8_PHOTOACTIVE, HIGH);
}

void resetarVariaveisShampoo() {
  opcaoShampoo = NENHUM;
  photoactive = true;
  volume = 0;
  volumeShampoo.setText("");
}

void resetarVariaveisTratamento() {

  for (int i = 0; i < 7; i++)
    condicionador[i] = false;

  for (int i = 0; i < 7; i++)
    fator[i] = 0;

  photoactive = true;
  volumeTotal = 0;
}

//=========================CARTAO SD E RTC=====================================
void initRTC() {
  //Aciona o relogio
  Rtc.Begin();
}

void initSdCard() {
  pinMode(SS, OUTPUT);
  isSdOk = true;
  if (!SD.begin(SS)) {
    isSdOk = false;
    Serial.println(F("Error : sd card nao encontrado"));
  }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

String prepararDadosSdShampoo() {
  String msg = "Shampoo ";

  RtcDateTime now = Rtc.GetDateTime();
  char data[12];
  char hora[10];

  snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), now.Day(), now.Month(), now.Year());
  snprintf_P(hora, countof(hora), PSTR("%02u:%02u"), now.Hour(), now.Minute());

  switch (opcaoShampoo) {
    case REGENERANT:
      msg += "Regenerant";
      break;
    case NUTRITION:
      msg += "Nutrition";
      break;
    case ANTIFRISO:
      msg += "Antifriso";
      break;
    case BIONEUTRAL:
      msg += "Bioneutral";
      break;
  }
  msg += ";";
  msg += "volume:";
  msg += volume;
  msg += ";";
  msg += "photo.:";
  msg += photoactive;
  msg += ";";
  msg += "hora:";
  msg += hora;
  msg += ";";
  msg += "data:";
  msg += data;
  msg += ";";
  msg += "profissional:";
  msg += profissional;
  msg += ";";
  msg += "comanda:";
  msg += numeroComanda;
  msg += "\n";

  return msg;
}

String prepararDadosWifiShampoo() {
  String m = "profissional:";
  m += profissional;
  m += "_";
  m += "comanda:";
  m += numeroComanda;
  m += "_";
  m += "photo.:";
  m += photoactive;
  m += "_";
  m += "numProdutos:1";
  m += "_";
  m += "tipo:Shampoo";
  m += "_";
  m += "volumeTotal:";
  m += volume;
  m += "_";
  RtcDateTime now = Rtc.GetDateTime();
  char data[11];
  char hora[7];
  snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), now.Day(), now.Month(), now.Year());
  snprintf_P(hora, countof(hora), PSTR("%02u#%02u"), now.Hour(), now.Minute());
  m += "hora:";
  m += hora;
  m += "_";
  m += "data:";
  m += data;
  m += "_";

  switch (opcaoShampoo) {
    case REGENERANT:
      m += "idProduto:1";
      break;
    case NUTRITION:
      m += "idProduto:2";
      break;
    case ANTIFRISO:
      m += "idProduto:3";
      break;
    case BIONEUTRAL:
      m += "idProduto:4";
      break;
  }
  m += "_";
  m += "vol.:";
  m += volume;
  m += "_";
  m += "porc.:";
  m += 100;
  m += "&";

  return m;
}

String criarMensagemJsonShampoo() {
  StaticJsonDocument<200> doc;
  doc["profissional"] = profissional;
  doc["numeroComanda"] = numeroComanda;
  doc["photoactive"] = photoactive;
  doc["tipo"] = "Shampoo";
  doc["volumeTotal"] = volume;

  switch (opcaoShampoo) {
    case REGENERANT:
      doc["idProduto"] = 1;
      break;
    case NUTRITION:
      doc["idProduto"] = 2;
      break;
    case ANTIFRISO:
      doc["idProduto"] = 3;
      break;
    case BIONEUTRAL:

      break;
  }

  RtcDateTime now = Rtc.GetDateTime();
  char data[11];
  char hora[7];
  snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), now.Day(), now.Month(), now.Year());
  snprintf_P(hora, countof(hora), PSTR("%02u#%02u"), now.Hour(), now.Minute());

  doc["hora"] = hora;
  doc["data"] = data;
  doc["volume"] = volume;

  String json = "";
  serializeJson(doc, json);

  return json;
}

String prepararDadosSdTratamento(unsigned long volumeBase, unsigned long volumeCondicionador[]) {
  RtcDateTime now = Rtc.GetDateTime();

  char data[12];
  char hora[10];

  snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), now.Day(), now.Month(), now.Year());
  snprintf_P(hora, countof(hora), PSTR("%02u:%02u"), now.Hour(), now.Minute());

  String prods[] = {"Nutrition", "Repair", "Density", "Discipline", "Antibreak", "Growth", "Curly"};

  String msg = "Tratamento";

  for (int i = 0; i < 7; i++) {
    if (condicionador[i]) {
      msg += ";";
      msg += prods[i];
      msg += ";";
      msg += "fator:";
      msg += fator[i];
      msg += ";";
      msg += "volume:";
      msg += volumeCondicionador[i];
    }
  }

  msg += ";";
  msg += "volumeTotal:";
  msg += volumeTotal;
  msg += ";";
  msg += "volumeBase:";
  msg += volumeBase;
  msg += ";";
  msg += "photo.:";
  msg += photoactive;
  msg += ";";
  msg += "hora:";
  msg += hora;
  msg += ";";
  msg += "data:";
  msg += data;
  msg += ";";
  msg += "profissional:";
  msg += profissional;
  msg += ";";
  msg += "comanda:";
  msg += numeroComanda;
  msg += "\n";

  return msg;
}

String prepararDadosWifiTratamento(unsigned long n, unsigned long volumeCondicionador[]) {
  String m = "profissional:";
  m += profissional;
  m += "_";
  m += "comanda:";
  m += numeroComanda;
  m += "_";
  m += "photo.:";
  m += photoactive;
  m += "_";
  m += "numProdutos:";
  m += n;
  m += "_";
  m += "tipo:Tratamento";
  m += "_";
  m += "volumeTotal:";
  m += volumeTotal;
  RtcDateTime now = Rtc.GetDateTime();
  char data[11];
  char hora[7];
  snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), now.Day(), now.Month(), now.Year());
  snprintf_P(hora, countof(hora), PSTR("%02u#%02u"), now.Hour(), now.Minute());
  m += "_";
  m += "hora:";
  m += hora;
  m += "_";
  m += "data:";
  m += data;

  for (int i = 0, j = 5; i < 7; i++, j++) {
    if (condicionador[i]) {
      m += "_";
      m += "idProduto:";
      m += j;
      m += "_";
      m += "vol.:";
      m += volumeCondicionador[i];
      m += "_";
      m += "porc.:";
      m += fator[i];
    }
  }
  m += "&";
  return m;
}

String criarMensagemJsonTratamento(unsigned long n, unsigned long volumeCondicionador[]) {
  StaticJsonDocument<200> doc;
  doc["profissional"] = profissional;
  doc["numeroComanda"] = numeroComanda;
  doc["photoactive"] = photoactive;
  doc["tipo"] = "Tratamento";
  doc["volumeTotal"] = volumeTotal;

  RtcDateTime now = Rtc.GetDateTime();
  char data[11];
  char hora[7];
  snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), now.Day(), now.Month(), now.Year());
  snprintf_P(hora, countof(hora), PSTR("%02u#%02u"), now.Hour(), now.Minute());

  doc["hora"] = hora;
  doc["data"] = data;

  JsonObject objCondicionador = doc.createNestedObject();

  for (int i = 0, j = 5; i < 7; i++, j++) {
    if (condicionador[i]) {
      objCondicionador["idProduto"] = j;
      objCondicionador["volume"] = volumeCondicionador[i];
      objCondicionador["porcentagem"] = fator[i];
    }
  }
  String json = "";
  serializeJson(doc, json);

  return json;
}

//=======================CONFIGURACAO DAS VALVULAS=======================
void lerConfiguracaoValvulas() {
  int tempos[QTDVALVULAS] = {0};

  //Serial.println("lendo dados na eeprom");
  for (int i = 0, j = 0; i < QTDVALVULAS; i++) {
    EEPROM.get(j, tempos[i]);
    j += sizeof(int);
  }

  K_valvula_0 = tempos[0]; //rele 1
  K_valvula_1 = tempos[1]; //rele 2
  K_valvula_2 = tempos[2]; //rele 3
  K_valvula_11 = tempos[3]; //rele 4

  for (int i = 4, j = 0; i < 11; i++, j++)
    tempoValvulaCondicionador[j] = tempos[i];
  tempoValvulaBase = tempos[11];
}

void setarCampoTexto(int valor, NexText texto) {
  char buff[6] = {' '};
  itoa(valor, buff, 10);
  texto.setText(buff);
}

void mostrarConfiguracaoValvulas() {
  int tempos[QTDVALVULAS] = {0};

  //Serial.println("lendo dados na eeprom");
  for (int i = 0, j = 0; i < QTDVALVULAS; i++) {
    EEPROM.get(j, tempos[i]);
    j += sizeof(int);
  }
  setarCampoTexto(tempos[0], rele1);
  setarCampoTexto(tempos[1], rele2);
  setarCampoTexto(tempos[2], rele3);
  setarCampoTexto(tempos[3], rele4);

  setarCampoTexto(tempos[4], rele16);
  setarCampoTexto(tempos[5], rele15);
  setarCampoTexto(tempos[6], rele14);
  setarCampoTexto(tempos[7], rele13);

  setarCampoTexto(tempos[8], rele12);
  setarCampoTexto(tempos[9], rele11);
  setarCampoTexto(tempos[10], rele10);
  setarCampoTexto(tempos[11], rele9);
}

int retornaInteiro(NexText texto) {
  char buff[6] = {' '};
  texto.getText(buff, sizeof(buff));
  return atoi(buff);
}

void salvarConfiguracaoValvulas() {
  int tempos[QTDVALVULAS] = {0};
  char buff[6];

  //shampoo
  tempos[0] = retornaInteiro(rele1);
  tempos[1] = retornaInteiro(rele2);
  tempos[2] = retornaInteiro(rele3);
  tempos[3] = retornaInteiro(rele4);
  //tratamento
  tempos[4] = retornaInteiro(rele16);
  tempos[5] = retornaInteiro(rele15);
  tempos[6] = retornaInteiro(rele14);
  tempos[7] = retornaInteiro(rele13);
  tempos[8] = retornaInteiro(rele12);
  tempos[9] = retornaInteiro(rele11);
  tempos[10] = retornaInteiro(rele10);
  tempos[11] = retornaInteiro(rele9);

  Serial.println(F("Salvando dados na eeprom"));
  for (int i = 0, j = 0; i < QTDVALVULAS; i++) {
    EEPROM.put(j, tempos[i]);
    j += sizeof(int);
  }
  Serial.println(F("dados salvos"));

  lerConfiguracaoValvulas();
}

void salvarConfiguracaoValvula(int k, NexText rele) { //comecar k em 0
  //
  int tempo = retornaInteiro(rele);

  Serial.println(F("Salvando dado na eeprom"));
  int j = 0;
  for (int i = 0; i < k; i++) {
    j += sizeof(int);
  }
  EEPROM.put(j, tempo);
  Serial.println(F("dado salvo"));

  lerConfiguracaoValvulas();
}

void inicializarEeprom() {
  Serial.println(F("Salvando dados na eeprom"));
  for (int i = 0, j = 0; i < QTDVALVULAS; i++) {
    EEPROM.put(j, 0);
    j += sizeof(int);
  }
  Serial.println(F("dados salvos"));
}

//===============================WIFI=============================================
void conectarWifi(String nomeRede, String senha) {

  wifiSerial.restart();
  conectado = false;

  int tentativas = 5;
  for (int i = 0; i < tentativas; i++) {
    if (wifiSerial.setOprToStation()) {
      Serial.print(F("to station ok\r\n"));
      break;
    } else {
      Serial.print(F("to station err\r\n"));
    }
  }

  for (int i = 0; i < tentativas; i++) {

    if (wifiSerial.joinAP(nomeRede, senha)) {
      conectado = true;
      Serial.print(F("Conectado na rede "));
      Serial.print(nomeRede);
      Serial.println();
      Serial.print(F("IP: "));
      Serial.println(wifiSerial.getLocalIP().c_str());
      break;
    } else {
      Serial.print(F("Join AP failure\r\n"));
    }
  }
}

void desconectarWifi() {
  wifiSerial.leaveAP();
}

void enviarDadosWifi(String msg) {
  wifiSerial.send(msg.c_str(), msg.length());
}

void iniciarWifi() {
  String arquivo = "wifi.txt";
  String nomeRede = "";
  String senha = "";
  if (SD.exists(arquivo)) {
    File forigem = SD.open(arquivo);
    if (forigem) {
      nomeRede = forigem.readStringUntil('\n');
      senha = forigem.readStringUntil('\n');
      forigem.close();
      conectarWifi(nomeRede, senha);
      if (conectado)
        redeConectada = nomeRede;
    }
  }
}

//============================================================================
void btnIniciarPopCallback(void *ptr) {
  char buff[11] = {0};
  txtComanda.getText(buff, sizeof(buff));
  String strComanda(buff);
  numeroComanda = strComanda;

  char buffSenha[11] = {0};
  txtPassword.getText(buffSenha, sizeof(buffSenha));
  String strSenha(buffSenha);

  if (strComanda == NULL || strComanda.equals("")) {
    txtMsg.setText("Comanda obrigatoria");
    return;
  }

  if (strSenha == NULL || strSenha.equals("")) {
    txtMsg.setText("Senha obrigatoria");
    return;
  }

  if (procurarSenha(strSenha)) {
    resetarPopupSenha();
    if (opcao == SHAMPOO) {
      rodaShampoo();
    }
    else if (opcao == TRATAMENTO)
      rodaTratamento();
  } else {
    txtMsg.setText("Senha nao encontrada");
  }
}

void btnShampooPushCallback(void *ptr) {
  resetarVariaveisShampoo();
}

void btnTreatmentPushCallback(void *ptr) {
  resetarVariaveisTratamento();
}

void resetarPopupSenha() {
  txtComanda.setText("");
  txtPassword.setText("");
  txtSenhaGerenciar.setText("");
}

void btnFecharGerenciarPopCallback(void *ptr) {
  page0.show();
}

void btnCadastroPopCallback(void *ptr) {
  resetarPopupSenha();
  opcao = CADASTRO;
  page12.show();
}

void btnFecharPopCallback(void *ptr) {
  resetarVariaveisShampoo();
  resetarVariaveisTratamento();
  resetarPopupSenha();

  if (opcao == SHAMPOO)
    page1.show();
  else if (opcao == TRATAMENTO)
    page2.show();
}

void btnWifiPopCallback(void *ptr) {
  page9.show();
  txtIP.setText("");
  txtSenha.setText("");

  txtConexao.setText("Procurando redes wifi...");
  buscarRedesDisponiveis();

  txtConexao.setText("Lendo IP remoto...");
  lerIpComputadorRemoto();

  txtConexao.setText("Verificando conexao...");
  String ip = "8.8.8.8";
  bool pingou = wifiSerial.ping(ip);
  if (pingou) {
    txtConexao.Set_font_color_pco(1024);
    txtConexao.setText("Conectado");
    char buffR[128];
    redeConectada.toCharArray(buffR, sizeof(buffR));
    comboRede.setText(buffR);
  } else {
    txtConexao.Set_font_color_pco(63488);
    txtConexao.setText("Desconectado");
    redeConectada = "";
    comboRede.setText("");
  }

}

void buscarRedesDisponiveis() {
  String str = wifiSerial.getAPList();
  String redes = "";

  char buff[1024];
  str.toCharArray(buff, 1024);

  char *pt;

  pt = strtok(buff, "+");
  while (pt) {
    String r = getNomeRede(pt);
    if (r.length() > 0)
      redes += r;
    pt = strtok(NULL, "+");
  }

  char redesBuff[1024];
  redes.toCharArray(redesBuff, 1024);
  redesBuff[strlen(redesBuff) - 1] = '\0'; //impede ultima linha vazia no combobox
  comboRede.setPath(redesBuff);
}

String getNomeRede(char * ptr) {
  int len = strlen(ptr);
  if (len < 10)
    return "";

  String nome = "";
  for (int i = 10; i < len; i++) {
    if (ptr[i] == '\"')
      break;
    nome += ptr[i];
  }

  return nome + "\r\n";
}

void lerIpComputadorRemoto() {
  if (!isSdOk)
    return;

  File myFile = SD.open("ip.txt");
  if (myFile) {
    String ip = myFile.readStringUntil('\n');
    hostIp = ip;
    char buff[17];
    ip.toCharArray(buff, 17);
    txtIP.setText(buff);
    myFile.close();
  } else {
    Serial.print(F("error opening "));
    Serial.println(F("ip.txt"));
  }
}

void btnGravarPopCallback(void *ptr) {
  char buff[17] = {0};
  txtIP.getText(buff, sizeof(buff));
  String ip(buff);
  hostIp = ip;

  if (SD.exists("ip.txt")) {
    SD.remove("ip.txt");
  }

  gravarSD(hostIp, "ip.txt");

  txtConexao.setText("IP gravado");
}
//===========================================================================
void btnRele1AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(0, rele1);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)K_valvula_0 * VOLUME_TESTE;
  liberarCondicionador(RELE1_REGENERANT, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele2AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(1, rele2);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)K_valvula_1 * VOLUME_TESTE;
  liberarCondicionador(RELE2_NUTRITION, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele3AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(2, rele3);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)K_valvula_2 * VOLUME_TESTE;
  liberarCondicionador(RELE3_ANTIFRISO, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele4AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(3, rele4);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)K_valvula_11 * VOLUME_TESTE;
  liberarCondicionador(RELE4_BIONEUTRAL, tempoFechado);
  txtMsgAt.setText("");
}

void btnRele9AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(11, rele9);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaBase * VOLUME_TESTE;
  liberarCondicionador(RELE9_BASE, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele16AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(4, rele16);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaCondicionador[0] * VOLUME_TESTE;
  liberarCondicionador(RELE16_NUTRI, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele15AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(5, rele15);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaCondicionador[1] * VOLUME_TESTE;
  liberarCondicionador(RELE15_REPAIR, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele14AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(6, rele14);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaCondicionador[2] * VOLUME_TESTE;
  liberarCondicionador(RELE14_DENSITY, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele13AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(7, rele13);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaCondicionador[3] * VOLUME_TESTE;
  liberarCondicionador(RELE13_DISCIPLINE, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele12AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(8, rele12);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaCondicionador[4] * VOLUME_TESTE;
  liberarCondicionador(RELE12_ANTIBREAK, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele11AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(9, rele11);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaCondicionador[5] * VOLUME_TESTE;
  liberarCondicionador(RELE11_GROWTH, tempoFechado);
  txtMsgAt.setText("");
}
void btnRele10AtPopCallback(void *ptr) {
  salvarConfiguracaoValvula(10, rele10);
  txtMsgAt.setText("Atualizado. Aguarde...");
  unsigned long tempoFechado = (unsigned long)tempoValvulaCondicionador[6] * VOLUME_TESTE;
  liberarCondicionador(RELE10_CURLY, tempoFechado);
  txtMsgAt.setText("");
}

//============================================================================
void gotoPage0() {
  page0.show();
}

void definirSaidas() {
  pinMode (RELE1_REGENERANT, OUTPUT);
  pinMode (RELE2_NUTRITION, OUTPUT);
  pinMode (RELE3_ANTIFRISO, OUTPUT);
  pinMode (RELE4_BIONEUTRAL, OUTPUT);
  pinMode (RELE8_PHOTOACTIVE, OUTPUT);

  pinMode (RELE16_NUTRI, OUTPUT);
  pinMode (RELE14_DENSITY, OUTPUT);
  pinMode (RELE15_REPAIR, OUTPUT);
  pinMode (RELE13_DISCIPLINE, OUTPUT);
  pinMode (RELE12_ANTIBREAK, OUTPUT);
  pinMode (RELE11_GROWTH, OUTPUT);
  pinMode (RELE10_CURLY, OUTPUT);
  pinMode (RELE9_BASE, OUTPUT);
}

NexTouch *nex_listen_list[] = {
  &btRegenerant,
  &btNutrition,
  &btAntiFriso,
  &btBioNeutral,
  &btMix,
  &btNutriTr,
  &btRepair,
  &btDensity,
  &btDiscipline,
  &btAntibreak,
  &btGrowth,
  &btCurly,
  &btMixTr,
  &slNutriTr,
  &slRepair,
  &slDensity,
  &slDiscipline,
  &slAntibreak,
  &slGrowth,
  &slCurly,
  &btnReles,
  &btnAtualizar,
  &btnCadastrar,
  &btnConectar,
  &btnIniciar,
  &btnShampoo,
  &btnTreatment,
  &btnFechar,
  &btnWifi,
  &btnGravar,
  &btnRele1At,
  &btnRele2At,
  &btnRele3At,
  &btnRele4At,
  &btnRele9At,
  &btnRele10At,
  &btnRele11At,
  &btnRele12At,
  &btnRele13At,
  &btnRele14At,
  &btnRele15At,
  &btnRele16At,
  &btPhotoactiveSh,
  &btPhotoactiveTr,
  &btnProximo,
  &btnFecharGerenciar,
  &btnCadastro,
  NULL
};

void setup() {
  Serial.begin(9600);
  
  nexInit();

  definirSaidas();
  resetarReles();
  resetarRelesTratamento();
  resetarVariaveisShampoo();
  resetarVariaveisTratamento();
  //inicializarEeprom();
  lerConfiguracaoValvulas();
  initSdCard();
  initRTC();
  desconectarWifi(); //modulo conecta na ultima rede assim que ligado
  lerIpComputadorRemoto();

  iniciarWifi();

  btnReles.attachPop(btnRelesPopCallback);
  btnCadastrar.attachPush(btnCadastrarPushCallback, &btnCadastrar);
  btnAtualizar.attachPush(btnAtualizarPushCallback, &btnAtualizar);
  btnConectar.attachPush(btnConectarPushCallback, &btnConectar);

  btRegenerant.attachPush(btRegenerantPushCallback, &btRegenerant);
  btNutrition.attachPush(btNutritionPushCallback, &btNutrition);
  btAntiFriso.attachPush(btAntiFrisoPushCallback, &btAntiFriso);
  btBioNeutral.attachPush(btBioNeutralPushCallback, &btBioNeutral);
  btMix.attachPush(btMixPushCallback, &btMix);

  btNutriTr.attachPush(btNutriTrPushCallback, &btNutriTr);
  btRepair.attachPush(btRepairPushCallback, &btRepair);
  btDensity.attachPush(btDensityPushCallback, &btDensity);
  btDiscipline.attachPush(btDisciplinePushCallback, &btDiscipline);
  btAntibreak.attachPush(btAntibreakPushCallback, &btAntibreak);
  btGrowth.attachPush(btGrowthPushCallback, &btGrowth);
  btCurly.attachPush(btCurlyPushCallback, &btCurly);
  btMixTr.attachPush(btMixTrPushCallback, &btMixTr);

  slNutriTr.attachPop(slNutriTrPopCallback);
  slRepair.attachPop(slRepairPopCallback);
  slDensity.attachPop(slDensityPopCallback);
  slDiscipline.attachPop(slDisciplinePopCallback);
  slAntibreak.attachPop(slAntibreakPopCallback);
  slGrowth.attachPop(slGrowthPopCallback);
  slCurly.attachPop(slCurlyPopCallback);

  btPhotoactiveSh.attachPush(btPhotoactiveShPushCallback, &btPhotoactiveSh);
  btPhotoactiveTr.attachPush(btPhotoactiveTrPushCallback, &btPhotoactiveTr);

  btnIniciar.attachPop(btnIniciarPopCallback);
  btnShampoo.attachPush(btnShampooPushCallback, &btnShampoo);
  btnTreatment.attachPush(btnTreatmentPushCallback, &btnTreatment);
  btnFechar.attachPop(btnFecharPopCallback);
  btnFecharGerenciar.attachPop(btnFecharGerenciarPopCallback);
  btnWifi.attachPop(btnWifiPopCallback);
  btnGravar.attachPop(btnGravarPopCallback);
  btnProximo.attachPop(btnProximoPopCallback);
  btnRele1At.attachPop(btnRele1AtPopCallback);
  btnRele2At.attachPop(btnRele2AtPopCallback);
  btnRele3At.attachPop(btnRele3AtPopCallback);
  btnRele4At.attachPop(btnRele4AtPopCallback);
  btnRele9At.attachPop(btnRele9AtPopCallback);
  btnRele10At.attachPop(btnRele10AtPopCallback);
  btnRele11At.attachPop(btnRele11AtPopCallback);
  btnRele12At.attachPop(btnRele12AtPopCallback);
  btnRele13At.attachPop(btnRele13AtPopCallback);
  btnRele14At.attachPop(btnRele14AtPopCallback);
  btnRele15At.attachPop(btnRele15AtPopCallback);
  btnRele16At.attachPop(btnRele16AtPopCallback);

  btnCadastro.attachPop(btnCadastroPopCallback);
}

void loop() {
  nexLoop(nex_listen_list);
  delay(150);
}
