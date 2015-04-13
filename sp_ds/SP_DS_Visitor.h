//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/02/21 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DS_VISITOR_H_
#define __SP_DS_VISITOR_H_

#include "sp_core/base/SP_Error.h"
class SP_DS_NodeVector;

/**	\brief	Visitor class corresponding to the visitor design pattern
  */


class SP_DS_Visitor
{
private:
protected:
public:
	
	/**	\brief	Constructor

    */
    SP_DS_Visitor();
    virtual ~SP_DS_Visitor();

	/* \brief visit the node vector 

		\param p_pcNodeVector the node vector to be visited
	*/
	virtual bool Visit(SP_DS_NodeVector * p_pcNodeVector);

};
#endif // __SP_DS_VISITOR_H_
