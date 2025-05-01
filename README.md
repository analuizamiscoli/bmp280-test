# ESP32S3 - BMP280 Test

Este projeto é um teste simples do sensor BMP280 usando ESP32-S3 com ESP-IDF.

## Endereços I2C usados:
- BMP280: `0x76`

## Como usar

1. Clone este repositório
2. Compile e grave com `idf.py build flash monitor`
3. Verifique a saída serial para os dados do sensor

## Componentes
- ESP32-S3
- Sensor BMP280
- Biblioteca I2C personalizada
