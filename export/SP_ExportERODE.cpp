//////////////////////////////////////////////////////////////////////
// $Author: chodak $
// $Version: 0.1 $
// $Date: 2017/11/26 $
// Short Description: export to ERODE
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/stdstream.h>

//#include "sp_core/tools/SP_SimpleNetBuilder.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "export/SP_ExportPT.h"

#include "export/SP_ExportERODE.h"

SP_ExportERODE::SP_ExportERODE() {
}

SP_ExportERODE::~SP_ExportERODE() {
}

bool SP_ExportERODE::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc) {
	return true;
}

bool SP_ExportERODE::OnDialogOk() {
	return true;
}

bool SP_ExportERODE::AcceptsDoc(SP_MDI_Doc* p_doc) {
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_PN_CLASS || cName == SP_DS_EXTPN_CLASS
			|| cName == SP_DS_TIMEPN_CLASS || cName == SP_DS_SPN_CLASS
			|| cName == SP_DS_CONTINUOUSPED_CLASS
			|| cName == SP_DS_HYBRIDPN_CLASS);
}

bool SP_ExportERODE::DoWrite() {
	const std::string INDENT1 = " ";
	const std::string INDENT2 = "  ";
	std::ostringstream outStream;

	const std::string strFileName =
			wxFileName(m_fileName).GetName().ToStdString();
//	std::string strPlaceName;
//	std::string strPlaceMarking;

//	std::list<SP_ContArc*> *pListPreArc = NULL;
//	std::list<SP_ContArc*> *pListPostArc = NULL;

	outStream << "begin model " << strFileName << std::endl;
	outStream << INDENT1 << "begin parameters" << std::endl;

	for (auto constatnPair : m_constantIdMap) {
		auto constant = constatnPair.second;
		if (!constant) {
			continue;
		}
		outStream << INDENT2 << constant->m_name << " = " << constant->m_value
				<< std::endl;
	}

	outStream << INDENT1 << "end parameters" << std::endl;
	outStream << INDENT1 << "begin init" << std::endl;

	// Discreete places
	for (auto placePair : m_placeIdMap) {
		auto *pPlace = placePair.second;
//		strPlaceName = pPlace->m_name;
//		strPlaceMarking = pPlace->m_value;
		outStream << INDENT2 << pPlace->m_name << " = " << pPlace->m_value
				<< std::endl;
	}
	// Continues places
	for (auto placePair : m_contPlaceIdMap) {
		auto *pPlace = placePair.second;
//		strPlaceName = pPlace->m_name;
//		strPlaceMarking = pPlace->m_value;
		outStream << INDENT2 << pPlace->m_name << " = " << pPlace->m_value
				<< std::endl;
	}

	outStream << INDENT1 << "end init" << std::endl;

	outStream << INDENT1 << "begin partition" << std::endl;
	outStream << INDENT1 << "end partition" << std::endl;

	outStream << INDENT1 << "begin reactions" << std::endl;
//	for (auto transitionPair : m_contTrIdMap) {
//		bool bFirstPlace = true;
//		auto *pTransition = transitionPair.second;
//		pListPreArc = m_preContTrMap[pTransition->m_id];
//		pListPostArc = m_postContTrMap[pTransition->m_id];
//		if(!pListPreArc || !pListPostArc) {
//			continue;
//		}
//		// reagents
//		outStream << INDENT2;
//		for(auto pPreArc : *pListPreArc) {
//			SP_ContPlace *prePlace = m_contPlaceIdMap[pPreArc->m_id];
//			outStream << (bFirstPlace ? "" : " + ") << prePlace->m_name;
//			bFirstPlace = false;
//		}
//		// product
//		bFirstPlace = true;
//		outStream << " -> ";
//		for(auto pPostArc : *pListPostArc) {
//			SP_ContPlace *postPlace = m_contPlaceIdMap[pPostArc->m_id];
//			outStream << (bFirstPlace ? "" : " + ") << postPlace->m_name;
//			bFirstPlace = false;
//		}
//
//		outStream << ", " << massActionValue(pTransition->m_function)
//				  << " [" << pTransition->m_name << "]"<< std::endl;
//	}
	// Discreete transitions
	appendTransition<
						TransitionIdMap,
						ArcMap,
						ArcMap,
						PlaceIdMap,
						SP_DiscrArc
					> (
						m_trIdMap,
						m_preTrMap,
						m_postTrMap,
						m_placeIdMap,
						INDENT1,
						INDENT2,
						outStream);
	// Continues transitions
	appendTransition<
						ContTransitionIdMap,
						ContArcMap,
						ContArcMap,
						ContPlaceIdMap,
						SP_ContArc
					> (
						m_contTrIdMap,
						m_preContTrMap,
						m_postContTrMap,
						m_contPlaceIdMap,
						INDENT1,
						INDENT2,
						outStream);

	outStream << INDENT1 << "end reactions" << std::endl;

	outStream << "end model" << std::endl;
	std::cout << outStream.str();
	m_file.Write(outStream.str());

	return true;
}

std::string SP_ExportERODE::massActionValue(const std::string &strFun) {
	std::vector<std::string> vstrTokens;
	bool bIsMassAction = false;
	std::string strMassActionValue = "0";

	boost::split(vstrTokens, strFun, boost::is_any_of("() "));
	for (auto token : vstrTokens) {
		if (bIsMassAction && !token.empty()) {
			strMassActionValue = token;
			break;
		}
		if (!bIsMassAction && !token.empty()) {
			if (token == "MassAction") {
				bIsMassAction = true;
			} else {
//				throw WriteExc(_FLINE_, "Update::output - only MassAction function is allowed!");
			}
		}
	}

	return strMassActionValue;
}

//bool SP_ExportERODE::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
//{
//	CHECK_POINTER(p_doc, return false);
//	CHECK_POINTER(p_doc->GetGraph(), return false);
//	CHECK_BOOL((!p_fileName.IsEmpty()), return false);
//
//	m_graph = p_doc->GetGraph();
//	m_fileName = p_fileName;
//
//	SP_SimpleNetBuilder builder;
//	if(builder(m_graph))
//	{
//		dssd::erode::writer writer{builder.GetNet()};
//		writer(m_fileName, "");
//	}
//
//	return !m_bError;
//}

