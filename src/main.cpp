#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <Arduino.h>

#define ENA 8
#define ENB 9
#define IN1 30
#define IN2 32
#define IN3 34
#define IN4 36
#define A1 18
#define B1 19
#define A2 20
#define B2 21

float v; //linear.x (m/s) subcribe
float omega; //angular.z(rad/s) subcribe
float vBack; //linear.x (m/s) publish
float omegaBack;//angular.z(rad/s) publish
int pret = 0; // Temp of time 
int cycle = 100; // cycle to read encoder & calculate PID (ms)
float vr_set, vl_set; // Speed left & right setting (m/s)
float vr_mea, vl_mea; // Speed left & right measuring (m/s)
int duty_left, duty_right; // Duty of PWM pulse. Range from -100 to 100 (%)
float Kp, Ki, Kd; // PID parameter
float P, I, D; // Value of Proportional Integral Differential
float L = 0.235; // distance between 2 wheel (m)
float r_wheel = 0.05; // radian of wheel (m)

void velReceived(const geometry_msgs::Twist &msgIn){
    v = msgIn.linear.x;
    omega = msgIn.angular.z;
}

geometry_msgs::Twist velBack;

ros::NodeHandle nh;
ros::Subscriber<geometry_msgs::Twist> subvel("/cmd_vel",&velReceived);
ros::Publisher pubvel("/velocity_publisher",&velBack);

void setup()
{
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    
    nh.initNode();
    nh.subscribe(subvel);
    nh.advertise(pubvel);
}
int calculate_vright(float v, float omega)
{
    return v + L*omega/2;
}
int calculate_vleft(float v, float omega)
{
    return v - L*omega/2;
}
/*
* Return duty
* Use both left & right motor
*/
int PID(float v_set, float v_mea)
{

}
void hash_PWM(int duty_left, int duty_right)
{
    while ((millis() - pret) < cycle)
    {
        if(duty_left >= 0)
        {
            analogWrite(ENA, duty_left);
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
        }
        if(duty_left < 0)
        {
            analogWrite(ENA, -duty_left);
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
        }
        if(duty_right >= 0)
        {
            analogWrite(ENB, duty_right);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
        }
        if(duty_right < 0)
        {
            analogWrite(ENB, -duty_right);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
        }
    }
}
void loop()
{
    velBack.linear.x = vBack;
    velBack.angular.z = omegaBack;
    pubvel.publish(&velBack);
    // nothing
    nh.spinOnce();

    vr_set = calculate_vright(v, omega);
    vl_set = calculate_vleft(v, omega);

    duty_right = PID(vr_set, vr_mea);
    duty_left = PID(vl_set, vl_mea);
    hash_PWM(duty_left, duty_right);

}