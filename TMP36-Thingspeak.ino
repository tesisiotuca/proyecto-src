//Definición de Variables
String ssid = "Simulator Wifi";
String password = "";
String host= "api.thingspeak.com";
int puerto= 80;
String uri = "/update?api_key=YIA2DIQKW9E22W1G&field1=";


void setup(){
  configurarESP8266();
}

void loop(){
  enviarTemperaturaESP8266();
}


int configurarESP8266(void) {
  // inicio de comunicación serial de la ESP8266
  Serial.begin(115200);   //Se inicia la velocidad de transmision
  Serial.println("AT");   //Prueba si el sistema AT funciona correctamente
  delay(10);        // Espera a que la ESP8266 genere una respuesta
  if (!Serial.find("OK")) //El comando AT devuelve OK, si la comunicación con el serial hubo inconveniente
  return 1; //Retorna el valor de 1
    
  // Conectando al Wifi
  //AT+CWJAP: Comando de la ESP8266 para conectar al AP(Punto de Acceso)
  //Parámetros SSID:Service Set Identifier->Identificador del Servicio del Punto de Acceso
  //Password: Contraseña
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\""); //Connect to AP,
  //AT+CWJAP ="abc", "0123456789"
  delay(10);        // Espera a que la ESP8266 genere una respuesta
 
  if (!Serial.find("OK")) 
  return 2; //Si hubo inconveniente en la comunición al conectar al AP retorna 2
  
  // Iniciando conexión TCP
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + puerto);
  delay(50);        // Espera a que la ESP8266 genere una respuesta
  if (!Serial.find("OK")) 
  return 3; //Si hubo error al iniciar la conexión TCP retorna el valor de 3
  
  return 0;
}

void enviarTemperaturaESP8266(void) {
  
  int temperatura = map(analogRead(A0),20,358,-40,125);
  
  // Construcción de HTTP
  String paqueteHTTP = "GET " + uri + String(temperatura) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  int longitud = paqueteHTTP.length();
  
  // Enviando la longitud de los datos que se enviarán
  Serial.print("AT+CIPSEND=");
  Serial.println(longitud);
  delay(10); // Espera a que la ESP8266 genere una respuesta

  // Enviando la Construcción HTTP
  Serial.print(paqueteHTTP);
  delay(10); // Espera a que la ESP8266 genere una respuesta
  if (!Serial.find("SEND OK\r\n")) 
  return;
  
  
}


