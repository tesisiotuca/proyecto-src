#include <DHT.h> // Librería para el DHT11/DHT22
#define DHTPIN 2 // Pin digital donde se conecta el sensor
#define DHTTYPE DHT11 //Definición del tipo de sensor
 
DHT dht(DHTPIN, DHTTYPE); // Se inicializa el sensor DHT11
 
void setup() {
    Serial.begin(9600); //Se inicializa la comunicación en serie con baurate a 9600
    dht.begin(); // Comienza el sensor DHT
}
 
void loop() {
  //delay(5000); // Espera de 5 segundos entre medidas
 
  float h = dht.readHumidity(); // Lectura de la humedad
  float t = dht.readTemperature(); // Lectura de la temperatura en grados centígrados (por defecto)
    
   // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
 

  Serial.print("H");
  Serial.print(h);
  Serial.print("\t");
  
  Serial.print("T");
  Serial.println(t);
  //Serial.println(" ºC ");
  delay(100);
  

}
