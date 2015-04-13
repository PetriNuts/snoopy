//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ATTRIBUTE_H__
#define __SP_DS_ATTRIBUTE_H__

// Attribute Types
enum class SP_ATTRIBUTE_TYPE
{
	SP_ATTRIBUTE_NULL,
	SP_ATTRIBUTE_TEXT,
	SP_ATTRIBUTE_NAME,
	SP_ATTRIBUTE_BOOL,
	SP_ATTRIBUTE_LOGIC,
	// pointless, signed number
	SP_ATTRIBUTE_NUMBER,
	// pointless, signed number to be used as unique ID
	SP_ATTRIBUTE_ID,
	SP_ATTRIBUTE_MULTIPLICITY,
	// double float, signed number
	SP_ATTRIBUTE_DOUBLE,
	// double float, signed number - used as multiplicity
	SP_ATTRIBUTE_DOUBLEMULTIPLICITY,
	// equation
	SP_ATTRIBUTE_EQUATION,
	// list of text
	SP_ATTRIBUTE_LIST,
	// list of text with several columns
	SP_ATTRIBUTE_COLLIST,
	//by Steffen Laarz
	SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY,
	SP_ATTRIBUTE_DOUBLEMARKINGDEPENDETMULTIPLICITY,
	SP_ATTRIBUTE_MARKING,
	SP_ATTRIBUTE_DOUBLEMARKING
};

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Data.h"
#include "sp_core/base/SP_Name.h"

class SP_WDG_DialogBase;
class SP_DS_Attribute : public SP_Error, public SP_Data, public SP_Name
{
private:
protected:
	SP_Data* m_pcParent;
	SP_ATTRIBUTE_TYPE m_eAttributeType;
	/**	\brief	The constructor

	 Intentionally declared protected, to ensure, only derived (concrete) attributes are constructed directly.

	 \param	p_pchName	The name of the newly created attribute
	 \param  p_eType     Type of the attribute defined as ::SP_ATTRIBUTE_TYPE
	 */
	SP_DS_Attribute(const wxString& p_pchName, SP_ATTRIBUTE_TYPE p_eType = SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NULL);

	bool m_bShowInGlobalDialog;

	//added by MD
	SP_ListAttribute m_lAttributes;

	/**
	 * return name, generated from classname + id
	 */
	wxString MakeParentName();

public:
	virtual ~SP_DS_Attribute();

	inline bool SetDataParent(SP_Data* p_pcParent)
	{
		m_pcParent = p_pcParent;
		return TRUE;
	}
	inline SP_Data* GetParent() const
	{
		return m_pcParent;
	}

	virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE) = 0;
	SP_DS_Attribute* CloneBase(SP_DS_Attribute* p_pcAttr, bool p_bCloneGr = TRUE);

	virtual wxString GetValueString() = 0;
	virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true) = 0;

	bool SetGlobalShow(bool p_bVal = TRUE);
	inline bool GetGlobalShow() const
	{
		return m_bShowInGlobalDialog;
	}

	bool MergeData(SP_Data* p_pcElement);
	bool Split(SP_DS_Attribute* p_pcAttr, SP_ListGraphic* p_plGraphics);

	//! need to do something special in your derived attributes?
	virtual bool OnMerge(SP_Data* p_pcElement)
	{
		return TRUE;
	}
	virtual bool OnSplit(SP_DS_Attribute* p_pcAttribute)
	{
		return TRUE;
	}

	bool ShowGraphic(SP_Graphic* p_pcGraphic);

	virtual bool CheckIntegrity();

	SP_WDG_DialogBase* RegisterDialogWidget(SP_WDG_DialogBase* p_pcWidget);

	virtual bool Update(bool p_bLocalOnly = FALSE);
	bool Remove();

	const wxString GetClassName();

	SP_ATTRIBUTE_TYPE GetAttributeType() const
	{
		return m_eAttributeType;
	}
	bool SetAttributeType(SP_ATTRIBUTE_TYPE p_eType)
	{
		m_eAttributeType = p_eType;
		return TRUE;
	}

	//added by MD on 2006/07/19
	virtual bool Equals(SP_Data *p_pcToCheck);

	virtual bool Equals(SP_DS_Attribute *p_pcToCheck,
			bool p_bCheckValue = true, bool p_bCheckName = true,
			bool p_bCheckType = true);

	//added by MD
	virtual SP_DS_Attribute* GetAttribute(const wxString& p_pchName);
	virtual inline const SP_ListAttribute* GetAttributes()	{ return &m_lAttributes; }
	virtual SP_DS_Attribute* AddAttribute(SP_DS_Attribute *p_pcAttrib);
	virtual SP_DS_Attribute* RemoveAttribute(SP_DS_Attribute *p_pcAttrib);

	//copy value from a given attributes, it needs to be implemented
	//in the derived class.
	virtual bool CopyValueFrom(SP_DS_Attribute* p_pcAttribute);

	/**
	 * return name of parent, if "Name" attribute present or classname + id
	 */
	wxString GetParentName();


};

#endif // __SP_DS_ATTRIBUTE_H__
