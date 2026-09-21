# Comunicație UART pe microcontrolere 8051 și PIC18F8722

---

# 🇷🇴 Română

## 1. Despre proiect

Acest proiect prezintă implementarea comunicației seriale **UART asincrone** pe două arhitecturi de microcontrolere: **8051** și **PIC18F8722**.

Pentru fiecare platformă au fost realizate două implementări:

1. comunicație UART gestionată prin **polling**;
2. comunicație UART gestionată prin **întreruperi hardware**.

Proiectul urmărește aceeași funcționalitate de bază — recepția unor comenzi prin interfața serială și modificarea stării unor ieșiri digitale — dar utilizează două metode diferite de procesare a recepției.

Toate implementările sunt în **C**, iar comportamentul circuitelor este verificat în **Proteus**, utilizând comunicația dintre microcontroler și un terminal serial virtual.

---

## 2. Funcționalitatea implementată

Microcontrolerul primește caractere prin interfața UART și le interpretează în funcție de implementarea utilizată.

### 8051 – varianta polling

În implementarea `01-uart-polling`, caracterele primite sunt procesate astfel:

| Caracter | Acțiune |
|---|---|
| `1` | Aprinde LED-ul conectat la `P1.0` |
| `2` | Aprinde LED-ul conectat la `P1.1` |
| `0` | Stinge ambele LED-uri |

Recepția este realizată prin verificarea continuă a flag-ului `RI`.

După recepție, caracterul primit este transmis înapoi prin UART, iar flag-ul de recepție este resetat.

---

### 8051 – varianta cu întreruperi

În implementarea `02-uart-interrupt`, recepția UART este mutată în rutina:

```c
void serial_ISR() interrupt 4
```

La apariția unei recepții:

1. caracterul este citit din `SBUF`;
2. valoarea este memorată în variabila `c`;
3. caracterul este retransmis;
4. flag-ul `RI` este resetat.

În bucla principală, valoarea recepționată este interpretată ca o cifră între `0` și `9`.

Pentru fiecare caracter este asociată o valoare hexadecimală corespunzătoare modelului de segmente:

| Caracter | Valoare P1 |
|---|---:|
| `0` | `0x3F` |
| `1` | `0x06` |
| `2` | `0x5B` |
| `3` | `0x4F` |
| `4` | `0x66` |
| `5` | `0x6D` |
| `6` | `0x7D` |
| `7` | `0x07` |
| `8` | `0x7F` |
| `9` | `0x6F` |

Prin urmare, această variantă demonstrează nu doar recepția UART prin întrerupere, ci și utilizarea caracterului recepționat pentru generarea unui model de afișare digitală pe portul `P1`.

---

### PIC18F8722 – varianta polling

În implementarea `01-uart-polling`, microcontrolerul **PIC18F8722** primește comenzile prin UART și controlează două ieșiri digitale:

```text
RD0 → LED 1
RD1 → LED 2
```

Comenzile implementate sunt:

| Caracter | Acțiune |
|---|---|
| `1` | Aprinde LED-ul de pe `RD0` |
| `2` | Aprinde LED-ul de pe `RD1` |
| `0` | Stinge ambele LED-uri |

Recepția este blocantă, deoarece funcția `read_serial()` așteaptă activ setarea flag-ului `RC1IF`.

La transmitere, funcția `write_serial()` așteaptă disponibilitatea registrului de transmisie prin `TX1IF`.

Programul transmite și mesaje de stare către terminal, de exemplu:

```text
Apasati 1, 2 sau 0
Led 1 aprins
Led 2 aprins
Led-uri stinse
```

---

### PIC18F8722 – varianta cu întreruperi

În implementarea `02-uart-interrupt`, recepția UART este gestionată în rutina:

```c
void interrupt ISR()
```

Întreruperea pentru recepție este activată prin:

```c
GIE = 1;
PEIE = 1;
RC1IE = 1;
```

Atunci când `RC1IF` este setat, caracterul este citit din `RCREG1` și este procesat direct în rutina de întrerupere.

Comenzile sunt:

```text
'1' → RD0 = 1
'2' → RD1 = 1
'0' → RD0 = 0, RD1 = 0
```

Bucla principală nu mai trebuie să aștepte recepția unui caracter:

```c
while(1)
{
}
```

Astfel, procesarea UART este declanșată de evenimentul hardware, iar procesorul poate utiliza bucla principală pentru alte activități.

---

## 3. Configurarea UART

### 8051

Interfața serială este configurată în **Serial Mode 1**:

```c
SCON = 0x50;
```

Timer 1 este configurat în modul 2 și utilizat pentru generarea baud rate-ului:

```c
TMOD = 0x20;
TH1 = 0xFD;
TR1 = 1;
```

În varianta cu întreruperi sunt activate întreruperea globală și întreruperea serială:

```c
EA = 1;
ES = 1;
```

Rutina serială utilizează vectorul de întrerupere `interrupt 4`.

---

### PIC18F8722

Comunicația este configurată ca UART asincron, cu:

```c
SYNC = 0;
CSRC = 1;
BRGH = 1;
CREN = 1;
TXEN = 1;
SPEN = 1;
```

Liniile hardware utilizate pentru comunicația serială sunt:

```text
RC6 → TX
RC7 → RX
```

În varianta polling este utilizat:

```c
SPBRG1 = 12;
```

iar în varianta cu întreruperi:

```c
SPBRG1 = 25;
```

Configurațiile sunt astfel implementate separat pentru cele două variante ale proiectului.

---

## 4. Polling vs. întreruperi

Un obiectiv important al proiectului este compararea celor două metode de gestionare a recepției UART.

### Polling

În varianta polling, programul verifică în mod repetat starea perifericului UART.

Structura generală este:

```text
Bucla principală
      │
      ▼
Verifică UART
      │
      ├── Nu există date → așteaptă
      │
      └── Date disponibile
               │
               ▼
        Citește caracterul
               │
               ▼
        Procesează comanda
```

Avantajul este simplitatea implementării. Dezavantajul este că procesorul rămâne ocupat cu verificarea perifericului în timpul așteptării.

---

### Întreruperi

În varianta cu întreruperi, recepția este declanșată de hardware.

```text
Program principal
      │
      ▼
Execută alte activități
      │
      │
      ├──── Recepție UART
      │
      ▼
   ISR UART
      │
      ▼
Citește și procesează caracterul
      │
      ▼
Revine în programul principal
```

Această abordare elimină necesitatea verificării continue a flag-ului UART în bucla principală și permite separarea procesării evenimentului de execuția programului principal.

---

## 5. Structura proiectului

```text
Comunicatie-UART-pe-microcontrolere-8051-si-PIC18F8722/
│
├── README.md
│
├── 8051/
│   ├── 01-uart-polling/
│   │   ├── proteus/
│   │   │   └── project.pdsprj
│   │   └── src/
│   │       └── main.c
│   │
│   └── 02-uart-interrupt/
│       ├── proteus/
│       │   └── project.pdsprj
│       └── src/
│           └── main.c
│
└── pic18f8722/
    ├── 01-uart-polling/
    │   ├── proteus/
    │   │   └── project.pdsprj
    │   └── src/
    │       └── main.c
    │
    └── 02-uart-interrupt/
        ├── proteus/
        │   └── project.pdsprj
        └── src/
            └── main.c
```

Structura separă explicit:

- platforma hardware;
- metoda de gestionare a UART;
- codul sursă;
- proiectul de simulare Proteus.

---

## 6. Simulare în Proteus

Fiecare implementare este asociată unui proiect Proteus separat.

Terminalul virtual este utilizat pentru transmiterea caracterelor către microcontroler și pentru observarea datelor transmise de acesta.

Fluxul de comunicație este:

```text
┌──────────────────┐
│  Virtual Terminal│
└────────┬─────────┘
         │ UART
         ▼
┌──────────────────┐
│  Microcontroller │
│  8051 / PIC18F   │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│ Digital Outputs  │
│ LEDs / P1        │
└──────────────────┘
```

Simularea permite verificarea comportamentului UART fără utilizarea unui circuit fizic.

---

## 7. Tehnologii și concepte utilizate

- **C**
- **8051**
- **PIC18F8722**
- **UART / USART**
- **Serial communication**
- **Polling**
- **Hardware interrupts**
- **Interrupt Service Routines (ISR)**
- **UART registers and flags**
- **Timers**
- **Digital I/O**
- **Proteus**
- **Virtual Terminal**
- **Embedded systems**

---

## 8. Competențe demonstrate

Prin acest proiect sunt demonstrate următoarele competențe tehnice:

- programarea microcontrolerelor în C;
- configurarea perifericului UART;
- transmiterea și recepția datelor seriale;
- lucrul cu registre hardware și flag-uri;
- configurarea timerelor;
- utilizarea întreruperilor hardware;
- implementarea rutinelor ISR;
- controlul porturilor digitale;
- diferențierea între polling și interrupt-driven processing;
- depanarea și validarea unei aplicații embedded prin simulare.

---

# 🇬🇧 English

## 1. Project Overview

This project implements and compares **asynchronous UART serial communication** on two microcontroller architectures: **8051** and **PIC18F8722**.

For each platform, two separate implementations are provided:

1. UART communication using **polling**;
2. UART reception using **hardware interrupts**.

The same general concept is implemented across both platforms: characters are received through the serial interface and interpreted as commands that modify digital outputs.

All implementations are written in **C** and validated through **Proteus simulations** using a virtual serial terminal.

---

## 2. Implemented Functionality

The microcontroller receives characters through UART and processes them according to the selected implementation.

### 8051 – Polling Implementation

In `01-uart-polling`, received characters control two LEDs connected to the `P1` port:

| Character | Action |
|---|---|
| `1` | Turns on the LED connected to `P1.0` |
| `2` | Turns on the LED connected to `P1.1` |
| `0` | Turns both LEDs off |

UART reception is handled by continuously checking the `RI` flag.

Once a character is received, it is read from `SBUF`, echoed back through UART, and the reception flag is cleared.

---

### 8051 – Interrupt-driven Implementation

In `02-uart-interrupt`, UART reception is handled inside:

```c
void serial_ISR() interrupt 4
```

When a character is received:

1. the character is read from `SBUF`;
2. it is stored in `c`;
3. it is transmitted back through UART;
4. the `RI` flag is cleared.

The main loop interprets characters from `0` to `9` and writes the corresponding hexadecimal digit pattern to port `P1`.

| Character | P1 value |
|---|---:|
| `0` | `0x3F` |
| `1` | `0x06` |
| `2` | `0x5B` |
| `3` | `0x4F` |
| `4` | `0x66` |
| `5` | `0x6D` |
| `6` | `0x7D` |
| `7` | `0x07` |
| `8` | `0x7F` |
| `9` | `0x6F` |

This implementation therefore demonstrates both interrupt-driven UART reception and conversion of received numerical characters into display patterns.

---

### PIC18F8722 – Polling Implementation

In `01-uart-polling`, the **PIC18F8722** receives UART commands and controls two digital outputs:

```text
RD0 → LED 1
RD1 → LED 2
```

The supported commands are:

| Character | Action |
|---|---|
| `1` | Turns on the LED connected to `RD0` |
| `2` | Turns on the LED connected to `RD1` |
| `0` | Turns both LEDs off |

UART reception is blocking because `read_serial()` continuously waits for the `RC1IF` flag.

Transmission is similarly synchronized with the `TX1IF` flag through `write_serial()`.

The microcontroller also sends status messages back to the virtual terminal, such as:

```text
Apasati 1, 2 sau 0
Led 1 aprins
Led 2 aprins
Led-uri stinse
```

---

### PIC18F8722 – Interrupt-driven Implementation

In `02-uart-interrupt`, UART reception is handled inside:

```c
void interrupt ISR()
```

UART reception interrupts are enabled through:

```c
GIE = 1;
PEIE = 1;
RC1IE = 1;
```

When `RC1IF` is asserted, the received character is read from `RCREG1` and processed directly inside the interrupt service routine.

The implemented commands are:

```text
'1' → RD0 = 1
'2' → RD1 = 1
'0' → RD0 = 0, RD1 = 0
```

The main program loop remains free:

```c
while(1)
{
}
```

This demonstrates an interrupt-driven architecture in which UART reception is handled asynchronously rather than through continuous polling in the main loop.

---

## 3. UART Configuration

### 8051

The serial peripheral is configured in **Serial Mode 1**:

```c
SCON = 0x50;
```

Timer 1 is configured in Mode 2 and used for serial baud-rate generation:

```c
TMOD = 0x20;
TH1 = 0xFD;
TR1 = 1;
```

For the interrupt-driven implementation, the global and serial interrupts are enabled:

```c
EA = 1;
ES = 1;
```

The UART interrupt handler uses interrupt vector `4`.

---

### PIC18F8722

The UART is configured for asynchronous communication using:

```c
SYNC = 0;
CSRC = 1;
BRGH = 1;
CREN = 1;
TXEN = 1;
SPEN = 1;
```

The serial interface uses:

```text
RC6 → TX
RC7 → RX
```

The polling implementation uses:

```c
SPBRG1 = 12;
```

while the interrupt-driven implementation uses:

```c
SPBRG1 = 25;
```

The UART configuration is therefore implemented independently for each version.

---

## 4. Polling vs. Interrupt-driven Reception

One of the main purposes of the project is to demonstrate the difference between polling and hardware-interrupt-based UART reception.

### Polling

With polling, the program repeatedly checks the UART peripheral for a newly received character.

```text
Main loop
    │
    ▼
Check UART
    │
    ├── No data → wait
    │
    └── Data available
             │
             ▼
       Read character
             │
             ▼
       Process command
```

This approach is straightforward to implement, but the processor remains occupied while waiting for incoming data.

---

### Interrupts

With interrupts, the UART peripheral signals the processor when a character is received.

```text
Main program
     │
     ▼
Execute other tasks
     │
     │
     ├──── UART reception
     │
     ▼
  UART ISR
     │
     ▼
Read and process character
     │
     ▼
Return to main program
```

This approach separates event handling from the main execution flow and avoids continuously checking the UART reception flag.

---

## 5. Project Structure

```text
Comunicatie-UART-pe-microcontrolere-8051-si-PIC18F8722/
│
├── README.md
│
├── 8051/
│   ├── 01-uart-polling/
│   │   ├── proteus/
│   │   │   └── project.pdsprj
│   │   └── src/
│   │       └── main.c
│   │
│   └── 02-uart-interrupt/
│       ├── proteus/
│       │   └── project.pdsprj
│       └── src/
│           └── main.c
│
└── pic18f8722/
    ├── 01-uart-polling/
    │   ├── proteus/
    │   │   └── project.pdsprj
    │   └── src/
    │       └── main.c
    │
    └── 02-uart-interrupt/
        ├── proteus/
        │   └── project.pdsprj
        └── src/
            └── main.c
```

The repository structure separates the hardware platform, UART handling method, source code, and Proteus simulation files.

---

## 6. Proteus Simulation

Each implementation is associated with a separate Proteus project.

A virtual serial terminal is used to send characters to the microcontroller and observe transmitted data.

The communication flow is:

```text
┌──────────────────┐
│  Virtual Terminal│
└────────┬─────────┘
         │ UART
         ▼
┌──────────────────┐
│  Microcontroller │
│  8051 / PIC18F   │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│ Digital Outputs  │
│ LEDs / P1        │
└──────────────────┘
```

Proteus provides a controlled environment for validating the UART behavior and digital outputs without requiring physical hardware.

---

## 7. Technologies and Concepts

- **C**
- **8051**
- **PIC18F8722**
- **UART / USART**
- **Asynchronous serial communication**
- **Polling**
- **Hardware interrupts**
- **Interrupt Service Routines (ISR)**
- **UART registers and status flags**
- **Timers**
- **Digital I/O**
- **Proteus**
- **Virtual Terminal**
- **Embedded systems**

---

## 8. Technical Skills Demonstrated

This project demonstrates practical experience with:

- microcontroller programming in C;
- UART peripheral configuration;
- serial data transmission and reception;
- hardware registers and status flags;
- timer configuration;
- hardware interrupt handling;
- interrupt service routines;
- digital I/O control;
- comparison of polling and interrupt-driven architectures;
- embedded-system validation through simulation.

---

## 👤 Autor / Author

**IonutD**
