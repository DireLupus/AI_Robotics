#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

bancroft::XDrive mainDrive(new vex::motor(PORT1, vex::gearSetting::ratio18_1, false), new vex::motor(PORT2, vex::gearSetting::ratio18_1, false), new vex::motor(PORT11, vex::gearSetting::ratio18_1, false), new vex::motor(PORT12, vex::gearSetting::ratio18_1, false));

point towerPosition[3][3];
int towerHeadings[3][3];


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

  towerPosition[0][0].x = -41 * -25.4; towerPosition[0][0].y = 44 * -25.4;
  towerPosition[1][0].x = -37 * -25.4; towerPosition[1][0].y = 0 * -25.4;
  towerPosition[2][0].x = -41 * -25.4; towerPosition[2][0].y = -48 * -25.4;

  towerPosition[0][1].x = 0 * -25.4; towerPosition[0][1].y = 38 * -25.4;
  towerPosition[1][1].x = -30 * -25.4; towerPosition[1][1].y = 0 * -25.4;
  towerPosition[2][1].x = 0 * -25.4; towerPosition[2][1].y = -37 * -25.4;

  towerPosition[0][2].x = 40 * -25.4; towerPosition[0][2].y = 50 * -25.4;
  towerPosition[1][2].x = 38 * -25.4; towerPosition[1][2].y = 0 * -25.4;
  towerPosition[2][2].x = 43 * -25.4; towerPosition[2][2].y = -42 * -25.4;
  
}