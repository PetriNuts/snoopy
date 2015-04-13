/*
 * SP_DS_xyPlotViewer.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 10.06.2011
 * Short Description:
 *///=================================================

#ifndef SP_DS_XYPLOTVIEWER_H_
#define SP_DS_XYPLOTVIEWER_H_

class SP_DS_xyPlotViewer: public SP_DS_PlotViewer
{
private:
	      int m_nDefaultLineWidth;
	      int m_nDefaultLineStyle;

	      bool m_bShowSymbols;
	      bool m_bShowLines;
protected:
           void OnMouseMotion(wxMouseEvent& event);
public:
		  SP_DS_xyPlotViewer(wxWindow* p_pcParent,wxSizer* p_pcSizer);
		  virtual ~SP_DS_xyPlotViewer();

		  //Create a plot
		  virtual Chart* CreateChart();

		  virtual void UpdateAttributes();
		  
		  //set the attribute values to the current one
		  virtual void LoadCurrentAttributeValues();

		  virtual void SaveViewToServerFormat(spsa::ModelView* p_pcServerSideView);
		  virtual void LoadViewFromServerFormat(spsa::ModelView* p_pcServerSideView);

		  //load and save from Snoopy format
		  virtual void SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView);
		  virtual void LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView);
};

#endif /* SP_DS_XYPLOTVIEWER_H_ */
