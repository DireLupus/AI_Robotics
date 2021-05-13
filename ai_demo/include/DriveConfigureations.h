#include "v5_vcs.h"
#include <queue>
#include <vector>
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       DriveConfigureations.h                                    */
/*    Author:       Jonathan Asher                                            */
/*    Created:      Thu Nov 19 2020                                           */
/*    Updated:      Tue Mar 02 2021                                           */
/*    Description:  Method declaration for Bancroft drive configureations     */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef DRIVE_CONFIG_H
#define DRIVE_CONFIG_H 1

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

namespace bancroft 
{
  class mWrapper 
  {
    private: 
      vex::mutex& lockable;
    public: 
      mWrapper(vex::mutex& l) : lockable(l) { lockable.lock(); }
      ~mWrapper() { lockable.unlock(); }
  };

  template <class T> class tFifo
  {
    private:
      std::queue<T> fifo;
      vex::mutex queueLock;
    public:
      bool pop(T& popable) { mWrapper tempWrapper(queueLock); if(!fifo.empty()) {  popable = fifo.front(); fifo.pop(); return true; } return false; }
      void push(T& pushable) { mWrapper tempWrapper(queueLock); fifo.push(pushable);  }
      bool isEmpty() { mWrapper tempWrapper(queueLock); return fifo.empty(); }
  };

  class Module
  {
    private:
      struct passable
      {
        Module* mod;
        float speed, revolutions;
        bool finished = false;
      };
      // Maximum RPM motors can run at
      float MAX_RPM = 200;
      bool mailboxEmpty;
      // Motors
      vex::motor_group* Motors;
      tFifo<passable*> mailbox;
      std::string name;

      static void mailboxTask(void* mail);
      static void runUntilTemp(void* arg);

    public:
      /* -Default Constructor- */
      Module();
      /* -Motor Constructor- */
      Module(vex::motor* m, std::string name);
      /* -Motor_Group Constructor- */
      Module(vex::motor_group* m_g, std::string name);

      bool canContinue();
      void setContinue(bool can) { mailboxEmpty = can; } 

      /* -Setters- */
      void setGearset(vex::gearSetting gear);
      /* -Getters- */
      vex::gearSetting getGearset();
      float getMaxRPM() { return MAX_RPM; }
      std::string getName();

      /* -Movement Methods- */
      void startAllMotors(); // if the bool is true, then it should use the currentSpeed variable as velocity
      // Stops the motors from spinning
      void stopAllMotors();
      // Sets motor velocity to run FORWARD at a set speed
      void runBackwardAt(float speed);
      // Sets motor velocity to run BACKWARD at a set speed
      void runForwardAt(float speed);

      void runUntil(float speed, float revolutions);
  };

  class XDrive
  {
    private:
      struct passable
      {
        XDrive* thisDrive;
        int velocityToRun;
        float distanceCM;
        float theta;
        bool finished;
        bool PID, strafe;
      };
      // Maximum RPM motors can run at
      float MAX_RPM = 200;
      tFifo<passable*> mailbox;
      bool mailboxEmpty;
      // Motors
      vex::motor_group* LeftFrontMotor;
      vex::motor_group* LeftBackMotor;
      vex::motor_group* RightFrontMotor;
      vex::motor_group* RightBackMotor;
      /* -Modules- */
      std::vector<bancroft::Module*> allModules;
      vex::inertial* inertialSensor;
      // Speed Buffer
      float currentSpeedLF;
      float currentSpeedLB;
      float currentSpeedRF;
      float currentSpeedRB;
      // Drive Specs
      vex::distanceUnits units;
      float externalGearing;
      float wheelRadius;
      float wheelCircumference;
      float wheelToWheel;
      // PID Fields
      float pVal;
      float iVal;
      float dVal;

      bool hasInertial = false;

      static void mailboxTask(void* mail);

      static void driveUntilTemp(void* arg);
      static void driveUntilTempPID(void* arg);

      static void strafeUntilTemp(void* arg);
      static void strafeUntilTempPID(void* arg);

      static void turnUntilTemp(void* arg);
      static void turnUntilTempPID(void* arg);

    public:
      /* -Motor Constructor- */
      XDrive(vex::motor* LF, vex::motor* LB, vex::motor* RF, vex::motor* RB);
      /* -Motor_Group Constructor- */
      XDrive(vex::motor_group* LF, vex::motor_group* LB, vex::motor_group* RF, vex::motor_group* RB);
      /* -Mutithreading- */
      void waitUntilComplete();

      /* -Modules- */
      void addModule(Module* addable);
      Module* getModule(std::string name);
      bool canContinue();
      void setContinue(bool can) { mailboxEmpty = can; } 

      /* -Setters- */
      void setGearset(vex::gearSetting gear);
      void setExternalGearset(float ratio);
      void setWheelRadius(float r);
      void setPIDValues(float p, float i, float d); // this is for later
      void setWheelToWheel(float t);
      void setInertial(vex::inertial* newInertial);
      /* -Getters- */
      vex::gearSetting getGearset();
      float getMaxRPM() { return MAX_RPM; }
      void getPIDValues(float &p, float &i, float &d);

      /* -Non Speed Buffer Clearing Methods- */
      void strafeForwardBy(float speed);
      void strafeBackwardBy(float speed);
      void strafeLeftBy(float speed);
      void strafeRightBy(float speed);
      void turnLeftBy(float speed);
      void turnRightBy(float speed);

      /* -Speed Buffer Clearing Methods- */
      // Causes the motors to spin
      void startAllMotors(bool variableMode); // if the bool is true, then it should use the currentSpeed variable as velocity
      // Stops the motors from spinning
      void stopAllMotors();
      // Sets motor velocity to run drive FORWARD at a set speed
      void strafeForwardAt(float speed);
      // Sets motor velocity to run drive BACKWARD at a set speed
      void strafeBackwardAt(float speed);
      // Sets motor velocity to run drive LEFT at a set speed
      void strafeLeftAt(float speed);
      // Sets motor velocity to run drive RIGHT at a set speed
      void strafeRightAt(float speed);
      // Sets motor velocity to turn drive LEFT at a set speed
      void turnLeftAt(float speed);
      // Sets motor velocity to turn drive RIGHT at a set speed
      void turnRightAt(float speed);

      /* -Distance Movement Methods- */
      void driveUntil(float speed, float length);
      void driveUntilPID(float speed, float length);

      void strafeUntil(float speed, float length);
      void strafeUntilPID(float speed, float length);

      void turnUntil(float speed, float theta);
      void turnUntilPID(float speed, float theta);
  };

  class HDrive
  {
    private:
      struct passable
      {
        HDrive* thisDrive;
        int velocityToRun;
        float distanceCM;
        float theta;
        bool finished;
      };
      // Max RPM
      float MAX_RPM = 200;
      tFifo<passable*> mailbox;
      bool mailboxEmpty;
      // Motors
      vex::motor_group* LeftDrive;
      vex::motor_group* RightDrive;
      /* -Modules- */
      std::vector<bancroft::Module*> allModules;
      vex::inertial* inertialSensor;
      // Speed Buffers
      float currentSpeedLD;
      float currentSpeedRD;
      // Drive Specs
      vex::distanceUnits units;
      //float motorGearing;
      float externalGearing;
      float wheelRadius;
      float wheelCircumference;
      float wheelToWheel;
      // PID fields

      bool hasInertial;

      static void mailboxTask(void* mail);

      static void driveUntilTemp(void* arg);

      static void turnUntilTemp(void* arg);

      float pVal;
      float iVal;
      float dVal;

    public:
      /* -Motor Constructor- */
      HDrive(vex::motor* LD, vex::motor* RD);
      /* -Motor_Group Constructor- */
      HDrive(vex::motor_group* LD, vex::motor_group* RD);

      /* -Mutithreading- */
      void waitUntilComplete();

      /* -Modules- */
      void addModule(Module* addable);
      Module* getModule(std::string name);
      bool canContinue();

      /* -Setters- */
      void setGearset(vex::gearSetting gear);
      void setExternalGearset(float ratio);
      void setWheelRadius(float r);
      void setWheelToWheel(float t);
      void setPIDValues(float p, float i, float d);
      void setInertial(vex::inertial* newInertial);
      /* -Getters- */
      vex::gearSetting getGearset();
      float getMaxRPM() { return MAX_RPM; }
      void getPIDValues(float &p, float &i, float &d);

      /* -Speed Buffer Clearing Methods- */
      // Sets motor velocity to run drive FORWARD at a set speed
      void driveForwardAt(float speed);
      // Sets motor velocity to run drive BACKWARD at a set speed
      void driveBackwardAt(float speed);
      // Sets motor velocity to turn drive LEFT at a set speed
      void turnLeftAt(float speed);
      // Sets motor velocity to turn drive RIGHT at a set speed
      void turnRightAt(float speed);

      /* -Non Speed Buffer Clearing Methods- */
      // Starts the motors spinning
      void startAllMotors(bool variableMode); // if the bool is true, then it should use the currentSpeed variable as velocity
      // Stops the motors from spinning
      void stopAllMotors();
      // Sets motor velocity to run drive FORWARD at a set speed
      void driveForwardBy(float speed);
      // Sets motor velocity to run drive BACKWARD at a set speed
      void driveBackwardBy(float speed);
      // Sets motor velocity to turn drive LEFT at a set speed
      void turnLeftBy(float speed);
      // Sets motor velocity to turn drive RIGHT at a set speed
      void turnRightBy(float speed);

      void rightDriveForwardAt(float speed);
      void leftDriveForwardAt(float speed);

      /* -Distance Movement Methods- */
      void driveUntil(float speed, float length);
      void driveUntilPID(float speed, float length);

      void turnUntil(float speed, float theta);
      void turnUntilPID(float speed, float theta);
  };
}

#endif