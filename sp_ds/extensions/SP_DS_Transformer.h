// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/26 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_Transformer_H__
#define __SP_DS_Transformer_H__

#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_DS_Transformer
{

private:
	bool TransformTransition(SP_DS_Graph* p_pcVal, SP_ListNode* p_pcTransformingNodes, vector<wxString> ComboBoxValue);

public:

	SP_DS_Transformer();
    ~SP_DS_Transformer();

    bool Check(SP_DS_Graph* p_pcVal);
	bool CheckElements(SP_DS_Graph* p_pcVal);
	bool CheckUniqueNames(SP_DS_Graph* p_pcVal);
	bool CheckTransition(SP_DS_Graph* p_pcVal);


};

#endif // __SP_DS_Transformer_H__
