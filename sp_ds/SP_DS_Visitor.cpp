//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/02/21 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////

#include "sp_ds/SP_DS_Visitor.h"
#include "sp_core/SP_Core.h"

	SP_DS_Visitor::SP_DS_Visitor(){}
     
	SP_DS_Visitor::~SP_DS_Visitor(){}

	bool 
	SP_DS_Visitor::Visit(SP_DS_NodeVector * p_pcNodeVector){
		return true;
	}
