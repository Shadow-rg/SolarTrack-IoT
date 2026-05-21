# SolarTrack IoT
SolarTrack IoT es un sistema de seguimiento solar inteligente desarrollado como proyecto académico.  
Combina hardware (ESP32/Arduino, sensores y servomotores) con un dashboard web conectado vía MQTT para monitorear en tiempo real la producción energética y la eficiencia de un panel solar.

## Características principales
- Seguimiento solar automático con servomotores en dos ejes (horizontal y vertical).
- Registro de intensidad solar, corriente, nivel de batería y eficiencia.
- Conexión WiFi + MQTT para envío de datos en tiempo real.
- Dashboard web con métricas dinámicas, gráficas comparativas y exportación CSV.
- Sincronización automática de datos cuando el dispositivo recupera conexión.

## Estructura del repositorio
- **arduino/** → Código del microcontrolador (ESP32/Arduino).
- **web/** → Código del dashboard web (HTML, JS, CSS).
- **docs/** → Diagramas de circuito y arquitectura del sistema.

## Hardware utilizado
- ESP32
- Arduino UNO  
- 2 panel solar 1.9W 135x130 mm 5V 380mA con cables soldados
- XS-170 Modulo de Expanción
- 1 sensor de corriente ACS712 30A
- TP4056 Cargador de baterias tipo C 5V
- 3 Baterias 18650 3.7V
- Display LCD 16x2 con I2c fondo azul
- MT3608 Elevador de voltaje Boost Step Up 6W 2A
- Nano 3.0 con cable USB
- 2 MG996R Servomotor Engranes de metal
- 4 Módulos sensores Fotoresistor LDR
- Interruptor Redondo KCD2 Rojo
- 2 metros de cable eléctrico 22 AWG Negro PVC
- 2 metros de cable eléctrico 22 AWG Rojo PVC
- 2 Juegos de Cables dupont largos 20cm
- Conector USB Hembra Tipo A 4 pines
- Porta pilas 18650 3 pilas 

## Diagrama del circuito
### Arduino con todos los componentes (sin ESP32)
![Circuito Arduino SolarTrack](docs/circuito_arduino_solartrack.jpeg)

### Arduino + placa de expansión (con ESP32)
![Circuito Arduino + ESP32](docs/circuito_arduino_esp32.jpeg)

## Ejecución
1. **Microcontrolador**: cargar arduino.ino en el ESP32/Arduino.  
2. **Servidor/Dashboard**: abrir web/index.html para login y web/dashboard.html para monitoreo.  
3. **MQTT Broker**: configurar en el código del ESP32 el broker (ej. HiveMQ, Mosquitto).  

## Licencia
Este proyecto está bajo la licencia MIT. Puedes usarlo, modificarlo y compartirlo libremente.
