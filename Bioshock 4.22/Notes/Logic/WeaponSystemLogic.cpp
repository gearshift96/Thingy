////////////////////////////////////////////////////////////////////////////////
//INCLUDES

#include <vector>
#include <string>
#include <iostream>

static const unsigned defaultRoundCount = 10;

////////////////////////////////////////////////////////////////////////////////
//LOGIC
//*optional; cases where state maybe unnecessary for
//           the core functionality of the system
#if 1

//!Trigger Group (open): Activate at 'Feeding' stage
//  -engageTrigger (run external logic)

/*
{
    //trigger.activationState = 'Feeding'
  if(state.current == trigger.activationState)
  {
    engageEntireTrigger();
  }
}
*/

////////////////////////////////////////

//Feeding:
//  -bolt beginning to close (animation + sound)
//  -transitionRoundFromMagToBolt (transfer of object)

void logic_feeding()
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosFeeding);
  BindEvent(boltAnimation, END, transitionRoundFromMagToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosFeeding));

  //state.next(); //DEPRECATED
*/

  std::cout << "Feeding" << std::endl;
}

////////////////////////////////////////

//Chambering:
//  -bolt finsihing to close (animation + sound)
//  -transitionRoundFromBoltToChamber (transfer of object)

void logic_chambing()
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosChambering);
  BindEvent(boltAnimation, END, transitionRoundFromBoltToChamber);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosChambering));

  //state.next(); //DEPRECATED
*/

  std::cout << "Chambing" << std::endl;
}

////////////////////////////////////////

//*Locking:
//  -*ensuring bolt is closed (logic check)
//  -locking bolt (animation + sound)

/*
{
    //float distanceForLocking - 
  if(bolt.distanceFromClosed() < bolt.distanceForLocking)
  {
    playAnimation(BoltPos<animation>(boltPosLocking));
    playSound(BoltPos<sound>(boltPosLocking));

    state.next();
  }
}
*/

////////////////////////////////////////

//!Trigger Group (close): Activate at 'Firing' stage
//  -engageTrigger (run external logic)

/*
{
    //trigger.activationState = 'Firing'
  if(state.current == trigger.activationState)
  {
    engageEntireTrigger();
  }
}
*/

////////////////////////////////////////

//Firing:
//  -*engage hammer (animation + sound)
//  -activateRoundInChamber (activate external system)
//  -if successful: continue
//  -else: don't continue

void logic_firing()
{
/*
  playAnimation(HammerPos<animation>(hammerPosEngaged));
  playSound(HammerPos<sound>(hammerPosEngaged));

  //DEPRECATED
  //if(activateRoundInChamber())
  //{
  //  state.next(); //SetState("Firing", AFTER);
  //}
*/

  std::cout << "Firing" << std::endl;
}

////////////////////////////////////////

//*Unlocking
//  -unlocking bolt (animation + sound)

/*
{
  playAnimation(BoltPos<animation>(boltPosChambering));
  playSound(BoltPos<sound>(boltPosChambering));

  state.next();
}
*/

////////////////////////////////////////

//Extracting:
//  -transitionRound(Remains)FromChamberToBolt (transfer of object)

void logic_extracting()
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosExtracting);
  BindEvent(boltAnimation, END, transitionRoundFromChamberToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosExtracting));

  //state.next(); //DEPRECATED
*/

  std::cout << "Extracting" << std::endl;
}

////////////////////////////////////////

//Ejecting:
//  -ejectRoundFromBolt (sound + particle effects + spawning object)

void logic_ejecting()
{
/*
  playSound(firearm.ejectionSound);
  spawnParticleEffect(round.ejectionParticleEffects);
  spawnObject(bolt.Round);
  bolt.Round = NULL;

  //state.next(); //DEPRECATED
*/

  std::cout << "Ejecting" << std::endl;
}

////////////////////////////////////////

//*Cocking
//  -ready hammer (animation + sound)

/*
{
  playAnimation(HammerPos<animation>(hammerPosFullCock));
  playSound(HammerPos<sound>(hammerPosFullCock));

  state.next();
}
*/

#endif

////////////////////////////////////////////////////////////////////////////////
//MACROS

#if 0

void engageEntireTrigger()
{
  setTriggerEngagement(0.0f);
}

void manualUnchamber()
{
  if(state.current == state.chambering)
  {
  }
}

//param val - value between 0.0f and 1.0f
void checkChamber(float val)
{
  float valToUse = val;

  if(val < bolt.feedPoint)
  {
    valToUse = bolt.feedPoint;
  }

  bolt.setPos(valToUse);
}

#endif

////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS
#if 0

//param a: value between 0.0f and 1.0f
//          1.0f being full RELEASE
//          0.0f being full PULL
void setTrigger(float a)
{
#ifdef DEBUG
  if(a > 1.0f || a < 0.0f)
  {
    debug.log();
    return;
  }
#endif

  std::vector<struct weaponStage>::const_iterator it = ti.begin();

  while(it != ti.end())
  {
      //TODO: Work out logic in notebook
    if(a >= it->triggerPos)
    {
      break;
    }

    ++it;
  }

  if(it != ti.end())
  {
    it->activateStage();
  }
}

float distanceFromClosed()
{
  return ;
}

#endif

////////////////////////////////////////////////////////////////////////////////
//HELPER FUNCTIONS
#if 0

weaponStage SetweaponStage(JSONobj obj)
{
  weaponStage ts;

    //Engine function call
  ts.activateStage = GetScript(obj, "activateStage");
  ts.triggerPos    = GetFloat(obj, "triggerPos");

  return ts;
}

void WS_deserialize(string scriptName)
{
  JSONobj obj= parse(scriptName, "bolt");

  while(obj.getNextObj() != nullptr)
  {
    ti.push_back(SetweaponStage(obj.curObj());
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES

enum triggerDirection //: int
{
  fromAscending,
  fromDecending,
  fromBoth
};

struct weaponStage
{
  //std::string name;         //UNUSED
  //float triggerPos;         //UNUSED //value from 0.0f to 1.0f
  //float rateOfChangeFactor; //UNUSED //rate at which to APPROACH this stage
  void (*activationStage)();  //function pointer
  //SCRIPT activateStage;     //UNUSED

    // changeInPos / time(seconds)
  //void rateOfPosChangeFixed(float);    //UNUSED

    // seconds to get to newPos; (newPos - curPos) / time[in seconds]
  //void rateOfPosChangeRelative(float); //UNUSED
};

struct weaponState
{
  std::vector<weaponStage> weaponStages; //sorted array using 'triggerPos' as key

    //function pointer; activates all weaponStages
  void activationState()
  {
    for(unsigned i = 0; i < weaponStages.size(); ++i)
    {
      weaponStages[i].activationStage();
    }
  }
};

//can be affected externally from system or by other weapon components
struct weaponComponent
{
  //std::string name;                  //UNUSED
  //float curPos;                      //UNUSED
  //float curRateOfChange;             //UNUSED
  //unsigned /*weaponState*/ curStage; //UNUSED

  //void activateComponent(float); //value to set for curPos //DEPRECATED

  weaponState activate;
  weaponState ready;
};

struct weaponSystem
{
/* //UNUSED
  enum states
  {
    Before,
    UpTo,
    After
  };
*/

    //string: name of stage for component to act towards
  //void activateComponent(states, std::string); //UNUSED

    //TODO: Change to std::vector<weaponComponent*>
  std::vector<weaponComponent> weapComps;
};

#if 1
struct ws_projectile
{
};

struct ws_round
{
  ws_projectile *projectile;
};

struct weapComp_Round : public weaponComponent
{
  ws_round *round = nullptr;
  //weapComp_Round *compTransferDesintation = NULL; //DEPRECATED
};

struct ws_magizine : public weapComp_Round
{
  unsigned roundCount = defaultRoundCount;
  //std::vector<ws_round> rounds; //UNUSED
};

struct weapComp_Port : public weaponComponent
{
  ws_magizine *magizine = nullptr;
};

#endif

#if 0
//Responsible for transferring round
// ^(mag->bolt->chamber; chamber->bolt->ejectionPort)
struct bolt: public weaponComponent
{
  void WS_deserialize(string);
  class Round*;

    //Constructor
  bolt() : curStage(0)
  {
    WS_deserialize("WeaponJSON.txt");

    for(unsigned i = 0; i < ti.size(); ++i)
    {
      BindEvent(ti[i].activateStage, ti[i].triggerPos);
    }
  }

  //REVIEW
#if 0
    //returns absolute value for distance
  float distanceFromHereTo(float val)
  {
    return abs(curPos - val);
  }
#endif

  //REVIEW
#if 0
  void setPos(float val)
  {
    if(val)

    //From, To, Rate
    my_lerp(curPos, val, curRateOfChange);
  }
#endif
};
#endif

////////////////////////////////////////////////////////////////////////////////

//LOGIC EXECUTION
////////////////////////////////////////////////////////////////////////////////

int main()
{
//////////////////////////////
//Weapon Components

  //Pre-fire
  weaponComponent trigger;
  weaponComponent action;
  weaponComponent hammer;

  //Firing
  weapComp_Port   feedPort;
  weapComp_Round  bolt/*(chamber)*/;
  weapComp_Round  chamber/*(bolt)*/;
  //weaponComponent ejectionPort; //UNUSED

  //Post-fire
  weaponComponent barrel;
  weaponComponent muzzle;

  //External Components
  ws_magizine* detactableMagizine = new ws_magizine;

//////////////////////////////
//Weapon States

  weaponState trigger_act;
  weaponState action_act;
  weaponState hammer_act;
  weaponState bolt_act;
  weaponState chamber_act;

  weaponState chamber_rdy;
  weaponState bolt_rdy;
  weaponState hammer_rdy;
  //weaponState action_rdy;  //UNUSED
  //weaponState trigger_rdy; //UNUSED

  //trigger.activationStage = ;

//////////////////////////////
//Weapon Stages

  weaponStage feeding;
  weaponStage chambing;
  //weaponStage closing; //UNUSED
  weaponStage extracting;
  weaponStage ejecting;
  //weaponStage opening; //UNUSED

//////////////////////////////
//Assign activationStage to weaponStage

  feeding.activationStage    = logic_feeding;
  chambing.activationStage   = logic_chambing;
  extracting.activationStage = logic_extracting;
  ejecting.activationStage   = logic_ejecting;

//////////////////////////////
//Assign weaponStage to weaponComponent

  bolt_act.weaponStages.push_back(feeding);
  bolt_act.weaponStages.push_back(chambing);
  //bolt_act.weaponStages.push_back(closing); //UNUSED
  bolt_rdy.weaponStages.push_back(extracting);
  bolt_rdy.weaponStages.push_back(ejecting);
  //bolt_rdy.weaponStages.push_back(opening); //UNUSED

//////////////////////////////
//Weapon Systems

  weaponSystem rifleAuto;

//////////////////////////////
//Assign weaponComponent to weaponSystem

  enum WEAP_COMPS_ENUMS
  {
    TRIGGER,
    ACTION,
    HAMMER,
    FEEDPORT,
    BOLT,
    CHAMBER,
    BARREL,
    MUZZLE
  };

  rifleAuto.weapComps.push_back(trigger);
  rifleAuto.weapComps.push_back(action);
  rifleAuto.weapComps.push_back(hammer);
  rifleAuto.weapComps.push_back(feedPort);
  rifleAuto.weapComps.push_back(bolt);
  rifleAuto.weapComps.push_back(chamber);
  rifleAuto.weapComps.push_back(barrel);
  rifleAuto.weapComps.push_back(muzzle);

  weaponSystem curWeapon = rifleAuto;
  std::string input;

  while(input != "q")
  {
    input.clear();
    std::cout << "\nENTER COMMAND" << std::endl;
    std::cin >> input;

    if(curWeapon.weapComps[FEEDPORT].magizine == nullptr)
    {
      std::cout << "*No Magizine" << std::endl;
    }

#if 0
    if(input == "t") //Check Mag
    {
      std::cout << "Ammo check:" << std::endl;
      if(curWeapon.weapComps[FEEDPORT].magizine == nullptr)
      {
        std::cout << "*No Magizine" << std::endl;
      }
      else if(curWeapon.weapComps[FEEDPORT].magizine->roundCount == 0)
      {
        std::cout << "*Magizine EMPTY" << std::endl;
      }
      else if(curWeapon.weapComps[FEEDPORT].magizine->roundCount > 0)
      {
        std::cout << "*" << curWeapon.weapComps[FEEDPORT].magizine->roundCount;
        std::cout << " Rounds in Magizine" << std::endl;
      }
    }
    else if(input == "r") //Reload
    {
      std::cout << "Reloading:" << std::endl;

      bool shouldReload = false;

        //If weapon already has magizine
      if(curWeapon.weapComps[FEEDPORT].magizine != nullptr)
      {
        if(detactableMagizine->roundCount == defaultRoundCount)
        {
          std::cout << "*Magizine FULL" << std::endl;
        }
        else
        {
          std::cout << "*Releasing Magizine" << std::endl;
          curWeapon.weapComps[FEEDPORT].magizine = nullptr;
          shouldReload = true;
        }
      }

      if(shouldReload == true)
      {
        detactableMagizine->roundCount = defaultRoundCount;
        curWeapon.weapComps[FEEDPORT].magizine = detactableMagizine;
      }
    }
#endif
  }

  delete detactableMagizine;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
