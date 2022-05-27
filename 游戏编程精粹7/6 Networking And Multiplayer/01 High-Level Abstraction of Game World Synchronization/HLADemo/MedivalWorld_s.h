#pragma once

#include "MedivalWorld_common.h"

namespace MedivalWorld
{
class Knight_S:public SynchEntity_S
{
	DECLARE_HLA_MEMBER(CConditionalBehaviorMember,CPoint,Position);
	DECLARE_HLA_MEMBER(CConditionalBehaviorMember,bool,IsRed);
public:
	virtual bool GatherTheChangeToMessage(CMessage &outputMessage,Protocol &outProtocol);
	virtual void GatherAllToMessage( CMessage &outputMessage );
	virtual HLAClassID GetClassID();
	virtual bool HasChanges();
	virtual void ClearTheChanges();
};

}
