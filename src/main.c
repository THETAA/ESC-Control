#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>

// Motor Settings
int Neutral = 1350;
int Full_Reverse = 950;
int Full_Throttle = 1750;

// Timings
int PulseLenght = 2500;

// Motor Values
int Motor0_Amount = 0;	// 0% Speed
int Motor1_Amount = 0;	// 0% Speed
int Motor2_Amount = 0;	// 0% Speed
int Motor3_Amount = 0;	// 0% Speed

void setup() {
	printf ("[!] Setting up Motors\n");
	pinMode(M1, OUTPUT);
	pinMode(M2, OUTPUT);
	pinMode(M3, OUTPUT);
	pinMode(M4, OUTPUT);
}

void MotorRun(void * MotorValue) {
	int motor = (int) MotorValue;
	int amount;

	printf ("[!] Motor %d Thread is running\n", motor);
	
	for (;;) {
		switch (motor) {
			case 0:
				amount = Motor0_Amount;
				break;
			case 1:
				amount = Motor1_Amount;
				break;
			case 2:
				amount = Motor2_Amount;
				break;
			case 3:
				amount = Motor3_Amount;
				break;
		}
		
		if (amount > 0) {			// Throttle
			int calc_delay = Neutral + ((Full_Throttle - Neutral) / 100 * amount);
			digitalWrite (motor, 1);
			usleep(calc_delay);
			digitalWrite (motor, 0);
			usleep(PulseLenght - calc_delay);
		} else if (amount < 0) {	// Reverse
			int calc_delay = Neutral - ((Neutral - Full_Reverse) / 100 * amount);
			digitalWrite (motor, 1);
			usleep(calc_delay);
			digitalWrite (motor, 0);
			usleep(PulseLenght - calc_delay);
		} else if (amount == 0){	// Neutral
			digitalWrite (motor, 1);
			usleep(Neutral);
			digitalWrite (motor, 0);
			usleep(PulseLenght - Neutral);
		}
	}
}

void MotorControl(int M1, int M2, int M3, int M4) {
	Motor0_Amount = M1;
	Motor1_Amount = M2;
	Motor2_Amount = M3;
	Motor3_Amount = M4;
}

void ESCcontrol() {
	MotorControl (15,15,15,15);	// Set every Motor to 15%
}

int main (void)
{
	if (wiringPiSetup () == -1)
		return 1 ;

	setup();

	pthread_t tMotor0;
	pthread_t tMotor1;
	pthread_t tMotor2;
	pthread_t tMotor3;

	pthread_create(&tMotor0, NULL, &MotorRun, (void *) 0);
	pthread_create(&tMotor1, NULL, &MotorRun, (void *) 1);
	pthread_create(&tMotor2, NULL, &MotorRun, (void *) 2);
	pthread_create(&tMotor3, NULL, &MotorRun, (void *) 3);

	pthread_join(&tMotor0, NULL);
	pthread_join(&tMotor1, NULL);
	pthread_join(&tMotor2, NULL);
	pthread_join(&tMotor3, NULL);

	ESCcontrol();
	
 	return 0 ;
}