////////////////////////////////////////////////////////////////////////////////
//INCLUDES

#include <vector>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//LOGIC
//*optional; cases where state maybe unnecessary for
//           the core functionality of the system
#if 0

//!Trigger Group (open): Activate at 'Feeding' stage
//  -engageTrigger (run external logic)

{
    //trigger.activationState = 'Feeding'
  if(state.current == trigger.activationState)
  {
    engageEntireTrigger();
  }
}

////////////////////////////////////////

//Feeding:
//  -bolt beginning to close (animation + sound)
//  -transitionRoundFromMagToBolt (transfer of object)

{
  animation* boltAnimation = BoltPos<animation>(boltPosFeeding);
  BindEvent(boltAnimation, END, transitionRoundFromMagToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosFeeding));

  state.next();
}

////////////////////////////////////////

//Chambering:
//  -bolt finsihing to close (animation + sound)
//  -transitionRoundFromBoltToChamber (transfer of object)

{
  animation* boltAnimation = BoltPos<animation>(boltPosChambering);
  BindEvent(boltAnimation, END, transitionRoundFromBoltToChamber);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosChambering));

  state.next();
}

////////////////////////////////////////

//*Locking:
//  -*ensuring bolt is closed (logic check)
//  -locking bolt (animation + sound)

{
    //float distanceForLocking - 
  if(bolt.distanceFromClosed() < bolt.distanceForLocking)
  {
    playAnimation(BoltPos<animation>(boltPosLocking));
    playSound(BoltPos<sound>(boltPosLocking));

    state.next();
  }
}

////////////////////////////////////////

//!Trigger Group (close): Activate at 'Firing' stage
//  -engageTrigger (run external logic)

{
    //trigger.activationState = 'Firing'
  if(state.current == trigger.activationState)
  {
    engageEntireTrigger();
  }
}

////////////////////////////////////////

//Firing:
//  -*engage hammer (animation + sound)
//  -activateRoundInChamber (activate external system)
//  -if successful: continue
//  -else: don't continue

{
  playAnimation(HammerPos<animation>(hammerPosEngaged));
  playSound(HammerPos<sound>(hammerPosEngaged));

  if(activateRoundInChamber())
  {
    state.next(); //SetState("Firing", AFTER);
  }
}

////////////////////////////////////////

//*Unlocking
//  -unlocking bolt (animation + sound)

{
  playAnimation(BoltPos<animation>(boltPosChambering));
  playSound(BoltPos<sound>(boltPosChambering));

  state.next();
}

////////////////////////////////////////

//Extracting:
//  -transitionRound(Remains)FromChamberToBolt (transfer of object)

{
  animation* boltAnimation = BoltPos<animation>(boltPosExtracting);
  BindEvent(boltAnimation, END, transitionRoundFromChamberToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosExtracting));

  state.next();
}

////////////////////////////////////////

//Ejecting:
//  -ejectRoundFromBolt (sound + particle effects + spawning object)

{
  playSound(firearm.ejectionSound);
  spawnParticleEffect(round.ejectionParticleEffects);
  spawnObject(bolt.Round);
  bolt.Round = NULL;

  state.next();
}

////////////////////////////////////////

//*Cocking
//  -ready hammer (animation + sound)

{
  playAnimation(HammerPos<animation>(hammerPosFullCock));
  playSound(HammerPos<sound>(hammerPosFullCock));

  state.next();
}

////////////////////////////////////////////////////////////////////////////////
//MACROS

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
  //std::string name;
  float triggerPos;     //value from 0.0f to 1.0f for this stage to be activated
  float rateOfChangeFactor; //rate at which to APPROACH this stage
  void (*activationLogic)(); //function pointer
  //SCRIPT activateStage;

    // changeInPos / time(seconds)
  void rateOfPosChangeFixed(float);

    // seconds to get to newPos; (newPos - curPos) / time[in seconds]
  void rateOfPosChangeRelative(float);
};

struct weaponState
{
    //sorted array using 'triggerPos' as key 
  std::vector<weaponStage> weaponStages;
};

//can be affected externally from system or by other weapon components
struct weaponComponent
{
  //std::string name;
  //float curPos;
  //float curRateOfChange;
  //unsigned /*weaponState*/ curStage;

  void activateComponent(float); //value to set for curPos

  weaponState ascending;
  weaponState descending;
};

struct weaponSystem
{
  enum states
  {
    Before,
    UpTo,
    After
  };

    //string: name of stage for component to act towards
  void activateComponent(states, std::string);

  std::vector<weaponComponent> weapComps;
};

#if 0
struct weapCompW_Round : public weaponComponent
{
  weapCompW_Round(weapCompW_Round* compTransferDesintation_)
  : compTransferDesintation(compTransferDesintation_)
  {
  }

  ws_round *round = NULL;
  weapCompW_Round *compTransferDesintation = NULL;
};

struct ws_projectile
{
};

struct ws_round
{
  ws_projectile projectile;
};

struct ws_magizine : public weapCompW_Round
{
  std::vector<ws_round> rounds;
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
//Weapon Systems

  weaponSystem rifleAuto;

//////////////////////////////
//Weapon Components

  //Pre-fire
  weaponComponent trigger;
  weaponComponent action;
  weaponComponent hammer;

  //Firing
  weaponComponent feedPort;
  weaponComponent /*weapCompW_Round*/ bolt/*(chamber)*/;
  weaponComponent /*weapCompW_Round*/ chamber/*(bolt)*/;
  weaponComponent ejectionPort;

  //Post-fire
  weaponComponent barrel;
  weaponComponent muzzle;

  //External Components
  weaponComponent /*ws_magizine*/ detactableMagizine;

//////////////////////////////
//Weapon States

  //trigger.activationLogic = ;

//////////////////////////////
//Weapon Stages

  //weaponStage bolt_feed;

//////////////////////////////

  //rifleAuto.weapComps.push_back();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
