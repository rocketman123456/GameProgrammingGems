
#include "MedivalWorld_c.h"

namespace MedivalWorld
{
void Knight_C::UpdateFromMessage( CMessage& msg )
{
	HLAMemberID memberID;
	while(Message_Read(msg,memberID))
	{
		switch(memberID)
		{
		case HlaId_Knight_Position:
			Message_Read(msg,Position);
			break;
		case HlaId_Knight_IsRed:
			Message_Read(msg,IsRed);
			break;
		}
	}
}
SynchEntity_C* RuntimeDelegate_C::CreateSynchEntityByClassID( HLAClassID classID )
{
	switch(classID)
	{
	case HlaId_Knight:
		return new Knight_C;
	}
	return NULL;
}
}
