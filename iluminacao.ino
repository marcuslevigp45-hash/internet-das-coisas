const int LEDre = 14;
const int LEDaz = 12;
const int LEDve = 13;

const int LDR = 1;
int valorLDR = 0;

const int BTMODO1 = 7
const int BTCOR2 = 6

int MODO = 1;
int CORAGORA = 0;

void setup() {  
  pinMode(LEDre, OUTPUT);
  pinMode(LEDaz, OUTPUT);
  pinMode(LEDve, OUTPUT);
  Serial.begin(9600);
  pinMode(LDR, INPUT);
}
void loop() {
  valorLDR = analogRead(LDR);
  Serial.println(valorLDR);
  if (valorLDR <= 3500) {
    digitalWrite(LEDre, HIGH);
  }
  else {
    digitalWrite(LEDre, LOW);
  }
  if (valorLDR > 3500 && valorLDR < 7000) {
    digitalWrite(LEDve, HIGH);
  }
  else {
    digitalWrite(LEDve, LOW);
  }
  if (valorLDR >7000) {
    digitalWrite(LEDaz, HIGH);
  }
  else {
    digitalWrite(LEDaz, LOW);
  }
  delay(100);
}
