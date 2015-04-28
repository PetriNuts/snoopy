//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ELEMENTCLASS_H__
#define __SP_ELEMENTCLASS_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Name.h"
#include "sp_ds/SP_DS_Attribute.h"

typedef std::shared_ptr<long> LongPtr;

class SP_DS_Graph;
class SP_DS_Animator;
class SP_WDG_DialogBase;

class SP_ElementClass: public SP_Error, public SP_Type, public SP_Name
{
private:
	LongPtr m_nIdCounter;
	bool m_bIdOwner;
protected:
    /**	\brief Graph object, this class is part of
    */
    SP_DS_Graph* m_pcGraph;
    wxString m_sShortcut;
    bool m_bShowInElementTree;

	bool m_bShowInDeclarationTreeOther;
	bool m_bShowInDeclarationTreeColorSet;

    void ResetIdCount();

public:
    // let's start m_nIdCounter with -1. The prototype element will
    // get -1, all "normal" nodes will start with 0
    SP_ElementClass(SP_ELEMENT_TYPE p_eType, const wxString& p_pchName, const LongPtr& p_nIdCounter = LongPtr(NULL));
    virtual ~SP_ElementClass();

    inline void SetShortcut(const wxString& p_sShortcut) {m_sShortcut = p_sShortcut;}
    inline wxString& GetShortcut() {return m_sShortcut;}

    long GetNewIdCount();
    bool TestIdCount(long p_nVal);
    const LongPtr& GetIdCountPtr();

    SP_WDG_DialogBase* RegisterGraphicWidget(SP_WDG_DialogBase* p_pcWidget);

    virtual bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal) { return FALSE; }
    /**	\brief	Accessor to the parent graph

	    \return	the m_pcGraph member
    */
    inline SP_DS_Graph* GetParentGraph() const { return m_pcGraph; }
    inline SP_DS_Graph* SetParentGraph(SP_DS_Graph* p_pcVal) { m_pcGraph = p_pcVal; return m_pcGraph; }

    // Animator
    virtual bool AddAnimator(SP_DS_Animator* p_pcAnimator);

    bool GetShowInElementTree() { return m_bShowInElementTree; }
    void SetShowInElementTree( bool p_bShowInElementTree ) { m_bShowInElementTree = p_bShowInElementTree; }

	bool GetShowInDeclarationTreeColorSet() { return m_bShowInDeclarationTreeColorSet; }
    void SetShowInDeclarationTreeColorSet( bool p_bShowInDeclarationTreeColorSet ) { m_bShowInDeclarationTreeColorSet = p_bShowInDeclarationTreeColorSet; }

	bool GetShowInDeclarationTreeOther() { return m_bShowInDeclarationTreeOther; }
    void SetShowInDeclarationTreeOther( bool p_bShowInDeclarationTreeOther ) { m_bShowInDeclarationTreeOther = p_bShowInDeclarationTreeOther; }

};

#endif // __SP_ELEMENTCLASS_H__
