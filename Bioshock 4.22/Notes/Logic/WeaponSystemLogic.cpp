//Author: Danny Jundi

////////////////////////////////////////////////////////////////////////////////
//INCLUDES

#include <vector>
#include <string>
#include <iostream>
#include <iomanip> //DEBUG

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
  TD_ACTIVE, //fromAscending,
  TD_READY,    //fromDecending,
  TD_EITHER,    //fromBoth
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
  unsigned rateOfFire;

    // changeInPos / time(seconds)
  //void rateOfPosChangeFixed(float);    //UNUSED

    // seconds to get to newPos; (newPos - curPos) / time[in seconds]
  //void rateOfPosChangeRelative(float); //UNUSED
};

//can be affected externally from system or by other weapon components
struct weaponComponent
{
  weaponComponent(std::string name_) : name(name_)
  {}

  struct weaponSystem* parent = nullptr;
  std::string name;
  //float curPos;                      //UNUSED
  //float curRateOfChange;             //UNUSED
  //unsigned /*weaponState*/ curStage; //UNUSED

  //void activateComponent(float); //value to set for curPos //DEPRECATED

  std::vector<weaponStage> weaponStages; //sorted array using 'triggerPos' as key
  unsigned curStage = 0;

  weaponComponent* compToActivate      = nullptr;
  triggerDirection conditionToActivate = TD_EITHER;

  weaponComponent* compToReady      = nullptr;
  triggerDirection conditionToReady = TD_EITHER;

  void CompToActivate(weaponComponent* compToActivate_,
                      triggerDirection trigDirCondition)
  {
    std::cout
    << std::left << std::setw(7)
    << name << " is setting: "
    << std::left << std::setw(7)
    << compToActivate_->name
    << " as component to ACTIVATE" << std::endl;

    compToActivate      = compToActivate_;
    conditionToActivate = trigDirCondition;
  }

  void CompToReady(weaponComponent* compToReady_,
                   triggerDirection trigDirCondition)
  {
    std::cout
    << std::left << std::setw(7)
    << name << " is setting: "
    << std::left << std::setw(7)
    << compToReady_->name
    << " as component to READY" << std::endl;

    compToReady      = compToReady_;
    conditionToReady = trigDirCondition;
  }

  bool IsReady()
  {
    return curStage == 0;
  }

  bool IsActive()
  {
    return curStage == weaponStages.size();
  }

    //function pointer; activates all active weaponStages
  virtual void activateComponent(bool activateNextComp = true)
  {
    std::cout << activateNextComp << " Activating " << name << ":" << std::endl;

    while(curStage < weaponStages.size())
    {
      if(
         weaponStages[curStage].trigDirection == TD_ACTIVE
         //|| weaponStages[curStage].trigDirection == TD_EITHER
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

if(activateNextComp)
{
    if(IsActive())
    {
      if(compToActivate != nullptr)
      {
        if(
           conditionToActivate == TD_EITHER
           || (conditionToActivate == TD_READY  && compToActivate->IsReady())
           || (conditionToActivate == TD_ACTIVE && compToActivate->IsActive())
          )
        {
          compToActivate->activateComponent();
        }
        else
        {
          std::cout << compToActivate->name << " did not satisfy condition to activate" << std::endl;
          std::cout << "  Condition needs to be: ";

          switch(conditionToActivate)
          {
            case(TD_READY):
              std::cout << "READY" << std::endl;
              break;
            case(TD_ACTIVE):
              std::cout << "ACTIVE" << std::endl;
              break;
            case(TD_EITHER):
              std::cout << "EITHER: this maybe an error..." << std::endl;
              break;
          }
        }
      }
      else
      {
        std::cout << name << " has no component to activate" << std::endl;
      }
    }
    else
    {
      std::cout << name << " could not activate" << std::endl;
    }
}
  }

  virtual void readyComponent(bool readyNextComp = true)
  {
    std::cout << readyNextComp << " Readying " << name << ":" << std::endl;

    while(curStage > 0)
    {
      if(
         weaponStages[curStage].trigDirection == TD_READY
         //|| weaponStages[curStage].trigDirection == TD_EITHER
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

if(readyNextComp)
{
    if(IsReady())
    {
      if(compToReady != nullptr)
      {
        if(
           conditionToActivate == TD_EITHER
           || (conditionToActivate == TD_READY  && compToReady->IsReady())
           || (conditionToActivate == TD_ACTIVE && compToReady->IsActive())
          )
        {
          compToReady->readyComponent();
        }
      }
    }
}
  }
};

struct FireMode
{
  unsigned roundsPerPull; //0 implies full auto //<-??
  bool triggerCheck; //to check if trigger is activie before continuing on sequence
  bool resetOnTriggerPull; //if(remainingRoundsPerPull == 0){/*reset anyway*/}
};

//Each weaponStage of the weaponComponent contain their unique firemode
//that will affect the firemode of the weapComp_Action
struct weapComp_Action : public weaponComponent
{
  weapComp_Action(std::string name_) : weaponComponent(name_)
  {}

  FireMode firemode;

  unsigned remainingRoundsPerPull; //once reached 0, end sequence

  void activateComponent();
};

#if 1
struct ws_projectile
{
};

struct ws_round
{
  ws_projectile *projectile = new ws_projectile;

  ~ws_round()
  {
    if(projectile != nullptr)
    {
      delete projectile;
    }
  }

  bool activateRound()
  {
    if(projectile != nullptr)
    {
      std::cout << "ActivatingRound" << std::endl;
      delete projectile;
      projectile = nullptr;

      return true;
    }

    return false;
  }
};

//Weapon Component that can contain a ws_round(s)
struct weapComp_Round : public weaponComponent
{
  weapComp_Round(std::string name_) : weaponComponent(name_)
  {}

  ws_round* round = nullptr;

  //weapComp_Round* shouldTransferRoundTo = nullptr;

/*
  void setRoundTransferDestination(weapComp_Round* dest)
  {
    shouldTransferRoundTo = dest;
  }
*/

  bool RoundTransferFrom(weapComp_Round* source)
  {
    if(source->round != nullptr)
    {
      if(round == nullptr)
      {
        std::cout << "Transferring round from source" << std::endl;

        //USE std::move
        round = source->round;
        source->round = nullptr;

        return true;
      }
      else
      {
        std::cout << "Round already at source" << std::endl;
      }
    }
    else
    {
      std::cout << "No round at destination" << std::endl;
    }

    return false;
  }

    //TODO: Should rewrite logic to be RoundTransferFrom?
  bool RoundTransferTo(weapComp_Round* dest)
  {
    if(round != nullptr)
    {
      if(dest->round == nullptr)
      {
        std::cout << "Transferring round to destination" << std::endl;

        //USE std::move
        dest->round = round;
        round = nullptr;

        return true;
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

    return false;
  }

  void activateRound()
  {
    if(round != nullptr)
    {
      std::cout << "Activating round in " << name << std::endl;
      round->activateRound();
    }
    else
    {
      std::cout << "No round to activate in " << name << std::endl;
    }
  }

/*
  void TransferRound()
  {
    RoundTransferTo(shouldTransferRoundTo);
  }
*/
};

struct weapComp_Mag : public weapComp_Round
{
  weapComp_Mag(std::string name_) : weapComp_Round(name_)
  {}

  ws_round* roundsContained = nullptr;
  unsigned roundCount = defaultRoundCount;

  void ReadyRound()
  {
    if(round == nullptr)
    {
      if(roundCount > 0)
      {
        std::cout << "Readying Round" << std::endl;
        --roundCount;
        round = roundsContained;
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
};

template <typename T>
struct colliderConditional
{
  T* reachableMag = nullptr;
};

static colliderConditional<weapComp_Mag> gcc;

//Responsible for making REQUESTS to the system to add & remove set weaponComponents
  //T is a weapComp_Mag
  //^In the specific use case in which this prototype is being developed
template <typename T>
struct weapComp_Port : public weaponComponent
{
  weapComp_Port(std::string name_) : weaponComponent(name_)
  {}

  T *object = nullptr;
  colliderConditional<T> cc;

  T* (*rulesForAdding)()   = nullptr;
  T* (*rulesForRemoving)() = nullptr;

  bool IsObjectAttached()
  {
    return object != nullptr;
  }

  bool IsObjectDisattached()
  {
    return object == nullptr;
  }

  void activateComponent();
  void readyComponent();

/* 
  void ReadyRound()
  {
    if(object != nullptr)
    {
      object->ReadyRound();
    }
  }

  bool RoundTransferTo(weapComp_Round* dest)
  {
    if(object != nullptr)
    {
      return object->RoundTransferTo(dest);
    }

    return true;
  }
 */
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

  template <typename T>
  void modComp(T* weapComp, T* (*rules)())
  {
    weapComp = rules();
  }

  enum WEAP_COMPS_ENUMS
  {
    TRIGGER,
    HAMMER,
    ACTION,
    FEEDPORT,
    BOLT,
    CHAMBER,
    BARREL,
    MUZZLE,
    MAGIZINE,
  };

    //string: name of stage for component to act towards
  //void activateComponent(states, std::string); //UNUSED

    //TODO: Change to std::vector<weaponComponent*>
  std::vector<weaponComponent*> weapComps;

  void addWeapComp(weaponComponent* weapComp)
  {
    weapComps.push_back(weapComp);

    if(weapComp != nullptr)
    {
      weapComp->parent = this;
    }
  }

  void debug()
  {
    std::cout << "**DEBUG START**" << std::endl;

    if(weapComps[weaponSystem::MAGIZINE] != nullptr)
    {
      std::cout
      << std::left << std::setw(25)
      << "Name of Magizine: "
      << weapComps[weaponSystem::MAGIZINE]->name << std::endl;
    }
    else
    {
      std::cout
      << std::left << std::setw(25)
      << "Name of Magizine: "
      << "N/A" << std::endl;
    }

    if(reinterpret_cast<weapComp_Port<weapComp_Mag>*>(weapComps[weaponSystem::FEEDPORT])->object != nullptr)
    {
      std::cout
      << std::left << std::setw(25)
      << "Name of Feedport object: "
      << reinterpret_cast<weapComp_Port<weapComp_Mag>*>(
           weapComps[weaponSystem::FEEDPORT])->object->name
      << std::endl;
    }
    else
    {
      std::cout
      << std::left << std::setw(25)
      << "Name of Feedport object: "
      << "N/A" << std::endl;
    }

    std::cout << "***DEBUG END***" << std::endl;
  }
};

void weapComp_Action::activateComponent()
{
  std::cout << "Actioning" << std::endl;

  if(firemode.triggerCheck)
  {
    if(parent->weapComps[weaponSystem::TRIGGER]->IsActive() == false)
    {
      return;
    }
  }

  //weaponComponent::activateComponent();
}

template <typename T>
void weapComp_Port<T>::activateComponent()
{
  parent->modComp<weapComp_Port<T>>(object, rulesForAdding);
}

template <typename T>
void weapComp_Port<T>::readyComponent()
{
  parent->modComp<weapComp_Port<T>>(object, rulesForRemoving);
}

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

bool logic_opening(struct weaponComponent* thisComp)
{
  std::cout << "Opening" << std::endl;

  reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::FEEDPORT])->readyComponent();

  return true;
}

////////////////////////////////////////
//Feeding:
//  -bolt beginning to close (animation + sound)
//  -transitionRoundFromMagToBolt (transfer of object)

bool logic_feeding(struct weaponComponent* thisComp)
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosFeeding);
  BindEvent(boltAnimation, END, transitionRoundFromMagToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosFeeding));

  //state.next(); //DEPRECATED
*/

  std::cout << "Feeding" << std::endl;

  reinterpret_cast<weapComp_Round*>(thisComp)->
    RoundTransferFrom
    (
      reinterpret_cast<weapComp_Round*>
        (thisComp->parent->weapComps[weaponSystem::FEEDPORT])
    );
  //reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::FEEDPORT])->activateComponent();

  return true;
}

////////////////////////////////////////
//Cycling:

bool logic_cycling(struct weaponComponent* thisComp)
{
  std::cout << "Cycling" << std::endl;

  reinterpret_cast<weapComp_Round*>(thisComp)->RoundTransferTo(reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT]));

  return true;
}

////////////////////////////////////////
//Loading:

bool logic_loading(struct weaponComponent* /* thisComp */)
{
  std::cout << "Loading" << std::endl;

  //IMPORTANT: How to take magizine (a weapon component)
  //external to the system to within the system?
    //There may be logic required at the weaponSystem level for
    //adding & removing weaponComponents to & from the system 

  return true;
}

////////////////////////////////////////
//Unloading:

bool logic_unloading(struct weaponComponent* /* thisComp */)
{
  std::cout << "Unloading" << std::endl;

  return true;
}

////////////////////////////////////////
//Readying

bool logic_readying(struct weaponComponent* thisComp)
{
  std::cout << "Readying" << std::endl;

  reinterpret_cast<weapComp_Mag*>(thisComp)->ReadyRound();

  return true;
}

////////////////////////////////////////
//Emptying:

bool logic_emptying(struct weaponComponent* /* thisComp */)
{
  std::cout << "Emptying" << std::endl;

  return true;
}

////////////////////////////////////////
//Chambering:
//  -bolt finsihing to close (animation + sound)
//  -transitionRoundFromBoltToChamber (transfer of object)

bool logic_chambing(struct weaponComponent* thisComp)
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosChambering);
  BindEvent(boltAnimation, END, transitionRoundFromBoltToChamber);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosChambering));

  //state.next(); //DEPRECATED
*/

  std::cout << "Chambing" << std::endl;

  return reinterpret_cast<weapComp_Round*>(thisComp)->RoundTransferFrom(reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT]));
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

bool logic_firing(struct weaponComponent* thisComp)
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

  bool returnVale = false;

  if(reinterpret_cast<weapComp_Round*>(thisComp)->round != nullptr)
  {
    returnVale = reinterpret_cast<weapComp_Round*>(thisComp)->round->activateRound();
  }

  return returnVale;
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

bool logic_extracting(struct weaponComponent* thisComp)
{
/*
  animation* boltAnimation = BoltPos<animation>(boltPosExtracting);
  BindEvent(boltAnimation, END, transitionRoundFromChamberToBolt);

  playAnimation(boltAnimation);
  playSound(BoltPos<sound>(boltPosExtracting));

  //state.next(); //DEPRECATED
*/

  std::cout << "Extracting" << std::endl;

  return reinterpret_cast<weapComp_Round*>(thisComp)->RoundTransferTo(reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT]));
}

////////////////////////////////////////
//Ejecting:
//  -ejectRoundFromBolt (sound + particle effects + spawning object)

bool logic_ejecting(struct weaponComponent* thisComp)
{
/*
  playSound(firearm.ejectionSound);
  spawnParticleEffect(round.ejectionParticleEffects);
  spawnObject(bolt.Round);
  bolt.Round = NULL;

  //state.next(); //DEPRECATED
*/

  std::cout << "Ejecting" << std::endl;

  if(reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT])->round != nullptr)
  {
    delete reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT])->round;
    reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT])->round = nullptr;
  }

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

bool logic_pulling(struct weaponComponent* thisComp)
{
  std::cout << "Pulling" << std::endl;

  if(reinterpret_cast<weapComp_Action*>(thisComp->parent->weapComps[weaponSystem::ACTION])->firemode.resetOnTriggerPull)
  {
    reinterpret_cast<weapComp_Action*>(thisComp->parent->weapComps[weaponSystem::ACTION])->remainingRoundsPerPull
      = reinterpret_cast<weapComp_Action*>(thisComp->parent->weapComps[weaponSystem::ACTION])->firemode.roundsPerPull;
  }

  return true;
}

////////////////////////////////////////
//Releasing

bool logic_releasing(struct weaponComponent* /* thisComp */)
{
  std::cout << "Releasing" << std::endl;
  return true;
}


////////////////////////////////////////
//Firemodes

  //semi
    //1
    //false/true  (same difference)
    //true /false (same difference)

  //full
    //0
    //true /false (false means to keep firing until ammo in system is exhausted)
    //true /false (same difference)

  //burst (3-round) w/ triggerCheck w/ resetOnTriggerPull
    //3
    //true
    //true
  //mode that does not need to complete burst
  //AND is reseted for each trigger pull

  //burst (3-round) w/ triggerCheck w/o resetOnTriggerPull
    //3
    //true
    //false
  //mode that does not need to complete burst
  //BUT will have less rounds on next trigger pull if not completed

  //burst (3-round) w/o triggerCheck w/ resetOnTriggerPull
    //3
    //false
    //true
  //mode that will complete all rounds in burst
  //AND is reseted for each trigger pull

  //burst (3-round) w/o triggerCheck w/o resetOnTriggerPull*
    //3
    //false
    //false
  //mode that will complete all rounds in burst
  //BUT will have less rounds on next trigger pull if not completed
  //*needs to still be considered in case a full burst could not be performed
  //(i.e. reload needed mid-burst)

/*
struct FireMode
{
  unsigned roundsPerPull;
  bool triggerCheck;
  bool resetOnTriggerPull;
}
*/

bool firemode_semi(struct weaponComponent* thisComp)
{
  std::cout << "Mode Semi" << std::endl;

  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.roundsPerPull      = 1;
  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.triggerCheck       = false;
  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.resetOnTriggerPull = true;

  return true;
}

bool firemode_burst3(struct weaponComponent* thisComp)
{
  std::cout << "Mode Burst 3" << std::endl;

  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.roundsPerPull      = 3;
  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.triggerCheck       = true;
  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.resetOnTriggerPull = true;

  return true;
}

bool firemode_full(struct weaponComponent* thisComp)
{
  std::cout << "Mode Full" << std::endl;

  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.roundsPerPull      = 0;
  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.triggerCheck       = true;
  reinterpret_cast<weapComp_Action*>(thisComp)->firemode.resetOnTriggerPull = true;

  return true;
}

////////////////////////////////////////
//Rules for adding/removing components

weapComp_Mag* rulesForAdding_Mag()
{
  std::cout << "Applying rule for adding   magizine" << std::endl;
  return gcc.reachableMag;
}

weapComp_Mag* rulesForRemoving_Mag()
{
  std::cout << "Applying rule for removing magizine" << std::endl;
  return nullptr;
}

#endif

////////////////////////////////////////////////////////////////////////////////
//LOGIC EXECUTION

int main()
{
//////////////////////////////
//Weapon Components

  //Pre-fire
  weaponComponent *trigger    = new weaponComponent("Trigger");
  weaponComponent *hammer     = new weaponComponent("Hammer");
  weapComp_Action *action     = new weapComp_Action("Action");

  //Firing
  weapComp_Port<weapComp_Mag> *feedPort  = new weapComp_Port<weapComp_Mag>("FeedPort");
  weapComp_Round  *bolt       = new weapComp_Round("Bolt");
  weapComp_Round  *chamber    = new weapComp_Round("Chamber");
  //weaponComponent *ejectPort  = new weaponComponent("EjectPort"); //UNUSED

  //Post-fire
  weaponComponent *barrel     = new weaponComponent("Barrel");
  weaponComponent *muzzle     = new weaponComponent("Muzzle");

  //External Components
  weapComp_Mag* detactableMag = new weapComp_Mag("Magizine");
  ws_round* basicRounds       = new ws_round;

  detactableMag->roundsContained = basicRounds;
  gcc.reachableMag               = detactableMag;

  //Debug
//std::cout << "feedPort->cc.reachableMag name:    ";
//if(feedPort->cc.reachableMag != nullptr)
//std::cout << feedPort->cc.reachableMag->name << std::endl;
//else
//std::cout << "N/A" << std::endl;

  feedPort->cc.reachableMag      = gcc.reachableMag;

  //Debug
//std::cout << "feedPort->cc.reachableMag name:    ";
//if(feedPort->cc.reachableMag != nullptr)
//std::cout << feedPort->cc.reachableMag->name << std::endl;
//else
//std::cout << "N/A" << std::endl;

  feedPort->rulesForAdding       = rulesForAdding_Mag;
  feedPort->rulesForRemoving     = rulesForRemoving_Mag;

#if 0
  trigger->CompToActivate(hammer);
  hammer->CompToActivate(bolt);
  bolt->CompToActivate(chamber);

#else
  //CompToActivate(component to activate, state component needs to be in for use)

std::cout << "\n**Initializing Weapons: START**" << std::endl;

#if 0
  std::cout << "Weapon: Opened-bolt Rifle" << std::endl;

    //Opened-bolt Rifle
  trigger->CompToActivate(action, TD_READY);
  action->CompToActivate(bolt, TD_READY);
  //***No hammer
  bolt->CompToActivate(chamber, TD_EITHER);

  chamber->CompToReady(bolt, TD_ACTIVE);
  bolt->CompToReady(action, TD_EITHER);

#elif 1

  std::cout << "Weapon: Closed-bolt Rifle" << std::endl;

    //Closed-bolt Rifle
  trigger->CompToActivate(action, TD_READY);
  action->CompToActivate(hammer, TD_READY);
  hammer->CompToActivate(bolt, TD_ACTIVE);
  bolt->CompToActivate(chamber, TD_EITHER);

  chamber->CompToReady(bolt, TD_ACTIVE);
  bolt->CompToReady(hammer, TD_EITHER);
  hammer->CompToReady(action, TD_EITHER);

#else

  std::cout << "Weapon: Semi-auto Pistol" << std::endl;

    //Semi-auto Pistol; bolt is slide
  trigger->CompToActivate(hammer, TD_EITHER);
  hammer->CompToActivate(bolt, TD_ACTIVE);

  chamber->CompToReady(bolt, TD_ACTIVE);
  bolt->CompToReady(hammer, TD_EITHER);

#endif

std::cout << "***Initializing Weapons: END***" << std::endl;

#endif

//////////////////////////////
//Weapon Stages

    //Trigger: ready when released
  weaponStage releasing(TD_READY);
  weaponStage pulling(TD_ACTIVE);

    //Hammer: ready when cocked
  weaponStage cocking(TD_READY);
  weaponStage engaging(TD_ACTIVE);

    //Action: uses firemodes
  weaponStage fm_semi(TD_EITHER);
  weaponStage fm_burst3(TD_EITHER);
  weaponStage fm_full(TD_EITHER);

    //Feedport
  weaponStage readying(TD_READY);
  weaponStage cycling(TD_ACTIVE);

    //Bolt: ready when opened
  weaponStage opening(TD_READY);
  weaponStage feeding(TD_ACTIVE);
  weaponStage ejecting(TD_READY);
  weaponStage extracting(TD_READY);
  weaponStage closing(TD_ACTIVE);

    //Chamber: ready when emptied
  weaponStage emptying(TD_READY);
  weaponStage chambing(TD_ACTIVE); //take round from BOLT

    //Round
  //weaponStage priming(TD_READY);
  //weaponStage firing(TD_ACTIVE);

//////////////////////////////
//Assign activationStage to weaponStage

    //Trigger
  releasing.activationStage  = logic_releasing;
  pulling.activationStage    = logic_pulling;

    //Hammer
  cocking.activationStage    = logic_cocking;
  engaging.activationStage   = logic_engaging;

    //Action
  fm_semi.activationStage    = firemode_semi;
  fm_burst3.activationStage  = firemode_burst3;
  fm_full.activationStage    = firemode_full;

    //FeedPort
  readying.activationStage   = logic_readying;
  cycling.activationStage    = logic_cycling;

    //Bolt
  opening.activationStage    = logic_opening;
  feeding.activationStage    = logic_feeding;
  ejecting.activationStage   = logic_ejecting;
  extracting.activationStage = logic_extracting;
  closing.activationStage    = logic_closing;

    //Chamber
  emptying.activationStage   = logic_emptying;
  chambing.activationStage   = logic_chambing;

    //Round
  //priming.activationStage    = logic_priming;
  //firing.activationStage     = logic_firing;

//////////////////////////////
//Assign weaponStage to weaponComponent

    //Trigger
  trigger->weaponStages.push_back(pulling);
  trigger->weaponStages.push_back(releasing);

    //Hammer
  hammer->weaponStages.push_back(cocking);
  hammer->weaponStages.push_back(engaging);

    //Action
  action->weaponStages.push_back(fm_semi);
  action->weaponStages.push_back(fm_burst3);
  action->weaponStages.push_back(fm_full);

    //Bolt
  bolt->weaponStages.push_back(opening);
  bolt->weaponStages.push_back(feeding);
  bolt->weaponStages.push_back(ejecting);
  bolt->weaponStages.push_back(extracting);
  bolt->weaponStages.push_back(closing);

    //Chamber
  chamber->weaponStages.push_back(emptying);
  chamber->weaponStages.push_back(chambing);

    //Round
  //basicRounds->weaponStages.push_back(firing);

//////////////////////////////
//Weapon Systems

  weaponSystem rifleAuto;

//////////////////////////////
//Assign weaponComponent to weaponSystem

    //NOTE: Needs to be in order of enum WEAP_COMPS_ENUMS
  rifleAuto.addWeapComp(trigger);  //TRIGGER,
  rifleAuto.addWeapComp(hammer);   //HAMMER,
  rifleAuto.addWeapComp(action);   //ACTION,
  rifleAuto.addWeapComp(feedPort); //FEEDPORT,
  rifleAuto.addWeapComp(bolt);     //BOLT,
  rifleAuto.addWeapComp(chamber);  //CHAMBER,
  rifleAuto.addWeapComp(barrel);   //BARREL,
  rifleAuto.addWeapComp(muzzle);   //MUZZLE,
  rifleAuto.addWeapComp(nullptr);  //MAGIZINE,

  weaponSystem curWeapon = rifleAuto;

//Debug
{
  std::cout << "\n**DEBUG START**" << std::endl;
  std::cout << "gcc.reachableMag name:           " << gcc.reachableMag->name << std::endl;
  std::cout << "detactableMag name:              " << detactableMag->name << std::endl;
                                                 
  std::cout << "FEEDPORT->object->name:          ";
  if(reinterpret_cast<weaponComponent*>(reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->object) != nullptr)
  std::cout << reinterpret_cast<weaponComponent*>(reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->object)->name << std::endl;
  else
  std::cout << "N/A" << std::endl;

  std::cout << "FEEDPORT->cc.reachableMag->name: ";
  if(reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->cc.reachableMag != nullptr)
  std::cout << reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->cc.reachableMag->name << std::endl;
  else
  std::cout << "N/A" << std::endl;
  std::cout << "***DEBUG END***" << std::endl;
}

    //Configure weapon to desired presets
  //action->readyComponent(false);
  //hammer->readyComponent(false);
//std::cout << "Bolt status BEFORE: " << std::boolalpha << bolt->IsActive() << std::noboolalpha << std::endl;
  //bolt->activateComponent(false);
//std::cout << "Bolt status AFTER:  " << std::boolalpha << bolt->IsActive() << std::noboolalpha << std::endl;

  std::string input;

  while(input != "q")
  {
    input.clear();
    std::cout << "\nENTER COMMAND" << std::endl;
    std::cin >> input;

#if 1
    if(input == "t") //Check Mag
    {
      std::cout << "INPUT Ammo check:" << std::endl;

      curWeapon.debug();

      if(curWeapon.weapComps[weaponSystem::MAGIZINE] == nullptr)//->object == nullptr)
      {
        std::cout << "*No Magizine" << std::endl;
      }
      else if(reinterpret_cast<weapComp_Mag*>(curWeapon.weapComps[weaponSystem::MAGIZINE])->roundCount == 0)
      {
        std::cout << "*Magizine EMPTY" << std::endl;
      }
      else if(reinterpret_cast<weapComp_Mag*>(curWeapon.weapComps[weaponSystem::MAGIZINE])->roundCount > 0)
      {
        std::cout << "*" << reinterpret_cast<weapComp_Mag*>(curWeapon.weapComps[weaponSystem::MAGIZINE])->roundCount;
        std::cout << " Rounds in Magizine" << std::endl;
      }
    }
    else if(input == "r") //Reload
    {
      std::cout << "INPUT Reloading:" << std::endl;

      bool shouldReload = true;

        //If weapon already has magizine
      if(curWeapon.weapComps[weaponSystem::MAGIZINE])
      {
        if(detactableMag->roundCount == defaultRoundCount)
        {
          std::cout << "*Magizine FULL" << std::endl;
          shouldReload = false;
        }
        else
        {
          std::cout << "*Releasing Magizine" << std::endl;
          curWeapon.weapComps[weaponSystem::FEEDPORT]->readyComponent(false);
        }
      }

      if(shouldReload == true)
      {
        if(bolt->IsReady() == false)
        {
          bolt->readyComponent(false);
        }

        std::cout << "*Inserting Magizine" << std::endl;
        detactableMag->roundCount = defaultRoundCount;

        curWeapon.debug();

        curWeapon.weapComps[weaponSystem::FEEDPORT]->activateComponent(false);
        bolt->activateComponent(false);
      }
    }
    else if(input == "f") //Fire Weapon
    {
      std::cout << "INPUT Firing:" << std::endl;

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
