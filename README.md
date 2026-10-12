# Détection de couleurs STM32F4 : capteur TCS34725 + écran LCD I2C

Système de détection de couleurs sur microcontrôleur **STM32F4** (famille F40x). Un capteur **TCS34725** mesure la lumière réfléchie par un objet, le programme en déduit la couleur, puis l'affiche sur un **écran LCD 16x2 en I2C** et sur la liaison série (UART).

Couleurs reconnues : Orange, Rouge, Rose, Violet, Bleu, Cyan, Vert, Jaune.



## Matériel

- Carte STM32F4 (F407, par exemple F4-Discovery)
- Capteur de couleur TCS34725
- Écran LCD 16x2 avec adaptateur I2C (PCF8574)
- Adaptateur USB-série 3,3 V (optionnel, pour le terminal)

## Câblage

Le capteur et l'écran partagent le même bus **I2C1**.

| Signal | Broche STM32 | Capteur TCS34725 | LCD (backpack I2C) |
|---|---|---|---|
| SCL | PB6 | SCL | SCL |
| SDA | PB9 (ou PB7, voir ci-dessous) | SDA | SDA |
| Alimentation | | 3,3 V | 5 V |
| Masse | GND | GND | GND |
| UART TX | PA9 | | |

- Adresses I2C : capteur `0x29`, écran `0x27` (ou `0x3F` selon le module).
- Choix de SDA : dans `i2c.h`, `I2C1_SDA_ON_PB9` vaut `1` pour PB9 et `0` pour PB7.
- Le bus tourne à **100 kHz**, limite du PCF8574.
- Terminal série : **115200 bauds, 8N1**, PA9 vers RX de l'adaptateur.

## Organisation du code

| Fichier | Rôle |
|---|---|
| `Color.c` | Programme principal : calcul de la teinte, plages de couleurs, affichage |
| `tcs34725.c/.h` | Pilote du capteur (ID, initialisation, lecture RGBC) |
| `lcd.c/.h` | Pilote de l'écran LCD via PCF8574 |
| `i2c.c/.h` | Couche I2C1 bas niveau (start, stop, lecture, écriture) |
| `usart.c/.h` | Liaison série USART1 (émission) |
| `delay.c/.h` | Délais en µs et ms via SysTick |
| `stm32f4xx_conf.h` | Sélection des périphériques de la bibliothèque |
| `stm32f4xx_it.c/.h` | Gestionnaires d'interruptions (vides) |

## Compilation

1. Créer un projet pour votre STM32F4 avec la **STM32F4xx StdPeriph Library**.
2. Ajouter les sources du projet ci-dessus, ainsi que `misc.c`, `stm32f4xx_gpio.c`, `stm32f4xx_i2c.c`, `stm32f4xx_rcc.c`, `stm32f4xx_usart.c`, `system_stm32f4xx.c` et le fichier de démarrage de la carte.
3. Définir les symboles : `USE_STDPERIPH_DRIVER` et `STM32F40_41xxx`.
4. Régler l'horloge selon le quartz de la carte. Pour une F4-Discovery : `HSE_VALUE=8000000` et `PLL_M = 8` dans `system_stm32f4xx.c`.

## Fonctionnement

1. Initialisation de l'I2C1, puis du LCD, puis de l'UART.
2. Lecture de l'ID du capteur (`0x44` attendu) et affichage sur l'écran.
3. Boucle principale :
   - 4 mesures espacées d'un temps d'intégration (environ 50 ms) ;
   - calcul des ratios R/C, G/C, B/C (C = canal Clear), puis conversion en **teinte** ;
   - moyenne des teintes, puis comparaison aux plages de couleurs ;
   - affichage du nom de la couleur à chaque changement.

L'écran montre « Couleur : » sur la première ligne et la couleur détectée sur la seconde. Hors des plages connues, il affiche « Not in range ».

## Réglages

Dans `Color.c` :

- `N_SAMPLES` : nombre de mesures moyennées.
- `DEBUG_HUE` : à `1`, l'UART envoie la valeur Clear et la teinte (×1000) à chaque cycle, pour régler les seuils.

Dans `tcs34725.c` : `TCS_GAIN_VALUE` (gain) et `TCS_ATIME_VALUE` (temps d'intégration).

Les seuils de teinte viennent du projet d'origine, réglés pour un autre capteur. Ils sont à recalibrer avec votre capteur, vos objets et votre éclairage.

## Dépannage

| Symptôme | Piste |
|---|---|
| L'écran reste sur « Init capteur... » | Le capteur ne répond pas : vérifier câblage, alimentation et adresse. Le code n'a pas de timeout I2C |
| Écran vide, rétroéclairage allumé | Régler le potentiomètre de contraste, ou essayer `LCD_I2C_ADDR` à `0x3F` dans `lcd.h` |
| Caractères incorrects sur l'écran | Vérifier que le bus est à 100 kHz |
| « Capteur inconnu » | L'ID lu n'est ni `0x44` ni `0x4D` : vérifier le modèle du capteur et le câblage |
| Couleurs fausses | Recalibrer les seuils avec `DEBUG_HUE`, et adapter gain et éclairage |
| Délais ou baud rate faux | Vérifier `HSE_VALUE` et `PLL_M` |
