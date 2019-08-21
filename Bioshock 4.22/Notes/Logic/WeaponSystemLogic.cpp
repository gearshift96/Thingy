////////////////////////////////////////////////////////////////////////////////
//INCLUDES

#include <vector>
#include <string>
#include <iostream>

static const unsigned defaultRoundCount = 10;

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
    ti.push_back(SetweaponStage(obj.curObj()));
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES

enum triggerDirection //: int
{
  TD_ACTIVATE, //fromAscending,
  TD_READY,    //fromDecending,
  //TD_BOTH,    //fromBoth
};

struct weaponStage
{
  weaponStage(triggerDirection trigDirection_) : trigDirection(trigDirection_)
  {}

  //std::string name;         //UNUSED
  //float triggerPos;         //UNUSED //value from 0.0f to 1.0f
  //float rateOfChangeFactor; //UNUSED //rate at which to APPROACH this stage
  triggerDirection trigDirection;
  bool (*activationStage)(struct weaponComponent* thisComp);  //function pointer
  //SCRIPT activateStage;     //UNUSED

    // changeInPos / time(seconds)
  //void rateOfPosChangeFixed(float);    //UNUSED

    // seconds to get to newPos; (newPos - curPos) / time[in seconds]
  //void rateOfPosChangeRelative(float); //UNUSED
};

//can be affected externally from system or by other weapon components
struct weaponComponent
{
  //std::string name;                  //UNUSED
  //float curPos;                      //UNUSED
  //float curRateOfChange;             //UNUSED
  //unsigned /*weaponState*/ curStage; //UNUSED

  //void activateComponent(float); //value to set for curPos //DEPRECATED

  std::vector<weaponStage> weaponStages; //sorted array using 'triggerPos' as key
  unsigned curStage = 0;

  weaponComponent* compToActivate = nullptr;

  void CompToActivate(weaponComponent* compToActivate_)
  {
    compToActivate = compToActivate_;
  }

  bool IsReady()
  {
    return curStage == 0;
  }

  bool IsActive()
  {
    return curStage == weaponStages.size();
  }

    //function pointer; activates all weaponStages
  void activateComponent()
  {
    while(curStage < weaponStages.size())
    {
      if(
         weaponStages[curStage].trigDirection == TD_ACTIVATE
         //|| weaponStages[curStage].trigDirection == TD_BOTH
        )
      {
        if(weaponStages[curStage].activationStage(this))
        {
          ++curStage;
        }
        else
        {
          break;
        }
      }
      else
      {
        ++curStage;
      }
    }

    if(IsActive())
    {
      if(compToActivate != nullptr)
      {
        if(compToActivate->IsReady())
        {
          compToActivate->activateComponent();
        }
      }
    }
  }

  void readyState()
  {
    while(curStage > 0)
    {
      if(
         weaponStages[curStage].trigDirection == TD_READY
         //|| weaponStages[curStage].trigDirection == TD_BOTH
        )
      {
        if(weaponStages[curStage].activationStage(this))
        {
          --curStage;
        }
        else
        {
          break;
        }
      }
      else
      {
        --curStage;
      }
    }
  }
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
  ws_round* round = nullptr;

  weapComp_Round* shouldTransferRoundTo = nullptr;

  void setRoundTransferDestination(weapComp_Round* dest)
  {
    shouldTransferRoundTo = dest;
  }

    //TODO: Should rewrite logic to be RoundTransferFrom?
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

  void TransferRound()
  {
    RoundTransferTo(shouldTransferRoundTo);
  }
};

struct ws_magizine : public weapComp_Round
{
  unsigned roundCount = defaultRoundCount;
  void ReadyRound()
  {
    if(round == nullptr)
    {
      if(roundCount > 0)
      {
        std::cout << "Readying Round" << std::endl;
        --roundCount;
        round = new ws_round;
      }
      else
      {
        std::cout << "No more Rounds to ready" << std::endl;
      }
    }
    else
    {
      std::cout << "Round already ready" << std::endl;
    }
  }
  //std::vector<ws_round> rounds; //UNUSED
};

struct weapComp_Port : public weaponComponent
{
  ws_magizine *magizine = nullptr;

  void ReadyRound()
  {
    if(magizine != nullptr)
    {
      magizine->ReadyRound();
    }
  }

  void TransferRound()
  {
    if(magizine != nullptr)
    {
      magizine->TransferRound();
    }
  }
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

bool logic_opening(struct weaponComponent* /* thisComp */)
{
  std::cout << "Opening" << std::endl;
  return true;
}

////////////////////////////////////////
//Feeding:
//  -bolt beginning to close (animation + sound)
//  -transitionRoundFromMagToBolt (transfer of object)

bool logic_feeding(struct weaponComponent* /* thisComp */)
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosFeeding);
  BindEvent(boltAnimation, END, transitionRoundFromMagToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosFeeding));

  //state.next(); //DEPRECATED
*/

  std::cout << "Feeding" << std::endl;

  return true;
}

////////////////////////////////////////
//Chambering:
//  -bolt finsihing to close (animation + sound)
//  -transitionRoundFromBoltToChamber (transfer of object)

bool logic_chambing(struct weaponComponent* /* thisComp */)
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosChambering);
  BindEvent(boltAnimation, END, transitionRoundFromBoltToChamber);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosChambering));

  //state.next(); //DEPRECATED
*/

  std::cout << "Chambing" << std::endl;
  return true;
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

bool logic_firing(struct weaponComponent* /* thisComp */)
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
  return true;
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

bool logic_extracting(struct weaponComponent* /* thisComp */)
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosExtracting);
  BindEvent(boltAnimation, END, transitionRoundFromChamberToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosExtracting));

  //state.next(); //DEPRECATED
*/

  std::cout << "Extracting" << std::endl;
  return true;
}

////////////////////////////////////////
//Ejecting:
//  -ejectRoundFromBolt (sound + particle effects + spawning object)

bool logic_ejecting(struct weaponComponent* /* thisComp */)
{
/*
  playSound(firearm.ejectionSound);
  spawnParticleEffect(round.ejectionParticleEffects);
  spawnObject(bolt.Round);
  bolt.Round = NULL;

  //state.next(); //DEPRECATED
*/

  std::cout << "Ejecting" << std::endl;
  return true;
}

////////////////////////////////////////
//Closing

bool logic_closing(struct weaponComponent* /* thisComp */)
{
  std::cout << "Closing" << std::endl;
  return true;
}

////////////////////////////////////////
//*Cocking
//  -ready hammer (animation + sound)

bool logic_cocking(struct weaponComponent* /* thisComp */)
{
/*
  playAnimation(HammerPos<animation>(hammerPosFullCock));
  playSound(HammerPos<sound>(hammerPosFullCock));

  state.next();
*/

  std::cout << "Cocking" << std::endl;
  return true;
}

////////////////////////////////////////
//*Engaging
//  -engage hammer (animation + sound)

bool logic_engaging(struct weaponComponent* /* thisComp */)
{
  std::cout << "Engaging" << std::endl;
  return true;
}

////////////////////////////////////////
//Pulling

bool logic_pulling(struct weaponComponent* /* thisComp */)
{
  std::cout << "Pulling" << std::endl;
  return true;
}

////////////////////////////////////////
//Releasing

bool logic_releasing(struct weaponComponent* /* thisComp */)
{
  std::cout << "Releasing" << std::endl;
  return true;
}

#endif

////////////////////////////////////////////////////////////////////////////////
//LOGIC EXECUTION

int main()
{
//////////////////////////////
//Weapon Components

  //Pre-fire
  weaponComponent *trigger  = new weaponComponent;
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

  trigger->CompToActivate(hammer);
  hammer->CompToActivate(bolt);
  bolt->CompToActivate(chamber);

//////////////////////////////
//Weapon Stages

    //Trigger
  weaponStage releasing(TD_READY);
  weaponStage pulling(TD_ACTIVATE);

    //Hammer
  weaponStage cocking(TD_READY);
  weaponStage engaging(TD_ACTIVATE);

    //Bolt
  weaponStage feeding(TD_ACTIVATE);
  weaponStage chambing(TD_ACTIVATE);
  weaponStage closing(TD_ACTIVATE);
  weaponStage extracting(TD_READY);
  weaponStage ejecting(TD_READY);
  weaponStage opening(TD_READY);

    //Chambering
  weaponStage filling(TD_ACTIVATE);
  weaponStage emptying(TD_READY);

//////////////////////////////
//Assign activationStage to weaponStage

    //Trigger
  releasing.activationStage  = logic_releasing;
  pulling.activationStage    = logic_pulling;

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

    //Chamber

//////////////////////////////
//Assign weaponStage to weaponComponent

    //Trigger
  trigger->weaponStages.push_back(pulling);
  trigger->weaponStages.push_back(releasing);

    //Hammer
  hammer->weaponStages.push_back(cocking);
  hammer->weaponStages.push_back(engaging);

    //Bolt
  bolt->weaponStages.push_back(opening);
  bolt->weaponStages.push_back(feeding);
  bolt->weaponStages.push_back(extracting);
  bolt->weaponStages.push_back(chambing);
  bolt->weaponStages.push_back(ejecting);
  bolt->weaponStages.push_back(closing);

    //Chambering
  chamber->weaponStages.push_back(filling);
  chamber->weaponStages.push_back(emptying);

//////////////////////////////
//Weapon Systems

  weaponSystem rifleAuto;

//////////////////////////////
//Assign weaponComponent to weaponSystem

    //NOTE: Needs to be in order of enum WEAP_COMPS_ENUMS
  rifleAuto.weapComps.push_back(trigger);
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
      std::cout << "Firing:" << std::endl;

      curWeapon.weapComps[weaponSystem::TRIGGER]->activateComponent();

      //if(curWeapon.weapComps[weaponSystem::TRIGGER]->IsActive())
      //{
      //  curWeapon.weapComps[weaponSystem::HAMMER]->activateComponent();
      //
      //  if(curWeapon.weapComps[weaponSystem::HAMMER]->IsActive())
      //  {
      //    curWeapon.weapComps[weaponSystem::BOLT]->activateComponent();
      //
      //    if(curWeapon.weapComps[weaponSystem::BOLT]->IsActive())
      //    {
      //      //TODO: activate round in chamber;
      //      //      this will need to be done in activateStage
      //    }
      //  }
      //}
    }
#endif
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
