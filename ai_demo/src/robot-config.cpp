#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

bancroft::XDrive mainDrive(new vex::motor(PORT1, vex::gearSetting::ratio18_1, false), new vex::motor(PORT2, vex::gearSetting::ratio18_1, false), new vex::motor(PORT11, vex::gearSetting::ratio18_1, false), new vex::motor(PORT12, vex::gearSetting::ratio18_1, false));


/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void) {
  mainDrive.addModule(new bancroft::Module(new vex::motor_group(*(new vex::motor(PORT13, vex::gearSetting::ratio18_1, true)), *(new vex::motor(PORT4, vex::gearSetting::ratio18_1, false)), *(new vex::motor(PORT5, vex::gearSetting::ratio18_1, true))), "Intake"));
  mainDrive.addModule(new bancroft::Module(new vex::motor(PORT7, vex::gearSetting::ratio18_1, true), "Indexer"));
  mainDrive.setGearset(vex::gearSetting::ratio18_1);
  mainDrive.setWheelRadius(3.81);
  mainDrive.setWheelToWheel(40.64);
  mainDrive.setPIDValues(3, 4, 2);
}

// My Previous Attempts:

/*
if(found && needAngle)
  {
    // -----Putting robot at 0,0----- 
    tempTracking.positionX -= local_map.pos.x;
    tempTracking.positionY -= local_map.pos.y;
    ocal_map.pos.x = 0;
    local_map.pos.y = 0;

    / -----Finding turn angle----- 
    rVal = sqrt(pow(tempTracking.positionX, 2) + pow(tempTracking.positionX, 2));
    // U = (cos(-local_map.pos.az + (2*PI)), sin(-local_map.pos.az + (2*PI)));
    // V = (tempTracking.positionX, tempTracking.positionY); 
    // (U DOT V) / rVal
    turnAngle = acos( (( cos(-local_map.pos.az + (2*PI)) * tempTracking.positionX ) +  ( sin(-local_map.pos.az + (2*PI)) * tempTracking.positionY )) / rVal);
          
    needAngle = false;
    atAngle = false;
  } else if(found && !needAngle)
  {
    if(!atAngle)
    {
      mainDrive.turnUntil(50, turnAngle);
      mainDrive.waitUntilComplete(); 
      atAngle = true;
    } else 
    {
      mainDrive.driveUntilPID(50, rVal/10.0);
      mainDrive.waitUntilComplete();
      found = false;
    }
  }
*/