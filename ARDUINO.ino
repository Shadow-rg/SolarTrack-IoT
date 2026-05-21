#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ==================================================
// CONFIGURACIÓN DE SENSORES LDR (Fotodiodos)
// ==================================================
const int LDR_TOP_LEFT = A0;     // LDR superior izquierdo
const int LDR_TOP_RIGHT = A1;    // LDR superior derecho
const int LDR_BOTTOM_LEFT = A2;  // LDR inferior izquierdo
const int LDR_BOTTOM_RIGHT = A3; // LDR inferior derecho

const int LIGHT_THRESHOLD = 10;  // Umbral de diferencia de luz

// ==================================================
// CONFIGURACIÓN DE SERVOMOTORES
// ==================================================
Servo servo_horizontal;  // Servo para movimiento horizontal
Servo servo_vertical;    // Servo para movimiento vertical

int pos_sh = 90; // Posición inicial servo horizontal
int pos_sv = 90; // Posición inicial servo vertical

const int UPPER_LIMIT_POS = 160; // Límite superior de movimiento
const int LOWER_LIMIT_POS = 20;  // Límite inferior de movimiento

// ==================================================
// SENSOR DE CORRIENTE ACS712
// ==================================================
const int CURRENT_SENSOR = A6;   // Pin analógico del sensor
const float SENSIBILITY = 0.185; // Sensibilidad del ACS712 (185 mV/A)
const int CURRENT_SAMPLES = 10;  // Número de muestras para promedio

// ==================================================
// PANTALLA LCD I2C
// ==================================================
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C y dimensiones

unsigned long lastTime = 0;     // Última actualización
unsigned long threshold = 2000; // Intervalo de actualización (ms)

// ==================================================
// SETUP
// ==================================================
void setup() {
  Serial.begin(115200);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();

  // Inicializar servos
  servo_vertical.attach(5);
  servo_horizontal.attach(6);
  servo_vertical.write(90);
  servo_horizontal.write(90);

  // Mensaje inicial
  lcd.setCursor(0,0);
  lcd.print("SolarTrack IoT");
  delay(2000);
  lcd.clear();
}

// ==================================================
// LOOP PRINCIPAL
// ==================================================
void loop() {
  // Lectura de los 4 LDRs
  int ldr_tl_value = analogRead(LDR_TOP_LEFT);
  int ldr_tr_value = analogRead(LDR_TOP_RIGHT);
  int ldr_bl_value = analogRead(LDR_BOTTOM_LEFT);
  int ldr_br_value = analogRead(LDR_BOTTOM_RIGHT);

  // Promedios para calcular dirección de luz
  int average_top    = (ldr_tl_value + ldr_tr_value) / 2;
  int average_bottom = (ldr_bl_value + ldr_br_value) / 2;
  int average_left   = (ldr_tl_value + ldr_bl_value) / 2;
  int average_right  = (ldr_tr_value + ldr_br_value) / 2;

  // Ajustar posición del panel solar
  moveSolarTracker(average_top, average_bottom, average_left, average_right);

  // Actualización periódica de datos
  if((millis() - lastTime) > threshold){
    lastTime = millis();

    // Medición de corriente
    float current_measured = medirCorriente(CURRENT_SENSOR, SENSIBILITY, CURRENT_SAMPLES);

    // Intensidad solar promedio
    int solar_intensity = (ldr_tl_value + ldr_tr_value + ldr_bl_value + ldr_br_value) / 4;

    // Cálculo de eficiencia y batería 
    int efficiency = map(solar_intensity, 0, 1023, 40, 99);
    int battery    = map(solar_intensity, 0, 1023, 15, 100);

    // Mostrar en LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("I:");
    lcd.print(current_measured,1);
    lcd.print("A");

    lcd.setCursor(9,0);
    lcd.print("B:");
    lcd.print(battery);
    lcd.print("%");

    lcd.setCursor(0,1);
    lcd.print("H:");
    lcd.print(pos_sh);
    lcd.print(" V:");
    lcd.print(pos_sv);

    // Enviar datos en formato JSON por Serial
    String payload = "{";
    payload += "\"solar\":" + String(solar_intensity) + ",";
    payload += "\"current\":" + String(current_measured,2) + ",";
    payload += "\"battery\":" + String(battery) + ",";
    payload += "\"servoH\":" + String(pos_sh) + ",";
    payload += "\"servoV\":" + String(pos_sv) + ",";
    payload += "\"efficiency\":" + String(efficiency);
    payload += "}";
    Serial.println(payload);
  }

  delay(30); // Pequeña pausa para estabilidad
}

// ==================================================
// FUNCIÓN: Movimiento del seguidor solar
// ==================================================
void moveSolarTracker(int average_top, int average_bottom, int average_left, int average_right){
  // Movimiento vertical
  if((average_top - average_bottom) > LIGHT_THRESHOLD && pos_sv < UPPER_LIMIT_POS){
    pos_sv++;
    servo_vertical.write(pos_sv);
  }
  else if((average_bottom - average_top) > LIGHT_THRESHOLD && pos_sv > LOWER_LIMIT_POS){
    pos_sv--;
    servo_vertical.write(pos_sv);
  }

  // Movimiento horizontal
  if((average_left - average_right) > LIGHT_THRESHOLD && pos_sh < UPPER_LIMIT_POS){
    pos_sh++;
    servo_horizontal.write(pos_sh);
  }
  else if((average_right - average_left) > LIGHT_THRESHOLD && pos_sh > LOWER_LIMIT_POS){
    pos_sh--;
    servo_horizontal.write(pos_sh);
  }
}

// ==================================================
// FUNCIÓN: Medición de corriente con ACS712
// ==================================================
float medirCorriente(int current_sensor, float sensibility, int samples){
  float sensor_read = 0;

  // Promediar lecturas
  for(int i=0; i<samples; i++){
    sensor_read += analogRead(current_sensor);
  }
  sensor_read = sensor_read / samples;

  // Convertir a voltaje
  sensor_read = sensor_read * (5.0 / 1023.0);

  // Calcular corriente (desplazamiento respecto a 2.5V)
  float current_measured = (sensor_read - 2.5) / sensibility;

  return current_measured;
}