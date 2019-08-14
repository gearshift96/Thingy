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
  unsigned roundCount;
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
//Weapon Systems

  weaponSystem rifleAuto;

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
  ws_magizine detactableMagizine;

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
//Assign Weapon Stages to Weapon Components

  bolt_act.weaponStages.push_back(feeding);
  bolt_act.weaponStages.push_back(chambing);
  //bolt_act.weaponStages.push_back(closing); //UNUSED
  bolt_rdy.weaponStages.push_back(extracting);
  bolt_rdy.weaponStages.push_back(ejecting);
  //bolt_rdy.weaponStages.push_back(opening); //UNUSED

//////////////////////////////

  //rifleAuto.weapComps.push_back();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
