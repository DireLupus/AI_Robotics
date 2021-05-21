#include "DriveConfigureations.h"
using namespace vex;

extern brain Brain;

extern bancroft::XDrive mainDrive;

struct point
{
  float x, y;
};

extern int towers[9][3];
extern point towerPosition[9];

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);
