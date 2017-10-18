// SP_CMD_MoveGraphic.cpp: Implementierung der Klasse SP_CMD_MoveGraphic.
//
//////////////////////////////////////////////////////////////////////

#include "sp_gui/commands/SP_CMD_MoveGraphic.h"
#include "sp_ds/SP_DS_Node.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_CMD_MoveGraphic::SP_CMD_MoveGraphic(SP_GUI_Canvas *p_pcCanvas, double p_nX, double p_nY)
: wxCommand(true, wxT("Move Graphic")),
	m_pcCanvas(p_pcCanvas),
	m_nX(p_nX),
	m_nY(p_nY)
{
	if(m_pcCanvas)
	{
		// Add selected node shapes, if any
		wxNode* l_pcNode = m_pcCanvas->GetDiagram()->GetShapeList()->GetFirst();

		while (l_pcNode)
		{
			wxShape* l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
			// Add connected arc with label
			if(l_pcShape->IsKindOf(CLASSINFO(wxLineShape)) && (
					(dynamic_cast<wxLineShape*>(l_pcShape)->GetTo()->Selected() ||
					dynamic_cast<wxLineShape*>(l_pcShape)->GetFrom()->Selected()))
					) {

				wxLineShape* l_pcLineShape = dynamic_cast<wxLineShape*>(l_pcShape);
				m_lLineShapes.push_back(l_pcLineShape);

			}
			if (l_pcShape->Selected() &&
				!l_pcShape->IsKindOf(CLASSINFO(wxLineShape)))
			{
				m_lShapes.push_back(l_pcShape);
			}
	        // Only move the line point(s) if both ends move too
			else if (l_pcShape->IsKindOf(CLASSINFO(wxLineShape)) &&
	            dynamic_cast<wxLineShape*>(l_pcShape)->GetTo()->Selected() &&
	            dynamic_cast<wxLineShape*>(l_pcShape)->GetFrom()->Selected())
	        {
	            wxLineShape* l_pcLineShape = dynamic_cast<wxLineShape*>(l_pcShape);

	            if (l_pcLineShape->GetLineControlPoints()->GetCount() > 2)
	            {
	                wxNode *node1 = l_pcLineShape->GetLineControlPoints()->GetFirst();
	                while (node1)
	                {
	                    wxRealPoint *point = (wxRealPoint*)node1->GetData();
	                    m_lLinePoints.push_back(point);
	                    node1 = node1->GetNext();
	                }
	            }
	        }
			l_pcNode = l_pcNode->GetNext();
		}
	}
}

SP_CMD_MoveGraphic::~SP_CMD_MoveGraphic()
{ }

bool
SP_CMD_MoveGraphic::Do() {

	return Move(m_nX, m_nY);
}

bool
SP_CMD_MoveGraphic::Undo() {

	return Move(-m_nX, -m_nY);
}

bool
SP_CMD_MoveGraphic::Move(double p_nX, double p_nY) {

	if (!m_pcCanvas || m_lShapes.empty()) return false;
	
	m_pcCanvas->SelectAll(false);
	m_pcCanvas->SelectAllShapes(m_lShapes, true);
	if(m_lShapes.size() == 1)
	{
		m_pcCanvas->MoveShape(m_lShapes.front(), p_nX, p_nY);
	}
	else
	{
		// move line points
		m_pcCanvas->MoveLinePoints(p_nX, p_nY);
		/*for(auto point : m_lLinePoints)
		{
			point->x += p_nX;
			point->y += p_nY;
		}*/
		m_pcCanvas->MoveShapes(m_lShapes, p_nX, p_nY);
		m_pcCanvas->MoveLineShapesChildren(m_lLineShapes, p_nX, p_nY);
	}
	return true;
}
