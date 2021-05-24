#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

bancroft::XDrive mainDrive(new vex::motor(PORT1, vex::gearSetting::ratio18_1, false), new vex::motor(PORT2, vex::gearSetting::ratio18_1, false), new vex::motor(PORT11, vex::gearSetting::ratio18_1, false), new vex::motor(PORT12, vex::gearSetting::ratio18_1, false));

point towerPosition[3][3];
float towerHeadings[3][3];


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

  towerPosition[0][0].x = -40 * -25.4; towerPosition[0][0].y = 47 * -25.4;  towerHeadings[0][0] = (-(3.0*PI)/4.0);
  towerPosition[1][0].x = -32 * -25.4; towerPosition[1][0].y = 6 * -25.4;   towerHeadings[1][0] = (-(PI)/2.0);
  towerPosition[2][0].x = -44 * -25.4; towerPosition[2][0].y = -36 * -25.4; towerHeadings[2][0] = (-(PI)/4.0);

  towerPosition[0][1].x = 0 * -25.4;   towerPosition[0][1].y = 29 * -25.4;  towerHeadings[0][1] = PI;
  towerPosition[1][1].x = -22 * -25.4; towerPosition[1][1].y = 6 * -25.4;   towerHeadings[1][1] = (PI/2.0);
  towerPosition[2][1].x = 0 * -25.4;   towerPosition[2][1].y = -27 * -25.4; towerHeadings[2][1] = (0);

  towerPosition[0][2].x = 26 * -25.4; towerPosition[0][2].y = 30 * -25.4;  towerHeadings[0][2] = ((3*PI)/4.0);
  towerPosition[1][2].x = 25 * -25.4; towerPosition[1][2].y = 0 * -25.4;   towerHeadings[1][2] = (PI/2.0);
  towerPosition[2][2].x = 37 * -25.4; towerPosition[2][2].y = -35 * -25.4; towerHeadings[2][2] = (PI/4.0);
  
}