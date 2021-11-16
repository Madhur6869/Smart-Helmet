const int xpin = A3;// x-axis of the accelerometer
const int ypin = A2; // y-axis
const int zpin = A1;  // z-axis (only on 3-axis models)
float rms1;//RMS Values of accelaration
float rms2;
float x,y,z,diff;
void setup() {
  // initialize the serial communications:
  Serial.begin(9600);
  pinMode(13,OUTPUT);//LED Indication for accident
}

void loop() {
x=analogRead(xpin);//Reading from ADXL335
y=analogRead(ypin);
z=analogRead(zpin);
rms1=sqrt(x*x+y*y+z*z);//Find RMS1 Value
delay(100);
 //Read again from sensor after delay 
x=analogRead(xpin);
y=analogRead(ypin);
z=analogRead(zpin);
rms2=sqrt(x*x+y*y+z*z);
diff=abs(rms1-rms2); //Find difference in RMS values
 if(diff>=100){//Check for threshold
  digitalWrite(13, HIGH);//Send signal to NodeMCU
  delay(1000);
  digitalWrite(13, LOW); 
 } 
}
