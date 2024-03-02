  // Library yang diperlukan
  #include <FirebaseESP8266.h>
  #include <ESP8266WiFi.h>
  #include <DHT.h>
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>  

  LiquidCrystal_I2C lcd(0x27, 16, 2);

  // Mendefinisikan pin dan tipe sensor DHT
  #define DHTPIN D5 //14
  #define DHTTYPE DHT11
  DHT dht(DHTPIN, DHTTYPE);



  // Koneksi ke Firebase & wifi
  #define FIREBASE_HOST "https://egg-incubation-default-rtdb.firebaseio.com/"
  #define FIREBASE_AUTH "AIzaSyAbVac5urzwmNgQ2aYJEbK-HpSSqRw--HM"
  #define WIFI_SSID     "Titik1"
  #define WIFI_PASSWORD "1234567890"  

  // mendeklarasikan objek data dari FirebaseESP8266
  FirebaseData firebaseData;
  #define relay1 D8 //15 SDA D2, SCL D1.

void setup() {
  
  Serial.begin(115200); //kecepatan pembacaan port
  
  lcd.begin(); //LCD
  dht.begin(); //DHT
  
  // Koneksi ke Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
      lcd.print("Connecting...");
      delay (1000);
      lcd.clear();
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
      lcd.print("     READY");
      lcd.setCursor(0,1);
      lcd.print("IP: ");
      lcd.println(WiFi.localIP());
      delay (1000);
      lcd.clear();
  Serial.print("Inkubator Telur Ayam Ready!");
  Serial.println(" ");


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH);

}

void loop(){
  
Serial.print("EGG INCUBATION");
Serial.println(" ");

  // Sensor DHT11 membaca suhu dan kelembaban
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Memeriksa apakah sensor berhasil mambaca suhu dan kelembaban
  if (isnan(t) || isnan(h)) {
    Serial.println("Gagal membaca sensor DHT11");
    return;
  }

  // Menampilkan suhu dan kelembaban pada serial monitor
    Serial.print("Suhu      : ");
    Serial.print(t);
    Serial.println(" °C");
    Serial.print("Kelembapan: ");
    Serial.print(h);
    Serial.println(" %");
    Serial.println(" ");

    //PRINT LCD
      lcd.clear();
      lcd.print("Inkubator Telur");
      delay (1000);
      lcd.clear();

      lcd.print("Suhu   : ");
      lcd.print(t);
      lcd.println(" C");

      lcd.setCursor(0,1);
      lcd.print("Lembab : ");
      lcd.print(h);
      lcd.println(" %");
      delay (1000);

  // Memberikan status suhu dan kelembaban kepada firebase
  if (Firebase.setFloat(firebaseData, "/EGG_INCUBATION/Suhu", t)){
      Serial.println("........................ Suhu terkirim");
    } else{
      Serial.println("........................ Suhu tidak terkirim");
      Serial.println("Karena: " + firebaseData.errorReason());
      lcd.clear();
      lcd.print("    Suhu ");
      lcd.setCursor(0,1);
      lcd.print("tidak terkirim");
    } 
    
  if (Firebase.setFloat(firebaseData, "/EGG_INCUBATION/Kelembapan", h)){
      Serial.println("........................ Kelembapan terkirim");
      Serial.println();
      Serial.println();
      Serial.println();
    } else{
      Serial.println("Kelembapan tidak terkirim");
      Serial.println("Karena: " + firebaseData.errorReason());
            lcd.clear();
      lcd.print("  Kelembapan");
      lcd.setCursor(0,1);
      lcd.print("tidak terkirim");
    
      Serial.println();
      Serial.println();
      Serial.println();
    }

 
 //Tombol otomatis atau manual
   if (Firebase.getString(firebaseData, "/EGG_INCUBATION/Sistem_Manual")) { 
    if  (firebaseData.dataType() == "string") {
      String FBStatus = firebaseData.stringData();//Sistem otomatis
      if (FBStatus == "false") {                                                         
      Serial.println("............ Sistem Otomatis ...........");
      lcd.clear();
      lcd.print("Sistem Otomatis");                 
      lcd.setCursor(0,1);
      lcd.print(" ");
      

      if(t < 29){
        digitalWrite(relay1, LOW); //ON LAMPU
        Serial.println("...........................  LAMPU ON");
        lcd.setCursor(0,1);
        lcd.print("Lampu ON");}

      if(t > 30){
        digitalWrite(relay1, HIGH); //OFF LAMPU
        Serial.println("...........................  LAMPU OFF");
        lcd.setCursor(0,1);
        lcd.print("Lampu OFF");}
        Serial.println(" ");
        }
      
        if (FBStatus == "true") {   //SISTEM MANUAL                                           
        Serial.println("............. Sistem Manual ............");
        lcd.clear();
        lcd.print("Sistem Manual");  
        lcd.setCursor(0,1);
        lcd.print(" ");
        

        if (Firebase.getString(firebaseData, "/EGG_INCUBATION/Lampu")) { //Sistem Lampu
          if  (firebaseData.dataType() == "string") {
          String FBStatus = firebaseData.stringData();
          if (FBStatus == "false") {                                                         
              Serial.println("...........................  LAMPU OFF");                     
            lcd.setCursor(0,1);
            lcd.print("Lampu OFF");
            digitalWrite(relay1, HIGH); } //Di relaynya terbalik

          else if (FBStatus == "true") {                                                  
                  Serial.println("...........................  LAMPU ON");
                    lcd.setCursor(0,1);
                    lcd.print("Lampu ON");
                    
                  digitalWrite(relay1, LOW); 
                  Serial.println(" ");                                               
                  }
          }
          else {Serial.println("Salah kode Lampu! isi dengan data ON/OFF");}
        }
      }
    }
      else {Serial.println("Salah kode Sistem_Manual! isi dengan data ON/OFF");}
    }
  
  delay(2000); // 2 DETIK
}