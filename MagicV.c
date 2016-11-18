#pragma config(Sensor, in1,    Claw_Pot,       sensorPotentiometer)
#pragma config(Sensor, in2,    Lift_Pot,       sensorPotentiometer)
#pragma config(Sensor, in3,    Gyro,           sensorGyro)
#pragma config(Sensor, in4,    AutoPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  Lift_Switch,    sensorTouch)
#pragma config(Sensor, dgtl2,  Drive_RightEncoder, sensorQuadEncoder)
#pragma config(Motor,  port1,           LDrive_Back,   tmotorVex393HighSpeed, openLoop)
#pragma config(Motor,  port2,           RTower_Inside, tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port3,           RTower_Outside, tmotorVex393, openLoop)
#pragma config(Motor,  port4,           RDrive_Front,  tmotorVex393HighSpeed, openLoop, reversed)
#pragma config(Motor,  port5,           Intake_Right,  tmotorVex393, openLoop)
#pragma config(Motor,  port6,           Intake_Left,   tmotorVex393, openLoop)
#pragma config(Motor,  port7,           LDrive_Front,  tmotorVex393HighSpeed, openLoop)
#pragma config(Motor,  port8,           LTower_Outside, tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port9,           LTower_Inside, tmotorVex393, openLoop)
#pragma config(Motor,  port10,          RDrive_Back,   tmotorVex393HighSpeed, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!

task liftcontrol();
volatile int Lift_Position;
int Lift_PositionCount = 1;
volatile int Lift_ControlActive = false;
volatile int Lift_Power;
#define Lift_UpperLimit 250 //(190)
#define Lift_Pos1 3000 //(2720)
#define Lift_Pos2 2300
#define Lift_Pos3 1600
#define Lift_PosRelease 1200
volatile bool Break;

task clawcontrol(); // Close + Open -
volatile int ClawActive = true;
volatile int Claw_Position;
volatile int Claw_Power;
#define Claw_Open 300 //(150)
#define Claw_Closed 2400 //(2310)
#define Claw_ClosedCube 2100
#define Claw_Mid 1300
#define Mid 0
#define Closed 1
#define Open 2
volatile int ClawPos = -1;

//Driver Control
const unsigned int TrueSpeed[128] =
{
	25,  25,  25,  25,  25,  25,  25,  25,  25,  25,
	25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	25, 25, 26, 26, 26, 27, 28, 28, 28, 29,
	29, 30, 30, 30, 30, 31, 31, 32, 32, 32,
	33, 33, 34, 34, 35, 35, 35, 36, 36, 37,
	37, 37, 37, 38, 38, 39, 39, 39, 40, 40,
	41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
	46, 47, 47, 48, 48, 49, 50, 50, 51, 52,
	52, 53, 54, 55, 56, 57, 57, 58, 59, 60,
	61, 62, 63, 64, 65, 66, 67, 67, 68, 70,
	71, 72, 72, 73, 74, 76, 77, 78, 79, 79,
	80, 81, 83, 84, 84, 86, 86, 87, 87, 88,
	88, 89, 89, 90, 90, 127, 127, 127
};
void SetDrive(int LeftDrivePower, int RightDrivePower){
	LeftDrivePower = LeftDrivePower > 127 ? 127 : LeftDrivePower;
	LeftDrivePower = LeftDrivePower < -127 ? -127 : LeftDrivePower;
	RightDrivePower = RightDrivePower > 127 ? 127 : RightDrivePower;
	RightDrivePower = RightDrivePower < -127 ? -127 : RightDrivePower;
	if(vexRT(Btn7L) == true){
		if(RightDrivePower > 0)
			RightDrivePower = TrueSpeed[RightDrivePower];
		else if(RightDrivePower < 0)
			RightDrivePower = -TrueSpeed[-RightDrivePower];
		else
			RightDrivePower = 0;

		LeftDrivePower = RightDrivePower;
	}
	else{
		if(LeftDrivePower > 0)
			LeftDrivePower = TrueSpeed[LeftDrivePower];
		else if(LeftDrivePower < 0)
			LeftDrivePower = -TrueSpeed[-LeftDrivePower];
		else
			LeftDrivePower = 0;

		// right
		if(RightDrivePower > 0)
			RightDrivePower = TrueSpeed[RightDrivePower];
		else if(RightDrivePower < 0)
			RightDrivePower = -TrueSpeed[-RightDrivePower];
		else
			RightDrivePower = 0;
	}

	motor[RDrive_Front] = RightDrivePower;
	motor[RDrive_Back] = RightDrivePower;
	motor[LDrive_Front] = LeftDrivePower;
	motor[LDrive_Back] = LeftDrivePower;
}


task DriveControl();
volatile bool BreakLoop;
volatile int DriveMode;
volatile float DesiredDriveValue;
volatile bool FirstLockCheck = false;
volatile bool UnderLoad;
volatile bool DriveActive;
volatile float LinekP = .1;
#define Line 1
#define Rotation 2
#define Off 3
#define F 1
#define B -1
#define R -1
#define L 1
int LeftTurnConst;
int RightTurnConst;
float NoLoadLeftTurnConst = 7.8;
float NoLoadRightTurnConst = 7.8;
float TurnConst;
void SetDriveControl(int Mode, int Value, int Time){
	DriveMode = Mode;
	SensorValue(Drive_RightEncoder) = 0;
	SensorValue(Gyro) = 0;
	TurnConst = (sgn(Value) == -1) ? NoLoadRightTurnConst : NoLoadLeftTurnConst;
	DesiredDriveValue = (Mode == Line) ? (Value*1800)/10.205 : (Value * TurnConst);
	clearTimer(T1);
	BreakLoop = false;
	DriveActive = true;
	while(BreakLoop == false && time1[T1] < (Time*1000)){
		wait1Msec(20);
	}
	FirstLockCheck = false;
	BreakLoop = false;
	DriveActive = false;
}

void SetLiftMotors(int Power){
	motor[LTower_Inside] = Power;
	motor[LTower_Outside] = Power;
	motor[RTower_Inside] = Power;
	motor[RTower_Outside] = Power;
}

void SetLiftPosition(int Position){
	Lift_Position = Position;
}

void Dump(int LiftPos){
	DriveActive = false;
	SetDrive(-120, -120);
	Lift_ControlActive = false;
	Lift_Power = 127;
	clearTimer(T2);
	while(Break == false && time1[T2] < 1000){
		if(SensorValue(Lift_Pot) < Lift_PosRelease){
			ClawPos = Open;
			Claw_Position = Open;
			wait1Msec(500);
			Break = true;
		}
		wait1Msec(20);
	}
	Break = false;
	Lift_ControlActive = true;
	SetLiftPosition(LiftPos);
	wait1Msec(500);
	SetDrive(0, 0);
	wait1Msec(250);
	DriveActive = true;
}


string sLStandard = "Standard Left";
string sLElims_Front = "Elims Front Left";
string sLElims_Back = "Elims Back Left";
string sProgrammingSkills = "Skills";
string sNone = "None";
string sRElims_Back = "Elims Back Right";
string sRElims_Front = "Elims Front Right";
string sRStandard = "Standard Right";
string SelectedAuton;

void DisplayAuto(){
	if(SensorValue(AutoPot) < 500){
		SelectedAuton = sLStandard;
	}
	else if(SensorValue(AutoPot) < 1000){
		SelectedAuton = sLElims_Front;
	}
	else if(SensorValue(AutoPot) < 1500){
		SelectedAuton = sLElims_Back;
	}
	else if(SensorValue(AutoPot) < 2000){
		SelectedAuton = sProgrammingSkills;
	}
	else if(SensorValue(AutoPot) < 2500){
		SelectedAuton = sNone;
	}
	else if(SensorValue(AutoPot) < 3000){
		SelectedAuton = sRElims_Back;
	}
	else if(SensorValue(AutoPot) < 3500){
		SelectedAuton = sRElims_Front;
	}
	else{
		SelectedAuton = sRStandard;
	}
	bLCDBacklight = true;
	clearLCDLine(0); clearLCDLine(1);
	displayLCDString(0, 0, SelectedAuton);
}
/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton(){
	SensorType(in3) = sensorNone;
	wait1Msec(2000);
	SensorType(in3) = sensorGyro;
	wait1Msec(1000);
	bStopTasksBetweenModes = true;
	SensorValue(Drive_RightEncoder) = 0;
	DisplayAuto();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task autonomous(){
	startTask(liftcontrol); startTask(clawcontrol); startTask(DriveControl);
	Auto();
	DisplayAuto();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////
task usercontrol(){
	startTask(liftcontrol); startTask(clawcontrol);
	bool Lift_Toggle1 = false; bool Lift_Toggle2 = false;
	bool Claw_Toggle = false; ClawPos = -1;
	int LeftDrive; int RightDrive;
	while (true){
		DisplayAuto();
		if(vexRT(Btn8R) == true){
			LeftDrive = 60;
			RightDrive = -60;
		}
		else{
			LeftDrive = abs(vexRT(Ch3)) > 20 ? vexRT(Ch3) : 0;
			RightDrive = abs(vexRT(Ch2)) > 20 ? vexRT(Ch2) : 0;
		}
		SetDrive(LeftDrive, RightDrive);
		if(vexRT(Btn6U) == 1){
			Lift_ControlActive = false;
			Lift_PositionCount = 1;
			Lift_Power = 127;
			if(SensorValue(Lift_Pot) < Lift_PosRelease){
				ClawPos = Open;
			}
		}
		else if (vexRT(Btn7D) == 1){
			Lift_ControlActive = false;
			Lift_PositionCount = 1;
			Lift_Power = -127;
		}
		else{
			Lift_Power = 0;
		}

		if(vexRT(Btn8U) == 1){
			ClawPos = Mid;
		}
		if(vexRT(Btn6D) == 1){
			if(Claw_Toggle == false){
				ClawPos++;
				if(ClawPos > 2){
					ClawPos = 1;
				}
				Claw_Toggle = true;
			}
		}
		else{
			Claw_Toggle = false;
		}

		if(vexRT(Btn5D) == 1){
			if(Lift_Toggle1 == false){
				Lift_PositionCount--;
				if(Lift_PositionCount < 1){
					Lift_PositionCount = 1;
				}
				Lift_ControlActive = true;
			}
			Lift_Toggle1 = true;
		}
		else{
			Lift_Toggle1 = false;
		}

		if(vexRT(Btn5U) == 1){
			if(Lift_Toggle2 == false){
				Lift_PositionCount++;
				if(Lift_PositionCount > 3){
					Lift_PositionCount = 3;
				}
				Lift_ControlActive = true;
			}
			Lift_Toggle2 = true;
		}
		else{
			Lift_Toggle2 = false;
		}

		if(ClawPos == Closed){
			Claw_Position = Claw_Closed;
		}
		else if (ClawPos == Mid){
			Claw_Position = Claw_Mid;
		}
		else if (ClawPos == Open){
			Claw_Position = Claw_Open;
		}

		switch(Lift_PositionCount){
		case 1:
			SetLiftPosition(Lift_Pos1);
			break;

		case 2:
			SetLiftPosition(Lift_Pos2);
			break;

		case 3:
			SetLiftPosition(Lift_Pos3);
			break;
		}
		if(vexRT(Btn8L) == 1){
			startTask(DriveControl);
			Auto();
			stopTask(DriveControl);
		}
	}
}
task clawcontrol(){
	int Claw_Current; float Claw_kP = .25;
	while(true){
		if(ClawActive){
			//Update Values
			Claw_Current = SensorValue(Claw_Pot); //Current Error

			if(ClawPos == Closed){
				Claw_Power = ((Claw_Position - Claw_Current) * Claw_kP);
				Claw_Power = Claw_Power < 0 ? 0 : Claw_Power;
			}
			else if(ClawPos == Open){
				Claw_Power = ((Claw_Position - Claw_Current) * Claw_kP * 1.25);
				Claw_Power = Claw_Power > 0 ? 0 : Claw_Power;
			}
			else if(ClawPos == Mid){
				Claw_Power = ((Claw_Position - Claw_Current) * Claw_kP * 1.25);
			}

			//System Limits
			if(SensorValue(Claw_Pot) > Claw_Closed){
				Claw_Power = Claw_Power > 0 ? 0 : Claw_Power;
			}

			if(SensorValue(Claw_Pot) < Claw_Open){
				Claw_Power = Claw_Power < 0 ? 0 : Claw_Power;
			}
			motor[Intake_Left] = Claw_Power;
			motor[Intake_Right] = Claw_Power;
			wait1Msec(20);
		}
	}
}

task liftcontrol(){
	int Lift_Current; float DkP; int Lift_Hold = 10;
	while (true){
		if(Lift_ControlActive){
			Lift_Current = SensorValue(Lift_Pot);
			DkP = ((Lift_Position - Lift_Current) > 0) ? .05 : .2;
			Lift_Power = (-(Lift_Position - Lift_Current) * DkP + Lift_Hold);
			if(Lift_Position == Lift_Pos1){
				if(Lift_Power > 30){
					Lift_Power = 30;
				}
			}
		}

		if(SensorValue(Lift_Switch) == true && SensorValue(Lift_Pot) > 1500){
			if(Lift_Power < 0){
				Lift_Power = 0;
			}
			else{
				Lift_Power = Lift_Power;
			}
		}

		if(SensorValue(Lift_Pot) < Lift_UpperLimit){
			if(Lift_Power > 0){
				Lift_Power = 0;
			}
			else{
				Lift_Power = Lift_Power;
			}
		}
		SetLiftMotors(Lift_Power);
		wait1Msec(20);
	}
}

task DriveControl(){

	int DrivePower; int DriveDirection; float CurrentDriveValue; float DkP; float DkI;
	int LockingThreshold = 15;
	int BrakingPower; int BrakingTime; short BatteryLvl;
	int P; int I; int IVal = 0; int I_Limit = 20;
	while(true){
		if(DriveActive){
			CurrentDriveValue = DriveMode == Line ? SensorValue(Drive_RightEncoder) : SensorValue(Gyro);
			DriveDirection = sgn(DrivePower);
			DkP = DriveMode == Line ? LinekP : .2;
			if(DriveMode == Rotation){
				DkP = DesiredDriveValue > (75 * TurnConst) ? .2 : .25;
			}
			DkI = DriveMode == Line ? 0 : .0;
			BrakingPower = DriveMode == Line ? 20 : 80;

			//if(DriveMode == Line){
			BrakingTime = 80;
			/*}
			else if(DriveMode == Rotation){
			BrakingTime = abs(DesiredDriveValue) > 720 ? 150 : 100;
			}*/
			P = (DesiredDriveValue - CurrentDriveValue);
			IVal = IVal + P;
			I = IVal * DkI;
			I = I < I_Limit ? I : I_Limit;
			I = I > -I_Limit ? I : -I_Limit;
			if(abs(P) > LockingThreshold){
				DrivePower = P * DkP + I;
			}
			if(abs(P) < LockingThreshold){
				DrivePower = -DriveDirection*BrakingPower;
				IVal = 0;
				FirstLockCheck = true;
			}
			BatteryLvl = nImmediateBatteryLevel/1000;
			DrivePower = (DrivePower * pow(8.4/BatteryLvl, 2));
			switch(DriveMode){
			case Line: SetDrive(DrivePower, DrivePower);	break;
			case Rotation: SetDrive(-DrivePower, DrivePower); break;
			case Off: SetDrive(0,0); break;
			}
			if(FirstLockCheck == true){
				wait1Msec(BrakingTime);
				SetDrive(0,0);
				wait1Msec(50);
				if(abs(P) < LockingThreshold){
					BreakLoop = true;
				}
				else{
					FirstLockCheck = false;
				}
			}
			wait1Msec(5);
		}
		else{
			wait1Msec(5);
		}
	}
}