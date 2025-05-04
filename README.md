# ESP32S3 - BMP280 + SD Card Logger

Este projeto realiza a leitura dos dados do sensor BMP280 e grava os valores de temperatura e pressão em um cartão microSD, usando ESP32-S3 com ESP-IDF.

## Endereços I2C usados:
- BMP280: `0x76`

## Pinos SPI usados para o cartão SD:
- CS: GPIO10  
- MOSI: GPIO11  
- MISO: GPIO13  
- CLK: GPIO12

## Como usar

1. Clone este repositório  
2. Compile e grave com `idf.py build flash monitor`  
3. Insira um cartão microSD e verifique a gravação no arquivo criado

## Componentes
- ESP32-S3  
- Sensor BMP280  
- Cartão microSD  
- Biblioteca I2C personalizada  
- Interface SPI para SD

