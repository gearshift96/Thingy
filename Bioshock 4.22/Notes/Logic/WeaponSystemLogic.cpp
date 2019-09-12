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

//Defines the behavior of the weaponComponent it is inserted into
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
  //unsigned rateOfFire;

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

  friend std::ostream& operator<<(std::ostream& os, WEAP_COMPS_ENUMS key)
  {
    switch(key)
    {
      case(weaponComponent::TRIGGER):
        os << "TRIGGER";
        break;
      case(weaponComponent::HAMMER):
        os << "HAMMER";
        break;
      case(weaponComponent::ACTION):
        os << "ACTION";
        break;
      case(weaponComponent::FEEDPORT):
        os << "FEEDPORT";
        break;
      case(weaponComponent::BOLT):
        os << "BOLT";
        break;
      case(weaponComponent::CHAMBER):
        os << "CHAMBER";
        break;
      case(weaponComponent::BARREL):
        os << "BARREL";
        break;
      case(weaponComponent::MUZZLE):
        os << "MUZZLE";
        break;
      case(weaponComponent::MAGIZINE):
        os << "MAGIZINE";
        break;
      default:
        os << "UNKNOWN";
        break;
    }

    return os;
  }

  weaponComponent(std::string name_, WEAP_COMPS_ENUMS wcType_)
  : name(name_), wcType(wcType_)
  {}

  virtual ~weaponComponent(){}

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
      unsigned curStageMod = curStage - 1;

      if(
         weaponStages[curStageMod].trigDirection == TD_ACTIVE
         || weaponStages[curStageMod].trigDirection == TD_EITHER
        )
      {
        if(weaponStages[curStageMod].activationStage(this) == false)
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
      unsigned curStageMod = curStage - 1;

      if(
         weaponStages[curStageMod].trigDirection == TD_READY
         || weaponStages[curStageMod].trigDirection == TD_EITHER
        )
      {
        if(weaponStages[curStageMod].activationStage(this) == false)
        {
          return false;
        }
      }

      --curStage;
    }

    return true;
  }
};

struct modCompCondtional
{
  modCompCondtional(weaponComponent::WEAP_COMPS_ENUMS weapCompAttacheType_)
  : weapCompAttacheType(weapCompAttacheType_)
  {}

  //modCompCondtional(const modCompCondtional & other)
  //: weapCompAttacheType(other.weapCompAttacheType)
  //{}

  const weaponComponent::WEAP_COMPS_ENUMS weapCompAttacheType;
  virtual weaponComponent* rulesForAdding() = 0;
  virtual bool rulesForRemoving() = 0;
};

//TODO: Reposition to end of DATA STRUCTURES
struct weaponSystem
{
  weaponSystem(std::string name_) : name(name_)
  {
    weapComps.reserve(weaponComponent::COUNT);
    weapComps.resize(weaponComponent::COUNT); //Needed for Microsoft compiler
  }

  std::string name;

  void addWeapComp(modCompCondtional *modCompCond)
  {
    weaponComponent* weapComp = modCompCond->rulesForAdding();

    if(weapComp != nullptr)
    {
      addWeapComp(weapComp);
    }
  }

  void removeWeapComp(modCompCondtional *modCompCond)
  {
    if(modCompCond->rulesForRemoving())
    {
      removeWeapComp(modCompCond->weapCompAttacheType);
    }
  }

  //DEPRECATED
#if 0
  void modComp(weaponComponent*& weapComp,
               weaponComponent* (*rules)())
  {
    weapComp = rules();

    if(weapComp != nullptr)
    {
      weapComp->parent = this;
    }
  }
#endif

  unsigned weaponRoundCount();

private:
  std::vector<weaponComponent*> weapComps;
  typedef std::pair<triggerDirection,
          weaponComponent::WEAP_COMPS_ENUMS> setCompInfo;
  std::queue<setCompInfo> setCompQueue;
public:
  unsigned roundCyclesFired = 0;

public:
  weaponComponent* getWeapComp(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    return weapComps[key];
  }

  void addWeapComp(weaponComponent* weapComp);

  void removeWeapComp(weaponComponent::WEAP_COMPS_ENUMS type)
  {
    //Free to World
    {
      //NOTE: For now that means deleting the object; no management of memory 
      delete weapComps[type];
    }
    weapComps[type] = nullptr;
  }

  void setComponent(triggerDirection condition,
                    weaponComponent::WEAP_COMPS_ENUMS key)
  {
    bool countCycle = false;

    {
      std::cout << "@Setting " << key << " to ";

      if(condition == TD_ACTIVE)
        std::cout << "ACTIVE" << std::endl;

      if(condition == TD_READY)
        std::cout << "READY" << std::endl;
    }

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
          //NOTE: Goal to distinguish between each completed cycle in auto fire
        if(countCycle && curComp.second == weaponComponent::ACTION)
        {
          std::cout << "Completed Round Cycle: "
          << ++roundCyclesFired << std::endl;
        }

        readyComponent(curComp.second);
      }
      else if(curComp.first == TD_ACTIVE)
      {
          //NOTE: Goal to distinguish between each completed cycle in auto fire
        if(curComp.second == weaponComponent::ACTION)
        {
          if(countCycle == false)
          {
            roundCyclesFired = 0;
            countCycle = true;
          }
          std::cout << "\nFiring Round Cycle: "
          << (roundCyclesFired + 1) << std::endl;
        }

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

  //RESEARCH: Give weapComp default weaponStages of Ready & Active
  //          These stages won't have user defined logic (i.e. script),
  //          but rather set logic to call a defined weapComp
  //          or weaponStage (set to either ready or active).
  //          Can still be set to nothing (e.g. compToActivate = nullptr)
//TODO: Combine logic of 'activateComponent'
//      and 'readyComponent' into single logic call
  bool activateComponent(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    weaponComponent* thisComp = weapComps[key];

    if(thisComp != nullptr)
    {
      if(thisComp->activateComponent())
      {
        weaponComponent* nextComp = weapComps[key]->compToActivate;

        std::cout
        << "***SUCCESS: "
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

  bool readyComponent(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    weaponComponent* thisComp = weapComps[key];

    if(thisComp != nullptr)
    {
      if(thisComp->readyComponent())
      {
        weaponComponent* nextComp = weapComps[key]->compToReady;

        std::cout
        << "***SUCCESS: "
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
            //DEBUG
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
        }
        else
        {
          //DEBUG
          {
            //std::cout
            //<< thisComp->name
            //<< " has no component to ready"
            //<< std::endl;
          }
        }

        return true;
      }
      else
      {
        //DEBUG
        {
          std::cout
          << "*NOTICE:  "
          << thisComp->name
          << " HAS NOT READIED"
          << std::endl;
        }
      }
    }

    return false;
  }
};

struct FireMode
{
  FireMode
  (
    std::string name_,
    unsigned roundsPerPull_,
    bool triggerCheck_,
    bool resetOnTriggerRelease_
  )
  : name(name_)
  , roundsPerPull(roundsPerPull_)
  , triggerCheck(triggerCheck_)
  , resetOnTriggerRelease(resetOnTriggerRelease_)

  {}

  std::string name;
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
    unsigned lastFireMode = curFiremode;

    ++curFiremode;

    if(curFiremode == firemodes.size())
    {
      curFiremode = 0;
    }

    FireMode firemodeToReturn = firemodes[curFiremode];

    std::cout << "*Changing fire modes: "
    << firemodes[lastFireMode].name
    << " -> "
    << firemodes[curFiremode].name
    << std::endl;

    return firemodeToReturn;
  }

  void debug()
  {
    std::cout << "*Current fire mode: "
    << firemodes[curFiremode].name << std::endl;
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

  virtual ~weapComp_Action(){}

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

//Weapon Component that can contain a ws_round(s)
struct weapComp_Round : public weaponComponent
{
  weapComp_Round(std::string name_, WEAP_COMPS_ENUMS wcType_)
  : weaponComponent(name_, wcType_)
  {}

  virtual ~weapComp_Round(){}

  //RENAME: DestroyRound
  void DestoryRound()
  {
    if(HasRound())
    {
      delete round;
      round = nullptr;
    }
  }

  bool HasRound()
  {
    return round != nullptr;
  }

  //TODO: Clean up logic for round transferring; refactor into common helper functions
  bool RoundTransferFrom(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    return
      RoundTransfer(
        reinterpret_cast<weapComp_Round*>(parent->getWeapComp(key)), this
      );
  }

  bool RoundTransferTo(weaponComponent::WEAP_COMPS_ENUMS key)
  {
    return
      RoundTransfer(
        this, reinterpret_cast<weapComp_Round*>(parent->getWeapComp(key))
      );
  }

  static bool RoundTransfer(weapComp_Round* srce, weapComp_Round* dest)
  {
//NOTE: This seperation of return logic allows for callers to create
//      defined behavior depending on whether a weapComp (srce & dest equally)
//      started with a round or not before making this function call 
    bool srceHasRound = srce->HasRound();
    bool destHasRound = dest->HasRound();

//**DEBUG START**
    {
      if(srceHasRound)
      {
        std::cout
        << std::left << std::setw(26)
        << "*Round at source: "
        << srce->name
        << std::endl;
      }
      else
      {
        std::cout
        << std::left << std::setw(26)
        << "*No round at source: "
        << srce->name
        << std::endl;
      }

      if(destHasRound)
      {
        std::cout
        << std::left << std::setw(26)
        << "*Round already at destination: "
        << dest->name
        << std::endl;
      }
      else
      {
        std::cout
        << std::left << std::setw(26)
        << "*No round at destination: "
        << dest->name
        << std::endl;
      }
    }
//***DEBUG END***

    if(srceHasRound)
    {
      if(destHasRound) //JAM
      {
        std::cout << "!Weapon Jam" << std::endl;

        return false;
      }
      else //if(!destHasRound) //TRANSFERRING
      {
        std::cout
        << std::left << std::setw(26)
        << "*Transferring round: "
        << srce->name
        << " -> "
        << dest->name
        << std::endl;

        //USE std::move
        dest->round = srce->round;
        srce->round = nullptr;

        return true;
      }
    }
    else //if(!srceHasRound)
    {
      if(destHasRound) //NOTHING, but round at source
      {
        return true;
      }
      else //if(!destHasRound) //NOTHING
      {
        return false;
      }
    }
  }

//DEPRECATED: Replaced by RoundTransfer
#if 0
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
      else //JAM
      {
        std::cout
        << std::left << std::setw(26)
        << "*Round already at destination: "
        << source->name
        << std::endl;
      }
    }
    else
    {
      //TODO: return HasRound();
//NOTE: This seperation of return logic allows for callers to create
//      defined behavior depending on whether a weapComp (srce & dest equally)
//      started with a round or not before making this function call 


      std::cout
      << std::left << std::setw(26)
      << "*No round at source: "
      << name
      << std::endl;
    }

    return false; //TEMP
  }

    //TODO: Should rewrite logic to combine
    //      RoundTransferFrom and RoundTransferTo?
  bool RoundTransferTo(weapComp_Round* dest)
  {
    if(round != nullptr)
    {
      if (dest == nullptr) //?
      {
        delete round;
        round = nullptr;
        return true;
      }

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
      else //JAM
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
      //TODO: return dest->HasRound();
//NOTE: This seperation of return logic allows for callers to create
//      defined behavior depending on whether a weapComp (srce & dest equally)
//      started with a round or not before making this function call 

      std::cout
      << std::left << std::setw(26)
      << "*No round at source"
      << name
      << std::endl;
    }

    return false; //TEMP
  }
#endif

  bool activateRound()
  {
    bool roundActivated = false;

    if(HasRound())
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

  virtual void debug() override
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

protected:
  ws_round* round = nullptr;
};

struct weapComp_Mag : public weapComp_Round
{
  weapComp_Mag(std::string name_, WEAP_COMPS_ENUMS wcType_, unsigned maxRoundCount_)
  : weapComp_Round(name_, wcType_), maxRoundCount(maxRoundCount_)
  {
  }

  ws_round roundToSet; //DEBUG

  unsigned magRoundCount()
  {
    unsigned roundCountTotal = roundCount;

    if(HasRound())
    {
      ++roundCountTotal;
    }

    return roundCountTotal;
  }

  unsigned magMaxCapacity()
  {
    return maxRoundCount;
  }

  bool IsFull()
  {
    return magRoundCount() == magMaxCapacity();
  }

  bool IsEmpty()
  {
    return magRoundCount() == 0;
  }

  void Reset()
  {
    std::cout << "Resetting: " << name << std::endl;

    roundCount = maxRoundCount;

    if(HasRound() == false)
    {
      SetRoundInternally();
    }
  }

  bool ReadyRound()
  {
    std::cout << "Readying Round" << std::endl;

    bool hasInternalRound = HasRound();
    bool hasReserveRounds = (roundCount > 0);

    if(hasInternalRound)
    {
      std::cout << "*Round already set to ready" << std::endl;

      if(hasReserveRounds)
      {
        std::cout << "*More Rounds Ready" << std::endl;
        return false;
      }
      else
      {
        std::cout << "*Last round in Mag" << std::endl;
        return true;
      }
    }
    else
    {
      if(hasReserveRounds)
      {
        std::cout << "*Setting round to ready" << std::endl;
        SetRoundInternally();
        return true;
      }
      else
      {
        std::cout << "*Magizine empty" << std::endl;
        return false;
      }
    }
  }

  void debug() override
  {
    weapComp_Round::debug();

    //TODO: Move logic in else to front of condition calls
    if(IsFull())
    {
      std::cout << "*Magizine FULL" << std::endl;
    }
    else if(IsEmpty())
    {
      std::cout << "*Magizine EMPTY" << std::endl;
    }
    else //Somewhere inbetween
    {
      std::cout
      << "*"
      << "Rounds in Magizine: "
      << magRoundCount()
      << " out of "
      << magMaxCapacity()
      << std::endl;
    }      
  }

private:
  void SetRoundInternally()
  {
    --roundCount;
    round = new ws_round;
  }

private:
  //std::vector<ws_round*> roundsContained; //SUSPENDED
  unsigned roundCount    = 0;
  unsigned maxRoundCount = 1;
};

//DEPRECATED
#if 0
struct colliderConditional
{
  colliderConditional()
  {
    reinterpret_cast<weapComp_Mag*>(reachableMag)->Reset();
  }

  weaponComponent* reachableMag
    = new weapComp_Mag("Magizine", weaponComponent::MAGIZINE, 10);
};

static colliderConditional gcc;
#endif

//Responsible for making REQUESTS to the system to add & remove set weaponComponents
  //T is a weapComp_Mag
  // ^In the specific use case in which this prototype is being developed
  //^DEPRECATED
struct weapComp_Port : public weaponComponent
{
  weapComp_Port(
    std::string name_,
    WEAP_COMPS_ENUMS wcType_,
    modCompCondtional *modCompCond_
  )
    : weaponComponent(name_, wcType_)
    , modCompCond(modCompCond_)
  {}

  //weapComp_Port(const weapComp_Port& other)
  //  : weaponComponent(other.name, other.wcType)
  //  , modCompCond(other.modCompCond)
  //{}

  virtual ~weapComp_Port(){}

  modCompCondtional *modCompCond;

    //MOVED: To modCompCondtional
  //const weaponComponent::WEAP_COMPS_ENUMS weapCompAttacheType;
  //weaponComponent **object = nullptr;
  //colliderConditional cc;

  //weaponComponent* (*rulesForAdding)()   = nullptr;
  //weaponComponent* (*rulesForRemoving)() = nullptr;

  void connectComp();

  void addComp()
  {
    parent->addWeapComp(modCompCond);
  }

  void removeComp()
  {
    parent->removeWeapComp(modCompCond);
  }

  bool IsObjectAttached()
  {
    return parent->getWeapComp(modCompCond->weapCompAttacheType) != nullptr;
  }

  bool IsObjectDisattached()
  {
    return parent->getWeapComp(modCompCond->weapCompAttacheType) == nullptr;
  }

  void debug() override
  {
    weaponComponent::debug();

    //std::cout << "*DEBUG START weapComp_Port*" << std::endl;

    if(parent->getWeapComp(modCompCond->weapCompAttacheType) != nullptr)
    {
      std::cout << "*Object is attached" << std::endl;

      parent->getWeapComp(modCompCond->weapCompAttacheType)->debug();
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

  for(
      weaponComponent::WEAP_COMPS_ENUMS i
        = static_cast<weaponComponent::WEAP_COMPS_ENUMS>(0);
      i < weaponComponent::COUNT;
      i = weaponComponent::WEAP_COMPS_ENUMS(static_cast<int>(i) + 1)
     )
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

unsigned weaponSystem::weaponRoundCount()
{
  unsigned roundCountTotal = 0;

  if (reinterpret_cast<weapComp_Mag*>(weapComps[weaponComponent::MAGIZINE]) == nullptr)
  {
    //std::cout << "*No Magizine" << std::endl;
  }
  else
  {
    //reinterpret_cast<weapComp_Mag*>(weapComps[weaponComponent::MAGIZINE])->debug();
    roundCountTotal += reinterpret_cast<weapComp_Mag*>(weapComps[weaponComponent::MAGIZINE])->magRoundCount();
  }

    //HACK
  if (reinterpret_cast<weapComp_Mag*>(weapComps[weaponComponent::CHAMBER]) == nullptr)
  {
    //std::cout << "*No Chamber" << std::endl;
  }
  else
  {
    //reinterpret_cast<weapComp_Mag*>(weapComps[weaponComponent::CHAMBER])->debug();
    roundCountTotal += reinterpret_cast<weapComp_Round*>(weapComps[weaponComponent::CHAMBER])->HasRound();
  }

  //HACK
  if (reinterpret_cast<weapComp_Mag*>(weapComps[weaponComponent::BOLT]) == nullptr)
  {
    //std::cout << "*No Bolt" << std::endl;
  }
  else
  {
    //reinterpret_cast<weapComp_Mag*>(weapComps[weaponComponent::BOLT])->debug();
    roundCountTotal += reinterpret_cast<weapComp_Round*>(weapComps[weaponComponent::BOLT])->HasRound();
  }

  return roundCountTotal;
}

////////////////////////////////////////////////////////////////////////////////
//LOGIC
//*optional; cases where state maybe unnecessary for
//           the core functionality of the system

//NOTE: **For rifle auto; could differ for other weapon types
//Trigger: ready when released; activates action**
  //releasing(TD_READY);
  //pulling(TD_ACTIVE);
//Hammer: ready when cocked; activates chamber**
  //cocking(TD_READY);
  //engaging(TD_ACTIVE);
//Action: uses firemodes; activates hammer**
  //reacting(TD_EITHER);
  //actioning(TD_EITHER);
//Feedport: ready when unloaded
  //unloading(TD_READY);
  //loading(TD_ACTIVE);
//Bolt: ready when opened
  //opening(TD_READY);    //BOLT
  //feeding(TD_ACTIVE);   //Round TO   BOLT; FROM MAG
  //ejecting(TD_READY);   //Round FROM BOLT; TO   EJECTION
  //chambing(TD_ACTIVE);  //Round TO   CHAMBER; FROM BOLT
  //extracting(TD_READY); //Round FROM CHAMBER; TO   BOLT
  //closing(TD_ACTIVE);   //BOLT
//Chamber: ready when emptied
  //emptying(TD_READY);
  //firing(TD_ACTIVE); //was in Round
//Magizine: ready when round is ready for transfer
  //cyclingInternally(TD_READY);
  //cyclingExternally(TD_ACTIVE);

//EJECTIONPORT
  //NO STAGES

////////////////////////////////////////
//==TRIGGER==
  //releasing(TD_READY);
  //pulling(TD_ACTIVE);

//Pulling
bool logic_pulling(struct weaponComponent* thisComp)
{
  std::cout << "Pulling" << std::endl;

  weapComp_Action* compAction = reinterpret_cast<weapComp_Action*>(
      thisComp->parent->getWeapComp(weaponComponent::ACTION));

  if(compAction->firemode.resetOnTriggerRelease)
  {
    compAction->remainingRoundsPerPull = compAction->firemode.roundsPerPull;
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

  if(thisComp->parent->getWeapComp(weaponComponent::BOLT)->IsActive() == false)
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

  //TODO: Add logic for proposition thisAction->firemode.triggerCheck

  if(thisAction->firemode.triggerCheck &&
     thisComp->parent->getWeapComp(weaponComponent::TRIGGER)->IsActive() == false)
  {
    std::cout << "*Trigger is not active" << std::endl;
    return true;
  }

  thisAction->parent->setComponent(TD_ACTIVE, weaponComponent::BOLT);

  if(thisAction->remainingRoundsPerPull > 0
     || thisAction->firemode.roundsPerPull == 0) //i.e. is full auto
  {
    thisComp->parent->setComponent(TD_ACTIVE, thisComp->wcType);
    std::cout << "*Re-Actioning" << std::endl;
    //return false; //EXPERIMENTAL
  }
  //else //EXPERIMENTAL
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

  reinterpret_cast<weapComp_Port*>(thisComp)->removeComp();

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

  reinterpret_cast<weapComp_Port*>(thisComp)->addComp();

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
       thisComp->parent->getWeapComp(weaponComponent::BOLT))->HasRound()
    )
  {
    reinterpret_cast<weapComp_Round*>(
       thisComp->parent->getWeapComp(weaponComponent::BOLT))->DestoryRound();

    //reinterpret_cast<weapComp_Round*>(
    //  thisComp->parent->weapComps[weaponComponent::BOLT])->RoundTransferTo(nullptr);
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
    weaponComponent::CHAMBER);
    //reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponComponent::CHAMBER]));

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
    weaponComponent::BOLT);
    //reinterpret_cast<weapComp_Round*>(thisComp->parent->weapComps[weaponComponent::BOLT]));

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

  //if(reinterpret_cast<weapComp_Round*>(thisComp)->round != nullptr)
  {
    roundActivated = reinterpret_cast<weapComp_Round*>(thisComp)->activateRound();
  }

  if(roundActivated)
  {
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

  //External Components
//weapComp_Mag* reachableMag
//  = new weapComp_Mag("Magizine", weaponComponent::MAGIZINE, 10);

struct modCompCond_Mag : public modCompCondtional
{
  modCompCond_Mag() : modCompCondtional(weaponComponent::MAGIZINE){}

  weaponComponent* rulesForAdding() override
  {
    std::cout << "Applying rule for adding magizine" << std::endl;
    //reachableMag->Reset();
    //return reachableMag;

    weapComp_Mag* reachableMag
      = new weapComp_Mag("Magizine", weaponComponent::MAGIZINE, 10);

    reachableMag->Reset();

    weaponStage cyclingInternally(TD_READY);//(TD_EITHER);
    weaponStage cyclingExternally(TD_ACTIVE);

    cyclingInternally.activationStage = logic_cyclingInternally;
    cyclingExternally.activationStage = logic_cyclingExternally;

    reachableMag->weaponStages.push_back(cyclingInternally);
    reachableMag->weaponStages.push_back(cyclingExternally);

    return reachableMag;
  }

  bool rulesForRemoving() override
  {
    std::cout << "Applying rule for removing magizine" << std::endl;
    return true;
  }
};

////////////////////////////////////////////////////////////////////////////////
//Function Definitions

void weaponSystem::addWeapComp(weaponComponent* weapComp)
{
  if(weapComp != nullptr)
  {
    weapComp->parent = this;

    if(weapComp->wcType == weaponComponent::FEEDPORT)
    {
      std::cout << "weapComp->wcType == weaponComponent::FEEDPORT" << std::endl;

      reinterpret_cast<weapComp_Port*>(weapComp)->connectComp();

      //reinterpret_cast<weapComp_Port*>(weapComp)->object
      //  = &weapComps[reinterpret_cast<weapComp_Port*>(weapComp)->weapCompAttacheType];
      //(*reinterpret_cast<weapComp_Port*>(weapComp)->object)            = nullptr;
      //reinterpret_cast<weapComp_Port*>(weapComp)->cc                   = gcc;
      //reinterpret_cast<weapComp_Port*>(weapComp)->rulesForAdding       = rulesForAdding_Mag;
      //reinterpret_cast<weapComp_Port*>(weapComp)->rulesForRemoving     = rulesForRemoving_Mag;
      ////feedPort->cc.reachableMag      = gcc.reachableMag;
    }

    if(weapComp->wcType < weapComps.capacity())
    weapComps[weapComp->wcType] = weapComp;
  }
}

void weapComp_Port::connectComp()
{
  //object = &parent->getWeapComp(weapCompAttacheType);
  //(*object) = nullptr;
  //cc = gcc;
  //rulesForAdding   = rulesForAdding_Mag;
  //rulesForRemoving = rulesForRemoving_Mag;
  ////feedPort->cc.reachableMag      = gcc.reachableMag;
}

////////////////////////////////////////////////////////////////////////////////
//LOGIC EXECUTION

int main()
{
//////////////////////////////
//Weapon Components

  //Pre-fire
  weaponComponent *trigger
    = new weaponComponent("Trigger", weaponComponent::TRIGGER);
  weaponComponent *hammer
    = new weaponComponent("Hammer", weaponComponent::HAMMER);
  weapComp_Action *action
    = new weapComp_Action("Action", weaponComponent::ACTION);

  //Firing
  weapComp_Port   *feedPort
    = new weapComp_Port("FeedPort", weaponComponent::FEEDPORT, new modCompCond_Mag);
  weapComp_Round  *bolt
    = new weapComp_Round("Bolt", weaponComponent::BOLT);
  weapComp_Round  *chamber
    = new weapComp_Round("Chamber", weaponComponent::CHAMBER);
  //weaponComponent *ejectPort  = new weaponComponent("EjectPort", weaponComponent::CHAMBER); //UNUSED

  //Post-fire
  weaponComponent *barrel
    = new weaponComponent("Barrel", weaponComponent::BARREL);
  weaponComponent *muzzle
    = new weaponComponent("Muzzle", weaponComponent::MUZZLE);

  //External Components
  //weapComp_Mag* detactableMag
  //  = new weapComp_Mag("Magizine", weaponComponent::MAGIZINE, 10);

  //detactableMag->Reset(); //TEMP

  //gcc.reachableMag               = detactableMag;

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
    hammer->CompToReady(action, TD_EITHER);
    //action->CompToReady(trigger, TD_EITHER);
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

  //MOVED: To internal logic of weapComp_Mag
    //Magizine: ready when round is ready for transfer
  //weaponStage cyclingInternally(TD_READY);//(TD_EITHER);
  //weaponStage cyclingExternally(TD_ACTIVE);

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

  //MOVED: To internal logic of weapComp_Mag
    //Magizine
  //cyclingInternally.activationStage = logic_cyclingInternally;
  //cyclingExternally.activationStage = logic_cyclingExternally;

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

  //MOVED: To internal logic of weapComp_Mag
    //Magizine
  //detactableMag->weaponStages.push_back(cyclingInternally);
  //detactableMag->weaponStages.push_back(cyclingExternally);

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
  //rifleAuto.addWeapComp(nullptr);  //MAGIZINE,

    //MOVED: To internal logic of weapComp_Port
  //feedPort->object = &rifleAuto.weapComps[weaponComponent::MAGIZINE];
  //(*feedPort->object)            = nullptr;
  //feedPort->cc                   = gcc;
  //feedPort->rulesForAdding       = rulesForAdding_Mag;
  //feedPort->rulesForRemoving     = rulesForRemoving_Mag;
  //feedPort->cc.reachableMag      = gcc.reachableMag;

  weaponSystem &curWeapon = rifleAuto;

  std::string input;

  while(input != "q")
  {
    input.clear();
    std::cout << "\nENTER COMMAND: ";
    std::cin >> input;

    if(input == "t") //Check weapon count
    {
      std::cout << "STATUS[Ammo check]" << std::endl;

      unsigned roundCount = curWeapon.weaponRoundCount();

      std::cout << "*Rounds in system in all: " << roundCount << std::endl;
    }
    else if(input == "r") //Reload
    {
      std::cout << "ACTION[Reloading]" << std::endl;

      bool shouldReload = true;

        //If weapon already has magizine
      if(curWeapon.getWeapComp(weaponComponent::MAGIZINE))
      {
        if(reinterpret_cast<weapComp_Mag*>(
             curWeapon.getWeapComp(weaponComponent::MAGIZINE))->IsFull())
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
        //reachableMag->Reset();
        //detactableMag->Reset();
        curWeapon.setComponent(TD_ACTIVE, weaponComponent::FEEDPORT);
        curWeapon.setComponent(TD_ACTIVE, weaponComponent::BOLT);
      }
      else
      {
        std::cout << "*Reloading Cancelled" << std::endl;
      }
    }
    else if(input == "f") //Fire Weapon; per trigger pull
    {
      std::cout << "ACTION[Firing]; Number of trigger pulls: ";

      unsigned triggerPulls = 1;

      std::cin >> triggerPulls;

      std::cout << triggerPulls << " PULL(s)" << std::endl;

      for(unsigned i = 0; i < triggerPulls; ++i)
      {
        curWeapon.setComponent(TD_ACTIVE, weaponComponent::TRIGGER);
        curWeapon.setComponent(TD_READY, weaponComponent::TRIGGER);
      }
    }
    else if(input == "c") //Fire Weapon; per rounds fired
    {
      std::cout << "ACTION[Firing]; Number of rounds to fire: ";

      unsigned roundsToFire = 1;

      std::cin >> roundsToFire;

      std::cout << roundsToFire << " ROUND(s) TO FIRE" << std::endl;

      unsigned i = 0;

      while(i < roundsToFire)
      {
        curWeapon.setComponent(TD_ACTIVE, weaponComponent::TRIGGER);
        curWeapon.setComponent(TD_READY, weaponComponent::TRIGGER);

        i += curWeapon.roundCyclesFired;

        //std::cout << "\ni: " << i << "\n" << std::endl; //DEBUG
      }
    }
    else if(input == "e") //FireModes
    {
      std::cout << "ACTION[Cycle FireModes]" << std::endl;

      //WIP; action should hold this info
      reinterpret_cast<weapComp_Action*>(
        curWeapon.getWeapComp(weaponComponent::ACTION))->firemode
      = firemodes.cycleFiremodes();
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
      firemodes.debug();
    }
    else if(input == "s") //status of firearm
    {
      std::cout
      << "STATUS[Weapon Component info of WeaponSystem: "
      << std::right << std::setw(10)
      << curWeapon.name << "]"
      << std::endl;

      for(
          weaponComponent::WEAP_COMPS_ENUMS i
            = static_cast<weaponComponent::WEAP_COMPS_ENUMS>(0);
          i < weaponComponent::COUNT;
          i = weaponComponent::WEAP_COMPS_ENUMS(static_cast<int>(i) + 1)
         )
      {
        if(curWeapon.getWeapComp(i) != nullptr)
        {
          curWeapon.getWeapComp(i)->weaponComponent::debug();
        }
        else
        {
          //TODO: Replace 'i' with a enumToString translation
          std::cout << "*This WeapSys is missing WeapComp type of: " << i << std::endl;
        }
      }
    }
    else
    {
      std::cout << "UNKNOWN COMMAND" << std::endl;
    }
  }

  std::cout << "QUIT" << std::endl;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
