#include "Nextion.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <EEPROM.h>
#include "ESP8266.h"
#include <ArduinoJson.h>
//#include <TimeLib.h>
#include <UnixTime.h>

UnixTime stamp(-3);
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

#define PIC_CONECTADO 18
#define PIC_DESCONECTADO 19

#define TENTATIVAS_CONEXAO 3
#define TENTATIVAS_NTP 5

#define interrupcao 2

// page0
NexButton btnReles = NexButton(0, 3, "btnReles");
NexButton btnShampoo = NexButton(0, 1, "bt0");
NexButton btnTreatment = NexButton(0, 2, "bt1");
NexButton btnWifi = NexButton(0, 5, "btWifi");
NexButton btnCadastro = NexButton(0, 4, "btnCadastro");
NexButton btnMapping = NexButton(0, 6, "bt2");
NexPicture picWifi = NexPicture(0, 7, "p0");
NexText txtPage0 = NexText(0, 8, "t0");
bool inPage0 = false;

// page1
NexDSButton btRegenerant = NexDSButton(1, 5, "bt1");
NexDSButton btNutrition = NexDSButton(1, 6, "bt2");
NexDSButton btAntiFriso = NexDSButton(1, 7, "bt3");
NexDSButton btBioNeutral = NexDSButton(1, 8, "bt4");
NexText volumeShampoo = NexText(1, 2, "t0");

NexButton btMix = NexButton(1, 4, "bt6");
NexPicture picPhotoactiveSh = NexPicture(1, 9, "p0");
NexButton btPhotoactiveSh = NexButton(1, 1, "bt0");
NexButton btnVoltar1 = NexButton(1, 3, "bt5");

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

NexButton btnVoltar2 = NexButton(2, 2, "bt8");

//page3
NexText msgLoading = NexText(3, 2, "t0");

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
NexButton btnVoltar7 = NexButton(7, 13, "b0");

// page7 cadastro de usuarios
NexText cadastroOutput = NexText(8, 16, "t8"); //Funcionou com 8.
NexText nomeProfissional = NexText(8, 9, "t6");
NexText senhaProfissional = NexText(8, 8, "t5");
NexText confirmaSenha = NexText(8, 10, "t7");
NexButton btnCadastrar = NexButton(8, 3, "btnCadastrar");
NexCheckbox cProfissional = NexCheckbox(8, 14, "c2");
NexCheckbox cGerente = NexCheckbox(8, 11, "c0");
NexCheckbox cTecnico = NexCheckbox(8, 12, "c1");
NexButton btnGerenciar = NexButton(8, 17, "b0");
NexText txtTecnico = NexText(8, 13, "t9");
NexButton btnVoltar8 = NexButton(8, 2, "btnVoltar");

//page8 conexao com a internet
NexComboBox comboRede = NexComboBox(9, 10, "txtRede");//Funcionou com 9.
NexText txtSenha = NexText(9, 4, "txtSenha");
NexText txtConexao = NexText(9, 7, "txtConexao");
NexButton btnConectar = NexButton(9, 6, "btnConectar");
NexText txtData = NexText(9, 11, "txtData");
NexText txtHora = NexText(9, 10, "txtHora");
NexButton btnSincr = NexButton(9, 9, "btnSincr");
NexButton btnVoltar9 = NexButton(9, 5, "b0");

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

//page11 
NexDataRecord tabela = NexDataRecord(13, 2,"data0");
NexButton btnInativar = NexButton(13, 1, "btnInativar");

//page12
NexDSButton btnCurto = NexDSButton(14, 3, "bt3");
NexDSButton btnMedio = NexDSButton(14, 4, "bt4");
NexDSButton btnLongo = NexDSButton(14, 5, "bt5");
NexButton btMixMap = NexButton(14, 2, "btMix");
NexButton btnVoltar14 = NexButton(14, 1, "btVoltar");

// paginas
NexPage page0 = NexPage(0, 0, "page0");
NexPage page1 = NexPage(1, 0, "page1");
NexPage page2 = NexPage(2, 0, "page2");
NexPage page3 = NexPage(3, 0, "page3");
NexPage page4 = NexPage(4, 0, "page4");
NexPage page5 = NexPage(5, 0, "page5");
NexPage page11 = NexPage(11, 0, "page9");
NexPage page7 = NexPage(7, 0, "page6");
NexPage page9 = NexPage(9, 0, "page8");
NexPage page12 = NexPage(12, 0, "page10");
NexPage page8 = NexPage(8, 0, "page7");
NexPage page13 = NexPage(13, 0, "page11");
NexPage page14 = NexPage(14, 0, "page12");

//variaveis shampoo
enum shampoo {REGENERANT, NUTRITION, ANTIFRISO, BIONEUTRAL, NENHUM};
shampoo opcaoShampoo;

enum funcionalidade {SHAMPOO, TRATAMENTO, RELES, CADASTRO, MAPPING};
funcionalidade opcao;

//variaveis shampoo
enum mapping {CURTO, MEDIO, LONGO, NONE};
mapping opcaoMapping;

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

unsigned long timeOffset = -3;
const int minutosOffset = 2;
volatile bool modoStatus;
//===================wifi===============
bool conectado = false;
String redeConectada = "";
String senhaConectada = "";

const int MAX_COMANDAS_OFFLINE = 20;

String HOST_NAME = "teste.k08.com.br";
int HOST_PORT = 80;
//=======================================
String profissional = "";
String numeroComanda = "";
//=======================BDs====================
const String BDPROFISSIONAIS = "PRO.TXT";
const String BDGERENTES = "GER.TXT";
const String BDTECNICOS = "TEC.TXT";
const String BDMASTER = "MTR.TXT";
//=======================CODIGOS===============
String codMaquina = "";
String codSalao = "";
String codUsuario = "";
//=======================VOLUMES===============
unsigned long volume = 0;
unsigned long volumeTotal = 0;
//=========================================================

void btPhotoactiveTrPushCallback(void *ptr) {
  photoactive = !photoactive;
}

void btPhotoactiveShPushCallback(void *ptr) {
  photoactive = !photoactive;
}
//======================================RESETS==========================================
void resetarPopupSenha() {
  txtComanda.setText("");
  txtPassword.setText("");
  txtSenhaGerenciar.setText("");
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
//=====================================SHAMPOO==========================================
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
//=====================================BOTOES TRATAMENTO==========================================
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
//================================================================================================
void btnProximoPopCallback(void *ptr) {
  char buffSenha[11] = {0};
  txtSenha.getText(buffSenha, sizeof(buffSenha));
  String strSenha(buffSenha);

  if (strSenha == NULL || strSenha.equals("")) {
    txtMsgGerenciar.setText("Senha obrigatoria");
    return;
  }

  if (opcao == RELES) {
    if (procurarSenha(strSenha, BDTECNICOS)) {
      resetarPopupSenha();
      page7.show();
      mostrarConfiguracaoValvulas();
    } else {
      txtMsgGerenciar.setText("Senha nao encontrada");
    }
  } else if (opcao == CADASTRO) {
    txtTecnico.setOpaqueness(0);
    cTecnico.setOpaqueness(0);
    if (procurarSenha(strSenha, BDMASTER)) {
      txtTecnico.setOpaqueness(127);
      cTecnico.setOpaqueness(127);
      resetarPopupSenha();
      page8.show();
    }
    else if (procurarSenha(strSenha, BDGERENTES)) {
      resetarPopupSenha();
      page8.show();
    } else {
      txtMsgGerenciar.setText("Senha nao encontrada");
    }
  }
}

void btnRelesPushCallback(void *ptr) {  
  resetarPopupSenha();
  opcao = RELES;
  page12.show();
  inPage0 = false;
}

void btnAtualizarPushCallback(void *ptr) {
  salvarConfiguracaoValvulas();
  gotoPage0();
}

//==========================================CADASTRAR USUARIO=====================================
void btnCadastrarPopCallback(void *ptr) {
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

  uint32_t iProfissional;
  uint32_t iGerente;
  uint32_t iTecnico;

  cProfissional.getValue(&iProfissional);
  cGerente.getValue(&iGerente);
  cTecnico.getValue(&iTecnico);

  if (iProfissional == 0 && iGerente == 0 && iTecnico == 0) {
    cadastroOutput.setText("Escolha ao menos um perfil");
    return;
  }

  if (iProfissional == 1)
    if (procurarSenha(strSenhaProfissional, BDPROFISSIONAIS)) {
      cadastroOutput.setText("Senha invalida.");
      return;
    }
  if (iTecnico == 1)
    if (procurarSenha(strSenhaProfissional, BDTECNICOS)) {
      cadastroOutput.setText("Senha invalida.");
      return;
    }
  if (iGerente == 1)
    if (procurarSenha(strSenhaProfissional, BDGERENTES)) {
      cadastroOutput.setText("Senha invalida.");
      return;
    }
  cadastrarProfissional(strNomeProfissional + ";" + strSenhaProfissional + ";"+"A"+"\n");
  cadastroOutput.Set_font_color_pco(1024);
  cadastroOutput.setText("Profissional cadastrado(a)");
  nomeProfissional.setText("");
  senhaProfissional.setText("");
  confirmaSenha.setText("");
  cProfissional.setValue(0);
  cGerente.setValue(0);
  cTecnico.setValue(0);
}

bool procurarSenha(String password, String filename) {
  bool encontrou = false;
  if (!isSdOk)
    return encontrou;

  File arquivo = SD.open(filename);
  if (arquivo) {
    while (arquivo.available()) {
      String nomeSenha = arquivo.readStringUntil('\n');
      int idxSeparador = nomeSenha.lastIndexOf(";");
      int iniSeparador = nomeSenha.indexOf(";");
      int idSeparador = nomeSenha.indexOf("#"); 
      
      String senha = nomeSenha.substring(iniSeparador+1, idxSeparador);
      profissional = nomeSenha.substring(idSeparador+1, iniSeparador);
	    codUsuario = nomeSenha.substring(0, idSeparador);
      char s = nomeSenha.charAt(idxSeparador+1);
 
      if (password.equals(senha) && s == 'A') {
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


void cadastrarProfissional(String str) {
  uint32_t iProfissional;
  uint32_t iGerente;
  uint32_t iTecnico;

  cProfissional.getValue(&iProfissional);
  cGerente.getValue(&iGerente);
  cTecnico.getValue(&iTecnico);

  int linhas[3] = {0}; 
  linhas[0] = contarLinhas(BDPROFISSIONAIS); 
  linhas[1] = contarLinhas(BDGERENTES); 
  linhas[2] = contarLinhas(BDTECNICOS); 
   
  int id = -1; 
  for(int i = 0; i < 3; i++){ 
	  if(linhas[i] > id) 
		  id = linhas[i]; 
  } 
   
  str = String(id) + "#" + str; 

  if (iProfissional == 1)
    gravarSD(str, BDPROFISSIONAIS);

  if (iGerente == 1)
    gravarSD(str, BDGERENTES);

  if (iTecnico == 1)
    gravarSD(str, BDTECNICOS);
}
//===========================================================================================================
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

//========================= RTC ==========================================
#define countof(a) (sizeof(a) / sizeof(a[0]))

void initRTC(){
   Rtc.Begin();
   if (!Rtc.GetIsRunning())
   {
       Serial.println(F("RTC was not actively running, starting now"));
       Rtc.SetIsRunning(true);
   }
   Rtc.Enable32kHzPin(false);
   Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne); //Habilta 1 alarme
}

void interrupcaoRtc(){
  detachInterrupt(digitalPinToInterrupt(interrupcao));  
  modoStatus=true;    
}

void setarAlarme(){
  int minutos = Rtc.GetDateTime().Minute() + minutosOffset; 
  if(minutos >= 60){
    minutos = minutos % 60;
  }
    
  DS3231AlarmOne alarm1(0, 0, minutos, 0, DS3231AlarmOneControl_MinutesSecondsMatch);
  Rtc.SetAlarmOne(alarm1);
  Rtc.LatchAlarmsTriggeredFlags();// Efetiva os alarmes
  attachInterrupt(digitalPinToInterrupt(interrupcao), interrupcaoRtc, FALLING);
  
  Serial.print("alarme ");
  Serial.print(Rtc.GetDateTime().Hour());
  Serial.print(":");
  Serial.println(minutos);  
}

bool sincronizarServidorNtp(){
    bool registrou = false;
    int tentativas  = 0;
    while(!registrou && tentativas < TENTATIVAS_NTP){
      Serial.println(F("registrando udp."));
      registrou = wifiSerial.registerUDP("201.49.148.135", 123); //a.st1.ntp.br
      tentativas++;
    }
    Serial.println(F("registrou udp."));
    byte ntpData[48] = {0};
    byte buffr[48] = {0};
    //ntpData[0] = 0x1B;
    ntpData[0] = 0b11100011;   // LI, Version, Mode
    ntpData[1] = 0;     // Stratum, or type of clock
    ntpData[2] = 6;     // Polling Interval
    ntpData[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    ntpData[12]  = 49;
    ntpData[13]  = 0x4E;
    ntpData[14]  = 49;
    ntpData[15]  = 52;

    bool enviou = false;
    uint32_t len = 0;
    tentativas  = 0;
    while(len == 0 && tentativas < TENTATIVAS_NTP){
      Serial.println(F("enviando requisicao."));
      wifiSerial.send(ntpData, 48);
      len = wifiSerial.recv(buffr, sizeof(buffr), 5000);
      tentativas++;
    }
    
    if (len > 0) {
      Serial.println(F("resposta recebida do servidor."));
      Serial.print(F("len: "));
      Serial.print(len);
      Serial.println(F(" bytes received. "));      
    }else{
      Serial.println(F("sem resposta do servidor."));
      return false;
    }
    
    if (wifiSerial.unregisterUDP()) {
        Serial.print(F("unregister udp ok\r\n"));
    }

    unsigned long highWord = word(buffr[40], buffr[41]);
    unsigned long lowWord = word(buffr[42], buffr[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    Serial.print(F("seconds since 1900: "));
    Serial.println(secsSince1900);

    unsigned long SEVENZYYEARS = 2208988800;//70 years = 2,208,988,800 seconds

    unsigned long currentEpoc = secsSince1900 - SEVENZYYEARS;// + timeOffset*3600;

    stamp.getDateTime(currentEpoc);
        
    return true;  
}

void ajustarRelogio(){
    RtcDateTime compiled = RtcDateTime(stamp.year, stamp.month, stamp.day, stamp.hour, stamp.minute, stamp.second, false);
    Rtc.SetDateTime(compiled);
}

void btnSincrPopCallback(void *ptr) {

  if(conectado){
    txtConexao.setText("Conectado. Sincronizando relogio...");
    if(sincronizarServidorNtp()){
      ajustarRelogio();
      setarAlarme();
	    mostrarDataHora();
      txtConexao.setText("Conectado. Relogio sincronizado.");
    }else{
      txtConexao.setText("Nao sincronizou com servidor NTP.");
    }
  }else{
    txtConexao.setText("Conecte para sincronizar o relogio.");
  }
}

void mostrarDataHora(){	
	RtcDateTime myrtc = Rtc.GetDateTime();
    char data[11];
    char hora[7];
    snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), myrtc.Day(), myrtc.Month(), myrtc.Year());
    snprintf_P(hora, countof(hora), PSTR("%02u:%02u"), myrtc.Hour(), myrtc.Minute());
	
	txtData.setText(data);
	txtHora.setText(hora);
}

//=========================CARTAO SD =====================================
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

void initSdCard() {
  pinMode(SS, OUTPUT);
  isSdOk = true;
  if (!SD.begin(SS)) {
    isSdOk = false;
    Serial.println(F("Error : sd card nao encontrado"));
  }
}



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
    case NENHUM:
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

void lerDadosBasicos(){
   if (!isSdOk)
    return;

  File myFile = SD.open("INF.txt");
  if (myFile) {
    String info = myFile.readStringUntil('\n');
    int idxSeparador = info.indexOf(":");
    codMaquina = info.substring(idxSeparador+1);

    info = myFile.readStringUntil('\n');
    idxSeparador = info.indexOf(":");
    codSalao = info.substring(idxSeparador+1);
    
    myFile.close();
  } else {
    Serial.print(F("error opening "));
    Serial.println(F("INF.txt"));
  }
}

bool iniciarConectar() {

  if (!isSdOk)
    return false;

  String str = "";

  int tentativas = 0;
  msgLoading.setText("Procurando redes wifi...");

  while(str.length() == 0 && tentativas < TENTATIVAS_CONEXAO){
    str = wifiSerial.getAPList();
    tentativas++;
  }

  str.trim();
  
  if(str.length() == 0){
    msgLoading.setText("Nenhuma rede encontrada");
    delay(2000);
    return false;
  }
  
  String redes[10] = {""};

  char buff[1024];
  str.toCharArray(buff, 1024);

  char *pt;
  int i = 0;
    
  pt = strtok(buff, "+");
  while (pt && i < 10) {
    String r = getNomeRede(pt);
    if (r.length() > 0)
      redes[i++] = r.substring(0, r.indexOf('\r')); 
    pt = strtok(NULL, "+");
  }
  
  
  String filename = "wifi.txt";
  String msg = ""; 

  if (!SD.exists(filename)) {
    return false;
  } 

  bool conectou = false;

  File arquivo = SD.open(filename);
  if (arquivo) {
    while (arquivo.available()) {
      String redeNome = arquivo.readStringUntil('\n');      
      String redeSenha = arquivo.readStringUntil('\n');

      for(int j = 0; j < i; j++){       
        if (redes[j].equals(redeNome)) {
          
          msg = "Conectando na rede "+redeNome+"...";
          msgLoading.setText(msg.c_str());
          conectou = conectarWifi(redeNome, redeSenha, TENTATIVAS_CONEXAO);
          if(conectou){
             redeConectada = redeNome;
             senhaConectada = redeSenha; 
             break;
          }
        }
      }
      if(conectou)
        break;
    }
    arquivo.close();
  } else {
    Serial.print(F("error opening "));
    Serial.println(filename);
  }

  if(!conectou){
    redeConectada = "";
    senhaConectada = "";
  }
  
  return conectou;
}

bool procurarRede(String rede, String senha, String filename) {
  bool encontrou = false;
  if (!isSdOk)
    return encontrou;

  File arquivo = SD.open(filename);
  if (arquivo) {
    while (arquivo.available()) {
      String redeNome = arquivo.readStringUntil('\n');      
      String redeSenha = arquivo.readStringUntil('\n');
 
      if (rede.equals(redeNome) && senha.equals(redeSenha)) {
        encontrou = true;
        break;
      }
    }
    arquivo.close();
  } else {
    Serial.print(F("error opening "));
    Serial.println(filename);
  }
  return encontrou;
}

String prepararDadosSdMapping() {
  String msg = "Mapping ";

  RtcDateTime now = Rtc.GetDateTime();
  char data[12];
  char hora[10];

  snprintf_P(data, countof(data), PSTR("%02u/%02u/%04u"), now.Day(), now.Month(), now.Year());
  snprintf_P(hora, countof(hora), PSTR("%02u:%02u"), now.Hour(), now.Minute());

   switch (opcaoMapping) {
    case CURTO:
      volume = 15;      
      break;
    case MEDIO:
      volume = 24;
      break;
    case LONGO:
      volume = 33;
      break;
    case NENHUM:
      break;
  }
  
  msg += ";";
  msg += "volume:";
  msg += volume;
  msg += ";";
  msg += "photo.:";
  msg += false;
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
//==========================================JSON===========================================================
String criarMensagemJsonShampoo() {
  StaticJsonDocument<200> doc;
  doc["codMaquina"] = codMaquina.toInt();
  doc["codSalao"] = codSalao.toInt();
  doc["codUsuario"] = codUsuario.toInt();
  doc["nomeUsuario"] = profissional;
  doc["numero"] = numeroComanda.toInt();
  doc["photoactive"] = photoactive;
  doc["volumeTotal"] = volume;
  doc["tipo"] = 0;

  RtcDateTime agora = Rtc.GetDateTime();
  char data[11];
  char hora[9];
  snprintf_P(data, countof(data), PSTR("%04u-%02u-%02u"), agora.Year(), agora.Month(), agora.Day());
  snprintf_P(hora, countof(hora), PSTR("%02u:%02u:%02u"), agora.Hour(), agora.Minute(), agora.Second());
  doc["data"] = String(data)+" "+String(hora);

  JsonArray items = doc.createNestedArray("items");
  
  JsonObject objItems = items.createNestedObject();

  switch (opcaoShampoo) {
      case REGENERANT:
        objItems["codProduto"] = 1;
        break;
      case NUTRITION:
        objItems["codProduto"] = 2;
        break;
      case ANTIFRISO:
        objItems["codProduto"] = 3;
        break;
      case BIONEUTRAL:
        objItems["codProduto"] = 4;
        break;
      case NENHUM:
        break;
   }
          
  objItems["volume"] = volume;
  objItems["porcentagem"] = 100;

  String dados = "";
  
  serializeJson(doc, dados);
  
  return dados;
}

String criarMensagemJsonTratamento(unsigned long n, unsigned long volumeCondicionador[]) {
  StaticJsonDocument<220> doc;
  doc["codMaquina"] = codMaquina;
  doc["codSalao"] = codSalao;
  doc["codUsuario"] = codUsuario;
  doc["nomeUsuario"] = profissional;
  doc["numero"] = numeroComanda;
  doc["photoactive"] = photoactive;
  doc["volumeTotal"] = volumeTotal;
  doc["tipo"] = 1;

  RtcDateTime now = Rtc.GetDateTime();
  char data[11];
  char hora[9];
  snprintf_P(data, countof(data), PSTR("%04u-%02u-%02u"), now.Year(), now.Month(), now.Day());
  snprintf_P(hora, countof(hora), PSTR("%02u:%02u:%02u"), now.Hour(), now.Minute(), now.Second());
  doc["data"] = String(data)+" "+String(hora);

  JsonArray items = doc.createNestedArray("items");

  for (int i = 0, j = 5; i < 7; i++, j++) {
    if (condicionador[i]) {
      JsonObject objItems = items.createNestedObject();
      objItems["codProduto"] = j;
      objItems["volume"] = volumeCondicionador[i];
      objItems["porcentagem"] = fator[i];
    }
  }
  String json = "";
  serializeJson(doc, json);
  return json;
}

String criarMensagemJsonMapping() {
  StaticJsonDocument<200> doc;
  doc["codMaquina"] = codMaquina;
  doc["codSalao"] = codSalao;
  doc["codUsuario"] = codUsuario;
  doc["nomeUsuario"] = profissional;
  doc["numero"] = numeroComanda;
  doc["photoactive"] = photoactive;
  doc["tipo"] = 2;

  RtcDateTime now = Rtc.GetDateTime();
  char data[11];
  char hora[9];
  snprintf_P(data, countof(data), PSTR("%04u-%02u-%02u"), now.Year(), now.Month(), now.Day());
  snprintf_P(hora, countof(hora), PSTR("%02u:%02u:%02u"), now.Hour(), now.Minute(), now.Second());
  doc["data"] = String(data)+" "+String(hora);

  switch (opcaoMapping) {
    case CURTO:
      volume = 15;      
      break;
    case MEDIO:
      volume = 24;
      break;
    case LONGO:
      volume = 33;
      break;
    case NENHUM:
      break;
  }
  doc["volumeTotal"] = volume;
   
  JsonArray items = doc.createNestedArray("items");  
  for(int i = 4; i < 7; i++){
    JsonObject objItems = items.createNestedObject();
    objItems["codProduto"] = i;
    objItems["volume"] = volume / 3;
    objItems["porcentagem"] = 33;
  }
  String dados = "";  
  serializeJson(doc, dados);  
  return dados;
}

String criarMensagemJsonStatus(){
  RtcDateTime agora = Rtc.GetDateTime();
  
  StaticJsonDocument<200> doc;
  doc["codigo"] = codMaquina.toInt();
  
  char data[12];
  char hora[10];
  snprintf_P(data, sizeof(data), PSTR("%04u-%02u-%02u"), agora.Year(), agora.Month(), agora.Day());
  snprintf_P(hora, sizeof(hora), PSTR("%02u:%02u:%02u"), agora.Hour(), agora.Minute(), agora.Second());
  doc["data"] = String(data)+" "+String(hora);

  String dados = "";  
  serializeJson(doc, dados);  
  return dados;
}

bool enviarJson(String data, String uri)
{
  uint8_t buffr[256] = {0};

  if (wifiSerial.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print(F("create tcp ok\r\n"));
  } else {
    Serial.print(F("create tcp err\r\n"));
    return false;
  } 

  String server = HOST_NAME + ":" + HOST_PORT;
   
  String postRequest =
    "POST " + uri + " HTTP/1.0\r\nHost: " + server + "\r\nAccept: */*\r\nContent-Length: " +
    data.length() + "\r\nContent-Type: application/json\r\n\r\n" + data;

  Serial.println(data);
  Serial.print(F("postRequest: "));
  Serial.println(postRequest);

  //estourou tamanho da string
  if(postRequest.length() == 0)
    return false;
  
   bool teste = wifiSerial.send(postRequest.c_str(), postRequest.length());
   Serial.print(F("Send: "));
   Serial.println(teste);

  uint32_t len = wifiSerial.recv(buffr, sizeof(buffr), 10000);
  Serial.print(F("size recebido do server json "));
  Serial.println(len);
  if (len > 0) {
    Serial.print(F("Received:["));
    for (uint32_t i = 0; i < len; i++) {
      Serial.print((char)buffr[i]);
    }
    Serial.print(F("]\r\n"));
  }
  wifiSerial.releaseTCP();
  return true;
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
bool isConectado(){
// nao funcionaram
//  bool ok = false;
//  if (wifiSerial.createTCP("teste.k08.com.br", "80")) {
//    ok = true;
//  }
//  wifiSerial.releaseTCP();
//  return ok;
//  bool conectado = wifiSerial.ping("8.8.8.8");
//  return conectado;

  if(redeConectada.length() > 0 && senhaConectada.length() > 0)
    return wifiSerial.joinAP(redeConectada, senhaConectada);

  return false;
}

bool conectarWifi(String nomeRede, String senha, int tentativas) {
   for (int i = 0; i < tentativas; i++) {
    Serial.print(F("Conectando na rede "));
    Serial.print(nomeRede);
    Serial.print(F(" com a senha "));
    Serial.println(senha);
    if (wifiSerial.joinAP(nomeRede, senha)) {
      Serial.print(F("Conectado na rede "));
      Serial.println(nomeRede);
      Serial.print(F("IP: "));
      Serial.println(wifiSerial.getLocalIP().c_str());
      return true;
    } else {
      Serial.print(F("Join AP failure\r\n"));
    }
  }
  return false;
}

void desconectarWifi() {
  wifiSerial.leaveAP();  
}

void enviarDadosWifi(String msg) {
  wifiSerial.send(msg.c_str(), msg.length());
}

void iniciarWifi() {
  
  //for (int i = 0; i < TENTATIVAS_CONEXAO; i++) {
    if (wifiSerial.setOprToStation()) {
      Serial.print(F("to station ok\r\n"));
      //break;
    } else {
      Serial.print(F("to station err\r\n"));
    }
  //}

  conectado = iniciarConectar();
          
  msgLoading.setText(conectado?"Conectado":"Desconectado");

  delay(2000);
}

int contarLinhas(String arquivo) { 
  int qtd = -1; 
  if (SD.exists(arquivo)) { 
    File forigem = SD.open(arquivo); 
    if (forigem) { 
	  qtd = 0; 
      while (forigem.available()) { 
        String linha = forigem.readStringUntil('\n'); 
        qtd++; 
      } 
      forigem.close(); 
    } 
  } 
  return qtd; 
} 

bool reenviarDadosTemporarios() {
  bool enviou = false;
  String arquivo = "tmp.txt";
  if (SD.exists(arquivo)) {
    File forigem = SD.open(arquivo);
    if (forigem) {
      enviou = true;
           
      while (forigem.available()) {
        String linha = forigem.readStringUntil('\n');
        enviarJson(linha,"/api/comanda/adiciona/");
      }      
      forigem.close();
      SD.remove(arquivo);
    }
  }
  delay(500);
  return enviou;
}

void btnConectarPushCallback(void *ptr) {
  desconectarWifi(); //desconecta da ultima rede conectado
 
  char buff1[31] = {0};
  comboRede.getText(buff1, sizeof(buff1));
  String nomeRede(buff1);

  char buffSenha[21] = {0};
  txtSenha.getText(buffSenha, sizeof(buffSenha));
  String senha(buffSenha);

  nomeRede.trim();
  senha.trim();
  
  if(nomeRede.length() == 0){
    txtConexao.setText("Rede obrigatoria");
	return;  
  }
  
  if(senha.length() == 0){
	txtConexao.setText("Senha obrigatoria");
	return;
  }

  txtConexao.setText("Conectando...");
  String n = nomeRede.substring(0, nomeRede.indexOf("\r")); //nextion coloca \r no final da string
  String s = senha.substring(0, senha.indexOf("\r"));
  conectado = conectarWifi(n, s, TENTATIVAS_CONEXAO);
  
  if (conectado) {
    redeConectada = n;
	  senhaConectada = s;
    String msg = "Conectado na rede " + nomeRede;
    char buff2[1024];
    msg.toCharArray(buff2, sizeof(buff2));
    txtConexao.Set_font_color_pco(1024);
    txtConexao.setText(buff2);

    if(!procurarRede(n, s, "wifi.txt")){
      String nomeSenha = n + "\n" + s + "\n";
      gravarSD(nomeSenha, "wifi.txt");
    }
  } else {
    txtConexao.Set_font_color_pco(63488);
    String msg = "Falha ao conectar na rede " + nomeRede;
    char buff3[1024];
    msg.toCharArray(buff3, sizeof(buff3));
    txtConexao.setText(buff3);
    redeConectada = "";
	  senhaConectada = "";
  }
  
  txtSenha.setText("");
}

String prepararDadosWifiShampoo() {
  String m = prepararDadosComum(1, volume, "Shampoo");
  
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
    case NENHUM:
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

String prepararDadosWifiTratamento(unsigned long n, unsigned long volumeCondicionador[]) {
  String m = prepararDadosComum(n, volumeTotal, "Tratamento");

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

String prepararDadosComum(int numProdutos, unsigned long v, String tipo){
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
  m += numProdutos;
  m += "_";
  m += "tipo:";
  m += tipo;
  m += "_";
  m += "volumeTotal:";
  m += v;
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
  return m;
}

String prepararDadosWifiMapping() {
  String m = prepararDadosComum(3, volumeTotal, "Mapping");

  for(int i = 4; i <=6; i++){
    m += "_";
    m += "idProduto:";
    m += 4;
    m += "_";
    m += "vol.:";
    m += volumeTotal/3;
    m += "_";
    m += "porc.:";
    m += 33;
  }
  m += "&";
  return m;
}

void btnWifiPopCallback(void *ptr) {
  page9.show();
  inPage0 = false;

  if(!wifiSerial.kick()){
    wifiSerial.restart();
  }

  mostrarDataHora();
  
  txtSenha.setText("");
  comboRede.setText("");
  
  txtConexao.setText("Procurando redes wifi...");
  buscarRedesDisponiveis();

  txtConexao.setText("Verificando conexao...");

  conectado = isConectado();

  if (conectado) {
    String msg = "Conectado na rede "+redeConectada;
    txtConexao.Set_font_color_pco(1024);
    txtConexao.setText(msg.c_str());
    //char buffR[128];
    //redeConectada.toCharArray(buffR, sizeof(buffR));
    //comboRede.setText(buffR);
  } else {
    txtConexao.Set_font_color_pco(63488);
    txtConexao.setText("Desconectado");
    redeConectada = "";
    senhaConectada = "";
  }
  
      
}

void buscarRedesDisponiveis() {

  String str = "";
  int tentativas = 0;
  while(str.length() == 0 && tentativas < TENTATIVAS_CONEXAO){
    str = wifiSerial.getAPList();
    tentativas++;
  }

  str.trim();
  
  String redes = "";

  char buff[1024];
  str.toCharArray(buff, 1024);

  char *pt;

  pt = strtok(buff, "+");
  while (pt) {
    String r = getNomeRede(pt);
    Serial.println(r);
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

//============================================================================

void btnShampooPushCallback(void *ptr) {
  resetarVariaveisShampoo();
  page1.show();
  inPage0 = false;
  volumeShampoo.setText("");  
}

void btnTreatmentPushCallback(void *ptr) {
  resetarVariaveisTratamento();
  page2.show();
  inPage0 = false;
  volumeTratamento.setText("");
}

void btnMappingPushCallback(void *ptr) {
  opcaoMapping=NONE;
  page14.show();
  inPage0 = false;
}

void btnFecharGerenciarPopCallback(void *ptr) {
  resetarPopupSenha();
  gotoPage0();
}

void btnCadastroPushCallback(void *ptr) {
  resetarPopupSenha();
  opcao = CADASTRO;
  page12.show();
  inPage0 = false;
}

void btnInativarPushCallback(void *ptr, int pos) {
  if (!isSdOk)
    return;

  //atualiza display
  char buff1[81] = {0};
  tabela.getText(buff1, sizeof(buff1));
  String nomeSenhaStatus(buff1);

  int iniSeparador = nomeSenhaStatus.indexOf("^");
  int fimSeparador = nomeSenhaStatus.lastIndexOf("^");

  String perfil = nomeSenhaStatus.substring(iniSeparador + 1, fimSeparador);
  Serial.print(F("perfil: "));  
  Serial.println(perfil);  
  String usuario = nomeSenhaStatus.substring(0, iniSeparador);
   
  String oldStatus = nomeSenhaStatus.substring(fimSeparador+1);
  String newStatus = oldStatus.equals("Ativo")?"Inativo":"Ativo";
  String atualizado = nomeSenhaStatus.substring(0, fimSeparador+1) + newStatus;
 
  uint32_t index;
  tabela.getValue(&index);
  tabela.up(atualizado, index);
 
  //atualiza arquivo
  File arquivo, copia;

  String filename = "";
  if(perfil.equals("Tecnico"))
    filename = BDTECNICOS;
  else if(perfil.equals("Gerente"))
    filename = BDGERENTES;
  else if(perfil.equals("Profissional"))
    filename = BDPROFISSIONAIS;

  String dados = "";
 
  arquivo = SD.open(filename, FILE_READ);
  copia = SD.open("bkp.txt", FILE_WRITE);
  if (arquivo) {
   
    while (arquivo.available()) {
      dados = arquivo.readStringUntil('\n');
      int idxSeparador = dados.indexOf(";");
      int lastSeparador = dados.lastIndexOf(";");
      int idSeparador = dados.indexOf("#");
             
      String nome = dados.substring(idSeparador+1, idxSeparador);
      if(usuario.equals(nome)){
        char s = dados.charAt(lastSeparador+1);
        if(s == 'A')
           dados.setCharAt(lastSeparador+1, 'I');
        else
           dados.setCharAt(lastSeparador+1, 'A');
      }
      dados += "\n";
      copia.print(dados);  
    }    
    arquivo.close();
    copia.close();

    SD.remove(filename);

    arquivo = SD.open(filename, FILE_WRITE);
    copia = SD.open("bkp.txt", FILE_READ);

    size_t n;  
    uint8_t buf[64];
    while ((n = copia.read(buf, sizeof(buf))) > 0) {
    arquivo.write(buf, n);
    }
    arquivo.close();
    copia.close();
    SD.remove("bkp.txt");
   
    Serial.print(F("escreveu: !!!!"));
    Serial.println(dados);    
  } else {
    Serial.print(F("error opening file"));
  }
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
//===============================RELES============================================
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

//=======================================USUARIOS CADASTRADOS===============================
void btnGerenciarPopCallback(void *ptr) {  
  page13.show();
  carregarUsuariosCadastrados();
}
void lerCadastro(String filename, String perfil) {
  if (!isSdOk)
    return;

  File arquivo = SD.open(filename);
  if (arquivo) {
    while (arquivo.available()) {
      String nomeSenhaStatus = arquivo.readStringUntil('\n');
    int iniSeparador = nomeSenhaStatus.indexOf(";");
      int fimSeparador = nomeSenhaStatus.lastIndexOf(";");
      int idSeparador = nomeSenhaStatus.indexOf("#");
      String senha = nomeSenhaStatus.substring(iniSeparador + 1, fimSeparador);
      String profissional = nomeSenhaStatus.substring(idSeparador+1, iniSeparador);
    String s = nomeSenhaStatus.substring(fimSeparador + 1, fimSeparador + 2).equals("A")?"Ativo":"Inativo";

      profissional = profissional + "^" + perfil + "^"+s;
      Serial.println(profissional);      
      tabela.insert(profissional);
    }
    arquivo.close();
  } else {
    Serial.print(F("error opening "));
    Serial.println(filename);
  }

  return;
}
void carregarUsuariosCadastrados(){
  tabela.clearAll();
  lerCadastro("TEC.TXT", "Tecnico");
  lerCadastro("GER.TXT", "Gerente");
  lerCadastro("PRO.TXT", "Profissional");  
}

//======================================MAPPING=================================================
void btnCurtoPushCallback(void *ptr) {
  opcaoMapping=CURTO;
}

void btnMedioPushCallback(void *ptr) {
  opcaoMapping=MEDIO;
}

void btnLongoPushCallback(void *ptr) {
  opcaoMapping=LONGO;  
}

void btMixMapPushCallback(void *ptr) {
  if (opcaoMapping == NONE) {
    return;
  }
  
  resetarPopupSenha();
  opcao = MAPPING;
  page11.show();
}
//=====================================EXECUCAO===============================================
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
    case NENHUM:
      break;
  }

  unsigned long fim = millis() + volShampoo;

  if (volShampoo > 0) {
    page4.show();
    if (photoactive && volume < 50){
      digitalWrite (RELE8_PHOTOACTIVE, LOW);
    }

    digitalWrite (rele, LOW);
    while (millis() < fim) {
    }
    resetarReles();
    
    page5.show();//retire seu produto

    gravarSD(prepararDadosSdShampoo(), "sham.txt");

    String dadosWifi = criarMensagemJsonShampoo();
    bool enviou = enviarJson(dadosWifi, "/api/comanda/adiciona/");
  
    if (enviou) {
      reenviarDadosTemporarios();
      delay(500);
    } else {
      dadosWifi += "\n";
      gravarSD(dadosWifi, "tmp.txt");
    }
  }
  gotoPage0();
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
    if (photoactive && volumeTotal < 50)
      digitalWrite (RELE8_PHOTOACTIVE, LOW);
    liberarCondicionador(RELE9_BASE, tempoReleBaseFechado);
    
    int n = 0;
    for (int i = 0, j = 40; i < 7; i++, j++) { // nutrition
      if (condicionador[i] && tempoReleFechado[i] > 0) {
        delay(500);
        liberarCondicionador(j, tempoReleFechado[i]);
        n++;
      }
    }
    resetarRelesTratamento();
    page5.show();//retire seu produto

    gravarSD(prepararDadosSdTratamento(volumeBase, volumeCondicionador), "trat.txt");
    
	  String dadosWifi = criarMensagemJsonTratamento(n, volumeCondicionador);
    bool enviou = enviarJson(dadosWifi, "/api/comanda/adiciona/");
    
    if (enviou) {
      reenviarDadosTemporarios();
      delay(500);
    } else {
      dadosWifi += "\n";
      gravarSD(dadosWifi, "tmp.txt");
    }
  }
  gotoPage0();
}

void rodaMapping() {

  volumeTotal = 0;
  
  int rele = RELE4_BIONEUTRAL;
  
  switch (opcaoMapping) {
    case CURTO:
      volumeTotal = 15;      
      break;
    case MEDIO:
      volumeTotal = 24;
      break;
    case LONGO:
      volumeTotal = 33;
      break;
    case NENHUM:
      break;
  }
  
  unsigned long volume = volumeTotal/3;
  unsigned long volShampoo = (unsigned long)volume * K_valvula_11;

  unsigned long fim = millis() + volShampoo;

  page4.show();
    digitalWrite (RELE8_PHOTOACTIVE, LOW);

    digitalWrite (rele, LOW);
    while (millis() < fim) {
    }
  digitalWrite (rele, HIGH);
  delay(500);
  
  unsigned long tempoT1 = volume * tempoValvulaCondicionador[0];
  unsigned long tempoT2 = volume * tempoValvulaCondicionador[1];

  liberarCondicionador(RELE16_NUTRI, tempoT1); // 
  delay(500);
  liberarCondicionador(RELE15_REPAIR, tempoT2); // 
  delay(500);  
  digitalWrite (RELE8_PHOTOACTIVE, HIGH);
  page5.show();//retire seu produto

  gravarSD(prepararDadosSdMapping(), "mapp.txt");
   String dadosWifi = criarMensagemJsonMapping();
   bool enviou = enviarJson(dadosWifi, "/api/comanda/adiciona/");
  
    if (enviou) {
      reenviarDadosTemporarios();
      delay(500);
    } else {
      dadosWifi += "\n";
      gravarSD(dadosWifi, "tmp.txt");
    }
  
  gotoPage0();
}


bool validarComandasEmEspera(){
  int emEspera = contarLinhas("tmp.txt");
  if(emEspera < MAX_COMANDAS_OFFLINE){
    String msg = String(emEspera) +" comandas em espera. Limite: "+ String(MAX_COMANDAS_OFFLINE); 
    txtPage0.setText(msg.c_str());
    return true;
  }else{
    String msg = "Máx. de " + String(MAX_COMANDAS_OFFLINE) +" comandas em espera atingido."; 
    txtMsg.setText(msg.c_str()); 
    txtPage0.setText(msg.c_str()); 
    return false;
  }  
}

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

  if(!validarComandasEmEspera()){
    return;
  }

//  if(!(numComandasOffline < MAX_COMANDAS_OFFLINE)){ 
//    String msg = "Máx. de " + String(MAX_COMANDAS_OFFLINE) +" comandas sem internet atingido."; 
//    txtMsg.setText(msg.c_str()); 
//	txtPage0.setText(msg.c_str());     
//    return; 
//  } 
   
//  conectado = isConectado(); 
//  if(!conectado){ 
//    numComandasOffline++; 
//    String msg = String(numComandasOffline) +" comandas sem internet. Limite: "+ String(MAX_COMANDAS_OFFLINE); 
//	txtPage0.setText(msg.c_str()); 
//  }else{ 
//    numComandasOffline = 0; 
//	txtPage0.setText(""); 
//	txtMsg.setText(""); 
//  }  

  if (procurarSenha(strSenha, BDPROFISSIONAIS)) {
    resetarPopupSenha();
    if (opcao == SHAMPOO) {
      rodaShampoo();
    }
    else if (opcao == TRATAMENTO)
      rodaTratamento();
    else if (opcao == MAPPING)
      rodaMapping();
  } else {
    txtMsg.setText("Senha nao encontrada");
  }
}

//========================================VOLTAR===============================================
void btnVoltar9PopCallback(void *ptr) {
  gotoPage0();
}
void btnVoltar8PopCallback(void *ptr) {
  gotoPage0();
}
void btnVoltar7PopCallback(void *ptr) {
  gotoPage0();
}
void btnVoltar1PopCallback(void *ptr) {
  gotoPage0();
}
void btnVoltar2PopCallback(void *ptr) {
  gotoPage0();
}
void btnVoltar14PopCallback(void *ptr) {
  gotoPage0();
}

//=============================================================================================

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

void definirEntradas() { 
   pinMode(interrupcao, INPUT_PULLUP); 
}

void gotoPage0(){
  page0.show();
  picWifi.setPic(conectado?PIC_CONECTADO:PIC_DESCONECTADO);
  inPage0 = true;
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
  &btnGerenciar,
  &btnCurto,
  &btnMedio,
  &btnLongo,
  &btnMapping,
  &btMixMap,
  &btnInativar,
  &btnSincr,
  &btnVoltar9,
  &btnVoltar8,
  &btnVoltar7,
  &btnVoltar1,
  &btnVoltar2,
  &btnVoltar14,
  NULL
};

void setup() {
  Serial.begin(9600);

  nexInit();
  page3.show();

  wifiSerial.leaveAP();

  definirEntradas();   
  definirSaidas();
  resetarReles();
  resetarRelesTratamento();
  resetarVariaveisShampoo();
  resetarVariaveisTratamento();
  //inicializarEeprom();
  lerConfiguracaoValvulas();
  initSdCard();
  //modoStatus = true;

  lerDadosBasicos();
  
  iniciarWifi();
  
  initRTC();
  setarAlarme();  

  if (conectado) {
    enviarJson(criarMensagemJsonStatus(), "/api/maquina/sit/");
    if(reenviarDadosTemporarios()){
      msgLoading.setText("Comandas enviadas com sucesso.");
      delay(2000);
    }   
  }

  msgLoading.setText("");

  btnReles.attachPush(btnRelesPushCallback, &btnReles);
  btnCadastrar.attachPop(btnCadastrarPopCallback);
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

  btMixMap.attachPush(btMixMapPushCallback, &btMixMap);
  btnCurto.attachPush(btnCurtoPushCallback, &btnCurto);
  btnMedio.attachPush(btnMedioPushCallback, &btnMedio);
  btnLongo.attachPush(btnLongoPushCallback, &btnLongo);

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
  btnMapping.attachPush(btnMappingPushCallback, &btnMapping);
  
  btnFechar.attachPop(btnFecharPopCallback);
  btnFecharGerenciar.attachPop(btnFecharGerenciarPopCallback);
  btnWifi.attachPop(btnWifiPopCallback);
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

  btnCadastro.attachPush(btnCadastroPushCallback, &btnCadastro);
  btnGerenciar.attachPop(btnGerenciarPopCallback);
  btnInativar.attachPush(btnInativarPushCallback, &btnInativar);
  
  btnSincr.attachPop(btnSincrPopCallback);
  btnVoltar9.attachPop(btnVoltar9PopCallback);
  btnVoltar8.attachPop(btnVoltar8PopCallback);
  btnVoltar7.attachPop(btnVoltar7PopCallback);
  btnVoltar1.attachPop(btnVoltar1PopCallback);
  btnVoltar2.attachPop(btnVoltar2PopCallback);
  btnVoltar14.attachPop(btnVoltar14PopCallback);
    
  gotoPage0();
}

void loop() {
  nexLoop(nex_listen_list);
  delay(150);
  
  if(modoStatus){
    conectado = isConectado();
	
    if(conectado)
      enviarJson(criarMensagemJsonStatus(), "/api/maquina/sit/");

    if(inPage0){
      picWifi.setPic(conectado?PIC_CONECTADO:PIC_DESCONECTADO);
    }
      
    setarAlarme();
    modoStatus=false;    
  }
}
