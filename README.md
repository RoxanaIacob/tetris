# Tetris

Proiectul constă în realizarea jocului Tetris, folosind Arduino și o matrice de LED-uri.  Jocul în sine se desfășoară pe matricea de led-uri, piesele fiind controlate cu ajutorul unui Joystick, iar nivelul și scorul fiind afișate pe un ecran LCD.



Componente:
1.	Microcontroller Arduino Uno: https://cleste.ro/arduino-uno-r3-atmega328p 
2.	Modul matrice led 8x8 MAX7219: https://cleste.ro/modul-matrice-max7219 
3.	Modul Joystick PS2: https://cleste.ro/modul-joystick-ps2 
4.	Ecran Lcd 1602 IIC/I2C: https://cleste.ro/ecran-lcd-1602-iic-i2c 
5.	2 butoane

Jocul începe cu afișarea pe ecranul LCD a celor două opțiuni posibile: S – start și R – reset. Prin apăsarea butonului start jocul începe: apariția pieselor pe matricea de led-uri. La fiecare piesă jucătorul primește un punctaj, când un rând din matrice este plin se resetează matricea, punctajul neschimbându-se, pentru a putea juca cât mai mult. În cazul în care piesa nu mai are loc, jocul se oprește. În orice moment se poate da reset la joc prin apăsarea butonului de reset.


Piese:




Controlul pieselor:



Mișcarea Joystick-ului la stânga: piesa se deplasează la stânga 
Mișcarea Joystick-ului la dreapta: piesa se deplasează la dreapta
Mișcarea Joystick-ului în sus: piesa își schimbă orientarea
Mișcarea Joystick-ului în jos: piesa se deplasează rapid în jos


Bibliografie

https://create.arduino.cc/projecthub/archievkumar19/the-tetris-4de129 
https://create.arduino.cc/projecthub/electropeak/using-8x8-dot-matrix-led-with-arduino-cascade-connection-5cecff
https://github.com/jaeheonshim/arduino-tetris/blob/main/tetris.ino 
