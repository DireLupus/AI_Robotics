/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*    Author:       Chris Davis                                               */
/*    Created:      Tue 18 May 2021                                           */
/*                                                                            */
/*    Description:  Miscellaneous Functions                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/




#include "vex.h"

#include <math.h>

using namespace vex;

auto findSetPoint(double goal, double c) ->   double;
auto turnPIDAbs(int setpoint)            ->   void;  //absolute degrees
auto turnPIDRel(int amount)              ->   void;  //relative degrees
auto getHeading()                        ->   double;//gets current heading
auto findGoal(int amount)                ->   double;//finds absolute coordinates

auto getx()                              ->   double;//finds xpos of bot
auto gety()                              ->   double;//finds ypos of bot
auto findDist(double fx, double fy)      ->   double;//finds dist between points
auto findAngle(double fx, double fy)     ->   double;//finds angle to go in

auto tSpeed(double pow)                  ->   void;  //spins in a turn at x speed



double findSetpoint(double goal, double c)//finds the degrees to spin
{
  double dist = goal-c;
  double realDist = dist;
  if (dist > 180)
{
	realDist = (360-dist)*-1;
}else if (dist<-180)
{
	realDist = (360 - fabs(dist));
}
  return realDist;
}

double findGoal(int amount)
{
  return (int)(amount + getHeading()) % 360;
}

void turnPIDAbs(int setpoint)//spins motors until on correct heading±error
{
  int count  = 0;
  double error, integral, derivative, kP, kI, kD, prevError, pow, range, safe;
  integral = 0;
  prevError = 0;
  safe = 0.02;//safe zone for endpoint
  range = 40;//degrees; range of integral safezone

  kP = 0;
  kI = 0;
  kD = 0;

  while(getHeading()!=setpoint)
  {
    error = findSetpoint(setpoint,getHeading());
    integral += error;
    if(error == 0 || (error < 0.5 && error > - 0.5))
    {
      integral  = 0;
    }
    if(error > range)
    {
      integral = 0;
    }
    if(integral>2)
    {
      integral=0;
    }
    derivative = error - prevError;
    if(prevError<=error+safe && prevError>=error-safe)
    {
      count++;
    }
    if(count>10)//if it starts looping back and forth over endpoint infinitely
    {
      break;
    }
    prevError = error;
    pow = error*kP +integral*kI + derivative*kD;
    tSpeed(pow);
    task::sleep(15);//∆T
  }
  tSpeed(0);
}



void tSpeed (double pow)
{
  Left1.spin(forward,v,voltageUnits::volt);
  Left2.spin(forward,v,voltageUnits::volt);
  Right1.spin(forward,v,voltageUnits::volt);
  Right2.spin(forward,v,voltageUnits::volt);
}





double findDist(double fx, double fy)
{
  int ix = getx();
  int iy = gety();

  return sqrt(pow((fx-ix),2) + pow((fy-iy),2));
}

double findAngle(double fx, double fy)
{
  int dx = fx - getx();
  int dy = fy - gety();

  if(dx != 0 && dy == 0)
  {
    if(dx>0)
    {
      return 90;
    }else if (dx<0)
    {
      return 270;
    }
  }else if(dx==0 &&  dy != 0)
  {
    return 180;
  }
  if(fy > 0)
  {
    return (int)(atan((dx)/(dy))) % 360;
  }else if(fy < 0){
    return (int)(atan((dx)/(dy)))+180;
  }

  return 0;
}
