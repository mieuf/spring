// _____________________________________________________
//
// RAI - Skirmish AI for TA Spring
// Author: Reth / Michael Vadovszki
// _____________________________________________________

#ifndef RAI_UNITDEF_H
#define RAI_UNITDEF_H

struct sRAIUnitDefBL;
struct sRAIUnitDef;
struct sRAIBuildList;
class cRAIUnitDefHandler;

//#include "LogFile.h"
//#include "Sim/Units/UnitDef.h"
//#include "ExternalAI/IAICallback.h"
//#include <map>
//#include <set>
#include "Global.h"

using std::string;
using std::set;
using std::pair;

const int TASK_NONE = 1; // uses default combat behaviors
const int TASK_CONSTRUCT = 2;
const int TASK_ASSAULT = 3;
const int TASK_SCOUT = 4;
const int TASK_SUICIDE = 5;
const int TASK_SUPPORT = 6;
const int TASK_TRANSPORT = 7;

struct sWeaponEfficiency
{
	float BestRange;	// Best range for fighting this enemy
	float rate;			// unused
};

struct sRAIPrerequisite
{
	sRAIPrerequisite(sRAIUnitDef* RAIud)
	{
		udr=RAIud;
		buildLine=0;
	};
	sRAIUnitDef *udr;
	int buildLine;		// This refers to how many other builders must be build to reach this option
};

struct sRAIUnitDefBL
{
	sRAIUnitDefBL(sRAIUnitDef* RAIud, sRAIBuildList* BuildList, float Efficiency=-1.0, int Task=-1);
	~sRAIUnitDefBL();

	sRAIUnitDef* RUD;
	sRAIBuildList* RBL;

	int udIndex;
//	int listIndex;
	float efficiency;
	int task;
};

struct sRAIUnitDef
{
	sRAIUnitDef(const UnitDef *unitdef, IAICallback* callback, cRAIGlobal* G, float EnergyToMetalRatio, cLogFile *l, float MaxFiringRange=3000.0);
	int GetPrerequisite(); // returns UnitDef ID
	int GetPrerequisiteNewBuilder();
	void SetUnitLimit(int num);
	void SetULConstructs(int num); // sets the number of units that can be built excluding 'UnitsActive' && 'UnitConstructsActive'
	sRAIUnitDefBL* GetBuildList(string Name);

	sRAIUnitDefBL *List[35];	// possible lists a unit may be assigned to
	int ListSize;				// 'List' array size in use
	int UnitLimit[2];			// [0]: virtual unit limit, max units RAI is willing to build, ud->maxThisUnit is considered  [1]: remaining geospots/metalspots, basicly a construction unit limit
	bool HighEnergyDemand;		// This unit requires an unbalanced amount of energy to remain active, EX: metal makers, moho-mines in XTA
	float MetalDifference;		// How much metal will this produce/cost when active
	float EnergyDifference;		// How much energy will this produce/cost when active
	float MetalPCost;			// How much metal production is needed to consider building this
	float EnergyPCost;			// How much energy production is needed to consider building this
	float CloakCostMax;			// = to moving cost for units that can move, = to normal cost for units that can not
	int WeaponCostMax;			// energy drain per second if firing all weapons continuously
	const WeaponDef *DGun;		// valid if ud->canDGun=true && a dgun was found, otherwise 0
	const WeaponDef *SWeapon;	// valid if a weapon is iW->def->stockpile && iW->def->manualfire, otherwise 0
	map<int,sRAIUnitDef*> BuildOptions;					// key value = UnitDef ID, You could think of this as an improved version of UnitDef->buildOptions
	map<int,sRAIUnitDef*> PrerequisiteOptions;			// key value = UnitDef ID, a list of the types of units can build this unit
	map<int,sRAIPrerequisite> AllPrerequisiteOptions;	// key value = UnitDef ID, a list of anything that can build this unit, or somthing that can build a unit that can build this unit, or so on..
//	map<int,sWeaponEfficiency> WeaponEff;				// key value = Enemy UnitDef ID
	sWeaponEfficiency WeaponLandEff;	// used if enemy unitdef is unknown
	sWeaponEfficiency WeaponAirEff;		// used if enemy unitdef is unknown
	sWeaponEfficiency WeaponSeaEff;		// used if enemy unitdef is unknown
	float WeaponGuardRange;				// Base Defences only, distance for nearby structures to be considered 'guarded'
	set<int> WeaponDamageType;			// types of armors that this unit can damage
//	map<int,sRAIPrerequisite> AllBuildOptions;			// key value = UnitDef ID, complete list of anything that this unit can build
	set<int> mb;				// key = MapBody index, list of map bodys this unit can be built at
	const UnitDef* ud;			// Always valid

	void CheckUnitLimit();		// called after 'UnitsActive' or 'UnitConstructs' is modified, updates 'RBUnitLimit'
	void CheckBuildOptions();	// called after 'RBUnitLimit','RBCost','RBPrereq','Disabled' or 'UnitsActive' is modified, updates 'CanBuild','CanBeBuilt','RBPrereq', and 'HasPrerequisite' for all 'BuildOptions'

	set<int> UnitsActive;			// existing units of this type, used to determine active build options
	set<int> UnitConstructsActive;	// existing constructions of this type
	int UnitConstructs;		// units of this type being built, used to help determine active build options
	bool CanBuild;			// 'UnitsActive' is > 0
	bool CanBeBuilt;		// reverse result of (Disabled, RBUnitLimit, RBCost, RBPrereq) - in other words, RAI is willing to build this
	bool HasPrerequisite;	// A unit in 'PrerequisiteOptions' has 'CanBuild' enabled
	bool Disabled;			// RAI will not build this unit, however it may still use the unit if gained through given,captured,resurrected
	bool RBUnitLimit;		// (R)estrict (B)uilding: the Max of this Unit has been built
	bool RBCost;			// (R)estrict (B)uilding: the Cost is too high to build right now
	bool RBPrereq;			// (R)estrict (B)uilding: No buildline of units in "AllPrerequisiteOptions" have 'CanBuild or CanBeBuilt' enabled

	bool IsBomber;
	bool IsNano();
	bool IsCategory(string category); // only used during initialization
private:
	bool CheckWeaponType(vector<UnitDef::UnitDefWeapon>::const_iterator udw, int type); // type = 1-3 (land,air,sea), only used during initialization
	void SetBestWeaponEff(sWeaponEfficiency *we, int type, float MaxFiringRange); // only used during initialization
};

struct sRAIBuildList
{
	sRAIBuildList(int MaxDefSize, cRAIUnitDefHandler *UDRHandler);
	~sRAIBuildList();
	void Disable(int udIndex, bool value=true);
	void UDefSwitch(int index1, int index2);

	string Name;
	sRAIUnitDefBL **UDef;	// Possible Units to Build on this list, index is valid if < UDefSize
	int UDefActive;			// 'UDef' indexes below this value have 'CanBeBuilt=true', for later entries 'CanBeBuilt=false'
	int UDefActiveTemp;		// Same as UDefActive, but may be moddifed to temperaryly disable some units within 'UDefActive'
	int UDefSize;
	int priority;			// Amount to build compared to other build lists, if this = -1 then these units are only built from special demand needs
	int minUnits;			// Build at least this many before building anything else, however build demands will take presidence.  Also, this used to rate the importantance of the buildlist
	float minEfficiency;	// The minimal efficiency a unit needs to be accepted for use in this build list.  Note: 1.0 = average value (default)
							// Remember that it is not uncommon for there to be a unit this is bad at everything, such units are assigned to the task they are least bad at in respect to MinEfficiency
	int unitsActive;		// The ammount of units build from this list, used to keep track of the ratio of build options

	cRAIUnitDefHandler *UDR;
	int index;
};

class cRAIUnitDefHandler
{
public:
	cRAIUnitDefHandler(IAICallback* callback, cRAIGlobal* G, cLogFile *log);
	~cRAIUnitDefHandler();

	map<int,sRAIUnitDef> UDR;	// complete record of all unit definitions found, key value = Unit Def ID
	sRAIBuildList *BL[35];
	int BLSize;
	int BLActive;
	void BLSwitch(int index1, int index2);

	// special buildlists
	sRAIBuildList *BLBuilder;
	sRAIBuildList *BLEnergy;
	sRAIBuildList *BLEnergyL;
	sRAIBuildList *BLMetal;
	sRAIBuildList *BLMetalL;
	sRAIBuildList *BLEnergyStorage;
	sRAIBuildList *BLMetalStorage;
	sRAIBuildList *BLMobileRadar;
	sRAIBuildList *BLAirBase;

	float EnergyToMetalRatio;
	float AverageConstructSpeed;
private:
	struct sBuildLine
	{
		sBuildLine(int id, int bl)
		{
			ID=id;
			BL=bl;
		};
		int ID;	// UnitDef ID
		int BL;	// BuildLine, the amount of prerequisites that need to be build to reach this option
	};
	cLogFile *l;
};

#endif
