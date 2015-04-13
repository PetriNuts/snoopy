//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: base class for data processors
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DATAPROCESSOR_H__)
#define __SP_IA_DATAPROCESSOR_H__

#include <wx/wx.h>
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Data.h"
#include "sp_ds/SP_DS_Graph.h"

class SP_IA_Command;

enum SP_IA_COMPARE_OP {
	SP_IA_NO_VALUE_COMPARE = 0,
	SP_IA_EQUALS, 
	SP_IA_EQUALS_NOT,
	SP_IA_CONTAINS,
	SP_IA_CONTAINS_CMPL,
	SP_IA_CONTAINS_NOT,
	SP_IA_IS_CONTAINED,
	SP_IA_IS_CONTAINED_CMPL,
	SP_IA_IS_CONTAINED_NOT,
	SP_IA_IS_LESS_THAN,
	SP_IA_IS_GREATER_THAN,

	SP_IA_LAST
};

class SP_IA_DataProcessor {

private:
	//cleans up special #IA#-attributes which may have been attached by any data processors
	static void RemoveAttachedAttributes(SP_Data *p_pcData);
	static void RemoveAttachedAttributes(const SP_ListAttribute *p_pcAttributes);

protected:
	SP_IA_DataProcessor();
	//interaction command, a data processor belongs to
	SP_IA_Command *m_pcParentCmd;
	
	bool m_bIsCopy;
	//true: only selected (SP_IA_SELECTED) elements are processed, others are just transfered
	//false: all elements are processed
	bool m_bProcessSelected;

public:
	virtual ~SP_IA_DataProcessor();

	//copies the data processor
	//must be implemented in derived data processors
	virtual SP_IA_DataProcessor *Copy() = 0;
	//makes a copy and sets the parent command
	SP_IA_DataProcessor *InitializedCopy(SP_IA_Command *p_pcPCmd);
	
	//does the actual data processor's work
	//must be implemented by every data processor
	virtual SP_ListData *DoProcessing(SP_ListData *p_pcData) = 0;
	//these two just call the above function
	SP_ListData *DoProcessing(SP_Data *p_pcData);
	SP_ListData *DoProcessing(SP_DS_Graph *p_pcGraph);

	//usual setters and getters
	inline void SetParentCommand(SP_IA_Command *p_pcPCmd) { m_pcParentCmd = p_pcPCmd; }
	inline void SetCopyStamp(bool p_bIsCopy) { m_bIsCopy = p_bIsCopy; }
	inline bool IsCopy() { return m_bIsCopy; }
	inline void SetProcessSelected(bool p_bSelected) { m_bProcessSelected = p_bSelected; }
	inline bool ProcessSelected() const { return m_bProcessSelected; }

	//this removes attached #IA#-attributes, deletes the data (if required) and resets IA-State
	static void ReleaseData(SP_Data *p_pcData, bool p_bDelete = false, bool p_bResetState = false);

};


#endif // !defined(__SP_IA_DATAPROCESSOR_H__)
