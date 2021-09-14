//variables setup
int pin_red = 11;
int pin_green = 10;
int pin_blue = 9;
int val_red = 0, val_green = 0, val_blue = 0;
int X_axis = 0, Y_axis = 0;
bool timer1_flag = LOW;
volatile byte state = HIGH;
int interruptPin = 2;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600); // Sets the data buad rate for serial data transmission
  
	// TIMER 1 for interrupt frequency 4 Hz:
	cli(); // stop interrupts
	TCCR1A = 0; // set entire TCCR1A register to 0
	TCCR1B = 0; // same for TCCR1B
	TCNT1  = 0; // initialize counter value to 0
	// set compare match register for 4 Hz increments
	OCR1A = 62499; // = 16000000 / (64 * 4) - 1 (must be <65536)
	// turn on CTC mode
	TCCR1B |= (1 << WGM12);
	// Set CS12, CS11 and CS10 bits for 64 prescaler
	TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	sei(); // allow interrupts

	//setup Hardware interrupt
	pinMode(interruptPin, INPUT_PULLUP); 
	attachInterrupt(digitalPinToInterrupt(interruptPin), sw, RISING);
	
    //pin modes
	pinMode(pin_red,OUTPUT);
	pinMode(pin_green,OUTPUT);
	pinMode(pin_blue,OUTPUT);  
}  

void loop() {
    
	if(timer1_flag == HIGH) //read potentiometers value when timer1 overflow
    {
       X_axis = analogRead(A0);
  	   Y_axis = analogRead(A1);
       timer1_flag = LOW;
    }
	
    if(state == HIGH)// mode A
    {
	   val_red = Y_axis;
	   val_green = X_axis;
	   digitalWrite(pin_green,val_green);
	   digitalWrite(pin_red  ,val_red);
    }
	else // mode B 
	{
	   val_blue   = X_axis;
	   val_green = X_axis;
	   val_red   = (Y_axis + X_axis )/ 2; // red led intensity in mode B determent by (X+Y)/2
	   digitalWrite(pin_red   ,val_red); 	
	   digitalWrite(pin_green ,val_green);
	   digitalWrite(pin_blue  ,val_blue);         
	}
	
	//print led values to the serial monitor
	Serial.print("green = ");
	Serial.print(val_green); 
	Serial.print(", Red = ");
	Serial.print(val_red); 
	Serial.print(", blue = ");
	Serial.print(val_blue);
	Serial.print('\n');
}

ISR(TIMER1_COMPA_vect){
    //interrupt commands for TIMER 1 here
    timer1_flag = HIGH;
}

void sw()//Hardware ISR
{
    state =! state;
}