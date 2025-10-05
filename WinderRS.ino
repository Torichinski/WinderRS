#include <EEPROM.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>


/*
 Единицы измерения физических величин:
 Длина - мм
 Площадь сечения - мм^2
 Линейная скорость - мм/с
 Угловая скорость - рад/с
 */
 
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
int launchs = 1;

// статические переменные
static int num;
static double diametr;
static double kc;
static float pi = 3.14;

Stepper myStepper(stepSpeed, motorPins[0], motorPins[1], motorPins[2], motorPins[3]);
// клавиатура

const byte ROWS = 4;
const byte COLS = 4; 
char hexaKeys[ROWS][COLS] = {
    {'1', '4', '7', '*'},
    {'2', '5', '8', '0'},
    {'3', '6', '9', '#'},
    {'A', 'B', 'C', 'D'}
};

byte rowPins[ROWS] = {27, 26, 25, 24}; 
byte colPins[COLS] = {23, 22, 43, 42}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Структура, хранящая параметры намотки
struct Elems{
      double diametr;    // диаметр паза
      int num;    // число витков
      double kc; // коэффициент отклонения формы сечения паза от идеальной окружности

      double paz_leng;  // длина паза вдоль намоточного вала     
      double wire_width_limit; // максимально допустимая ширина электромагнита с обмоткой     
      double stator_height;   // высота статора
      double stator_diametr;   // диаметр статора
      double angle_motor_speed; // угловая скорость намоточного вала 
      double wire_diametr; // толщина провода   
      int coils_quality; // количество электромагнитов на одном статоре
      double winder_radius; // радиус фильеры намоточного вала
      double extra_wire_len; // запасная длина провода для соединения
};

Elems* elem_ptr;
Elems elem;

// Условный сигнал с пьезоэлемента
void buzzer(int qu = 1){
 for(int i = 1; i <= qu; i++){
  tone(buz, freq, 20);
 }
}

class pars {  // управление и обработка входных данных
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

// сохранение параметров модели в память EEPROM
Elems save_model_eeprom(){  
  Serial.begin(115200);
  
  EEPROM.write(0, launchs);
  EEPROM.update(0, launchs);

  byte stec_num = EEPROM.read(0) ? 0 : stec_num = 1;
  EEPROM.get(stec_num, elem);
  EEPROM.put(stec_num, elem);
  }

Elems read_pars(){ }
};

// Функция получения входных данных от пользователя
Elems menu_keypad() {
    int currentParam = 0;
    String inputValue = "";

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
                            lcd.println("Enter diametr:");  
                            elem.diametr = inputValue.toFloat();
                            break;
                        case 1:
                            lcd.println("Enter num:");  
                            elem.num = inputValue.toInt();
                            break;
                        case 2:
                            lcd.println("Enter kc:");  
                            elem.kc = inputValue.toFloat();
                            break;
                        case 3:
                            lcd.println("Enter coil width:");
                            elem.paz_leng = inputValue.toFloat();
                            break;
                    }
                    inputValue = "";
                    currentParam++;
                    if (currentParam > 3) {
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
                        case 3:
                            elem.paz_leng = inputValue.toFloat();
                            break;
                    }
                    
                    lcd.clear();
                    if(elem.diametr && elem.num && elem.kc != 0){
                      buzzer(3);
                      launchs++;
                      return elem;
                    }
                    else{
                      lcd.println("No pars!!");
                      delay(5000);
                      lcd.clear();
                      currentParam = 0;
                    }

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
        buzzer();
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
    delay(5000);
    lcd.clear();
    
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

// Функция для расчета длины проволоки и времени её намотки
double math(double diametr, double num, double kc, double paz_leng) {
    double times = 0;
    smooth();
    double len = (pi * diametr * num) * kc;
    double bending_coefficient; 
    double line_speed = elem.winder_radius * elem.angle_motor_speed;
    times = (len / line_speed);
    return times;
}

// Функция для настройки и фильтрации параметров
Elems smooth(){ 
  int num = elem.num;
  double diametr = elem.diametr;
  double kc = elem.kc;
  double paz_leng = elem.paz_leng;
  
  if (elem.num > 10000 || elem.diametr > 30 || elem.kc > 2.5 || elem.paz_leng > 60){
   lcd.println("Invalid pars!");
   return;
  }
  // Переменные, заданные по умолчанию
  double wire_diametr = elem.wire_diametr = 0.2;
  double wire_width_limit = elem.wire_width_limit = 3.5;
  double winder_radius = elem.winder_radius = 9.2;
  double angle_motor_speed = elem.angle_motor_speed = 1.2;
  
}

// настройка направляющего подвеса вручную с помощью энкодера
void motor_step(float delta) {
    int val = analogRead(stepSelector);
    int steps = map(val, 0, 1023, 0, 100); // Наятройка скорости от выходного сигнала
    myStepper.step(steps);
    Serial.write(steps);
    delay(10);

    // изменение положения подвеса в процессе обмотки
    if(delta != 0){
      int dstep = 99;
      int dtime = round(delta / dstep);
      myStepper.step(99);
      delay(dtime);
      myStepper.step(steps);
    }
}

// Направляющая клешня-захват
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

void loop() {
    hang();
    motor_step(0);
    
    Elems elem = menu_keypad();

    //  запись типа намотки в массив
    pars model[10];
    
    pars mark_1(diametr, num, kc);
    mark_1.setter(diametr, num, kc);
    mark_1 = model[0];

    // Расчет времени намотки
    double times = math(elem.diametr, elem.num, elem.kc, elem.paz_leng);
    
    if(times != 0){
    // Запуск намоточного механизма
    winder(times);
    } 
    
}













