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
/*
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
*/
#endif

////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS
#if 0
/*
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
*/
#endif

////////////////////////////////////////////////////////////////////////////////
//HELPER FUNCTIONS
#if 0
/*
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
*/
#endif

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES

//RENAME: TriggerDestination
enum triggerDirection //: int
{
  TD_ACTIVE,
  TD_READY,
  TD_EITHER,
  //TD_NOT_ACTIVE,
  //TD_NOT_READY,
  TD_COUNT   //NEEDS TO REMAIN LAST
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
  //TODO: add debug() so all weaponComponents can print debug info relative to debugging their use cases

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
  void activateComponent(/*bool activateNextComp = true*/)
  {
    std::cout << /*activateNextComp <<*/ "Activating " << name << ":" << std::endl;

    while(curStage < weaponStages.size())
    {
      if(
         weaponStages[curStage].trigDirection == TD_ACTIVE
         || weaponStages[curStage].trigDirection == TD_EITHER
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

    if(curStage == weaponStages.size())
    {
      --curStage;
    }

//if(activateNextComp)
//{
    if(IsActive())
    {
      if(compToActivate != nullptr)
      {
        std::cout 
        << name
        << " is set to activate "
        << compToActivate->name
        << std::endl;

        if(
           conditionToActivate == TD_EITHER
           || (conditionToActivate == TD_READY  && compToActivate->IsReady())
           || (conditionToActivate == TD_ACTIVE && compToActivate->IsActive())
          )
        {
#if 1
          compToActivate->activateComponent();
#else
          if(compToActivate->activateComponent())
          {
            std::cout
            << "*SUCCESS: "
            << name
            << " ACTIVATED"
            << std::endl;
          }
          else
          {
            std::cout
            << "*NOTICE:  "
            << name
            << " HAS NOT ACTIVATED"
            << std::endl;
          }
#endif
        }
        else
        {
          std::cout
          << compToActivate->name
          << " did not satisfy condition to be activated"
          << std::endl;

          std::cout << "Condition needs to be: ";

          switch(conditionToActivate)
          {
            case(TD_READY):
              std::cout << "READY" << std::endl;
              break;
            case(TD_ACTIVE):
              std::cout << "ACTIVE" << std::endl;
              break;
            case(TD_EITHER):
              std::cout << "EITHER; this maybe an error..." << std::endl;
              break;
            default:;
          }
        }
      }
      else //DEBUG
      {
        //std::cout << name << " has no component to activate" << std::endl;
      }
    }
    else
    {
      std::cout << name << " could not completely activate" << std::endl;
    }
//}
  }

  void readyComponent(/*bool readyNextComp = true*/)
  {
    std::cout << /*readyNextComp <<*/ "Readying " << name << ":" << std::endl;

    while(curStage > 0)
    {
      if(
         weaponStages[curStage].trigDirection == TD_READY
         || weaponStages[curStage].trigDirection == TD_EITHER
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

      if(curStage == 0)
      {
        break;
      }
    }

//if(readyNextComp)
//{
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
//}
  }
};

struct FireMode
{
  FireMode
  (
    std::string modeName_,
    unsigned roundsPerPull_,
    bool triggerCheck_,
    bool resetOnTriggerRelease_
  )
  : modeName(modeName_)
  , roundsPerPull(roundsPerPull_)
  , triggerCheck(triggerCheck_)
  , resetOnTriggerRelease(resetOnTriggerRelease_)

  {}

  std::string modeName;
  unsigned roundsPerPull; //0 implies full auto //<-??
  bool triggerCheck; //to check if trigger is activie before continuing on sequence
  bool resetOnTriggerRelease; //if(remainingRoundsPerPull == 0){/*reset anyway*/}
};

struct FireModes
{
  std::vector<FireMode> firemodes;
  unsigned curFiremode = 0;

  FireMode cycleFiremodes()
  {
    FireMode firemodeToReturn = firemodes[++curFiremode];

    if(curFiremode == firemodes.size())
    {
      curFiremode = 0;
    }

    return firemodeToReturn;
  }
};

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

  //burst (3-round) w/ triggerCheck w/ resetOnTriggerRelease
    //3
    //true
    //true
  //mode that does not need to complete burst
  //AND is reseted for each trigger pull

  //burst (3-round) w/ triggerCheck w/o resetOnTriggerRelease
    //3
    //true
    //false
  //mode that does not need to complete burst
  //BUT will have less rounds on next trigger pull if not completed

  //burst (3-round) w/o triggerCheck w/ resetOnTriggerRelease
    //3
    //false
    //true
  //mode that will complete all rounds in burst
  //AND is reseted for each trigger pull

  //burst (3-round) w/o triggerCheck w/o resetOnTriggerRelease*
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
  std::string modeName;
  unsigned roundsPerPull;
  bool triggerCheck;
  bool resetOnTriggerRelease;
}
*/

static FireMode semi("Semi-Auto", 1 ,false, true);
static FireMode burst3("Burst 3", 3, true, true);
static FireMode full("Full-Auto", 0, true, true);

//Each weaponStage of the weaponComponent contain their unique firemode
//that will affect the firemode of the weapComp_Action
struct weapComp_Action : public weaponComponent
{
  weapComp_Action(std::string name_) : weaponComponent(name_)
  {}

  FireMode firemode = semi;

  unsigned remainingRoundsPerPull; //once reached 0, end sequence

  //void activateComponent(bool activateNextComp = true) override;
};

#if 1
struct ws_projectile
{
};

struct ws_round
{
  friend struct weapComp_Round; //friend declaration

  ws_projectile *projectile = new ws_projectile;

  ~ws_round()
  {
    if(projectile != nullptr)
    {
      delete projectile;
    }
  }

private:
  bool activateRound()
  {
    if(projectile != nullptr)
    {
      std::cout << "*ActivatingRound" << std::endl;
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
        std::cout
        << std::left << std::setw(26)
        << "*Transferring round FROM: "
        << source->name
        << std::endl;

        //USE std::move
        round = source->round;
        source->round = nullptr;

        return true;
      }
      else
      {
        std::cout
        << std::left << std::setw(26)
        << "*Round already at source: "
        << source->name
        << std::endl;
      }
    }
    else
    {
      std::cout
      << std::left << std::setw(26)
      << "*No round at destination: "
      << name
      << std::endl;
    }

    return true; //false; //TEMP
  }

    //TODO: Should rewrite logic to be RoundTransferFrom?
  bool RoundTransferTo(weapComp_Round* dest)
  {
    if(round != nullptr)
    {
      if(dest->round == nullptr)
      {
        std::cout
        << std::left << std::setw(26)
        << "*Transferring round TO: "
        << dest->name
        << std::endl;

        //USE std::move
        dest->round = round;
        round = nullptr;

        return true;
      }
      else
      {
        std::cout
        << std::left << std::setw(26)
        << "*Round already at destination: "
        << dest->name
        << std::endl;
      }
    }
    else
    {
      std::cout
      << std::left << std::setw(26)
      << "*No round at source"
      << name
      << std::endl;
    }

    return true; //false; //TEMP
  }

  bool activateRound()
  {
    bool roundActivated = false;

    if(round != nullptr)
    {
      std::cout << "*Activating round in:     " << name << std::endl;
      roundActivated = round->activateRound();
    }
    else
    {
      std::cout << "*No round to activate in: " << name << std::endl;
    }

    return roundActivated;
  }

/*
  void TransferRound()
  {
    RoundTransferTo(shouldTransferRoundTo);
  }
*/

  void debug()
  {
    if(round == nullptr)
    {
      std::cout << "Round NOT there" << std::endl;
    }
    else
    {
      std::cout << "Round IS there" << std::endl;
    }
  }
};

struct weapComp_Mag : public weapComp_Round
{
  weapComp_Mag(std::string name_) : weapComp_Round(name_)
  {}

  ws_round* roundsContained = nullptr;
  unsigned roundCount = defaultRoundCount;

  void Reset()
  {
    std::cout << "Resetting: " << name << std::endl;

    roundCount = defaultRoundCount;
    readyComponent();
  }

  void ReadyRound()
  {
    std::cout << "Readying Round" << std::endl;

    if(round == nullptr)
    {
      if(roundCount > 0)
      {
        std::cout << "A round has been set to ready" << std::endl;
        --roundCount;
        round = roundsContained;
      }
      else
      {
        std::cout << "No more rounds to set toready" << std::endl;
      }
    }
    else
    {
      std::cout << "Round already set to ready" << std::endl;
    }
  }
};

template <typename T>
struct colliderConditional
{
  T* reachableMag = nullptr;
};

static colliderConditional<weapComp_Mag> gcc;

#endif

#if 0
/*
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
*/
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
  void modComp(T*& weapComp, T* (*rules)())
  {
//**DEBUG START**
{
    if(weapComp != nullptr)
    {
      std::cout
      << std::left << std::setw(32)
      << "**Modding START**: "
      << weapComp->name
      << " "
      << weapComp
      << std::endl;
    }
    else
    {
      std::cout
      << std::left << std::setw(32)
      << "**Modding START**: "
      << "No weaponComp"
      << " "
      << weapComp
      << std::endl;
    }
}
//***DEBUG END***

    weapComp = rules();

    if(weapComp != nullptr)
    {
      weapComp->parent = this;

        //TODO: Research a way to have 'weapComp' in modComp()
        //      correspond to its relative data in parent->weapComps 
      //weapComp->parent->weapComps[weaponSystem::MAGIZINE] = weapComp; //TEMP
    }

//**DEBUG START**
{
    if(weapComp != nullptr)
    {
      std::cout
      << std::left << std::setw(32)
      << "***Modding END***: "
      << weapComp->name
      << " "
      << weapComp
      << std::endl;
    }
    else
    {
      std::cout
      << std::left << std::setw(32)
      << "***Modding END***: "
      << "No weaponComp"
      << " "
      << weapComp
      << std::endl;
    }
}
//***DEBUG END***
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

  void debug();
};

//Responsible for making REQUESTS to the system to add & remove set weaponComponents
  //T is a weapComp_Mag
  //^In the specific use case in which this prototype is being developed
template <typename T>
struct weapComp_Port : public weaponComponent
{
  weapComp_Port(std::string name_) : weaponComponent(name_)
  {}

  T *(*object) = nullptr;
  colliderConditional<T> cc;

  T* (*rulesForAdding)()   = nullptr;
  T* (*rulesForRemoving)() = nullptr;

  void addComp()
  {
    if(object != nullptr)
    {
      parent->modComp<T>((*object), rulesForAdding);
    }
  }

  void removeComp()
  {
    if(object != nullptr)
    {
      parent->modComp<T>((*object), rulesForRemoving);
    }
  }

  bool IsObjectAttached()
  {
    return (*object) != nullptr;
  }

  bool IsObjectDisattached()
  {
    return (*object) == nullptr;
  }
};

//DEPRECATED
#if 0
void weapComp_Action::activateComponent(bool /* activateNextComp */)
{
  std::cout << "Activating Action" << std::endl;

  if(firemode.triggerCheck)
  {
    if(parent->weapComps[weaponSystem::TRIGGER]->IsActive() == false)
    {
      if(firemode.resetOnTriggerRelease)
      {
        remainingRoundsPerPull = firemode.roundsPerPull;
      }

      return;
    }
  }

  if(remainingRoundsPerPull == 0)
  {
    remainingRoundsPerPull = firemode.roundsPerPull;
    return;
  }

//weaponComponent::activateComponent(false);
}

template <typename T>
void weapComp_Port<T>::activateComponent(bool activateNextComp)
{
  std::cout << "Activating Port" << std::endl;

  //parent->modComp<weapComp_Port<T>>((*object), rulesForAdding);
  //addComp();

weaponComponent::activateComponent(activateNextComp);
}

template <typename T>
void weapComp_Port<T>::readyComponent(bool readyNextComp)
{
  std::cout << "Readying Port" << std::endl;

  //parent->modComp<weapComp_Port<T>>((*object), rulesForRemoving);
  //removeComp();

weaponComponent::activateComponent(readyNextComp);
}
#endif

void weaponSystem::debug()
{
  std::cout << "\n**DEBUG START**" << std::endl;

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

  if(
     (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(
       weapComps[weaponSystem::FEEDPORT])->object)
     != nullptr
    )
  {
    std::cout
    << std::left << std::setw(25)
    << "Name of Feedport object: "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(
         weapComps[weaponSystem::FEEDPORT])->object)->name
    << std::endl;
  }
  else
  {
    std::cout
    << std::left << std::setw(25)
    << "Name of Feedport object: "
    << "N/A" << std::endl;
  }

  std::cout << "***DEBUG END***\n" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//LOGIC
//*optional; cases where state maybe unnecessary for
//           the core functionality of the system

//Trigger: ready when released
  //releasing(TD_READY);
  //pulling(TD_ACTIVE);
//Hammer: ready when cocked
  //cocking(TD_READY);
  //engaging(TD_ACTIVE);
//Action: uses firemodes
  //readying(TD_EITHER);
  //actioning(TD_EITHER);
//Feedport: ready when unloaded
  //unloading(TD_READY);
  //loading(TD_ACTIVE);
//Bolt: ready when opened
  //opening(TD_READY);
  //feeding(TD_ACTIVE);
  //ejecting(TD_READY);
  //chambing(TD_ACTIVE); //was in Chamber //take round from BOLT
  //extracting(TD_READY);
  //closing(TD_ACTIVE);
//Chamber: ready when emptied
  //emptying(TD_READY);
  //firing(TD_ACTIVE); //was in Round
//Magizine: ready when round is ready for transfer
  //cyclingInternally(TD_READY);
  //cyclingExternally(TD_ACTIVE);

////////////////////////////////////////
//==TRIGGER==
  //releasing(TD_READY);
  //pulling(TD_ACTIVE);

//Pulling
bool logic_pulling(struct weaponComponent* thisComp)
{
  std::cout << "Pulling" << std::endl;

  if(reinterpret_cast<weapComp_Action*>(thisComp->parent->weapComps[weaponSystem::ACTION])->firemode.resetOnTriggerRelease)
  {
    reinterpret_cast<weapComp_Action*>(thisComp->parent->weapComps[weaponSystem::ACTION])->remainingRoundsPerPull
      = reinterpret_cast<weapComp_Action*>(thisComp->parent->weapComps[weaponSystem::ACTION])->firemode.roundsPerPull;
  }

  std::cout << "*Success" << std::endl;

  return true;
}

//Releasing
bool logic_releasing(struct weaponComponent* /* thisComp */)
{
  std::cout << "Releasing" << std::endl;

  std::cout << "*Success" << std::endl;

  return true;
}

////////////////////////////////////////
//==HAMMER==
  //cocking(TD_READY);
  //engaging(TD_ACTIVE);

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

  std::cout << "*Success" << std::endl;

  return true;
}

//*Engaging
//  -engage hammer (animation + sound)
bool logic_engaging(struct weaponComponent* thisComp)
{
  std::cout << "Engaging" << std::endl;

  if(thisComp->parent->weapComps[weaponSystem::BOLT]->IsActive() == false)
  {
    std::cout << "*Bolt is not actived" << std::endl;
    return false;
  }

  std::cout << "*Success" << std::endl;

  return true;
}

////////////////////////////////////////
//==ACTION==
  //readying(TD_EITHER);
  //actioning(TD_EITHER);

//Reacting:
bool logic_reacting(struct weaponComponent* thisComp)
{
  std::cout << "Reacting" << std::endl;

  weapComp_Action* thisAction = reinterpret_cast<weapComp_Action*>(thisComp);

  thisAction->parent->weapComps[weaponSystem::BOLT]->activateComponent(/* false */);

/*
  if(thisAction->firemode.triggerCheck)
  {
    if(thisAction->parent->weapComps[weaponSystem::TRIGGER]->IsActive() == false)
    {
      if(thisAction->firemode.resetOnTriggerRelease)
      {
        thisAction->remainingRoundsPerPull = thisAction->firemode.roundsPerPull;
      }

      return false;
    }
  }
*/

  if(thisAction->remainingRoundsPerPull > 0
     || thisAction->firemode.roundsPerPull == 0) //i.e. is full auto
  {
    thisComp->activateComponent();
    std::cout << "*Re-Actioning" << std::endl;
    return false;
  }
  else
  {
    thisAction->parent->weapComps[weaponSystem::TRIGGER]->readyComponent();
    std::cout << "*Success" << std::endl;
    return true;
  }
}

//Actioning:
bool logic_actioning(struct weaponComponent* thisComp)
{
  std::cout << "Actioning" << std::endl;

  weapComp_Action* thisAction = reinterpret_cast<weapComp_Action*>(thisComp);

  if(thisAction->remainingRoundsPerPull == 0)
  {
    if(thisAction->firemode.roundsPerPull > 0) //i.e. not full auto
    {
      thisComp->readyComponent();
      std::cout << "*No more rounds remaining this pull" << std::endl;
      return false;
    }
  }
  else
  {
    --thisAction->remainingRoundsPerPull;
  }

  std::cout << "*Success" << std::endl;

  return true;
}

////////////////////////////////////////
//==FEEDPORT==
  //unloading(TD_READY);
  //loading(TD_ACTIVE);

//Unloading:
bool logic_unloading(struct weaponComponent* thisComp)
{
  std::cout << "Unloading" << std::endl;

  reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->removeComp();

  std::cout << "*Success" << std::endl;

  return true;
}

//Loading:
bool logic_loading(struct weaponComponent* thisComp)
{
  std::cout << "Loading" << std::endl;

  //IMPORTANT: How to take magizine (a weapon component)
  //external to the system to within the system?
    //There may be logic required at the weaponSystem level for
    //adding & removing weaponComponents to & from the system 

#if 0
//**DEBUG START**
{
  if(thisComp->parent->weapComps[weaponSystem::MAGIZINE] != nullptr)
  {
    std::cout
    << std::left << std::setw(32)
    << "**weapComps[MAGIZINE] START**: "
    << thisComp->parent->weapComps[weaponSystem::MAGIZINE]->name
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }
  else
  {
    std::cout
    << std::left << std::setw(32)
    << "**weapComps[MAGIZINE] START**: "
    << "No weaponComp"
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }

  if(
     (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
     != nullptr
    )
  {
    std::cout
    << std::left << std::setw(32)
    << "**logic_loading START**: "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)->name
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }
  else
  {
    std::cout
    << std::left << std::setw(32)
    << "**logic_loading START**: "
    << "No weaponComp"
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }
}
//***DEBUG END***
#endif

  reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->addComp();

  std::cout << "*Success" << std::endl;

#if 0
//**DEBUG START**
{
  if(
     (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
     != nullptr
    )
  {
    std::cout
    << std::left << std::setw(32)
    << "***logic_loading END***: "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)->name
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }
  else
  {
    std::cout
    << std::left << std::setw(32)
    << "***logic_loading END***: "
    << "No weaponComp"
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }

  if(thisComp->parent->weapComps[weaponSystem::MAGIZINE] != nullptr)
  {
    std::cout
    << std::left << std::setw(32)
    << "***weapComps[MAGIZINE] END***: "
    << thisComp->parent->weapComps[weaponSystem::MAGIZINE]->name
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }
  else
  {
    std::cout
    << std::left << std::setw(32)
    << "***weapComps[MAGIZINE] END***: "
    << "No weaponComp"
    << " "
    << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->object)
    << std::endl;
  }
}
//***DEBUG END***
#endif

  return true;
}

////////////////////////////////////////
//==BOLT==
  //opening(TD_READY);
  //feeding(TD_ACTIVE);
  //ejecting(TD_READY);
  //chambing(TD_ACTIVE); //was in Chamber //take round from BOLT
  //extracting(TD_READY);
  //closing(TD_ACTIVE);

//Opening:
bool logic_opening(struct weaponComponent* thisComp)
{
  std::cout << "Opening" << std::endl;

  thisComp->parent->weapComps[weaponSystem::MAGIZINE]->readyComponent();

  std::cout << "*Success" << std::endl;

  return true;
}

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

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  thisComp->parent->weapComps[weaponSystem::MAGIZINE]->activateComponent();

  std::cout << "*Success" << std::endl;

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  return true;
}

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

  std::cout << "*Success" << std::endl;

  return true;
}

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

  bool returnVal = reinterpret_cast<weapComp_Round*>(thisComp)->RoundTransferTo(
    reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::CHAMBER]));

  //TEMP: forcing success
  if (returnVal == false)
  {
    std::cout << "*FORCING Success; result was " << std::boolalpha << returnVal << std::endl;
    returnVal = true; //DESIRED_EFFECT
  }
  else
  {
    std::cout << "*Success" << std::endl;
  }

  return returnVal;
}

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

  //thisComp->parent->weapComps[weaponSystem::CHAMBER]->readyComponent(/* false */);

  std::cout << "*Success" << std::endl;

  return true;
}

//Closing
bool logic_closing(struct weaponComponent* /* thisComp */)
{
  std::cout << "Closing" << std::endl;

  std::cout << "*Success" << std::endl;

  return true;
}

////////////////////////////////////////
//==CHAMBER==
  //emptying(TD_READY);
  //firing(TD_ACTIVE); //was in Round

//Emptying:
bool logic_emptying(struct weaponComponent* thisComp)
{
  std::cout << "Emptying" << std::endl;

  bool returnVal= reinterpret_cast<weapComp_Round*>(thisComp)->RoundTransferTo(
    reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT]));

  //TEMP: forcing success
  if (returnVal == false)
  {
    std::cout << "*FORCING Success; result was " << std::boolalpha << returnVal << std::endl;
    returnVal = true; //DESIRED_EFFECT
  }
  else
  {
    std::cout << "*Success" << std::endl;
  }

  return returnVal;
}

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

  bool roundActivated = false;

  if(reinterpret_cast<weapComp_Round*>(thisComp)->round != nullptr)
  {
    roundActivated = reinterpret_cast<weapComp_Round*>(thisComp)->activateRound();
  }

  //thisComp->parent->weapComps[weaponSystem::BOLT]->readyComponent();

  if(roundActivated == true)
  {
    std::cout << "*Success" << std::endl;
  }

  return roundActivated;
}

////////////////////////////////////////
//==MAGIZINE==
  //cyclingInternally(TD_READY);
  //cyclingExternally(TD_ACTIVE);

//Cycling Internally:
bool logic_cyclingInternally(struct weaponComponent* thisComp)
{
  std::cout << "Cycling Internally" << std::endl;

  reinterpret_cast<weapComp_Mag*>(thisComp)->ReadyRound();

  std::cout << "*Success" << std::endl;

  return true;
}

//Cycling Externally:
bool logic_cyclingExternally(struct weaponComponent* thisComp)
{
  std::cout << "Cycling Externally" << std::endl;

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  bool returnVal = reinterpret_cast<weapComp_Round*>(thisComp)->RoundTransferTo(
    reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponSystem::BOLT]));

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  //TEMP: forcing success
  if (returnVal == false)
  {
    std::cout << "*FORCING Success; result was " << std::boolalpha << returnVal << std::endl;
    returnVal = true; //DESIRED_EFFECT
  }
  else
  {
    std::cout << "*Success" << std::endl;
  }

  return returnVal;
}

////////////////////////////////////////////////////////////////////////////////
//Rules for adding/removing components

weapComp_Mag* rulesForAdding_Mag()
{
  std::cout << "Applying rule for adding magizine" << std::endl;
  return gcc.reachableMag;
}

weapComp_Mag* rulesForRemoving_Mag()
{
  std::cout << "Applying rule for removing magizine" << std::endl;
  return nullptr;
}

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
  detactableMag->Reset();
  gcc.reachableMag               = detactableMag;

  FireModes firemodes;
  firemodes.firemodes.push_back(semi);
  firemodes.firemodes.push_back(burst3);
  firemodes.firemodes.push_back(full);

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
  hammer->CompToActivate(chamber, TD_EITHER);
  //hammer->CompToActivate(bolt, TD_ACTIVE);
  //bolt->CompToActivate(chamber, TD_EITHER);

  chamber->CompToReady(bolt, TD_ACTIVE);
  bolt->CompToReady(hammer, TD_EITHER);
  hammer->CompToReady(action, TD_ACTIVE);

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
  weaponStage reacting(TD_READY);
  weaponStage actioning(TD_ACTIVE);

  //Feedport: ready when unloaded
  weaponStage unloading(TD_READY);
  weaponStage loading(TD_ACTIVE);

    //Bolt:   ready when opened
  weaponStage opening(TD_EITHER);
  weaponStage feeding(TD_ACTIVE);
  weaponStage ejecting(TD_READY);
  weaponStage chambing(TD_ACTIVE); //was in Chamber //take round from BOLT
  weaponStage extracting(TD_READY);
  weaponStage closing(TD_ACTIVE);

   //Chamber: ready when emptied
  weaponStage emptying(TD_READY);
  weaponStage firing(TD_ACTIVE); //was in Round

  //Magizine: ready when round is ready for transfer
  weaponStage cyclingInternally(TD_EITHER);
  weaponStage cyclingExternally(TD_ACTIVE);

    //Round
  //weaponStage priming(TD_READY); //Round

//////////////////////////////
//Assign activationStage to weaponStage

    //Trigger
  releasing.activationStage  = logic_releasing;
  pulling.activationStage    = logic_pulling;

    //Hammer
  cocking.activationStage    = logic_cocking;
  engaging.activationStage   = logic_engaging;

    //Action
  reacting.activationStage   = logic_reacting;
  actioning.activationStage  = logic_actioning;

    //Feedport
  unloading.activationStage  = logic_unloading;
  loading.activationStage    = logic_loading;

    //Bolt
  opening.activationStage    = logic_opening;
  feeding.activationStage    = logic_feeding;
  ejecting.activationStage   = logic_ejecting;
  chambing.activationStage   = logic_chambing; //was in Chamber
  extracting.activationStage = logic_extracting;
  closing.activationStage    = logic_closing;

    //Chamber
  emptying.activationStage   = logic_emptying;
  firing.activationStage     = logic_firing; //was in Round

    //Magizine
  cyclingInternally.activationStage   = logic_cyclingInternally;
  cyclingExternally.activationStage   = logic_cyclingExternally;

    //Round
  //priming.activationStage    = logic_priming;

//////////////////////////////
//Assign weaponStage to weaponComponent

    //Trigger
  trigger->weaponStages.push_back(pulling);
  trigger->weaponStages.push_back(releasing);

    //Hammer
  hammer->weaponStages.push_back(cocking);
  hammer->weaponStages.push_back(engaging);

    //Action
  action->weaponStages.push_back(reacting);
  action->weaponStages.push_back(actioning);

    //Feedport
  feedPort->weaponStages.push_back(unloading);
  feedPort->weaponStages.push_back(loading);

    //Bolt
  bolt->weaponStages.push_back(opening);
  bolt->weaponStages.push_back(feeding);
  bolt->weaponStages.push_back(ejecting);
  bolt->weaponStages.push_back(chambing); //was in Chamber
  bolt->weaponStages.push_back(extracting);
  bolt->weaponStages.push_back(closing);

    //Chamber
  chamber->weaponStages.push_back(emptying);
  chamber->weaponStages.push_back(firing); //was in Round

    //Magizine
  detactableMag->weaponStages.push_back(cyclingInternally);
  detactableMag->weaponStages.push_back(cyclingExternally);

    //Round

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

  feedPort->object               = reinterpret_cast<weapComp_Mag**>(&rifleAuto.weapComps[weaponSystem::MAGIZINE]);
  (*feedPort->object)            = nullptr;
  feedPort->cc                   = gcc;
  feedPort->rulesForAdding       = rulesForAdding_Mag;
  feedPort->rulesForRemoving     = rulesForRemoving_Mag;
  //feedPort->cc.reachableMag      = gcc.reachableMag;

  weaponSystem &curWeapon = rifleAuto;

  std::cout << "\nDEBUGGING ADDRESSES" << std::endl;

  std::cout
  << std::left
  << std::setw(50)
  << "&rifleAuto->weapComps[weaponSystem::MAGIZINE]: "
  << &rifleAuto.weapComps[weaponSystem::MAGIZINE]
  << std::endl;

  std::cout
  << std::left
  << std::setw(50)
  << "&curWeapon->weapComps[weaponSystem::MAGIZINE]: "
  << &curWeapon.weapComps[weaponSystem::MAGIZINE]
  << std::endl;

  std::cout
  << std::left
  << std::setw(50)
  << "feedPort->object : "
  << feedPort->object 
  << std::endl;

//Debug
{
  std::cout << "\n**DEBUG START**" << std::endl;
  std::cout << "gcc.reachableMag name:           " << gcc.reachableMag->name << std::endl;
  std::cout << "detactableMag name:              " << detactableMag->name << std::endl;

  std::cout << "FEEDPORT->object->name:          ";
  if((*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->object) != nullptr)
  std::cout << (*reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->object)->name << std::endl;
  else
  std::cout << "N/A" << std::endl;

  std::cout << "FEEDPORT->cc.reachableMag->name: ";
  if(reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->cc.reachableMag != nullptr)
  std::cout << reinterpret_cast<weapComp_Port<weapComp_Mag>*>(curWeapon.weapComps[weaponSystem::FEEDPORT])->cc.reachableMag->name << std::endl;
  else
  std::cout << "N/A" << std::endl;
  std::cout << "***DEBUG END***" << std::endl;
}

  std::string input;

  while(input != "q")
  {
    input.clear();
    std::cout << "\nENTER COMMAND: ";
    std::cin >> input;

#if 1
    if(input == "t") //Check Mag
    {
      std::cout << "INPUT Ammo check:" << std::endl;

      curWeapon.debug();

      if(curWeapon.weapComps[weaponSystem::MAGIZINE] == nullptr)//->(*object) == nullptr)
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
          curWeapon.weapComps[weaponSystem::FEEDPORT]->readyComponent(/* false */);
        }
      }

      if(shouldReload == true)
      {
          //Setting Bolt Open
        if(curWeapon.weapComps[weaponSystem::BOLT]->IsReady() == false)
        {
          curWeapon.weapComps[weaponSystem::BOLT]->readyComponent(/* false */);
        }

        //curWeapon.debug(); //DEBUG

        std::cout << "*Inserting Magizine" << std::endl;
        detactableMag->Reset();
        curWeapon.weapComps[weaponSystem::FEEDPORT]->activateComponent(/* false */);
        curWeapon.weapComps[weaponSystem::BOLT]->activateComponent(/* false */);

        //curWeapon.debug(); //DEBUG
      }
      else
      {
        std::cout << "*Reloading Cancelled" << std::endl;
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
    else if(input == "e") //FireModes
    {
      std::cout << "INPUT FireModes:" << std::endl;

      reinterpret_cast<weapComp_Action*>(curWeapon.weapComps[weaponSystem::ACTION])->firemode = firemodes.cycleFiremodes();
    }
    else if(input == "b") //Open Bolt
    {
      std::cout << "INPUT Open Bolt:" << std::endl;

      curWeapon.weapComps[weaponSystem::BOLT]->readyComponent(/* false */);
    }
    else if(input == "n") //Close Bolt
    {
      std::cout << "INPUT Close Bolt:" << std::endl;

      curWeapon.weapComps[weaponSystem::BOLT]->activateComponent(/* false */);
    }
#endif
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
