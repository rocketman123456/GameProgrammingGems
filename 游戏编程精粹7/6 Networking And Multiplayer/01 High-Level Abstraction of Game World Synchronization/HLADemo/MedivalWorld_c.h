#pragma once

#include "MedivalWorld_common.h"

namespace MedivalWorld
{
class Knight_C:public SynchEntity_C
{
public:
	CPoint Position;
	bool IsRed;
	virtual void UpdateFromMessage(CMessage& msg);
};

class RuntimeDelegate_C:public IHlaRuntimeDelegate_C
{
public:
	virtual SynchEntity_C* CreateSynchEntityByClassID(HLAClassID classID);
};
;
}
