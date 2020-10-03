// led_test.pde - Control de LED tricolor por el puerto serie

#define LED_ANODO 3
#define LED_RED 2
#define LED_GREEN 4
#define LED_BLUE 5

int ledRedStatus = HIGH;
int ledGreenStatus = HIGH;
int ledBlueStatus = HIGH;

void toggle(int& state) {
    if (state == HIGH) {
        state = LOW;
    }
    else {
        state = HIGH;
    }
}

void setup()
{
    // Configurar los puertos digitales conectados al LED
    pinMode(LED_ANODO, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    
    digitalWrite(LED_ANODO, HIGH);
    digitalWrite(LED_RED, ledRedStatus);
    digitalWrite(LED_GREEN, ledGreenStatus);
    digitalWrite(LED_BLUE, ledBlueStatus);
    
    // Configurar la conexion por el puerto serie
    Serial.begin(9600);
}

void loop()
{
    if (Serial.available() > 0) {
        int command = Serial.read();
        switch(command) {
        case 'R':
            toggle(ledRedStatus);
            digitalWrite(LED_RED, ledRedStatus);
            break;
        case 'G':
            toggle(ledGreenStatus);
            digitalWrite(LED_GREEN, ledGreenStatus);
            break;
        case 'B':
            toggle(ledBlueStatus);
            digitalWrite(LED_BLUE, ledBlueStatus);
            break;
        }
    }
}
