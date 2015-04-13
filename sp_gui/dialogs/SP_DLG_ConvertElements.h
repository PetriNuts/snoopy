/*
 * SP_DLG_ConvertElements.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 1.11.2010
 * Short Description:
 ==============================================================================*/
#include <wx/wx.h>

#ifndef SP_DLG_CONVERTELEMENTS_H_
#define SP_DLG_CONVERTELEMENTS_H_

/*
 * Convert selected elements (Place,Transitions, or edges) into another (acceptable) type
 */
class SP_DLG_ConvertElements :public wxDialog, public SP_Error
{
 private:
	      //Store selected elements
	      SP_ListData m_lSelectedElements;
          SP_DS_Graph* m_pcGraph;
          SP_MDI_View* m_pcView;
		  wxString m_sNetClassName;

	      wxComboBox *m_pcPossibleSelectionsComboBox;

	      wxString m_sOriginalElementType;
	      wxString m_sConvertToType;

		  wxCheckBox* m_pcCopyBrushBox;

		  bool m_nCopyBrush;

		  bool m_nCanConvert;
protected:
	      SP_VectorString GetPossibleConversionType(const wxString& p_sSelectedName,const wxString& p_sSelectedType);
	      wxString GetSelectedType( const wxString& p_sSelectedElementType);

	      //convert nodes (places and transitions)
	      bool ConvertNodes();

	      //convert edges
	      bool ConvertEdges();

	      void ShowConversionMessage(const SP_VectorString& p_asConvertedNames,
								     const wxString& p_sOriginalType,
								     const wxString& p_sConvertedType);

	      bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcSrc, SP_Data* p_pcTrg);

	      bool CheckEdgeRequirementofNode(SP_DS_Node* p_pcNodeToCheck, SP_DS_Node* p_pcToConvertType);

		  bool UpdateSourceEdgeGraphics(SP_DS_Edge* p_pcEdge,
			                            const SP_ListGraphic* p_pcOldGraphicList,
								        const SP_ListGraphic* p_pcNewGraphicList);

		  bool UpdateTargetEdgeGraphics(SP_DS_Edge* p_pcEdge,
			                            const SP_ListGraphic* p_pcOldGraphicList,
								        const SP_ListGraphic* p_pcNewGraphicList);

		  SP_DS_Node* ConvertNode(SP_DS_Node* p_pcOldNode,SP_DS_Nodeclass* p_pcNewNodeClass);

		  void RemoveGraphicFromCanvas(SP_Graphic *p_pcGraphic);

		  SP_Graphic* FindGraphic(const SP_ListGraphic* p_pcOldGraphicList,
								  const SP_ListGraphic* p_pcNewGraphicList,
								  SP_Graphic* p_pcOldGraphic);
 public:
	      void DoConversion();
		  SP_DLG_ConvertElements(SP_MDI_View* p_pcView,wxWindow *p_parent,const wxString& p_title = wxT("Convert To"));
		  ~SP_DLG_ConvertElements();

		  bool CanConvert(){return m_nCanConvert;}
};

#endif /* SP_DLG_CONVERTELEMENTS_H_ */
