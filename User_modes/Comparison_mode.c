#include"stdio.h"

//REFERENCE_MODE

//STATICTIC_MODE

#define REFERENCE_MODE
//#define STATISTIC_MODE


char *msg;
	
#ifdef REFERENCE_MODE
int Data_Analysis(int sensor1,int sensor2,int sensor3)
{

	if((sensor1 == sensor3) && (sensor2==sensor3))
	{
		printf("All sensor data is correct\n");
		msg = "All sensor data is correct";
		return sensor3;
	}
	else if((sensor1 != sensor3) && (sensor2 == sensor3))
	{
		printf("Sensor 1 data is wrong\n");
		msg = "Sensor 1 data is wrong";
		return sensor3;
	}
	else if((sensor1 == sensor3) && (sensor2 != sensor3))
	{
		printf("Sensor 2 data is wrong\n");
		msg = "Sensor 2 data is wrong";
		return sensor3;
	}
  else if((sensor1 == sensor2) && (sensor1 != sensor3))
	{
		printf("Sensor 3 data is wrong\n");
		msg = "Sensor 3 data is wrong";
		return sensor3;
	}
	else
	{
		printf("All 3 sensors have different values\n");
		msg = "All 3 sensors have different values, Considering reference sensor";
		return sensor3;
	}

}
#endif


#ifdef STATISTIC_MODE
int Data_Analysis(int sensor1,int sensor2,int sensor3)
{

	if(((sensor1 == sensor2) && (sensor1 == sensor3))||((sensor1 == sensor2) && (sensor2 == sensor3)))
	{
		printf("Final value will be average of all 3 sensors\n");
		msg = "Final value will be average of all 3 sensors";
		int avg_temp = (sensor1 + sensor2 + sensor3)/3;
		return avg_temp;
	}
	else if((sensor1 == sensor2+1 || sensor1 == sensor2-1) && (sensor1 == sensor3+2 || sensor1 == sensor3-2) && (sensor2 == sensor3+1 || sensor3 == sensor3-1))
	{
		printf("All sensor values are in range\n");
		msg = "All sensor values are in range";
		int avg_temp = (sensor1 + sensor2 + sensor3)/3;
		return avg_temp;
	}
	else if((sensor1 == sensor2+1 || sensor1 ==  sensor2-1) && (sensor1 != sensor3+2 || sensor1 != sensor3-2) && (sensor2 != sensor3+1 || sensor3 != sensor3-1))
	{
		printf("Sensor 3 is not in range\n");
		msg = "Sensor 3 is not in range";
		int avg_temp = (sensor1 + sensor2)/2;
		return avg_temp;
	}
	else if((sensor1 != sensor2+1 || sensor1 != sensor2-1) && (sensor1 == sensor3+1 || sensor1 == sensor3-1) && (sensor2 != sensor3+1 || sensor3 != sensor3-1))
	{
		printf("Sensor 2 is not in range\n");
		msg = "Sensor 2 is not in range";
		int avg_temp = (sensor1 + sensor3)/2;
		return avg_temp;
	}
	else if((sensor1 != sensor2+1 || sensor1 != sensor2-1) && (sensor1 != sensor3+1 || sensor1 != sensor3-1) && (sensor2 == sensor3+1 || sensor3 == sensor3-1))
	{
		printf("Sensor 1 is not in range\n");
		msg = "Sensor 1 is not in range";
		int avg_temp = (sensor3 + sensor2)/2;
		return avg_temp;
	}
	else
	{
		printf("All are Faulty Values");
		msg = "All are Faulty Values";
		return 0;
	}

}
#endif

int main()
{

Data_Analysis(24,23,21);

return 0;
}
