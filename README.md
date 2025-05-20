# Dual-Microcontoller-Door-Locker-Security-System

### Project Overview 📖
A secure smart-door control system built with two ATmega32 MCUs: an HMI_ECU (user interface) and a Control_ECU (logic & actuation). Users enter passwords via keypad & LCD, which are authenticated over UART, stored in external EEPROM. The system drives a motorized lock, monitors motion with PIR, and sounds alarms via buzzer for enhanced security.

### Objective 🎯
- Implement password authentication with storage in I2C EEPROM  
- Provide HMI via LCD & keypad on HMI_ECU  
- Communicate commands & responses over UART between ECUs  
- Control door motor via H-bridge and PWM  
- Detect motion with PIR sensor to hold door open  
- Trigger buzzer alerts on failed attempts and security breaches  

### Key Features 🚀
1. **Password Protection**: Set, verify, and change 5‑digit passwords stored in EEPROM.  
2. **UART Communication**: Bidirectional data exchange between HMI_ECU and Control_ECU.  
3. **Motorized Lock Control**: H-bridge-driven motor rotates CW/CCW for door lock/unlock.  
4. **PIR Sensor**: Detects motion post-unlock to keep door ajar.  
5. **Security Lockout**: Three consecutive failed attempts → 1‑minute lockout + buzzer.  
6. **User Feedback**: LCD messages guide through steps; buzzer signals errors.  

### Hardware Connections 🛠️
- **HMI_ECU**:  
  - LCD (8‑bit): RS→PC0, E→PC1, D0–D7→PA0–PA7  
  - Keypad 4×4: Rows→PB0–PB3, Cols→PB4–PB7  
  - UART: TXD→Control_RXD, RXD←Control_TXD  

- **Control_ECU**:  
  - EEPROM (I2C): SCL→PC0, SDA→PC1  
  - Buzzer: PC7  
  - H-bridge: IN1→PD6, IN2→PD7, EN→OC0/PB3  
  - PIR Sensor: PC2  
  - Door Motor: H-bridge outputs  

### Operation Flow 🔄

#### Step 1: Create System Password  
- Prompt: **Please Enter Password** → mask input with `*`.  
- Re-enter for confirmation: **Please re-enter the same Pass**.  
- HMI_ECU sends both to Control_ECU via UART.  
- On match → store in EEPROM & proceed. On mismatch → repeat.

#### Step 2: Main Menu  
Display options: `+` Open Door, `-` Change Password.

#### Step 3: Open Door (`+`)  
- Prompt for password.  
- On correct:  
  - Rotate motor CW for 15 s → **Door is Unlocking**.  
  - Wait until PIR detects motion → **Wait for people to Enter**.  
  - Rotate motor CCW for 15 s → **Door is Locking**.  

#### Step 4: Change Password (`-`)  
- Authenticate current password.  
- On correct → repeat **Step 1**.

#### Step 5: Failed Attempts  
- After 3 consecutive wrong passwords:  
  - Buzzer ON + **Error** message for 1 min.  
  - Keypad disabled during lockout.  
  - Return to **Step 2**.

### System Requirements ⚙️
- **MCU Frequency**: 8 MHz  
- **Microcontroller**: ATmega32 (both ECUs)  
- **Architecture**: Layered drivers + application logic per ECU

### Drivers & API 📚
- **GPIO Driver (shared)**:  
  ```c
  void GPIO_init(void);
  void GPIO_setPinDirection(uint8 port, uint8 pin, uint8 dir);
  void GPIO_writePin(uint8 port, uint8 pin, uint8 value);

- **UART Driver (shared)**:  
  ```c
  typedef struct {
  UART_BitDataType data_bits;
  UART_ParityType parity;
  UART_StopBitType stop_bits;
  UART_BaudRateType baud_rate;
  } UART_ConfigType;

  void UART_init(const UART_ConfigType *config);
  void UART_sendByte(uint8 data);
  uint8 UART_receiveByte(void);

- **I2C (TWI) Driver (Conrol_ECU)**:  
  ```c
  typedef struct {
  TWI_AddressType address;
  TWI_BaudRateType bit_rate;
  } TWI_ConfigType;

  void TWI_init(const TWI_ConfigType *config);
  void TWI_writeByte(uint8 mem_addr, uint8 data);
  uint8 TWI_readByte(uint8 mem_addr);

- **LCD Driver (HMI_ECU)**:  
  ```c
  void LCD_init(void);
  void LCD_clear(void);
  void LCD_displayString(const char* str);
  void LCD_moveCursor(uint8 row, uint8 col);

- **Keypad Driver (HMI_ECU)**:  
  ```c
  uint8 Keypad_getKey(void);

- **PWM Driver (Contril_ECU)**:  
  ```c
  void PWM_Timer0_Start(uint8 duty_cycle); // F_CPU/64, non-inverting

- **Timer Driver (shared)**:  
  ```c
    typedef struct {
  uint16 initial_value;
  uint16 compare_value;
  Timer_ID_Type timer_id;
  Timer_ClockType clock;
  Timer_ModeType mode;
  } Timer_ConfigType;
  
  void Timer_init(const Timer_ConfigType *config);
  void Timer_deInit(Timer_ID_Type id);
  void Timer_setCallBack(void (*cb)(void), Timer_ID_Type id);

- **PIR Driver (Control_ECU)**:  
  ```c
  void PIR_init(void);
  uint8 PIR_getState(void);

- **DC Motor Driver (Control_ECU)**:  
  ```c
  void DcMotor_Init(void);
  void DcMotor_Rotate(DcMotor_State state, uint8 speed);
  
- **Buzzer Driver (Control_ECU)**:  
  ```c
  void Buzzer_init(void);
  void Buzzer_on(void);
  void Buzzer_off(void);

- **EEPROM Driver (Control_ECU)**:  
  ```c
  void EEPROM_init(void);
  void EEPROM_writePassword(uint8 *pass);
  void EEPROM_readPassword(uint8 *pass);

 ### Simulation on Proteus 🖥️
![image](https://github.com/user-attachments/assets/0eee2664-5c58-49b2-83c9-c1259e5995d3)
