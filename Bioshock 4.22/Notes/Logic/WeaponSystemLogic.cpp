//Author: Danny Jundi

////////////////////////////////////////////////////////////////////////////////
//INCLUDES

#include <vector>
#include <string>
#include <iostream>
#include <iomanip> //DEBUG
#include <queue>

////////////////////////////////////////////////////////////////////////////////
//STATIC DATA

static const unsigned defaultRoundCount = 10; //TEMP

//RENAME: TriggerDestination
enum triggerDirection
{
  TD_ACTIVE,
  TD_READY,
  TD_EITHER,
  //TD_NOT_ACTIVE,
  //TD_NOT_READY,
  TD_COUNT //NEEDS TO REMAIN LAST
};

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES

struct weaponStage
{
  weaponStage(triggerDirection trigDirection_)
  : trigDirection(trigDirection_)
  {}

  //std::string name;         //UNUSED
  //float triggerPos;         //SUSPENDED //value from 0.0f to 1.0f
  //float rateOfChangeFactor; //SUSPENDED //rate at which to APPROACH this stage
  triggerDirection trigDirection;
  bool (*activationStage)(struct weaponComponent* thisComp); //function pointer
  unsigned rateOfFire;

    // changeInPos / time(seconds)
  //void rateOfPosChangeFixed(float);    //SUSPENDED

    // seconds to get to newPos; (newPos - curPos) / time[in seconds]
  //void rateOfPosChangeRelative(float); //SUSPENDED
};

//can be affected externally from system or by other weapon components
struct weaponComponent
{
  //TODO: add debug() so all weaponComponents can print
  //      debug info relative to debugging their use cases

  enum WEAP_COMPS_ENUMS
  {
    TRIGGER,  //0
    HAMMER,   //1
    ACTION,   //2
    FEEDPORT, //3
    BOLT,     //4
    CHAMBER,  //5
    BARREL,   //6
    MUZZLE,   //7
    MAGIZINE, //8
    COUNT     //COUNT
  };

  weaponComponent(std::string name_, WEAP_COMPS_ENUMS wcType_)
  : name(name_), wcType(wcType_)
  {}

  friend struct weaponSystem;
  struct weaponSystem* parent = nullptr;
  std::string name;
  //float curPos;                  //SUSPENDED
  //float curRateOfChange;         //SUSPENDED
  //void activateComponent(float); //SUSPENDED //value to set for curPos

  std::vector<weaponStage> weaponStages; //sorted array using 'triggerPos' as key
  unsigned curStage = 0;
  WEAP_COMPS_ENUMS wcType;

  weaponComponent* compToActivate      = nullptr;
  triggerDirection conditionToActivate = TD_EITHER;

  weaponComponent* compToReady      = nullptr;
  triggerDirection conditionToReady = TD_EITHER;

  //param compToActivate_:  component to activate
  //param trigDirCondition: state component needs to be in for use
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
    return curStage == (weaponStages.size() + 1);
  }

  virtual void debug()
  {
    std::cout
    << "Name: "
    << std::left << std::setw(8)
    << name
    << " \\\\ Status: ";

    if(IsReady())
    {
      std::cout << "READY" << std::endl;
    }
    else if(IsActive())
    {
      std::cout << "ACTIVE" << std::endl;
    }
    else
    {
      std::cout << "SOMEWHERE IN MIDDLE" << std::endl;
    }
  }

private:
  bool activateComponent()
  {
    std::cout << "Activating " << name << ":" << std::endl;

    if(curStage == 0)
    {
      ++curStage;
    }

    while(curStage < weaponStages.size() + 1)
    {
      if(
         weaponStages[curStage - 1].trigDirection == TD_ACTIVE
         || weaponStages[curStage - 1].trigDirection == TD_EITHER
        )
      {
        if(weaponStages[curStage - 1].activationStage(this) == false)
        {
          return false;
        }
      }

      ++curStage;
    }

    return true;
  }

  bool readyComponent()
  {
    std::cout << "Readying " << name << ":" << std::endl;

    if(curStage == weaponStages.size() + 1)
    {
      --curStage;
    }

    while(curStage > 0)
    {
      if(
         weaponStages[curStage - 1].trigDirection == TD_READY
         || weaponStages[curStage - 1].trigDirection == TD_EITHER
        )
      {
        if(weaponStages[curStage - 1].activationStage(this) == false)
        {
          return false;
        }
      }

      --curStage;
    }

    return true;
  }
};

struct weaponSystem
{
  weaponSystem(std::string name_) : name(name_)
  {}

  std::string name;

  template <typename T>
  void modComp(T*& weapComp, T* (*rules)())
  {
    weapComp = rules();

    if(weapComp != nullptr)
    {
      weapComp->parent = this;
    }
  }

//private:
  std::vector<weaponComponent*> weapComps;
  typedef std::pair<triggerDirection,
          weaponComponent::WEAP_COMPS_ENUMS> setCompInfo;
  std::queue<setCompInfo> setCompQueue;

public:
  void addWeapComp(weaponComponent* weapComp)
  {
    weapComps.push_back(weapComp);

    if(weapComp != nullptr)
    {
      weapComp->parent = this;
    }
  }

  //TODO: removeWeapComp

  void setComponent(triggerDirection condition,
                    weaponComponent::WEAP_COMPS_ENUMS key)
  {
    if(setCompQueue.empty() == false)
    {
      setCompQueue.push(std::make_pair(condition, key));
      return;
    }

    setCompQueue.push(std::make_pair(condition, key));

    while(setCompQueue.empty() == false)
    {
      setCompInfo curComp = setCompQueue.front();

      if(curComp.first == TD_READY)
      {
        readyComponent(curComp.second);
      }
      else if(curComp.first == TD_ACTIVE)
      {
        activateComponent(curComp.second);
      }

      setCompQueue.pop();
    }
  }

  bool IsActive(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    if(weapComps[key] != nullptr)
    {
      return weapComps[key]->IsActive();
    }

    return false;
  }

  bool IsReady(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    if(weapComps[key] != nullptr)
    {
      return weapComps[key]->IsReady();
    }

    return false;
  }

  void debug();

//private: //TEMP: DISABLED FOR DEVELOPMENT

  //TODO: Give weapComp default weaponStages of Ready & Active
  //      These stages won't have user defined logic (i.e. script),
  //      but rather set logic to call a defined weapComp
  //      or weaponStage (set to either ready or active).
  //      Can still be set to nothing (e.g. compToActivate = nullptr)
  bool activateComponent(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    weaponComponent*& thisComp = weapComps[key];

    if(thisComp != nullptr)
    {
      if(thisComp->activateComponent())
      {
        weaponComponent*& nextComp = weapComps[key]->compToActivate;

        std::cout
        << "*SUCCESS: "
        << thisComp->name
        << " ACTIVATED"
        << std::endl;

        if(nextComp != nullptr)
        {
          std::cout 
          << "**" << thisComp->name
          << " is set to activate "
          << nextComp->name
          << std::endl;

          if(
             nextComp->conditionToActivate == TD_EITHER
             || (nextComp->conditionToActivate == TD_READY
                  && nextComp->IsReady())
             || (nextComp->conditionToActivate == TD_ACTIVE
                  && nextComp->IsActive())
            )
          {
            setComponent(TD_ACTIVE, nextComp->wcType);
          }
          else
          {
            std::cout
            << nextComp->name
            << " did not satisfy condition to be activated"
            << std::endl;

            std::cout << "Condition needs to be: ";

            switch(nextComp->conditionToActivate)
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
          //std::cout
          //<< thisComp->name
          //<< " has no component to activate"
          //<< std::endl;
        }

        return true;
      }
      else
      {
        std::cout
        << "*NOTICE:  "
        << thisComp->name
        << " HAS NOT ACTIVATED"
        << std::endl;
      }
    }

    return false;
  }

  void readyComponent(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    weaponComponent*& thisComp = weapComps[key];

    if(thisComp != nullptr)
    {
      if(thisComp->readyComponent())
      {
        weaponComponent*& nextComp = weapComps[key]->compToReady;

        std::cout
        << "*SUCCESS: "
        << thisComp->name
        << " READIED"
        << std::endl;

        if(nextComp != nullptr)
        {
          std::cout 
          << "**" << thisComp->name
          << " is set to ready "
          << nextComp->name
          << std::endl;

          if(
             nextComp->conditionToReady == TD_EITHER
             || (nextComp->conditionToReady == TD_READY
                  && nextComp->IsReady())
             || (nextComp->conditionToReady == TD_ACTIVE
                  && nextComp->IsActive())
            )
          {
            setComponent(TD_READY, nextComp->wcType);
          }
          else
          {
            std::cout
            << nextComp->name
            << " did not satisfy condition to be readied"
            << std::endl;

            std::cout << "Condition needs to be: ";

            switch(nextComp->conditionToReady)
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
          //std::cout
          //<< thisComp->name
          //<< " has no component to ready"
          //<< std::endl;
        }
      }
      else
      {
        std::cout
        << "*NOTICE:  "
        << thisComp->name
        << " HAS NOT READIED"
        << std::endl;
      }
    }
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
  unsigned roundsPerPull; //0 implies full auto //infinity in a place of zero
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
  weapComp_Action(std::string name_, WEAP_COMPS_ENUMS wcType_)
  : weaponComponent(name_, wcType_)
  {}

  FireMode firemode = semi;

  unsigned remainingRoundsPerPull; //once reached 0, end sequence

  //void activateComponent(bool activateNextComp = true) override;
};

struct ws_projectile
{
  //BLANK
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

template <typename T>
struct colliderConditional
{
  T* reachableMag = nullptr;
};

//Weapon Component that can contain a ws_round(s)
struct weapComp_Round : public weaponComponent
{
  weapComp_Round(std::string name_, WEAP_COMPS_ENUMS wcType_)
  : weaponComponent(name_, wcType_)
  {}

  ws_round* round = nullptr;

  bool RoundTransferFrom(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    return RoundTransferFrom(
      reinterpret_cast<weapComp_Round*>(parent->weapComps[key]));
  }

  bool RoundTransferTo(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    return RoundTransferTo(
      reinterpret_cast<weapComp_Round*>(parent->weapComps[key]));
  }

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

    return false; //TEMP
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

    return false; //TEMP
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

  void debug()
  {
    weaponComponent::debug();

    //std::cout << "*DEBUG START weapComp_Round*" << std::endl;

    if(round == nullptr)
    {
      std::cout << "Round NOT in: " << name << std::endl;
    }
    else
    {
      std::cout << "Round IS in:  " << name << std::endl;
    }

    //std::cout << "**DEBUG END weapComp_Round**" << std::endl;
  }
};

struct weapComp_Mag : public weapComp_Round
{
  weapComp_Mag(std::string name_, WEAP_COMPS_ENUMS wcType_)
  : weapComp_Round(name_, wcType_)
  {
    ReadyRound();
  }

  //ws_round* roundsContained = new ws_round;
  unsigned roundCount = defaultRoundCount;

  void Reset()
  {
    std::cout << "Resetting: " << name << std::endl;

    //?
    roundCount = defaultRoundCount - 1;
    //parent->setComponent(TD_READY, wcType);
    //ReadyRound();
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
        round = new ws_round;
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

  void debug()
  {
    weapComp_Round::debug();

    unsigned roundCountTotal = roundCount;

    if(round == nullptr)
    {
      ++roundCountTotal;
    }

    if(roundCountTotal == 0)
    {
      std::cout << "*Magizine EMPTY" << std::endl;
    }
    else if(roundCountTotal > 0)
    {
      std::cout
      << "*"
      << roundCountTotal
      << " Rounds in Magizine out of: "
      << defaultRoundCount /*maxRoundCount*/
      << std::endl;
    }      

    if(roundCountTotal == defaultRoundCount /*maxRoundCount*/)
    {
      std::cout << "*Magizine FULL" << std::endl;
    }
  }
};

static colliderConditional<weapComp_Mag> gcc;

//Responsible for making REQUESTS to the system to add & remove set weaponComponents
  //T is a weapComp_Mag
  //^In the specific use case in which this prototype is being developed
template <typename T>
struct weapComp_Port : public weaponComponent
{
  weapComp_Port(std::string name_, WEAP_COMPS_ENUMS wcType_)
  : weaponComponent(name_, wcType_)
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

  void debug()
  {
    weaponComponent::debug();

    //std::cout << "*DEBUG START weapComp_Port*" << std::endl;

    if(object != nullptr && (*object) != nullptr)
    {
      std::cout << "*Object is attached" << std::endl;

      (*object)->debug();
    }
    else
    {
      std::cout << "*No object is attached" << std::endl;
    }

    //std::cout << "**DEBUG END weapComp_Port**" << std::endl;
  }
};

//TODO: Should call debug of all weaponComps
void weaponSystem::debug()
{
  std::cout << "**DEBUG START: weaponSystem**" << std::endl;

  for(unsigned i = 0; i < weaponComponent::COUNT; ++i)
  {
    if(weapComps[i] != nullptr)
    {
      //std::cout << "**DEBUG START: WeapComp**" << std::endl;

      weapComps[i]->debug();

      //std::cout << "***DEBUG END: WeapComp***" << std::endl;
    }
    else
    {
      //TODO: Replace 'i' with a enumToString translation
      std::cout << "*This WeapSys is missing WeapComp type of: " << i << std::endl;
    }
  }

  std::cout << "***DEBUG END: weaponSystem***" << std::endl;
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
  //reacting(TD_EITHER);
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

  if(reinterpret_cast<weapComp_Action*>(
      thisComp->parent->weapComps[weaponComponent::ACTION])->firemode.resetOnTriggerRelease)
  {
    reinterpret_cast<weapComp_Action*>(
      thisComp->parent->weapComps[weaponComponent::ACTION])->remainingRoundsPerPull
      = reinterpret_cast<weapComp_Action*>(
          thisComp->parent->weapComps[weaponComponent::ACTION])->firemode.roundsPerPull;
  }

  return true;
}

//Releasing
bool logic_releasing(struct weaponComponent* /* thisComp */)
{
  std::cout << "Releasing" << std::endl;

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

  return true;
}

//*Engaging
//  -engage hammer (animation + sound)
bool logic_engaging(struct weaponComponent* thisComp)
{
  std::cout << "Engaging" << std::endl;

  if(thisComp->parent->weapComps[weaponComponent::BOLT]->IsActive() == false)
  {
    std::cout << "*Bolt is not activated" << std::endl;
    return false;
  }

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

  thisAction->parent->setComponent(TD_ACTIVE, weaponComponent::BOLT);

  //TODO: Add logic for proposition thisAction->firemode.triggerCheck

  if(thisAction->remainingRoundsPerPull > 0
     || thisAction->firemode.roundsPerPull == 0) //i.e. is full auto
  {
    thisComp->parent->setComponent(TD_ACTIVE, thisComp->wcType);
    std::cout << "*Re-Actioning" << std::endl;
    return false;
  }
  else
  {
      //TODO: can set as CompToReady
    //thisAction->parent->setComponent(TD_READY, weaponComponent::TRIGGER);
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
      thisComp->parent->setComponent(TD_READY, thisComp->wcType);

      std::cout << "*No more rounds remaining this pull" << std::endl;
      return false;
    }
  }
  else
  {
    --thisAction->remainingRoundsPerPull;
  }

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

  return true;
}

//Loading:
bool logic_loading(struct weaponComponent* thisComp)
{
  std::cout << "Loading" << std::endl;

  //NOTE: How to take magizine (a weapon component)
  //external to the system to within the system?
    //ANSWER: There may be logic required at the weaponSystem level
    //for adding & removing weaponComponents to & from the system 

  reinterpret_cast<weapComp_Port<weapComp_Mag>*>(thisComp)->addComp();

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

  thisComp->parent->setComponent(TD_READY, weaponComponent::MAGIZINE);

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
*/

  std::cout << "Feeding" << std::endl;

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  bool returnVal = thisComp->parent->activateComponent(weaponComponent::MAGIZINE);

  //TEMP: forcing success
  if (returnVal == false)
  {
    std::cout
    << "*FORCING Success; result was "
    << std::boolalpha
    << returnVal
    << std::endl;
    returnVal = true; //DESIRED_EFFECT
  }

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  return returnVal;
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
*/

  std::cout << "Ejecting" << std::endl;

  if(
     reinterpret_cast<weapComp_Round*>(
       thisComp->parent->weapComps[weaponComponent::BOLT])->round 
     != nullptr
    )
  {
    delete reinterpret_cast<weapComp_Round*>(
      thisComp->parent->weapComps[weaponComponent::BOLT])->round;
    reinterpret_cast<weapComp_Round*>(
      thisComp->parent->weapComps[weaponComponent::BOLT])->round = nullptr;
  }

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
*/

  std::cout << "Chambing" << std::endl;

  bool returnVal = reinterpret_cast<weapComp_Round*>(thisComp)->RoundTransferTo(
    reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponComponent::CHAMBER]));

  //TEMP: forcing success
  if (returnVal == false)
  {
    std::cout << "*FORCING Success; result was " << std::boolalpha << returnVal << std::endl;
    returnVal = true; //DESIRED_EFFECT
  }

  return returnVal;
}

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

  thisComp->parent->readyComponent(weaponComponent::CHAMBER/* false */);
  //thisComp->parent->setComponent(TD_READY, weaponComponent::CHAMBER);

  return true;
}

//Closing
bool logic_closing(struct weaponComponent* /* thisComp */)
{
  std::cout << "Closing" << std::endl;

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
    reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponComponent::BOLT]));

  //TEMP: forcing success
  if (returnVal == false)
  {
    std::cout << "*FORCING Success; result was " << std::boolalpha << returnVal << std::endl;
    returnVal = true; //DESIRED_EFFECT
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
*/

  std::cout << "Firing" << std::endl;

  bool roundActivated = false;

  if(reinterpret_cast<weapComp_Round*>(thisComp)->round != nullptr)
  {
    roundActivated = reinterpret_cast<weapComp_Round*>(thisComp)->activateRound();

    //thisComp->parent->weapComps[weaponSystem::BOLT]->readyComponent();
    thisComp->parent->setComponent(TD_READY, weaponComponent::BOLT);
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

  return true;
}

//Cycling Externally:
bool logic_cyclingExternally(struct weaponComponent* thisComp)
{
  std::cout << "Cycling Externally" << std::endl;

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  bool returnVal =
    reinterpret_cast<weapComp_Round*>(thisComp)
      ->RoundTransferTo(weaponComponent::BOLT);

  //reinterpret_cast<weapComp_Round*>(thisComp)->debug(); //DEBUG

  //TEMP: forcing success
  if (returnVal == false)
  {
    std::cout
    << "*FORCING Success; result was "
    << std::boolalpha
    << returnVal
    << std::endl;

    returnVal = true; //DESIRED_EFFECT
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
  weaponComponent *trigger    = new weaponComponent("Trigger", weaponComponent::TRIGGER);
  weaponComponent *hammer     = new weaponComponent("Hammer", weaponComponent::HAMMER);
  weapComp_Action *action     = new weapComp_Action("Action", weaponComponent::ACTION);

  //Firing
  weapComp_Port<weapComp_Mag> *feedPort
    = new weapComp_Port<weapComp_Mag>("FeedPort", weaponComponent::FEEDPORT);
  weapComp_Round  *bolt       = new weapComp_Round("Bolt", weaponComponent::BOLT);
  weapComp_Round  *chamber    = new weapComp_Round("Chamber", weaponComponent::CHAMBER);
  //weaponComponent *ejectPort  = new weaponComponent("EjectPort"); //UNUSED

  //Post-fire
  weaponComponent *barrel     = new weaponComponent("Barrel", weaponComponent::BARREL);
  weaponComponent *muzzle     = new weaponComponent("Muzzle", weaponComponent::MUZZLE);

  //External Components
  weapComp_Mag* detactableMag = new weapComp_Mag("Magizine", weaponComponent::MAGIZINE);
  //ws_round* basicRounds       = new ws_round;

  //detactableMag->roundsContained = basicRounds;
  //detactableMag->Reset();
  gcc.reachableMag               = detactableMag;

  FireModes firemodes;
  firemodes.firemodes.push_back(semi);
  firemodes.firemodes.push_back(burst3);
  firemodes.firemodes.push_back(full);

//////////////////////////////
//Initializing Weapon Comps

std::cout << "\n**Initializing Weapons: START**" << std::endl;

#if 0

  //TODO: REVISE
  {
    std::cout << "Weapon: Opened-bolt Rifle" << std::endl;

      //Opened-bolt Rifle
    trigger->CompToActivate(action, TD_READY);
    action->CompToActivate(bolt, TD_READY);
    //***No hammer
    bolt->CompToActivate(chamber, TD_EITHER);

    chamber->CompToReady(bolt, TD_ACTIVE);
    bolt->CompToReady(action, TD_EITHER);
  }

#elif 1

  {
    std::cout << "Weapon: Closed-bolt Rifle" << std::endl;

      //Closed-bolt Rifle
    trigger->CompToActivate(action, TD_READY);
    action->CompToActivate(hammer, TD_READY);
    hammer->CompToActivate(chamber, TD_EITHER);
    //hammer->CompToActivate(bolt, TD_ACTIVE);
    //bolt->CompToActivate(chamber, TD_EITHER);

    //chamber->CompToReady(bolt, TD_ACTIVE);
    bolt->CompToReady(hammer, TD_EITHER);
    hammer->CompToReady(action, TD_ACTIVE);
    action->CompToReady(trigger, TD_EITHER);
  }

#else

  //TODO: REVISE
  {
    std::cout << "Weapon: Semi-auto Pistol" << std::endl;

      //Semi-auto Pistol; bolt is slide
    trigger->CompToActivate(hammer, TD_EITHER);
    hammer->CompToActivate(bolt, TD_ACTIVE);

    chamber->CompToReady(bolt, TD_ACTIVE);
    bolt->CompToReady(hammer, TD_EITHER);
  }

#endif

std::cout << "***Initializing Weapons: END***" << std::endl;

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
  weaponStage opening(TD_READY); //(TD_EITHER);
  weaponStage feeding(TD_ACTIVE);
  weaponStage ejecting(TD_READY);
  weaponStage chambing(TD_ACTIVE); //was in Chamber //take round from BOLT
  weaponStage extracting(TD_READY);
  weaponStage closing(TD_ACTIVE);

   //Chamber: ready when emptied
  weaponStage emptying(TD_READY);
  weaponStage firing(TD_ACTIVE); //was in Round

  //Magizine: ready when round is ready for transfer
  weaponStage cyclingInternally(TD_READY);//(TD_EITHER);
  weaponStage cyclingExternally(TD_ACTIVE);

    //TODO:   Round
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
  cyclingInternally.activationStage = logic_cyclingInternally;
  cyclingExternally.activationStage = logic_cyclingExternally;

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

    //TODO: Round

//////////////////////////////
//Weapon Systems

  weaponSystem rifleAuto("Rifle Auto");

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

  feedPort->object =
    reinterpret_cast<weapComp_Mag**>(
      &rifleAuto.weapComps[weaponComponent::MAGIZINE]
    );
  (*feedPort->object)            = nullptr;
  feedPort->cc                   = gcc;
  feedPort->rulesForAdding       = rulesForAdding_Mag;
  feedPort->rulesForRemoving     = rulesForRemoving_Mag;
  //feedPort->cc.reachableMag      = gcc.reachableMag;

  weaponSystem &curWeapon = rifleAuto;

  std::string input;

  while(input != "q")
  {
    input.clear();
    std::cout << "\nENTER COMMAND: ";
    std::cin >> input;

    if(input == "t") //Check Mag
    {
      std::cout << "STATUS[Ammo check]" << std::endl;

      //TODO: Move following logic to their respective debug()

      if(curWeapon.weapComps[weaponComponent::MAGIZINE] == nullptr)
      {
        std::cout << "*No Magizine" << std::endl;
      }
      else
      {
        curWeapon.weapComps[weaponComponent::MAGIZINE]->debug();
      }
    }
    else if(input == "r") //Reload
    {
      std::cout << "ACTION[Reloading]" << std::endl;

      bool shouldReload = true;

        //If weapon already has magizine
      if(curWeapon.weapComps[weaponComponent::MAGIZINE])
      {
        if(reinterpret_cast<weapComp_Mag*>(
             curWeapon.weapComps[weaponComponent::MAGIZINE])->roundCount
           == defaultRoundCount)
        {
          shouldReload = false;
        }
        else
        {
          std::cout << "*Releasing Magizine" << std::endl;
          curWeapon.setComponent(TD_READY, weaponComponent::FEEDPORT);
        }
      }

      if(shouldReload == true)
      {
          //Setting Bolt Open
        if(curWeapon.IsReady(weaponComponent::BOLT) == false)
        {
          curWeapon.setComponent(TD_READY, weaponComponent::BOLT);
        }

        std::cout << "*Beginning process to inserting Magizine" << std::endl;
        detactableMag->Reset();
        curWeapon.setComponent(TD_ACTIVE, weaponComponent::FEEDPORT);
        curWeapon.setComponent(TD_ACTIVE, weaponComponent::BOLT);
      }
      else
      {
        std::cout << "*Reloading Cancelled" << std::endl;
      }
    }
    else if(input == "f") //Fire Weapon
    {
      std::cout << "ACTION[Firing]" << std::endl;

      curWeapon.setComponent(TD_ACTIVE, weaponComponent::TRIGGER);
      curWeapon.setComponent(TD_READY, weaponComponent::TRIGGER);
    }
    else if(input == "e") //FireModes
    {
      std::cout << "ACTION[Cycle FireModes]" << std::endl;

      //WIP
      //reinterpret_cast<weapComp_Action*>(
      //  curWeapon.weapComps[weaponComponent::ACTION])->firemode
      //= firemodes.cycleFiremodes();
    }
    else if(input == "b") //Open Bolt
    {
      std::cout << "ACTION[Open Bolt]" << std::endl;

      curWeapon.setComponent(TD_READY, weaponComponent::BOLT);
    }
    else if(input == "n") //Close Bolt
    {
      std::cout << "ACTION[Close Bolt]" << std::endl;

      curWeapon.setComponent(TD_ACTIVE, weaponComponent::BOLT);
    }
    else if(input == "d") //debug of firearm
    {
      std::cout
      << "STATUS[Debug of WeaponSystem: "
      << std::right << std::setw(10)
      << curWeapon.name << "]"
      << std::endl;
      curWeapon.debug();
    }
    else if(input == "s") //status of firearm
    {
      std::cout
      << "STATUS[Weapon Component info of WeaponSystem: "
      << std::right << std::setw(10)
      << curWeapon.name << "]"
      << std::endl;

      for(unsigned i = 0; i < weaponComponent::COUNT; ++i)
      {
        if(curWeapon.weapComps[i] != nullptr)
        {
          curWeapon.weapComps[i]->weaponComponent::debug();
        }
        else
        {
          //TODO: Replace 'i' with a enumToString translation
          std::cout << "*This WeapSys is missing WeapComp type of: " << i << std::endl;
        }
      }
    }
  }

  std::cout << "QUIT" << std::endl;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
