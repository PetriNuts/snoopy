//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/mdi/SP_MDI_Diagram.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "snoopy.h"

#include "sp_core/SP_GPR_Canvas.h"

IMPLEMENT_CLASS(SP_MDI_Diagram, wxDiagram)

SP_MDI_Diagram::SP_MDI_Diagram(SP_MDI_Doc* p_pcDoc) :
	wxDiagram(), m_pcDoc(p_pcDoc), m_bGridVisible(true)
{
}

SP_MDI_Diagram::~SP_MDI_Diagram()
{
}

void SP_MDI_Diagram::Redraw(wxDC& dc)
{
	int i, j;
	int grid;
	int maxx, maxy, xanf, yanf;

	if (!dc.IsOk() || !GetCanvas())
		return;

	wxString l_Netclass = m_pcDoc->GetNetclassName();
	if(m_pcDoc->GetType() == SP_DOC_COARSE)
	{
		l_Netclass = m_pcDoc->GetParentDoc()->GetNetclassName();
	}

	//resetting grid in case it has changed
	SetGridSpacing((double) (wxGetApp().GetCanvasPrefs())->GetGridSpacing(l_Netclass));
	SetSnapToGrid((wxGetApp().GetCanvasPrefs())->GridSnappingEnabled(l_Netclass));
	SetGridVisible((wxGetApp().GetCanvasPrefs())->GridVisible(l_Netclass));

	// drawing grid points
	if (m_bGridVisible)
	{
		grid = (wxGetApp().GetCanvasPrefs())->GetGridSpacing(l_Netclass);

		dc.SetPen(*wxGREY_PEN);

		GetCanvas()->GetVirtualSize(&maxx, &maxy);

		xanf = dc.LogicalToDeviceX(0);
		yanf = dc.LogicalToDeviceY(0);
		maxx -= xanf;
		maxy -= yanf;

		for (i = -xanf; i < maxx; i += grid)
		{
			for (j = -yanf; j < maxy; j += grid)
			{
				dc.DrawPoint(i, j);
			}
		}
	}

	wxDiagram::Redraw(dc);
#if 0
	// Draw line crossings
	wxLineCrossings lineCrossings;
	lineCrossings.FindCrossings(*this);
	lineCrossings.DrawCrossings(*this, dc);
#endif // 0
}

void SP_MDI_Diagram::SetGridVisible(bool p_bVal)
{
	m_bGridVisible = p_bVal;
}
/*	switch (p_bVal)
 {
 case SP_GRID_STYLE_NONE:
 SetSnapToGrid(FALSE);
 break;
 case SP_GRID_STYLE_INVISIBLE:
 case SP_GRID_STYLE_DOTTED:
 if ((wxGetApp().GetCanvasPrefs())
 ->GridSnappingEnabled(m_pcDoc->GetDocumentTemplate()->GetDescription())) {
 SetSnapToGrid(TRUE);
 } else {
 SetSnapToGrid(FALSE);
 }
 break;
 }
 m_eGridStyle = p_bVal;
 }

 SP_GRID_STYLES_T
 SP_MDI_Diagram::GetGridStyle() {
 if (!m_eGridStyle) {
 m_eGridStyle = (wxGetApp().GetCanvasPrefs())
 ->GetGridStyle(m_pcDoc->GetDocumentTemplate()->GetDescription());
 }
 return m_eGridStyle;
 }*/
