#include "../include/DriveConfigureations.h" 
#include <iostream>
#include <fstream>
#include <math.h>
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       DriveConfigureations.cpp                                  */
/*    Author:       Jonathan Asher                                            */
/*    Created:      Thu Nov 19 2020                                           */
/*    Updated:      Tue Mar 02 2021                                           */
/*    Description:  Method implementation for Bancroft drive configureations  */
/*                                                                            */
/*----------------------------------------------------------------------------*/
namespace bancroft
{
  /* ---------------------------------------------Module Methods--------------------------------------------- */
  Module::Module()
  {
    Motors = nullptr;
    MAX_RPM = -1;
  }

  Module::Module(vex::motor* m, std::string n)
  {
    Motors = new vex::motor_group(*m);
    name = n;
    mailboxEmpty = true;
    vex::thread mailboxIO(mailboxTask, static_cast<void*>(&mailbox));
  }

  Module::Module(vex::motor_group* m_g, std::string n)
  {
    Motors = m_g;
    name = n;
    mailboxEmpty = true;
    vex::thread mailboxIO(mailboxTask, static_cast<void*>(&mailbox));
  }

  void Module::setGearset(vex::gearSetting gear)
  {
    if(gear == vex::gearSetting::ratio6_1)
    {
      MAX_RPM = 100;
    } else if(gear == vex::gearSetting::ratio18_1)
    {
      MAX_RPM = 200;
    } else if(gear == vex::gearSetting::ratio36_1)
    {
      MAX_RPM = 600;
    }
  }

  vex::gearSetting Module::getGearset()
  {
    if(MAX_RPM == 100)
    {
      return vex::gearSetting::ratio6_1;
    } else if(MAX_RPM == 200)
    {
      return vex::gearSetting::ratio18_1;
    } else if(MAX_RPM == 600)
    {
      return vex::gearSetting::ratio36_1;
    } else 
    {
      return vex::gearSetting::ratio18_1;
    }
  }

  std::string Module::getName() { return name; } 

  void Module::runForwardAt(float speed)
  {
    if(speed > MAX_RPM) { speed = MAX_RPM; }
    if(speed < -MAX_RPM) { speed = -MAX_RPM; }
    
    Motors->setVelocity(speed, vex::velocityUnits::rpm);
  }

  void Module::runBackwardAt(float speed)
  {
    if(speed > MAX_RPM) { speed = MAX_RPM; }
    if(speed < -MAX_RPM) { speed = -MAX_RPM; }
    
    Motors->setVelocity(-speed, vex::velocityUnits::rpm);
  }

  void Module::startAllMotors()
  {
    Motors->spin(vex::directionType::fwd);
  }

  void Module::stopAllMotors()
  {
    Motors->stop();
  }

  void Module::runUntilTemp(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->mod->mailboxEmpty = false;

    while(fabs(instance->mod->Motors->rotation(vex::rotationUnits::rev)) < instance->revolutions)
    {
      instance->mod->runForwardAt(instance->speed);
      instance->mod->startAllMotors();
      vex::this_thread::sleep_for(5);
    }
    instance->mod->stopAllMotors();
    instance->mod->Motors->resetRotation();
    instance->finished = true;
  }

  void Module::runUntil(float speed, float revolutions)
  {
    passable* tempPassable = new passable;
    tempPassable->mod = this;
    tempPassable->speed = speed;
    tempPassable->revolutions = revolutions;
    tempPassable->finished = false;

    mailbox.push(tempPassable);
  }

  bool Module::canContinue()
  {
    return mailboxEmpty;
  }

  void Module::mailboxTask(void* mail)
  {
    if(mail == NULL)
      return;
    
    tFifo<passable*>* tempMailbox = static_cast<tFifo<passable*>*>(mail);
    passable* pTemp;

    while(true)
    {
      if(tempMailbox->isEmpty())
      {
        vex::this_thread::sleep_for(16);
      } else 
      {

        if(tempMailbox->pop(pTemp))
        {
            runUntilTemp(static_cast<void*>(pTemp));
            while(!pTemp->finished)
            {
              vex::this_thread::sleep_for(66);
            } 
  
          if(tempMailbox->isEmpty()) { pTemp->mod->mailboxEmpty = true; }
        }
      }
    }
  }
  /* ---------------------------------------------XDrive Methods--------------------------------------------- */
   
  XDrive::XDrive(vex::motor* LF, vex::motor* LB, vex::motor* RF, vex::motor* RB) 
  { 
    LeftFrontMotor = new vex::motor_group(*LF);
    LeftBackMotor = new vex::motor_group(*LB);
    RightFrontMotor = new vex::motor_group(*RF);
    RightBackMotor = new vex::motor_group(*RB);

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;

    mailboxEmpty = true;
    vex::thread mailboxIO(mailboxTask, static_cast<void*>(&mailbox));
  } 

  XDrive::XDrive(vex::motor_group* LF, vex::motor_group* LB, vex::motor_group* RF, vex::motor_group* RB) 
  { 
    LeftFrontMotor = LF;
    LeftBackMotor = LB;
    RightFrontMotor = RF;
    RightBackMotor = RB;

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;

    mailboxEmpty = true;
    vex::thread mailboxIO(mailboxTask, static_cast<void*>(&mailbox));
  } 

  void XDrive::waitUntilComplete()
  {
    bool canLeave = false;
   
    vex::wait(50, vex::msec);
    while(!canLeave)
    {
      canLeave = true;
      for(int i = 0; i < allModules.size(); i++)
      {
        if(!allModules[i]->canContinue() || !this->canContinue())
        {
          canLeave = false;
        } 
      }
      vex::this_thread::sleep_for(66);
    }
  }

  void XDrive::addModule(Module *addable)
  {
    this->allModules.push_back(addable);
  }
  
  Module* XDrive::getModule(std::string name) 
  { 
    for(int i = 0; i < allModules.size(); i++)
    {
      if(allModules[i]->getName() == name)
      {
        return allModules[i];
      }
    }

    return NULL;
  }

  void XDrive::setGearset(vex::gearSetting gear)
  {
    if(gear == vex::gearSetting::ratio6_1)
    {
      MAX_RPM = 100;
    } else if(gear == vex::gearSetting::ratio18_1)
    {
      MAX_RPM = 200;
    } else if(gear == vex::gearSetting::ratio36_1)
    {
      MAX_RPM = 600;
    }
  }

   
  void XDrive::setExternalGearset(float ratio)
  {
    externalGearing = ratio;
  }

   
  void XDrive::setWheelRadius(float r)
  {
    wheelRadius = r;
    wheelCircumference = (2 * PI * r);
  }

  void XDrive::setWheelToWheel(float t)
  {
    wheelToWheel = t;
  }

   
  void XDrive::setPIDValues(float p, float i, float d)
  {
    pVal = p;
    iVal = i;
    dVal = d;
  }

  void XDrive::setInertial(vex::inertial* newInertial)
  {
    inertialSensor = newInertial;
    hasInertial = true;
  }

   
  vex::gearSetting XDrive::getGearset()
  {
    if(MAX_RPM == 100)
    {
      return vex::gearSetting::ratio6_1;
    } else if(MAX_RPM == 200)
    {
      return vex::gearSetting::ratio18_1;
    } else if(MAX_RPM == 600)
    {
      return vex::gearSetting::ratio36_1;
    } else 
    {
      return vex::gearSetting::ratio18_1;
    }
  }

   
  void XDrive::getPIDValues(float &p, float &i, float &d)
  {
    p = pVal;
    i = iVal;
    d = dVal;
  }

   
  void XDrive::strafeForwardBy(float speed)
  {
    currentSpeedLF += speed;
    currentSpeedLB += speed;
    currentSpeedRF -= speed;
    currentSpeedRB -= speed;
  }

   
  void XDrive::strafeForwardAt(float speed)
  {
    LeftFrontMotor->setVelocity(speed, vex::velocityUnits::rpm);
    LeftBackMotor->setVelocity(speed, vex::velocityUnits::rpm);
    RightFrontMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    RightBackMotor->setVelocity(-speed, vex::velocityUnits::rpm);

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

   
  void XDrive::strafeBackwardBy(float speed)
  {
    currentSpeedLF -= speed;
    currentSpeedLB -= speed;
    currentSpeedRF += speed;
    currentSpeedRB += speed;
  }

   
  void XDrive::strafeBackwardAt(float speed)
  {
    LeftFrontMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    LeftBackMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    RightFrontMotor->setVelocity(speed, vex::velocityUnits::rpm);
    RightBackMotor->setVelocity(speed, vex::velocityUnits::rpm);

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

   
  void XDrive::strafeLeftBy(float speed)
  {
    currentSpeedLF -= speed;
    currentSpeedLB += speed;
    currentSpeedRF -= speed;
    currentSpeedRB += speed;
  }

   
  void XDrive::strafeLeftAt(float speed)
  {
    LeftFrontMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    LeftBackMotor->setVelocity(speed, vex::velocityUnits::rpm);
    RightFrontMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    RightBackMotor->setVelocity(speed, vex::velocityUnits::rpm);

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

   
  void XDrive::strafeRightBy(float speed)
  {
    currentSpeedLF += speed;
    currentSpeedLB -= speed;
    currentSpeedRF += speed;
    currentSpeedRB -= speed;
  }

   
  void XDrive::strafeRightAt(float speed)
  {
    LeftFrontMotor->setVelocity(speed, vex::velocityUnits::rpm);
    LeftBackMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    RightFrontMotor->setVelocity(speed, vex::velocityUnits::rpm);
    RightBackMotor->setVelocity(-speed, vex::velocityUnits::rpm);

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

   
  void XDrive::turnLeftBy(float speed)
  {
    currentSpeedLF -= speed;
    currentSpeedLB -= speed;
    currentSpeedRF -= speed;
    currentSpeedRB -= speed;
  }

   
  void XDrive::turnLeftAt(float speed)
  {
    LeftFrontMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    LeftBackMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    RightFrontMotor->setVelocity(-speed, vex::velocityUnits::rpm);
    RightBackMotor->setVelocity(-speed, vex::velocityUnits::rpm);

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

   
  void XDrive::turnRightBy(float speed)
  {
    currentSpeedLF += speed;
    currentSpeedLB += speed;
    currentSpeedRF += speed;
    currentSpeedRB += speed;
  }

   
  void XDrive::turnRightAt(float speed)
  {
    LeftFrontMotor->setVelocity(speed, vex::velocityUnits::rpm);
    LeftBackMotor->setVelocity(speed, vex::velocityUnits::rpm);
    RightFrontMotor->setVelocity(speed, vex::velocityUnits::rpm);
    RightBackMotor->setVelocity(speed, vex::velocityUnits::rpm);

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

   
  void XDrive::startAllMotors(bool variableMode)
  {
    if(!variableMode)
    {
      LeftFrontMotor->spin(vex::directionType::fwd);
      LeftBackMotor->spin(vex::directionType::fwd);
      RightFrontMotor->spin(vex::directionType::fwd);
      RightBackMotor->spin(vex::directionType::fwd);
    } else 
    {
      // This section is to ensure that a motor does not get assigned a velocity it cannot be assigned
      if(currentSpeedLF > MAX_RPM) { currentSpeedLF = MAX_RPM; }
      if(currentSpeedLF < -MAX_RPM) { currentSpeedLF = -MAX_RPM; }
      if(currentSpeedLB > MAX_RPM) { currentSpeedLB = MAX_RPM; }
      if(currentSpeedLB < -MAX_RPM) { currentSpeedLB = -MAX_RPM; }
      if(currentSpeedRF > MAX_RPM) { currentSpeedRF = MAX_RPM; }
      if(currentSpeedRF < -MAX_RPM) { currentSpeedRF = -MAX_RPM; }
      if(currentSpeedRB > MAX_RPM) { currentSpeedRB = MAX_RPM; }
      if(currentSpeedRB < -MAX_RPM) { currentSpeedRB = -MAX_RPM; }

      LeftFrontMotor->spin(vex::directionType::fwd, currentSpeedLF, vex::velocityUnits::rpm);
      LeftBackMotor->spin(vex::directionType::fwd, currentSpeedLB, vex::velocityUnits::rpm);
      RightFrontMotor->spin(vex::directionType::fwd, currentSpeedRF, vex::velocityUnits::rpm);
      RightBackMotor->spin(vex::directionType::fwd, currentSpeedRB, vex::velocityUnits::rpm);
    }

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

   
  void XDrive::stopAllMotors()
  {
    LeftFrontMotor->stop();
    LeftBackMotor->stop();
    RightFrontMotor->stop();
    RightBackMotor->stop();

    currentSpeedLF = 0;
    currentSpeedLB = 0;
    currentSpeedRF = 0;
    currentSpeedRB = 0;
  }

  bool XDrive::canContinue()
  {
    return mailboxEmpty;
  }

  void XDrive::mailboxTask(void* mail)
  {
    if(mail == NULL)
      return;
    
    tFifo<passable*>* tempMailbox = static_cast<tFifo<passable*>*>(mail);
    passable* pTemp;

    while(true)
    {
      if(tempMailbox->isEmpty())
      {
        vex::this_thread::sleep_for(16);
      } else 
      {

        if(tempMailbox->pop(pTemp))
        {
            if(pTemp->theta)
            {
              turnUntilTemp(static_cast<void*>(pTemp));
            } else
            {
              if(!pTemp->PID)
              {
                if(!pTemp->strafe)
                {
                  driveUntilTemp(static_cast<void*>(pTemp));
                } else 
                {
                  strafeUntilTemp(static_cast<void*>(pTemp));
                }
              } else 
              {
                if(!pTemp->strafe)
                {
                  driveUntilTempPID(static_cast<void*>(pTemp));
                } else 
                {
                  strafeUntilTempPID(static_cast<void*>(pTemp));
                }
              }
            } 
            while(!pTemp->finished)
            {
              vex::this_thread::sleep_for(66);
            } 
  
          if(tempMailbox->isEmpty()) { pTemp->thisDrive->mailboxEmpty = true; }
        }
      }
    }
  }

  void XDrive::driveUntil(float speed, float length)
  {
    passable* tempPassable = new passable;
    tempPassable->thisDrive = this;
    tempPassable->velocityToRun = speed;
    tempPassable->distanceCM = length;
    tempPassable->theta = 0;
    tempPassable->finished = false;
    tempPassable->PID = false;
    tempPassable->strafe = false;

    mailbox.push(tempPassable);
  }

  void XDrive::driveUntilPID(float speed, float length)
  {
    passable* tempPassable = new passable;
    tempPassable->thisDrive = this;
    tempPassable->velocityToRun = speed;
    tempPassable->distanceCM = length;
    tempPassable->finished = false;
    tempPassable->PID = true;
    tempPassable->strafe = false;

    mailbox.push(tempPassable);
  }

  void XDrive::strafeUntil(float speed, float length)
  {
    passable* tempPassable = new passable;
    tempPassable->thisDrive = this;
    tempPassable->velocityToRun = speed;
    tempPassable->distanceCM = length;
    tempPassable->finished = false;
    tempPassable->PID = false;
    tempPassable->strafe = true;

    mailbox.push(tempPassable);
  }

  void XDrive::strafeUntilPID(float speed, float length)
  {
    passable* tempPassable = new passable;
    tempPassable->thisDrive = this;
    tempPassable->velocityToRun = speed;
    tempPassable->distanceCM = length;
    tempPassable->finished = false;
    tempPassable->PID = true;
    tempPassable->strafe = true;

    mailbox.push(tempPassable);
  }

  void XDrive::turnUntil(float speed, float t)
  {
    passable* tempPassable = new passable;
    tempPassable->thisDrive = this;
    tempPassable->velocityToRun = speed;
    tempPassable->theta = t;
    tempPassable->finished = false;

    mailbox.push(tempPassable);
  }

  void XDrive::driveUntilTemp(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;

    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();

    while(
    (((fabs(instance->thisDrive->LeftFrontMotor->rotation(vex::rotationUnits::rev)) + 
    fabs(instance->thisDrive->LeftBackMotor->rotation(vex::rotationUnits::rev)) +
    fabs(instance->thisDrive->RightFrontMotor->rotation(vex::rotationUnits::rev)) + 
    fabs(instance->thisDrive->RightBackMotor->rotation(vex::rotationUnits::rev))) / 4.0) * (instance->thisDrive->wheelCircumference))
    < instance->distanceCM * sin(PI/4))
    {
      instance->thisDrive->strafeForwardAt(instance->velocityToRun);
      instance->thisDrive->startAllMotors(false);
      vex::this_thread::sleep_for(5);
    }
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();
    instance->finished = true;
  }

  void XDrive::driveUntilTempPID(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;

    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();
    
    float error = 0;
    float prevError = 0;
    float setpoint = instance->distanceCM * sin(PI/4);
    float integral = 0;
    float derivative = 0;
    float runPower = 0;

    float averageEncoder;

    do {
      averageEncoder = ((fabs(instance->thisDrive->LeftFrontMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->LeftBackMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->RightFrontMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->RightBackMotor->rotation(vex::rotationUnits::rev))) / 4) * (instance->thisDrive->wheelCircumference);
      error = setpoint - averageEncoder; 
      integral = integral + error;

      if (error >= 0) //error = 0 or passes setpoint
      {
        integral = 0;
      }
      if (error*instance->thisDrive->pVal + derivative*instance->thisDrive->pVal) 
      {
        integral = 0;
      }

      derivative = error - prevError;
      prevError = error;
      runPower = error*instance->thisDrive->pVal + integral*instance->thisDrive->iVal + derivative*instance->thisDrive->dVal;

      if(runPower > instance->velocityToRun) { runPower = instance->velocityToRun; }
      if(runPower < -instance->velocityToRun) { runPower = -instance->velocityToRun; }

      instance->thisDrive->strafeForwardAt(runPower);
      instance->thisDrive->startAllMotors(false); 
      vex::this_thread::sleep_for(5);
    } while( !(error < 5 && error > -5) );
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();
    instance->finished = true;
  }

  void XDrive::strafeUntilTemp(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;

    while(
    (((fabs(instance->thisDrive->LeftFrontMotor->rotation(vex::rotationUnits::rev)) + 
    fabs(instance->thisDrive->LeftBackMotor->rotation(vex::rotationUnits::rev)) +
    fabs(instance->thisDrive->RightFrontMotor->rotation(vex::rotationUnits::rev)) + 
    fabs(instance->thisDrive->RightBackMotor->rotation(vex::rotationUnits::rev))) / 4.0) * (instance->thisDrive->wheelCircumference))
    < instance->distanceCM * sin(PI/4))
    {
      instance->thisDrive->strafeRightAt(instance->velocityToRun);
      instance->thisDrive->startAllMotors(false);
      vex::this_thread::sleep_for(5);
    }
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();
    instance->finished = true;
  }

  void XDrive::strafeUntilTempPID(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;
    
    float error = 0;
    float prevError = 0;
    float setpoint = instance->distanceCM * sin(PI/4);
    float integral = 0;
    float derivative = 0;
    float runPower = 0;

    float averageEncoder;

    do {
      averageEncoder = ((fabs(instance->thisDrive->LeftFrontMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->LeftBackMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->RightFrontMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->RightBackMotor->rotation(vex::rotationUnits::rev))) / 4) * (instance->thisDrive->wheelCircumference);
      error = setpoint - averageEncoder; 
      integral = integral + error;

      if (error >= 0) //error = 0 or passes setpoint
      {
        integral = 0;
      }
      if (error*instance->thisDrive->pVal + derivative*instance->thisDrive->pVal) 
      {
        integral = 0;
      }

      derivative = error - prevError;
      prevError = error;
      runPower = error*instance->thisDrive->pVal + integral*instance->thisDrive->iVal + derivative*instance->thisDrive->dVal;

      if(runPower > instance->velocityToRun) { runPower = instance->velocityToRun; }
      if(runPower < -instance->velocityToRun) { runPower = -instance->velocityToRun; }

      instance->thisDrive->strafeRightAt(runPower);
      instance->thisDrive->startAllMotors(false); 
      vex::this_thread::sleep_for(5);
    } while( !(error < 5 && error > -5) );
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();
    instance->finished = true;
  }

  void XDrive::turnUntilTemp(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;

    if(!instance->thisDrive->hasInertial)
    {
      while(
      (((fabs(instance->thisDrive->LeftFrontMotor->rotation(vex::rotationUnits::rev)) + 
      fabs(instance->thisDrive->LeftBackMotor->rotation(vex::rotationUnits::rev)) +
      fabs(instance->thisDrive->RightFrontMotor->rotation(vex::rotationUnits::rev)) + 
      fabs(instance->thisDrive->RightBackMotor->rotation(vex::rotationUnits::rev))) / 4.0) * (instance->thisDrive->wheelCircumference))
      < instance->theta * (instance->thisDrive->wheelToWheel/1.9))
      {
        instance->thisDrive->turnLeftAt(instance->velocityToRun);
        instance->thisDrive->startAllMotors(false);
        vex::this_thread::sleep_for(5);
      }
    } else if(instance->thisDrive->hasInertial)
    {
      while( !(instance->thisDrive->inertialSensor->heading() + 0.5 > instance->theta && instance->thisDrive->inertialSensor->heading() - 0.5 < instance->theta) ) // Need to do testing to see how this works
      {
        instance->thisDrive->turnLeftAt(instance->velocityToRun);
        instance->thisDrive->startAllMotors(false);
        vex::this_thread::sleep_for(5);
      }
    }
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();
    instance->finished = true;
  }

  void XDrive::turnUntilTempPID(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;
    
    float error = 0;
    float prevError = 0;
    float setpoint = instance->theta * (instance->thisDrive->wheelToWheel/1.9);
    float integral = 0;
    float derivative = 0;
    float runPower = 0;

    float averageEncoder;

    do {
      averageEncoder = ((fabs(instance->thisDrive->LeftFrontMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->LeftBackMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->RightFrontMotor->rotation(vex::rotationUnits::rev)) + fabs(instance->thisDrive->RightBackMotor->rotation(vex::rotationUnits::rev))) / 4) * (instance->thisDrive->wheelCircumference);
      error = setpoint - averageEncoder; 
      integral = integral + error;

      if (error >= 0) //error = 0 or passes setpoint
      {
        integral = 0;
      }
      if (error*instance->thisDrive->pVal + derivative*instance->thisDrive->pVal) 
      {
        integral = 0;
      }

      derivative = error - prevError;
      prevError = error;
      runPower = error*instance->thisDrive->pVal + integral*instance->thisDrive->iVal + derivative*instance->thisDrive->dVal;

      if(runPower > instance->velocityToRun) { runPower = instance->velocityToRun; }
      if(runPower < -instance->velocityToRun) { runPower = -instance->velocityToRun; }

      instance->thisDrive->turnLeftAt(runPower);
      instance->thisDrive->startAllMotors(false); 
      vex::this_thread::sleep_for(5);
    } while( !(error < 5 && error > -5) );
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftFrontMotor->resetRotation();
    instance->thisDrive->LeftBackMotor->resetRotation();
    instance->thisDrive->RightFrontMotor->resetRotation();
    instance->thisDrive->RightBackMotor->resetRotation();
    instance->finished = true;
  }

  /* ---------------------------------------------HDrive Methods--------------------------------------------- */
   
  HDrive::HDrive(vex::motor *LD, vex::motor *RD)
  {
    LeftDrive = new vex::motor_group(*LD);
    RightDrive = new vex::motor_group(*RD);

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }
   
  HDrive::HDrive(vex::motor_group *LD, vex::motor_group *RD)
  {
    LeftDrive = LD;
    RightDrive = RD;

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }

  void HDrive::waitUntilComplete()
  {
    bool canLeave = false;
   
    vex::wait(50, vex::msec);
    while(!canLeave)
    {
      canLeave = true;
      for(int i = 0; i < allModules.size(); i++)
      {
        if(!allModules[i]->canContinue())
        {
          canLeave = false;
        } 
      }
      vex::this_thread::sleep_for(66);
    }
  }

  void HDrive::addModule(Module *addable)
  {
    this->allModules.push_back(addable);
  }
  
  Module* HDrive::getModule(std::string name) 
  { 
    for(int i = 0; i < allModules.size(); i++)
    {
      if(allModules[i]->getName() == name)
      {
        return allModules[i];
      }
    }

    return NULL;
  }
   
  void HDrive::setGearset(vex::gearSetting gear)
  {
    if(gear == vex::gearSetting::ratio6_1)
    {
      MAX_RPM = 100;
    } else if(gear == vex::gearSetting::ratio18_1)
    {
      MAX_RPM = 200;
    } else if(gear == vex::gearSetting::ratio36_1)
    {
      MAX_RPM = 600;
    }
  }
   
  void HDrive::setExternalGearset(float ratio)
  {
    externalGearing = ratio;
  }
   
  void HDrive::setWheelRadius(float r)
  {
    wheelRadius = r;
    wheelCircumference = (2 * PI * r);
  }

  void HDrive::setWheelToWheel(float t)
  {
    wheelToWheel = t;
  }
   
  void HDrive::setPIDValues(float p, float i, float d)
  {
    pVal = p;
    iVal = i;
    dVal = d;
  }

  void HDrive::setInertial(vex::inertial* newInertial)
  {
    inertialSensor = newInertial;
    hasInertial = true;
  }

   
  vex::gearSetting HDrive::getGearset()
  {
    if(MAX_RPM == 100)
    {
      return vex::gearSetting::ratio6_1;
    } else if(MAX_RPM == 200)
    {
      return vex::gearSetting::ratio18_1;
    } else if(MAX_RPM == 600)
    {
      return vex::gearSetting::ratio36_1;
    } else 
    {
      return vex::gearSetting::ratio18_1;
    }
  }

   
  void HDrive::getPIDValues(float &p, float &i, float &d)
  {
    p = pVal;
    i = iVal;
    d  = dVal;
  }

   
  void HDrive::driveForwardAt(float speed)
  {
    if(speed > MAX_RPM) { speed = MAX_RPM; }
    if(speed < -MAX_RPM) { speed = -MAX_RPM; }

    LeftDrive->setVelocity(speed, vex::velocityUnits::rpm);
    RightDrive->setVelocity(speed, vex::velocityUnits::rpm);

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }

   
  void HDrive::driveForwardBy(float speed)
  {
    currentSpeedLD += speed;
    currentSpeedRD += speed;
  }

   
  void HDrive::driveBackwardAt(float speed)
  {
    if(speed > MAX_RPM) { speed = MAX_RPM; }
    if(speed < -MAX_RPM) { speed = -MAX_RPM; }

    LeftDrive->setVelocity(-speed, vex::velocityUnits::rpm);
    RightDrive->setVelocity(-speed, vex::velocityUnits::rpm);

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }

   
  void HDrive::driveBackwardBy(float speed)
  {
    currentSpeedLD -= speed;
    currentSpeedRD -= speed;
  }

   
  void HDrive::turnLeftAt(float speed) 
  {
    if(speed > MAX_RPM) { speed = MAX_RPM; }
    if(speed < -MAX_RPM) { speed = -MAX_RPM; }

    LeftDrive->setVelocity(-speed, vex::velocityUnits::rpm);
    RightDrive->setVelocity(speed, vex::velocityUnits::rpm);

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }

   
  void HDrive::turnLeftBy(float speed)
  {
    currentSpeedLD -= speed;
    currentSpeedRD += speed;
  }

   
  void HDrive::turnRightAt(float speed) 
  {
    if(speed > MAX_RPM) { speed = MAX_RPM; }
    if(speed < -MAX_RPM) { speed = -MAX_RPM; }

    LeftDrive->setVelocity(speed, vex::velocityUnits::rpm);
    RightDrive->setVelocity(-speed, vex::velocityUnits::rpm);

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }

   
  void HDrive::turnRightBy(float speed)
  {
    currentSpeedLD += speed;
    currentSpeedRD -= speed;
  }

  void HDrive::leftDriveForwardAt(float speed)
  {
    currentSpeedLD += speed;
  }

  void HDrive::rightDriveForwardAt(float speed)
  {
    currentSpeedRD += speed;
  }

   
  void HDrive::startAllMotors(bool variableMode)
  {
    if(variableMode)
    {
      if(currentSpeedLD > MAX_RPM) { currentSpeedLD = MAX_RPM; }
      if(currentSpeedLD < -MAX_RPM) { currentSpeedLD = -MAX_RPM; }
      if(currentSpeedRD > MAX_RPM) { currentSpeedRD = MAX_RPM; }
      if(currentSpeedRD < -MAX_RPM) { currentSpeedRD = -MAX_RPM; }

      LeftDrive->spin(vex::directionType::fwd, currentSpeedLD, vex::velocityUnits::rpm);
      RightDrive->spin(vex::directionType::fwd, currentSpeedRD, vex::velocityUnits::rpm);
    } else 
    {
      LeftDrive->spin(vex::directionType::fwd);
      RightDrive->spin(vex::directionType::fwd);
    }

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }

   
  void HDrive::stopAllMotors()
  {
    LeftDrive->stop();
    RightDrive->stop();

    currentSpeedLD = 0;
    currentSpeedRD = 0;
  }

  bool HDrive::canContinue()
  {
    return mailboxEmpty;
  }

  void HDrive::mailboxTask(void* mail)
  {
    if(mail == NULL)
      return;
    
    tFifo<passable*>* tempMailbox = static_cast<tFifo<passable*>*>(mail);
    passable* pTemp;

    while(true)
    {
      if(tempMailbox->isEmpty())
      {
        vex::this_thread::sleep_for(16);
      } else 
      {

        if(tempMailbox->pop(pTemp))
        {
            if(pTemp->theta)
            {
              turnUntilTemp(static_cast<void*>(pTemp));
            } else 
            {
              driveUntilTemp(static_cast<void*>(pTemp));
            }
            while(!pTemp->finished)
            {
              vex::this_thread::sleep_for(66);
            } 
  
          if(tempMailbox->isEmpty()) { pTemp->thisDrive->mailboxEmpty = true; }
        }
      }
    }
  }

  void HDrive::driveUntil(float speed, float revolutions)
  {
    passable* tempPassable = new passable;
    tempPassable->thisDrive = this;
    tempPassable->velocityToRun = speed;
    tempPassable->distanceCM = revolutions;
    tempPassable->finished = false;

    mailbox.push(tempPassable);
  }

  void HDrive::turnUntil(float speed, float t)
  {
    passable* tempPassable = new passable;
    tempPassable->thisDrive = this;
    tempPassable->velocityToRun = speed;
    tempPassable->theta = t;
    tempPassable->finished = false;

    mailbox.push(tempPassable);
  }

  void HDrive::driveUntilTemp(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;
    
    instance->thisDrive->LeftDrive->resetRotation();
    instance->thisDrive->RightDrive->resetRotation();

    while(
    (((fabs(instance->thisDrive->LeftDrive->rotation(vex::rotationUnits::rev)) + 
    fabs(instance->thisDrive->RightDrive->rotation(vex::rotationUnits::rev)) / 2.0) * (instance->thisDrive->wheelCircumference))
    < instance->distanceCM))
    {
      instance->thisDrive->driveForwardAt(instance->velocityToRun);
      instance->thisDrive->startAllMotors(false);
      vex::this_thread::sleep_for(5);
    }
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftDrive->resetRotation();
    instance->thisDrive->RightDrive->resetRotation();
    instance->finished = true;
  }

  void HDrive::turnUntilTemp(void* arg)
  {
    if(arg == NULL)
      return;

    passable* instance = static_cast<passable*>(arg);

    instance->thisDrive->mailboxEmpty = false;

    if(!instance->thisDrive->hasInertial)
    {
      while(
      (((fabs(instance->thisDrive->LeftDrive->rotation(vex::rotationUnits::rev)) + 
      fabs(instance->thisDrive->RightDrive->rotation(vex::rotationUnits::rev)) / 2.0) * (instance->thisDrive->wheelCircumference))
      < instance->theta * (instance->thisDrive->wheelToWheel/1.9)))
      {
        instance->thisDrive->turnLeftAt(instance->velocityToRun);
        instance->thisDrive->startAllMotors(false);
        vex::this_thread::sleep_for(5);
      }
    } else if(instance->thisDrive->hasInertial)
    {
      while( !(instance->thisDrive->inertialSensor->heading() + 0.5 > instance->theta && instance->thisDrive->inertialSensor->heading() - 0.5 < instance->theta) ) // Need to do testing to see how this works
      {
        instance->thisDrive->turnLeftAt(instance->velocityToRun);
        instance->thisDrive->startAllMotors(false);
        vex::this_thread::sleep_for(5);
      }
    }
    instance->thisDrive->stopAllMotors();
    instance->thisDrive->LeftDrive->resetRotation();
    instance->thisDrive->RightDrive->resetRotation();
    instance->finished = true;
  }
}