
#include "MedivalWorld_s.h"

namespace MedivalWorld
{
	bool Knight_S::GatherTheChangeToMessage( CMessage &outputMessage ,Protocol &outProtocol)
	{
		outProtocol=Protocol_Undefined;
		bool ret=false;
		Protocol prot;
		// for each member, gather the change and append to the message and retrieve the appropriate protocol
		ret |= m_Position_INTERNAL.GatherTheChangeToMessage(HlaId_Knight_Position,outputMessage,prot);
		if(ret)
		{
			outProtocol=CombineProtocol(outProtocol,prot);
		}
		ret |= m_IsRed_INTERNAL.GatherTheChangeToMessage(HlaId_Knight_IsRed,outputMessage,prot);
		if(ret)
		{
			outProtocol=CombineProtocol(outProtocol,prot);
		}
		return ret;
	}
	void Knight_S::GatherAllToMessage( CMessage &outputMessage )
	{
		m_Position_INTERNAL.GatherAllToMessage(HlaId_Knight_Position,outputMessage);
		m_IsRed_INTERNAL.GatherAllToMessage(HlaId_Knight_IsRed,outputMessage);
	}
	HLAClassID Knight_S::GetClassID()
	{
		return HlaId_Knight;
	}
	bool Knight_S::HasChanges()
	{
		// for each member variable...
		if(m_Position_INTERNAL.IsChanged()) return true;
		if(m_IsRed_INTERNAL.IsChanged()) return true;
		return false;
	}
	void Knight_S::ClearTheChanges()
	{
		// for each member variable...
		m_Position_INTERNAL.ClearTheChange();
		m_IsRed_INTERNAL.ClearTheChange();
	}
}
