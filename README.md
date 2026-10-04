# 🎨 Détection de couleurs avec STM32 et TCS34725

## 📌 Description

Ce projet consiste à développer un système embarqué basé sur un **microcontrôleur STM32** permettant de détecter la couleur dominante d'un objet à l'aide du capteur **TCS34725**.

Le capteur mesure les composantes :

* 🔴 Rouge (R)
* 🟢 Vert (G)
* 🔵 Bleu (B)
* ⚪ Clear (luminosité globale)

Les données sont récupérées par le STM32 via le protocole **I²C**. Le microcontrôleur analyse ensuite les valeurs mesurées afin d'identifier la couleur dominante.

Le résultat est :

* affiché à l'aide de LEDs RGB ;
* envoyé via **UART** ;
* affiché sur un **LCD 16×2 I²C** ;
* comptabilisé pour chaque couleur.

---

## 🧩 Architecture du système

```text
                         ┌──────────────────┐
                         │      STM32       │
                         │                  │
                         │  Programme en C  │
                         └────────┬─────────┘
                                  │
                 ┌────────────────┼────────────────┐
                 │                │                │
                I²C              UART             GPIO
                 │                │                │
          ┌──────┴──────┐         │         ┌──────┴──────┐
          │             │         │         │             │
          ▼             ▼         ▼         ▼             ▼
      TCS34725         LCD      PC/     LEDs RGB     LED capteur
   Capteur couleur   16×2     Terminal
```

---

## 🔌 Connexions

### TCS34725 → STM32

| TCS34725 | STM32 | Fonction           |
| -------- | ----- | ------------------ |
| SCL      | PB10  | Horloge I²C        |
| SDA      | PB11  | Données I²C        |
| LED      | PB9   | Commande de la LED |
| VCC      | 3.3 V | Alimentation       |
| GND      | GND   | Masse              |


---

## 📡 Protocoles de communication

### I²C

Le **TCS34725** communique avec le STM32 via I²C.

Le STM32 agit comme **maître**, tandis que le TCS34725 agit comme **esclave**.

Les fonctions HAL utilisées sont :

```c
HAL_I2C_Master_Transmit();
HAL_I2C_Master_Receive();
```

L'adresse utilisée dans le code est :

```c
0x52
```

Le LCD est également piloté via une interface I²C.

---

### UART

Le protocole UART est utilisé pour transmettre le résultat de la détection.

Exemples de messages :

```text
= Red
= Green
= Blue
```

La transmission est réalisée avec :

```c
HAL_UART_Transmit();
```

---

### GPIO

Les GPIO permettent de :

* commander la LED du capteur ;
* allumer la LED rouge ;
* allumer la LED verte ;
* allumer la LED bleue.

La fonction HAL utilisée est :

```c
HAL_GPIO_WritePin();
```

---

# 🔄 Fonctionnement du programme

## 1. Activation de la LED du capteur

La fonction :

```c
Read_cts34725();
```

commence par activer la LED du TCS34725 :

```c
HAL_GPIO_WritePin(
    LedSensor_GPIO_Port,
    LedSensor_Pin,
    GPIO_PIN_SET
);
```

Cette LED permet d'éclairer l'objet afin d'obtenir une mesure de couleur.

---

## 2. Lecture du canal Clear

Le registre Clear est lu via I²C :

```c
0x94
```

Deux octets sont récupérés afin de reconstruire une valeur sur 16 bits.

```c
Clear_value =
    (((int)Clear_data[1]) << 8)
    | Clear_data[0];
```

---

## 3. Lecture du rouge

Le registre rouge utilisé est :

```c
0x96
```

Les deux octets reçus sont combinés afin d'obtenir :

```c
Red_value
```

---

## 4. Lecture du vert

Le registre vert est :

```c
0x98
```

La valeur obtenue est stockée dans :

```c
Green_value
```

---

## 5. Lecture du bleu

Le registre bleu est :

```c
0x9A
```

La valeur obtenue est stockée dans :

```c
Blue_value
```

---

## 6. Identification de la couleur

Les valeurs R, G et B sont comparées.

### Rouge

```c
if(Red_value > Blue_value &&
   Red_value > Green_value)
```

Si la composante rouge est la plus importante :

```text
→ Rouge détecté
```

### Vert

```c
else if(Green_value > Red_value &&
        Green_value > Blue_value)
```

Si la composante verte est la plus importante :

```text
→ Vert détecté
```

### Bleu

```c
else if(Blue_value > Red_value &&
        Blue_value > Green_value)
```

Si la composante bleue est la plus importante :

```text
→ Bleu détecté
```

---

# 💡 Indication par LED

Lorsqu'une couleur est détectée, la LED correspondante est activée.

### Rouge

```text
🔴 ON
🟢 OFF
🔵 OFF
```

### Vert

```text
🔴 OFF
🟢 ON
🔵 OFF
```

### Bleu

```text
🔴 OFF
🟢 OFF
🔵 ON
```

---

# 📺 Affichage LCD

Le LCD affiche le nombre de détections pour chaque couleur.

Exemple :

```text
R:3 G:2 B:5
```

Cela signifie :

```text
3 détections rouges
2 détections vertes
5 détections bleues
```

L'affichage est réalisé avec :

```c
lcd16x2_i2c_printf();
```

---

# 🔢 Gestion des compteurs

Les compteurs utilisés sont :

```c
uint32_t NR;
uint32_t NG;
uint32_t NB;
```

avec :

* `NR` → nombre de détections rouges
* `NG` → nombre de détections vertes
* `NB` → nombre de détections bleues

La variable :

```c
c
```

permet de mémoriser la dernière couleur détectée.

Cela évite d'incrémenter le compteur à chaque boucle lorsque la même couleur reste devant le capteur.

Exemple :

```text
Objet rouge
    ↓
Détection rouge
    ↓
NR++
    ↓
c = 1
    ↓
Rouge toujours présent
    ↓
pas de nouvelle incrémentation
```

---

# 🧪 Test du capteur

La fonction :

```c
Test_cts34725();
```

permet de vérifier la réponse du capteur via I²C.

Le programme lit une information d'identification du composant et compare la valeur reçue à la valeur attendue.

Cela permet de vérifier que la communication avec le capteur fonctionne.

---

# 🔁 Réinitialisation

La fonction :

```c
rest_cal();
```

remet les compteurs à zéro :

```c
NR = 0;
NG = 0;
NB = 0;
```

La fonction :

```c
resetvar();
```

réinitialise également plusieurs variables internes du programme.

---

# 🛠️ Technologies utilisées

| Technologie           | Utilisation                        |
| --------------------- | ---------------------------------- |
| C                     | Développement embarqué             |
| STM32                 | Microcontrôleur                    |
| STM32 HAL             | Abstraction matérielle             |
| I²C                   | Communication avec TCS34725 et LCD |
| UART                  | Transmission des résultats         |
| GPIO                  | Commande des LEDs                  |
| TCS34725              | Détection des couleurs             |
| LCD 16×2              | Affichage                          |
| STM32CubeMX / CubeIDE | Configuration et développement     |

---


# 🚀 Améliorations possibles



## 1. Amélioration de la classification

La méthode actuelle repose uniquement sur :

```text
R > G et R > B
```

Elle peut donc être sensible aux conditions d'éclairage.

Une meilleure approche serait de normaliser les valeurs :

```text
Rnorm = R / Clear
Gnorm = G / Clear
Bnorm = B / Clear
```

puis d'utiliser des seuils de classification.

---

## 2. Calibration

Une calibration pourrait être ajoutée afin de prendre en compte :

* la lumière ambiante ;
* la distance entre le capteur et l'objet ;
* les caractéristiques de la LED ;
* les différences entre les capteurs.

---

## 3. Remplacer les valeurs magiques

Au lieu de :

```c
0x94
0x96
0x98
0x9A
```

on pourrait définir :

```c
#define TCS34725_CLEAR_REG 0x94
#define TCS34725_RED_REG   0x96
#define TCS34725_GREEN_REG 0x98
#define TCS34725_BLUE_REG  0x9A
```

Cela rend le code plus lisible et plus maintenable.

---

## 6. Utilisation de fonctions HAL adaptées

Pour les lectures de registres, il serait possible d'utiliser les fonctions HAL de lecture mémoire I²C afin de rendre le code plus clair et plus robuste.

---

# 🎯 Compétences mises en œuvre

Ce projet permet de mettre en pratique plusieurs compétences en systèmes embarqués :

* Programmation en **C**
* Développement sur **STM32**
* Utilisation de la **STM32 HAL**
* Communication **I²C**
* Communication **UART**
* Configuration et utilisation des **GPIO**
* Lecture de registres d'un capteur
* Manipulation de données 16 bits
* Traitement et classification de mesures
* Affichage sur LCD
* Gestion d'E/S embarquées
* Débogage et validation d'un système embarqué

---

# 📌 Résumé

Le système repose sur un STM32 qui communique avec un TCS34725 par I²C. Le capteur fournit les composantes rouge, verte et bleue sur 16 bits. Le STM32 compare ces valeurs pour déterminer la couleur dominante.

Le résultat est ensuite :

```text
                    TCS34725
                       │
                       │ I²C
                       ▼
                     STM32
                       │
          ┌────────────┼────────────┐
          │            │            │
         GPIO         UART          I²C
          │            │            │
          ▼            ▼            ▼
       LEDs RGB       PC          LCD
```
