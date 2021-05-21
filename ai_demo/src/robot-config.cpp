#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

bancroft::XDrive mainDrive(new vex::motor(PORT1, vex::gearSetting::ratio18_1, false), new vex::motor(PORT2, vex::gearSetting::ratio18_1, false), new vex::motor(PORT11, vex::gearSetting::ratio18_1, false), new vex::motor(PORT12, vex::gearSetting::ratio18_1, false));
int towers[9][3] = 
{
  {}, {}, {},
  {}, {}, {},
  {}, {}, {}
};

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