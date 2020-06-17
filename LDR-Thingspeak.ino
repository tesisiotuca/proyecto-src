//Definición de Variables
String ssid = "Simulator Wifi"; //Red de identificación de API
String password = ""; //sin contraseña
String host= "api.thingspeak.com"; //Host Thingspeak
int puerto= 80; //Puerto de Conexión
String uri = "/update?api_key=YIA2DIQKW9E22W1G&field1=";
//Cambiar en uri el valor de API_KEY generado en Thingspeak
///update?api_key=(API_KEY)&field1=


void setup(){
  //Función que inicia la comunicación de la ESP8266
  //y establece el la conexión TCP por el punto de
  //acceso especificado
  configurarLDR();
}

void loop(){
  //Función a llamarse continuamente para enviar el
  //valor del sensor de LDR
  enviarLDR();
}

int configurarLDR(void) {
  // inicio de comunicación serial de la ESP8266
  Serial.begin(115200);   //Se inicia la velocidad de transmision
  pinMode(10,OUTPUT); //Definición Pin LED de Salida
  pinMode(A0,INPUT); //Definición Pin A0 de LDR como Entrada
  
  Serial.println("AT");   //Prueba si el sistema AT funciona correctamente
  delay(10);        // Espera a que la ESP8266 genere una respuesta
  if (!Serial.find("OK")) //El comando AT devuelve OK, 
  return 1; //Retorna el valor de 1, si hubo inconveniente
    
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



void enviarLDR(void) {
  
  int luz = map(analogRead(A0),344,1017,0,1023);
  
  if(luz>512){
    digitalWrite(10,HIGH);
  }
  else{
  digitalWrite(10,LOW);
  }
  Serial.println(luz);

  
  
  // Construcción de HTTP
  String paqueteHTTP = "GET " + uri + String(luz) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
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


