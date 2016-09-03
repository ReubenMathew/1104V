#pragma config(Motor,  port1,           leftTower_TL,  tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port2,           rightDrive,    tmotorVex393, openLoop)
#pragma config(Motor,  port3,           leftDrive,     tmotorVex393, openLoop)
#pragma config(Motor,  port4,           rightTower_TR, tmotorVex393, openLoop)
#pragma config(Motor,  port5,           rightTower_BL, tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port6,           rightTower_BR, tmotorVex393, openLoop)
#pragma config(Motor,  port7,           leftTower_BR,  tmotorVex393, openLoop)
#pragma config(Motor,  port8,           leftTower_BL,  tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port9,           leftTower_TR,  tmotorVex393, openLoop)
#pragma config(Motor,  port10,          rightTower_TL, tmotorVex393, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)


#pragma competitionControl(Competition)

#include "Vex_Competition_Includes.c"

int rightBack;
int leftBack;

int trueSpeed(int power) {
	int tsArray[128] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		5,10,12,13,13,14,15,15,15,15,
		20,20,20,20,20,20,20,20,20,20,
		25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
		35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
		41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
		46, 47, 47, 48, 48, 49, 50, 50, 51, 52,
		52, 53, 54, 55, 56, 57, 57, 58, 59, 60,
		61, 62, 63, 64, 65, 66, 67, 68, 72, 77,
		82, 87, 92, 97, 103, 108, 113, 118, 120, 125,
		126,127,127,127,127,127,127,127,127,127,
		127,127,127,127,127,127,127,127
	};
	return ((power>0)?1:-1)*tsArray[power*((power>0)?1:-1)];
}
bool backDriveBool;

void waitForPress()
{
	while(nLCDButtons == 0){}
	wait1Msec(5);
}

void waitForRelease()
{
	while(nLCDButtons != 0){}
	wait1Msec(5);
}




//***************


void tank(int left, int right) {
	motor[leftDrive] = left;
	motor[rightDrive] = right;

}


task usercontrol()
{
  // User control code here, inside the loop
  while (true)
  {
  //bool straightBool;
		//while(straightBool == true){
		//}
	//	backDrive();
		//while(backDriveBool == 1){
		//	straightBool = true;
		//}
  int leftLift;
	int rightLift;
motor[port1] =  motor[port7] = motor[port8] = motor[port9] = leftLift;
motor[port4] = motor[port5] = motor[port6] = motor[port10] = rightLift;
if(vexRT[Btn5U] == 1){
	rightLift = 80;
	leftLift = 80;
} else if (vexRT [Btn5u] == 0) {
rightLift = 0;
	leftLift = 0;
}
if(vexRT[Btn5D] == 1){
	rightLift = -80;
	leftLift = -80;
}else if (vexRT [Btn5u] == 0) {
rightLift = 0;
	leftLift = 0;
}

		 motor[rightDrive] = -rightBack;
		 motor[leftDrive] = leftBack;

		rightBack = trueSpeed(vexRT[Ch2]);
		leftBack = trueSpeed(vexRT[Ch3]);
  }
}
