#include <DynamixelSerial3.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"

#define sens1 HIGH //droite
#define sens2 LOW  //gauche
#define PWM1 5  //moteurs
#define PWM2 6
#define DIR1 4  
#define DIR2 7
#define Id1 1   //servos
#define Id2 2
#define Id3 3
#define Id4 4
#define vitesse_servo 100
#define a 175
#define b 400
#define trigPin A0   //ultrason
#define echoPin A2

HTU21D myHumidity;
MPU6050 mpu6050(Wire);

float duration, distance;
long timer = 0;
float  R, rayon, alpha1, alpha2, alpha3, alpha4, Alpha, Alpha1, Alpha2, Alpha3, Alpha4;
int vitesse, sens, Angle, sensPrec;
char mode;

void setup() {
  Serial.begin(9600);
  
  pinMode(PWM1,OUTPUT); 
  pinMode(PWM2,OUTPUT);
  pinMode(DIR1,OUTPUT); 
  pinMode(DIR2,OUTPUT);
  Dynamixel.begin(1000000,3);
  Dynamixel.setEndless(Id1,OFF); 
  Dynamixel.setEndless(Id2,OFF);
  Dynamixel.setEndless(Id3,OFF); 
  Dynamixel.setEndless(Id4,OFF);
  Dynamixel.move(Id1,512);
  Dynamixel.move(Id2,512);
  Dynamixel.move(Id3,512);
  Dynamixel.move(Id4,512);
  
  myHumidity.begin();
  Wire.begin();
  mpu6050.begin();
  
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  //Serial.println();
  //Serial.print("Mars Rover \n");
}

void loop() {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        //calcul de la distance
        distance = (duration / 2.0) * 0.0343;
        float humd = myHumidity.readHumidity();
        float temp = myHumidity.readTemperature();
        //distance par rapport à l'obstacle le plus proche
        mpu6050.update();
        Serial.print(distance);
        Serial.print("mm");
        Serial.print(",");
        //température
        Serial.print(temp);
        Serial.print("°C");
        Serial.print(",");
        //humidité
        Serial.print(humd);
        Serial.print("%");
        Serial.print(",");
        //Pitch,Roll,Yaw
        Serial.print(mpu6050.getAngleX());
        Serial.print(",");
        Serial.print(mpu6050.getAngleY());
        Serial.print(",");
        Serial.print(mpu6050.getAngleZ());
        Serial.print(",");
        Serial.println();
        delay(10);
        
  if(Serial.available()>0){
    
    String cc=Serial.readString(); 
    char comm[cc.length()];
    cc.toCharArray(comm,cc.length());
    sscanf(comm,"%d,%d,%d",&sens,&vitesse,&Angle);
    
    /*Serial.println(sens);
    Serial.println(angle);
    Serial.println(vitesse);*/
    ///    mode=Serial.read();
////    //c pour commander & d pour l'afichage des capteurs
//    sens=Serial.parseInt();
//    Serial.println(sens);
//    //1 pour avancer & 0 pour reculer
//    angle=Serial.parseFloat();
//    Serial.println(angle);
//    //entre -90 et 90
//    vitesse=Serial.parseInt();
//    Serial.println(vitesse);
//    //entre 0 et 255
//if(Serial.read()=='\n'){
      Alpha=Angle*PI/180.0;
      R=b/tan(Alpha);
      cmd_dynamixel(R);
      //Serial.print("yallah");
      if(sens!=sensPrec){
        cmd_moteur_droite(0);
        cmd_moteur_gauche(0); 
      }
      if(abs(R)<a && R>0){
        if(sens==1){ //en avant à droite
          cmd_moteur_droite(vitesse);
          cmd_moteur_gauche(-vitesse);
         // Serial.print("EN AVANT abs(R)<a && R>0 \n");
}
        if(sens==0){ //en arrière à droite
          cmd_moteur_droite(-vitesse);
          cmd_moteur_gauche(vitesse);
          //Serial.print("EN ARRIERE abs(R)<a && R>0 \n");
      }
      }
     if(abs(R)<a && R<0){
        //Serial.print("khdam");
        if(sens==1){ //en avant à gauche
          cmd_moteur_droite(-vitesse);
          cmd_moteur_gauche(vitesse);
          //Serial.print("EN AVANT abs(R)<a && R<0 \n");
      }
        if(sens==0){ //en arrière à gauche
          cmd_moteur_droite(vitesse);
          cmd_moteur_gauche(-vitesse);
          //Serial.print("EN ARRIERE abs(R)<a && R<0 \n");
      }
      }
     if(abs(R)>a){
      if(sens==1){ //en avant à gauche
          cmd_moteur_droite(vitesse);
          cmd_moteur_gauche(vitesse);
          //Serial.print("EN AVANT & R>a \n");
     }
     if(sens==0){ //en arrière à gauche
          cmd_moteur_droite(-vitesse);
          cmd_moteur_gauche(-vitesse);
          //Serial.print("EN ARRIERE & R>a \n");
     }
    } 
    sensPrec = sens;   
}
}  

void cmd_dynamixel(float rayon){
  //R=b/tan(alpha);
  Alpha1=(atan(b/(rayon+a)))*180.0/PI; //gauche avant
  Alpha2=(atan(b/(rayon-a)))*180.0/PI; //droite avant
  Alpha3=(-atan(b/(rayon-a)))*180.0/PI; //gauche arrière
  Alpha4=(-atan(b/(rayon+a)))*180.0/PI; //droite arrière
  alpha1=int(Alpha1*1023.0/300.0);
  alpha2=int(Alpha2*1023.0/300.0);
  alpha3=int(Alpha3*1023.0/300.0);
  alpha4=int(Alpha4*1023.0/300.0);
  Dynamixel.moveSpeed(Id1,512+alpha4,vitesse_servo);
  Dynamixel.moveSpeed(Id2,512+alpha3,vitesse_servo);
  Dynamixel.moveSpeed(Id3,512+alpha2,vitesse_servo);
  Dynamixel.moveSpeed(Id4,512+alpha1,vitesse_servo);
  }

void cmd_moteur_droite(int V){
  //1;5;3
  if(V>0){
    digitalWrite(DIR1,sens2);
    analogWrite(PWM1,V);
  }
  if(V<0){
    digitalWrite(DIR1,sens1);
    analogWrite(PWM1,-V);
  }
  if(V==0){
   // digitalWrite(DIR1,sens1);
    analogWrite(PWM1,0);
  }
  
}

void cmd_moteur_gauche(int V){
  //2;6;4
  if(V>0){
    digitalWrite(DIR2,sens1);
    analogWrite(PWM2,V);
  }
  if(V<0){
    digitalWrite(DIR2,sens2);
    analogWrite(PWM2,-V);
  }
   if(V==0){
   // digitalWrite(DIR1,sens1);
    analogWrite(PWM2,0);
  }
}
 
