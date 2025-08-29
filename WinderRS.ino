
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

// 24.08.2025
// переменные портов
LiquidCrystal lcd(28, 29, 30, 31, 32, 33);
const int motorPins[] = {4, 5, 6, 7};
Servo sev_up;
Servo sev_down;
const int buz = 35;
const int wndUp = 45; // сигнал на обмотчик
const int wndDown = 44;
// кнопки-управление
const int Onbut = 14;
const int Canselbut = 36;
const int Continbut = 37;
const int stepSelector = A10;
const int ena = A3;
const int angSelect = A11;
// цифровые переменные
bool isRun = false;
unsigned long mitime = millis();
const int stepSpeed = 200;
const int windSpeed = 255;
const int freq = 345;
int cur_angle = -1;
int launchs;

// статические параметры намотки
static int num;
static double diametr;
static double kc;

Stepper myStepper(stepSpeed, motorPins[0], motorPins[1], motorPins[2], motorPins[3]);
// клавиатура

const byte ROWS = 4; //число строк у нашей клавиатуры
const byte COLS = 4; //число столбцов у нашей клавиатуры
char hexaKeys[ROWS][COLS] = {// здесь мы располагаем названия наших клавиш, как на клавиатуре,для удобства пользования
    {'1', '4', '7', '*'},
    {'2', '5', '8', '0'},
    {'3', '6', '9', '#'},
    {'A', 'B', 'C', 'D'}
};

byte rowPins[ROWS] = {27, 26, 25, 24}; //к каким выводам подключаем управление строками
byte colPins[COLS] = {23, 22, 43, 42}; //к каким выводам подключаем управление столбцами

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

struct Elems{
      double diametr;
      int num;
      double kc;
};

Elems* elem_ptr;

class pars {  // подстройка и обработка входных данных
  public:
    double diametr;
    int num;
    double kc;  
    
    double paz_leng;
    double stator_height;
    double angle_motor_speed;

pars(double _diametr, int _num, double _kc){
  _diametr = diametr;
  _num = num;
  _kc = kc;
}

pars() : diametr(0), num(0), kc(0) {}

~pars(){ }

void setter(double diametr, int num, double kc){
    this -> diametr = diametr;
    this -> num = num;
    this -> kc = kc;
    }

double machine_cor(double diametr, int num, double kc){ }

void save_model(){  // сохранение параметров модели в память EEPROM
  }
};

Elems menu_keypad() {
    int currentParam = 0;
    String inputValue = "";
    Elems elem;

    while (true) {
        char sign = customKeypad.getKey();
        if (sign != NO_KEY) {
            switch (sign) {
                case '0' ... '9':
                    inputValue += sign;
                    break;
                case '*': // кнопка для смены параметра
                    switch (currentParam) {
                        case 0:
                            elem.diametr = inputValue.toFloat();
                            break;
                        case 1:
                            elem.num = inputValue.toInt();
                            break;
                        case 2:
                            elem.kc = inputValue.toFloat();
                            break;
                    }
                    inputValue = "";
                    currentParam++;
                    if (currentParam > 2) {
                        currentParam = 0;
                    }
                    break;
                case '#': // Начать ввод заново
                    inputValue = "";
                    currentParam = 0;
                    break;
                case 'A': // завершение ввода
                    switch (currentParam) {
                        case 0:
                            elem.diametr = inputValue.toFloat();
                            break;
                        case 1:
                            elem.num = inputValue.toInt();
                            break;
                        case 2:
                            elem.kc = inputValue.toFloat();
                            break;
                    }
                    return elem;
                case 'B':
                    currentParam--;
                    if (currentParam < 0) {
                        currentParam = 0;
                    }
                    break;
            }
        }
    
    // команды кнопок
    if (!digitalRead(Continbut)) {
        buzzer();
        lcd.clear();
        currentParam++;
      }
    if (!digitalRead(Canselbut)) {
        lcd.clear();
        currentParam--;
      }
    }
}
        

void setup() {
    // порты
    pinMode(Onbut, INPUT_PULLUP);
    pinMode(Canselbut, INPUT_PULLUP);
    pinMode(Continbut, INPUT_PULLUP);
    
    lcd.begin(16, 2);
    lcd.setCursor(1, 0);
    lcd.println("on");
    
    sev_up.attach(39);
    sev_down.attach(38);
    pinMode(ena, INPUT);
    pinMode(stepSelector, INPUT);
    pinMode(angSelect, INPUT);
    pinMode(wndUp, OUTPUT);
    pinMode(wndDown, OUTPUT);
    myStepper.setSpeed(200);
    // модули
    Serial.begin(9600);
}

//функция для расчета времени намотки
double math(double diametr, double num, double kc) { // leng
    double times = 0;
    double pi = 3.14;
    double len = pi * diametr * num;
    double line_speed;
    times = (len / 5) * kc;
    return times;
}

void buzzer(){
  tone(buz, freq, 20);  
}

double smooth(class pars){ // указатель на параметры объекта структуры
  int num = num;
}

void motor_step() {
    int val = analogRead(stepSelector);
    int steps = map(val, 0, 1023, 0, 100); // Наятройка скорости от выходного сигнала
    myStepper.step(steps);
    Serial.write(steps);
    delay(10);
}

void hang() {
    int val = analogRead(angSelect);
    int angle = int(val / 1024.0 * 270.0);
    angle = constrain(angle, 0, 180);

      if(cur_angle != angle){
        buzzer();
        cur_angle = angle;
      }

    sev_up.write(angle);
    sev_down.write(angle);
}

void winder(double times) {
    analogWrite(ena, windSpeed);
      if (times != 0){
    digitalWrite(wndUp, INPUT);
    delay(times);
    digitalWrite(ena, LOW);
    digitalWrite(wndUp, OUTPUT);
    lcd.clear();
    lcd.print("Wind fin");
    launchs++;
      }
}


// подвижный держатель заготовки
void holder(){ 
}

void loop() {
    hang();
    motor_step();
    menu_keypad();
    
    Elems elem = menu_keypad();

    //  запись типа намотки в память
    pars model[10];
    
    pars mark_1(diametr, num, kc);
    mark_1.setter(diametr, num, kc);
    mark_1 = model[0];

    // Расчет времени намотки
    double times = math(elem.diametr, elem.num, elem.kc);
    
    if(times != 0){
    // Запуск намоточного механизма
    winder(times);
    } 
    
}
