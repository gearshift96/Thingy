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
//Opening:

void logic_opening()
{
  std::cout << "Opening" << std::endl;
}

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
//Closing

void logic_closing()
{
  std::cout << "Closing" << std::endl;
}

////////////////////////////////////////
//*Cocking
//  -ready hammer (animation + sound)

void logic_cocking()
{
/*
  playAnimation(HammerPos<animation>(hammerPosFullCock));
  playSound(HammerPos<sound>(hammerPosFullCock));

  state.next();
*/

  std::cout << "Cocking" << std::endl;
}

////////////////////////////////////////
//*Engaging
//  -engage hammer (animation + sound)

void logic_engaging()
{
  std::cout << "Engaging" << std::endl;
}

////////////////////////////////////////
//Pulling

void logic_pulling()
{
  std::cout << "Pulling" << std::endl;
}

////////////////////////////////////////
//Releasing

void logic_releasing()
{
  std::cout << "Releasing" << std::endl;
}

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
  //unsigned curStage = 0;

    //function pointer; activates all weaponStages
  void activateState()
  {
    for(unsigned i = 0; i < weaponStages.size(); ++i)
    {
      weaponStages[i].activationStage();
    }
  }

  void readyState()
  {
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

    //string: name of stage for component to act towards
  //void activateComponent(states, std::string); //UNUSED

    //TODO: Change to std::vector<weaponComponent*>
  std::vector<weaponComponent*> weapComps;
};

#if 1
struct ws_projectile
{
};

struct ws_round
{
  ws_projectile *projectile;
  void activateRound();
};

struct weapComp_Round : public weaponComponent
{
  ws_round *round = nullptr;
  void RoundTransferTo(weapComp_Round* dest)
  {
    if(round != nullptr)
    {
      if(dest->round == nullptr)
      {
        std::cout << "Transfering round to destination" << std::endl;
        dest->round = round;
        round = nullptr;
      }
      else
      {
        std::cout << "Round already at destination" << std::endl;
      }
    }
    else
    {
      std::cout << "No round at source" << std::endl;
    }
  }
};

struct ws_magizine : public weapComp_Round
{
  unsigned roundCount = defaultRoundCount;
  void ReadyRound()
  {
    if(roundCount > 0 && round == nullptr)
    {
      --roundCount;
      round = new ws_round;
    }
  }
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
  weaponComponent *trigger  = new weaponComponent;
  weaponComponent *action   = new weaponComponent;
  weaponComponent *hammer   = new weaponComponent;

  //Firing
  weapComp_Port   *feedPort = new weapComp_Port;
  weapComp_Round  *bolt     = new weapComp_Round;
  weapComp_Round  *chamber  = new weapComp_Round;
  //weaponComponent ejectionPort; //UNUSED

  //Post-fire
  weaponComponent *barrel   = new weaponComponent;
  weaponComponent *muzzle   = new weaponComponent;

  //External Components
  ws_magizine* detactableMagizine = new ws_magizine;

//////////////////////////////
//Weapon States

  weaponState trigger_act;
  //weaponState action_act; //UNUSED
  weaponState hammer_act;
  weaponState bolt_act;
  //weaponState chamber_act; //UNUSED

  //weaponState chamber_rdy; //UNUSED
  weaponState bolt_rdy;
  weaponState hammer_rdy;
  //weaponState action_rdy; //UNUSED
  weaponState trigger_rdy;

  //trigger.activationStage = ;

//////////////////////////////
//Weapon Stages

    //Trigger
  weaponStage pulling;
  weaponStage releasing;

    //Hammer
  weaponStage cocking;
  weaponStage engaging;

    //Bolt
  weaponStage feeding;
  weaponStage chambing;
  weaponStage closing;
  weaponStage extracting;
  weaponStage ejecting;
  weaponStage opening;

//////////////////////////////
//Assign activationStage to weaponStage

    //Trigger
  pulling.activationStage    = logic_pulling;
  releasing.activationStage  = logic_releasing;

    //Hammer
  cocking.activationStage    = logic_cocking;
  engaging.activationStage   = logic_engaging;

    //Bolt
  closing.activationStage    = logic_closing;
  feeding.activationStage    = logic_feeding;
  chambing.activationStage   = logic_chambing;
  extracting.activationStage = logic_extracting;
  ejecting.activationStage   = logic_ejecting;
  opening.activationStage    = logic_opening;

//////////////////////////////
//Assign weaponStage to weaponComponent

    //Trigger
  trigger_act.weaponStages.push_back(pulling);
  trigger_rdy.weaponStages.push_back(releasing);

    //Hammer
  hammer_act.weaponStages.push_back(cocking);
  hammer_rdy.weaponStages.push_back(engaging);

    //Bolt
  bolt_act.weaponStages.push_back(feeding);
  bolt_act.weaponStages.push_back(chambing);
  bolt_act.weaponStages.push_back(closing);
  bolt_rdy.weaponStages.push_back(extracting);
  bolt_rdy.weaponStages.push_back(ejecting);
  bolt_rdy.weaponStages.push_back(opening);

//////////////////////////////
//Weapon Systems

  weaponSystem rifleAuto;

//////////////////////////////
//Assign weaponComponent to weaponSystem

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

#if 1
    if(input == "t") //Check Mag
    {
      std::cout << "Ammo check:" << std::endl;
      if(reinterpret_cast<weapComp_Port*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->magizine == nullptr)
      {
        std::cout << "*No Magizine" << std::endl;
      }
      else if(reinterpret_cast<weapComp_Port*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->magizine->roundCount == 0)
      {
        std::cout << "*Magizine EMPTY" << std::endl;
      }
      else if(reinterpret_cast<weapComp_Port*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->magizine->roundCount > 0)
      {
        std::cout << "*" << reinterpret_cast<weapComp_Port*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->magizine->roundCount;
        std::cout << " Rounds in Magizine" << std::endl;
      }
    }
    else if(input == "r") //Reload
    {
      std::cout << "Reloading:" << std::endl;

      bool shouldReload = true;

        //If weapon already has magizine
      if(reinterpret_cast<weapComp_Port*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->magizine != nullptr)
      {
        if(detactableMagizine->roundCount == defaultRoundCount)
        {
          std::cout << "*Magizine FULL" << std::endl;
          shouldReload = false;
        }
        else
        {
          std::cout << "*Releasing Magizine" << std::endl;
          reinterpret_cast<weapComp_Port*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->magizine = nullptr;
        }
      }

      if(shouldReload == true)
      {
        std::cout << "Inserting Magizine" << std::endl;
        detactableMagizine->roundCount = defaultRoundCount;
        reinterpret_cast<weapComp_Port*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->magizine = detactableMagizine;
      }
    }
    else if(input == "f") //Fire Weapon
    {
      
    }
#endif
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
