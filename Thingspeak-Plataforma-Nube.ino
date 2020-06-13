#include <LiquidCrystal.h>

//Pines de los Sensores
const int PIN_SENSOR_TEMPERATURA = A2;
const int PIN_SENSOR_HUMEDAD = A4;
const int PIN_SENSOR_LED = A0;

//Utiliza LEDs para indicar la condición de funcionamiento del ESP8266
const int PIN_LED_ERROR = 13;
const int PIN_LED_OK = 12;

//Pines que están conectados a la pantalla LCD
const int PIN_LCD_RS = 2;
const int PIN_LCD_ENABLE =3;
const int PIN_LCD_DB4 = 4;
const int PIN_LCD_DB5 = 5;
const int PIN_LCD_DB6 = 6;
const int PIN_LCD_DB7 = 7;

// Parámetros del puerto serial
const unsigned long SERIAL_BAUDRATE = 115200L; //Velocidad de Transmisión
const unsigned long SERIAL_TIMEOUT  = 5000L;
const unsigned long TIEMPO_MOSTRANDO_RESPUESTA = 4000L; 
bool _mostrarComandosDisplay;

// Características de LCD
const int LCD_NUMERO_LINEAS = 2;
const int LCD_NUMERO_COLUMNAS = 16;

// Instancia la clase y define los pines conectados a la pantalla LCD
// NOTA: R/W siempre en LOW - usando 4 bits
LiquidCrystal _lcd(PIN_LCD_RS, PIN_LCD_ENABLE, 
                   PIN_LCD_DB4, PIN_LCD_DB5, PIN_LCD_DB6, PIN_LCD_DB7);

// Redes de WiFi del simulador
String _ssidName     = "";    // Nombre de la red
String _ssidPassword = "";            // Sin contraseña
int _tcpHttpPort     = 80;            // Puerto HTTP en la conexión TCP


// Datos para accceder al sitio web
String _siteHost    = "api.thingspeak.com";    // URL
String _siteAPPID   = "4BUKOQC1DMO51H0V";   // Appkey en el sitio web de Thingspeak
String _siteURIbase = "/update?api_key=" + _siteAPPID;
String _siteField1  = "&field1=";
String _siteField2  = "&field2=";
String _siteField3  = "&field3=";


// Parâmetros para a lectura da temperatura
const int TEMPERATURA_CELSIUS_MINIMA = -40;    // Menor temperatura de sensor [°C]
const int TEMPERATURA_CELSIUS_MAXIMA = 125;   // Mayor temperatura de sensor [°C]

int _lecturaMinima;      // Guardando menor lectura (-40°C)
int _lecturaMaxima;     // Guardando maior lectura (+125°C)

void setup() {
  // Inicializa la lógica del LED del error
  pinMode(PIN_LED_ERROR, OUTPUT);
  digitalWrite(PIN_LED_ERROR, LOW);
  
  // Inicializando la lógica de los sensores
  pinMode(PIN_SENSOR_TEMPERATURA, INPUT);
  pinMode(PIN_SENSOR_HUMEDAD, INPUT);
  pinMode(PIN_SENSOR_LED, INPUT);
  
  // Inicializa el LCD
  _lcd.begin(LCD_NUMERO_COLUMNAS,LCD_NUMERO_LINEAS);
  _lcd.print("TESIS IoT");
  _lcd.setCursor(0,1);
  _lcd.print("App-Sensores");
  delay(2000);  // Tiempo de mensaje visible en pantalla
  
  // Prepara para calibrar la temperatura
  // Inicializa con los valores del ejercicio con el sensor de temperatura.
  
  _lecturaMinima = 20;  // Use 9999 si desea calibrar automáticamente
  _lecturaMaxima = 358; // Use -9999 si desea realizar la calibración automática

  //Inicializa la lógica de ESP8266
  pinMode(PIN_LED_OK, OUTPUT);
  Serial.begin(SERIAL_BAUDRATE);
  Serial.setTimeout(SERIAL_TIMEOUT);
  
  
  // Confirma que el ESP8266 está operativo al inicializar el módulo
  bool esp8266OK = sendCommandTo8266("AT+RST", "ready");
  if (!esp8266OK) {           
    digitalWrite(PIN_LED_OK, LOW);   
    _lcd.setCursor(0,1);
    _lcd.print("ERROR en ESP8266!");
    while (1) { // Bloquea la ejecución del programa.
      delay(500);
      _lcd.noDisplay();
      delay(500);
      _lcd.display();
    }
  } else {                // SÍ, se enciende el LED verde
    digitalWrite(PIN_LED_OK, HIGH);
    // Conéctese al WiFi Simulator usando el comando AT + CWJAP
    String loginWiFi = "AT+CWJAP=\"" + _ssidName + "\",\"" + _ssidPassword + "\"";
    sendCommandTo8266(loginWiFi, "OK");

    // Abre el canal de comunicación TCP con el sitio web utilizando el comando AT + CIPSTART
    String acessoTCP = "AT+CIPSTART=\"TCP\",\"" + _siteHost + "\"," + String(_tcpHttpPort);
    sendCommandTo8266(acessoTCP, "OK");

    delay(2000);
   } 

}

//Envía los datos de los sensores periodicamente a la web
void loop() {
  // Lee el sensor de temperatura (precisión 4,88 mV)
  int lecturaSensor = analogRead(PIN_SENSOR_TEMPERATURA);
  // Realiza la autocalibración
  if (lecturaSensor > _lecturaMaxima)      
      _lecturaMaxima = lecturaSensor;
  if (lecturaSensor < _lecturaMinima)
      _lecturaMinima = lecturaSensor;
  
  // Convierte la lectura del sensor a temperatura Celsius
  int temperaturaCelsius = map(lecturaSensor,_lecturaMinima,_lecturaMaxima,TEMPERATURA_CELSIUS_MINIMA,TEMPERATURA_CELSIUS_MAXIMA);
  String temperaturaCelsiusTexto = numberToString(temperaturaCelsius);

 
  // Sensor de nivel de Humedad
  int nivelHumedad = analogRead(PIN_SENSOR_HUMEDAD);
  int nivelHumedadPorc = map(nivelHumedad,0, 1023, 0, 100);
  String nivelHumedadTexto = numberToString(nivelHumedadPorc);
  
  
  
  // Sensor do motor
  bool led = digitalRead(PIN_SENSOR_LED);
  String ledTexto = led ? "1" : "0";
  
  
  
  // Muestra el estado de los sensores, si no usa la pantalla LCD para los comandos AY
  if (!_mostrarComandosDisplay)
    mostrarDisplayLCD(temperaturaCelsiusTexto, nivelHumedadTexto, ledTexto);

  
  // Enviar sensores a la web
  enviarSensores(temperaturaCelsiusTexto, nivelHumedadTexto, ledTexto);
  
  
  // Enviar sensores a cada 1 segundo
  delay(1000);

}



//FUNCION ENVIAR SENSORES
bool enviarSensores(String temperatura, String nivel, String led) {
  bool sucesso = true;
  
   // Construir la solicitud HTTP
  String urlCompleta = _siteURIbase + 
               _siteField1 + temperatura +
                       _siteField2 + nivel +
                       _siteField3 + led;
    
  String httpPacket = "GET " + urlCompleta + " HTTP/1.1\r\nHost: " + _siteHost + "\r\n\r\n";
  int length = httpPacket.length();
  
  // Enviando el tamaño del paquete a ESP8266
  String tamanoPaquete = "AT+CIPSEND=" + numberToString(length);  
  sucesso &= sendCommandTo8266(tamanoPaquete, ">");
  sucesso &= sendCommandTo8266(httpPacket, "SEND OK");
  
  return sucesso;
}


//FUNCION DE MOSTRAR EN DISPLAY LCD
void mostrarDisplayLCD(String temperatura, String nivel, bool led) {
  //Imprimiendo en pantalla el valor de la temperatura
  _lcd.setCursor(0,0);
  _lcd.print("Temp:"); 
  _lcd.print(temperatura);  
  _lcd.print('\xB2'); 
  _lcd.print("C ");
  
  //Imprimiendo en pantalla el valor de Humedad
  _lcd.setCursor(0,1);
  _lcd.print("Hum:"); 
  _lcd.print(nivel);  
  _lcd.print("%  ");
  
  //Imprimiendo en pantalla el valor de motor
  if (led)
  _lcd.print("ON ");
  else
  _lcd.print("   ");
  
  if (!led)
  _lcd.print("OFF ");
  else
  _lcd.print("   ");
}


//FUNCION QUE ENVIA EL COMANDO AT A LA ESP8266 Y ESPERA UNA CADENA DE RESPUESTA.
//SI SE PRODUCE UN ERROR, SE ENCIENDE EL LED ROJO.

bool sendCommandTo8266(String comando, char * aguardar) {
  bool sucesso = false;
  
  // Envía el comando a ESP8266 y espere a que se procese.
  Serial.println(comando);
  Serial.flush();
  delay(50);
  
  // Muestra el comando AT en el display LCD
  if (_mostrarComandosDisplay)  { // Está mostrando AT en display LCD?
    imprimeDisplay(1, comando);   // SIM, mostra el comando enviado
    imprimeDisplay(2, "");        // Aguarda la respuesta
  }
  
  // Verifica si hay una respuesta a ser guardada
  if (0 == aguardar[0])
    sucesso = true;
  else if (_mostrarComandosDisplay) {  // Mostrando AT en la pantalla LCD?
    sucesso = verificarRespuesta(aguardar);
    delay(TIEMPO_MOSTRANDO_RESPUESTA);  // SIM, permite a lectura
  } else
    sucesso = Serial.find(aguardar);  // NO, solo espera la respuesta
    
  digitalWrite(PIN_LED_ERROR, !sucesso); //Error, en ESP8266 se enciende LED Rojo
  return sucesso;
}


bool verificarRespuesta(char * aguardar) {
  bool sucesso = false;
  
  if (0 == aguardar[0])     
    sucesso = true;       
  else {
    unsigned long tiempoLimite = millis() + SERIAL_TIMEOUT;
    int index = 0;
    String respuesta = "";
    while (millis() <= tiempoLimite) {
      if (!Serial.available())    // recibe algo?
        delay(1);         // NO, espere 1ms
      else {            
        // SIM, procesa una respuesta
        char recibido = Serial.read();
        // Deja los caracteres de control visibles en la pantalla LCD
        if ('\r' == recibido)      respuesta += '\xFE';
        else if ('\n' == recibido)   respuesta += '\xFF';
        else if (recibido < ' ')     respuesta += '\xFC';
        else              
        respuesta += recibido;
        
        if (recibido != aguardar[index]) { // encontro el caracter en la cadena?
          index = 0;            // no, continua esperando
        } else {              // SIM, verifica si termino
          index++;
          if (0 == aguardar[index]) {     // Encontré lo que buscaba?
            sucesso = true;           
            break;
          }
        }
      }
    } 
    imprimeDisplay(0, respuesta);  // Muestra la respuesta
  } 
  
  return sucesso;
}

/**
 * @brief Escribe mensajes intercambiados con el ESP8266 en la pantalla LCD
 * mientras no esté conectado con el dispositivo.
 * @param comoEscribir indica la condición de escritura:
 * = 1, borra la pantalla antes de escribir en la primera línea.
 * = 2, borra la segunda línea antes de escribir.
 * = 3, borra la primera línea antes de escribir.
 * Cualquier otro valor, continúa con el cursor actual.
 * texto Texto a escribir en la pantalla LCD.
 */


void imprimeDisplay(byte comoEscribir, String texto) {
  if (1 == comoEscribir)      _lcd.clear();
  else if (2 == comoEscribir)   limpiarlineaLCD(1);
  else if (3 == comoEscribir)   limpiarlineaLCD(0);
  String textoC = texto.substring(0, LCD_NUMERO_COLUMNAS);
  _lcd.print(textoC);
}

//borra la línea de la pantalla LCD, dejando el cursor encendido
void limpiarlineaLCD(byte linea) {
  _lcd.setCursor(0, linea);
  _lcd.print("                ");
  _lcd.setCursor(0, linea);
}


// Función necesaria para corregir el error en String (int)
String numberToString(int valor) {
  char numero[6];
  sprintf(numero, "%i", valor);
  return String(numero);
}
