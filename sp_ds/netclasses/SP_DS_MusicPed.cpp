//////////////////////////////////////////////////////////////////////
// $Author: dbayer $
// $Version: 0.1 $
// $Date: 2008/06/25 $
// Short Description: petri net w/ sound transitions
//////////////////////////////////////////////////////////////////////

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/netclasses/SP_DS_MusicPed.h"

#include "sp_ds/SP_DS_Nodeclass.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/extensions/SP_DS_MusicPedAnimation.h"
#include "sp_ds/animators/SP_DS_MusTransAnimator.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogSoundList.h"

#include "sp_gr/shapes/SP_GR_MusicTransition.h"




#include "sp_defines.h"
#include "snoopy.h"

#include "sp_core/SP_GPR_Animation.h"

SP_DS_MusicPed::SP_DS_MusicPed()
:SP_DS_SimplePed(SP_DS_MUSICPN_CLASS)
{
	m_sExtension = wxT("spm");
	m_sFilter = wxT("*.") + m_sExtension;
}

SP_DS_MusicPed::SP_DS_MusicPed(const wxString& p_pchName)
:SP_DS_SimplePed(p_pchName)
{
	m_sExtension = wxT("spm");
	m_sFilter = wxT("*.") + m_sExtension;
}


SP_DS_Graph*
SP_DS_MusicPed::CreateGraph(SP_DS_Graph* p_graph)
{
	if (!SP_DS_SimplePed::CreateGraph(p_graph))
	{
		return NULL;
	}

	SP_DS_Attribute* l_pcAttr;
	SP_DS_Nodeclass* l_pcNS;

	// add more attributes to transition class
	l_pcNS = p_graph->GetNodeclass(wxT("Transition"));

	// add option to disable sound playback on firing
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_BoolAttribute( wxT("Play Sound when Transition fires"), true ));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("Sound")));

	SP_DS_ColListAttribute* l_pcColList;
	unsigned int l_nNewRow;

	// add attribute to attach sound files to transitions
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ColListAttribute(wxT("Sound"), SP_COLLIST_STRING, 2));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogSoundList(wxT("Sound")));

	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
	l_pcColList->SetColLabel( 0, wxT("Sound List"));
	l_pcColList->SetColLabel( 1, wxT("Sound"));
	l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	SP_GR_Node* l_pcGr;
	l_pcGr = new SP_GR_MusicTransition(l_pcNS->GetPrototype());
	l_pcNS->SetGraphic(l_pcGr);

	l_pcNS->AddAnimator(new SP_DS_MusTransAnimator());

	//set animation
	int	refresh = wxGetApp().GetAnimationPrefs()->GetRefresh(GetName());
	int duration = wxGetApp().GetAnimationPrefs()->GetDuration(GetName());
	SP_ANIM_STEP_T stepping = wxGetApp().GetAnimationPrefs()->GetStepping(GetName());
	p_graph->SetAnimation(new SP_DS_MusicPedAnimation(refresh, duration, stepping));

	return p_graph;
}

