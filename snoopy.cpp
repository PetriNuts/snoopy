//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Main replacement
//////////////////////////////////////////////////////////////////////
#include "snoopy.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_gui/dialogs/SP_DLG_CheckForUpdate.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"

#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Fonts.h"

#include "sp_revision.h"

#include "export/SP_ExportManager.h"
#include "export/SP_ExportEPS.h"
#include "export/SP_ExportXfig.h"
#include "export/SP_ExportLatex.h"
#include "export/SP_ExportReport.h"
#include "export/SP_ExportAPNN.h"
#include "export/SP_ExportANDL.h"
#include "export/SP_ExportCANDL_new.h"
#include "export/SP_ExportCPP.h"
#include "export/SP_ExportSMART.h"
#include "export/SP_ExportPRISM.h"
#include "export/SP_ExportGCM.h"
#include "export/SP_ExportINA.h"
#include "export/SP_ExportTIM.h"
#include "export/SP_ExportTMD.h"
#include "export/SP_ExportPed2TimePT.h"
#include "export/SP_ExportTimePT2Ped.h"
#include "export/SP_ExportMaria.h"
#include "export/SP_ExportProd.h"
#include "export/SP_ExportLoLA.h"
#include "export/SP_ExportLLPEP.h"
#include "export/SP_ExportTINA.h"
#include "export/SP_ExportRomeo.h"
#include "export/SP_ExportContPed2Ped.h"
#include "export/SP_ExportPed2ContPed.h"
#include "export/SP_ExportContPed2PedConservative.h"
#include "export/SP_ExportContPed2PedConvert.h"
#include "export/SP_ExportContPed2ExtPT.h"
#include "export/SP_ExportContPed2StochPed.h"
#include "export/SP_ExportPed2ExtPT.h"
#include "export/SP_ExportExtPT2ContPed.h"
#include "export/SP_ExportExtPT2Ped.h"
#include "export/SP_ExportContPed2PedRed.h"
#include "export/SP_ExportContPed2LaTeX.h"
#include "export/SP_ExportContPed2SBML.h"
#include "export/SP_ExportPed2ModuloNets.h"
#include "export/SP_ExportModuloNets2Ped.h"
#include "export/SP_ExportContPed2METATOOL.h"
#include "export/SP_ExportMETATOOL.h"
#include "export/SP_ExportMusicPed2Ped.h"
#include "export/SP_ExportPed2MusicPed.h"
#include "export/SP_ExportMIF.h"
#include "export/SP_ExportSyRePa.h"
#include "export/SP_ExportFTree2ExtFTree.h"
#include "export/SP_ExportModelica.h"
#include "export/SP_ExportContODE2Text.h"
#include "export/SP_ExportContODE2Octave.h"

#include "export/SP_ExportSPN2Matlab.h"
#include "export/SP_ExportCPN2Matlab.h"
#include "export/SP_ExportTPN2Matlab.h"
#include "export/SP_ExportCUDA.h"
#include "export/SP_ExportFspn2SPN.h"//Added by G.Assaf 2019
#include "export/SP_ExportSPN2FSPN.h"
#include "export/SP_ExportFCPN2CPN.h"
#include "export/SP_ExportCPN2FCPN.h"
#include "export/SP_ExportFHPN2HPN.h"
#include "export/SP_ExportHPN2FHPN.h"
#include "export/SP_ExportFSPN2FCPN.h"
#include "export/SP_ExportFSPN2FHPN.h"
#include "export/SP_ExportFCPN2FHPN.h"
#include "export/SP_ExportFHPN2FSPN.h"
#include "export/SP_ExportFHPN2FCPN.h"
#include "export/SP_ExportFCPN2FSPN.h"


#include "sp_gui/interaction/SP_IA_Manager.h"

#include "export/SP_ExportStochPed2Ped.h"
#include "export/SP_ExportStochPed2ExtPT.h"
#include "export/SP_ExportStochPed2ContPed.h"
#include "export/SP_ExportPed2StochPed.h"
#include "export/SP_ExportExtPT2StochPed.h"
#include "export/SP_ExportStochPed2TimePT.h"
#include "export/SP_ExportPNML.h"
#include "export/SP_ExportSBML.h"
#include "export/SP_ExportCSV.h"

#include "export/SP_ExportFAU.h"

#include "import/SP_ImportManager.h"
#include "import/SP_ImportFTree_DNF.h"
#include "import/SP_ImportAPNN.h"
#include "import/SP_ImportANDL.h"
#include "import/SP_ImportCANDL.h"
#include "import/SP_ImportPNML.h"
#include "import/SP_ImportPNML2ColPN.h"
#include "import/SP_ImportPED.h"
#include "import/SP_ImportTINA.h"
#include "import/SP_ImportSBML2cntPn.h"
#include "import/SP_ImportSBML2extPN.h"
#include "import/SP_ImportSBML2sPn.h"
#include "import/SP_ImportCSV2SPN.h"
#include "import/SP_ImportCSV2ColPN.h" //liu 2010
#include "import/erode/SP_ImportERODE.h"

#include "export/SP_ExportColPT2PT.h"
#include "export/SP_ExportColExtPT2ExtPT.h" //2009 liu
#include "export/SP_ExportColStPN2StochPed.h"
#include "export/SP_ExportExtPT2ColExtPT.h" //2009 liu
#include "export/SP_ExportPed2ColPT.h"  //2009 liu
#include "export/SP_ExportStochPed2ColSPT.h" //2009 liu

#include "export/SP_ExportColSPN2ColCPN.h"
#include "export/SP_ExportColStPN2ColExPN.h"
#include "export/SP_ExportColStPN2ColPT.h"

#include "export/SP_ExportColExtPT2ColStPN.h"

#include "export/SP_ExportColExtPT2ColCPN.h"

#include "export/SP_ExportColExtPT2ColPN.h"

#include "export/SP_ExportColPT2ColExtPN.h"
#include "export/SP_ExportColPT2ColStPN.h"
#include "export/SP_ExportColPT2ColCPN.h"

#include "export/SP_Export2HPN.h"

#include "export/SP_ExportColCPN2ColPT.h"
#include "export/SP_ExportColCPN2ColExtPT.h"


#include "export/SP_ExportColSPN2CSV.h"
//#include "export/SP_ExportColExtPT2CSV.h"
//#include "export/SP_ExportColPT2CSV.h"
//#include "export/SP_ExportColCPN2CSV.h"

#include "export/SP_ExportColPT2Structure.h"

#include "export/SP_ExportColCPN2ColSPN.h"

#include "export/SP_ExportColHPN2ColCPN.h"
#include "export/SP_ExportColHPN2ColSPN.h"
#include "export/SP_ExportColHPN2ColEPN.h"
#include "export/SP_ExportColHPN2ColPN.h"

#include "export/SP_ExportColHPN2HybridPN.h"

#include "export/SP_ExportColSPN2ColHPN.h"
#include "export/SP_ExportColCPN2ColHPN.h"
#include "export/SP_ExportColEPN2ColHPN.h"
#include "export/SP_ExportColPN2ColHPN.h"
#include "export/SP_ExportHPN2SPN.h"
#include "export/SP_ExportColPN2SSC.h"

#include "export/SP_ExportHybridPN2ColHybridPN.h"

#include "export/SP_ExportContPed2ColCPT.h"
#include "export/SP_ExportColCPN2ContPed.h"

#include "export/SP_ExportColExtPT2CPNTool.h"

#include "export/SP_ExportColPN2APNN.h"
#include "export/SP_ExportColPN2ANDL.h"
#include "export/SP_ExportColPN2ANDL_new.h"
#include "export/SP_ExportColPN2SBML.h"

#include "export/SP_ExportHPN2CPN.h"
#include "export/SP_ExportHPN2SPN.h"

#include "export/SP_ExportERODE.h"
//by george
#include "export/SP_ExportColSPN2ColFSPN.h"
#include "export/SP_ExportColCPN2ColFCPN.h"
#include "export/SP_ExportColHPN2ColFHPN.h"
#include "export/SP_ExportColFHPN2ColHPN.h"
#include "export/SP_ExportColFCPN2ColCPN.h"
#include "export/SP_ExportColFSPN2ColSPN.h"
#include "export/SP_ExportColFCPN2FCPN.h"
#include "export/SP_ExportColFSPN2FSPN.h"
#include "export/SP_ExportColFHPN2FHPN.h"
#include "export/SP_ExportColFHPN2ColFSPN.h"
#include "export/SP_ExportColFHPN2ColFCPN.h"
#include "export/SP_ExportColFSPN2ColFHPN.h"
#include "export/SP_ExportColFCPN2ColFHPN.h"
#include "export/SP_ExportColFCPN2ColFSPN.h"
#include "export/SP_ExportColFSPN2ColFCPN.h"

#include <wx/cmdline.h>
#include <wx/fs_zip.h>
#include <wx/datetime.h>
#include <wx/config.h>
#include <locale.h>

#include <wx/sysopt.h>
#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"//for command line layout, by george 2021

#ifdef __WIN32__
#ifdef __WXDEBUG__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // __WXDEBUG__
#endif //__WIN32__
IMPLEMENT_APP(Snoopy)

Snoopy::Snoopy() :
	m_bMaximize(FALSE), m_nXPos(-1), m_nYPos(-1), m_nWidth(800),
			m_nHeight(600), m_pcCanvasNormalBrush(NULL),
			m_pcCanvasLogicBrush(NULL), m_pcDocmanager(NULL),
			m_pcExportManager(NULL), m_pcImportManager(NULL),
			m_pcIAManager(NULL)
{
	srand(wxDateTime::Now().GetTicks());
#ifdef __WIN32__
#ifdef __WXDEBUG__
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_crtBreakAlloc = 207800;
#endif // __WXDEBUG__
#endif //__WIN32__
}

Snoopy::~Snoopy()
{
}

bool Snoopy::ProcessCommandLine(wxArrayString& p_aargs)
{


#if wxABI_VERSION > 30000
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, "hw", "help", "show this help message", wxCMD_LINE_VAL_NONE,
		wxCMD_LINE_OPTION_HELP },

		{ wxCMD_LINE_PARAM, NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE
		| wxCMD_LINE_PARAM_OPTIONAL },


		{ wxCMD_LINE_OPTION, "l", "layout algorithm", "\n\t 1: FMMM algorithm,\n\t 2: Planarization algorithm,\n\t 3: Sugiyama algorithm",wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_OPTION, "p", "export", "1: EPS Export",wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_SWITCH, "s", "Save", "save" },
		{ wxCMD_LINE_SWITCH, "c", "Close", "close" },
		{ wxCMD_LINE_SWITCH, "v", "Version", "version" },
		{ wxCMD_LINE_SWITCH, "l1", "l1", "layout using FMMM algorithm" },
		{ wxCMD_LINE_SWITCH, "l2", "l2", "layout using Planarization algorithm" },
		{ wxCMD_LINE_SWITCH, "l3", "l3", "layout using Sugiyama algorithm" },
		{ wxCMD_LINE_SWITCH, "p1", "p1", "Export to EPS" },

		{ wxCMD_LINE_NONE } };
#else
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, wxT("hw"), wxT("help"), wxT("show this help message"), wxCMD_LINE_VAL_NONE,
		wxCMD_LINE_OPTION_HELP },

		{ wxCMD_LINE_PARAM, NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE
		| wxCMD_LINE_PARAM_OPTIONAL },

		{ wxCMD_LINE_OPTION, "l", "layout", "specify layout algorithm",wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_OPTION, "p", "export", "1: EPS Export",wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_SWITCH, "s", "Save", "save" },
		{ wxCMD_LINE_SWITCH, "c", "Close", "close" },
		{ wxCMD_LINE_SWITCH, "v", "Version", "version" },
		{ wxCMD_LINE_SWITCH, "l1", "l1", "layout using FMMM algorithm" },
		{ wxCMD_LINE_SWITCH, "l2", "l2", "layout using Planarization algorithm" },
		{ wxCMD_LINE_SWITCH, "l3", "l3", "layout using Sugiyama algorithm" },
		{ wxCMD_LINE_SWITCH, "p1", "p1", "Export to EPS" },
		{ wxCMD_LINE_NONE } };
#endif
	wxCmdLineParser parser(cmdLineDesc, argc, argv);

	int parsRes = parser.Parse();

	if (parsRes > 0)
	{
		SP_MESSAGEBOX(wxT("Syntax error detected, aborting."),
			wxT("Error in command line"), wxOK | wxICON_EXCLAMATION);
		return FALSE;
	}



	if (parsRes == -1)
	{
		SP_LOGMESSAGE(wxT("Help was provided, terminating."));
		return FALSE;
	}
	else if (parsRes > 0)
	{
		SP_MESSAGEBOX(wxT("Syntax error detected, aborting."),
			wxT("Error in command line"), wxOK | wxICON_EXCLAMATION);
		return FALSE;
	}

	auto args = parser.GetArguments();

	wxString l_sArgs;
	for (auto it1 = args.begin(); it1 != args.end(); ++it1)
	{
		l_sArgs << (*it1).GetShortName();

	}
	for (int i = 0; i < parser.GetParamCount(); i++)
	{
		p_aargs.Add(parser.GetParam(i));
	}

	long l_nx, l_np;
	wxString s;

	if (parser.Found(wxT("v")))// Snoopy's version
	{
		SP_MESSAGEBOX(SP_APP_REVISION +"\n" + SP_APP_RELEASE);
		return false;
	}

	if (l_sArgs == wxT("c"))//a case with only close option is provided/ (beside to th efile name)
	{
		return false;
	}

	if (parser.Found(wxT("l"), &l_nx) || parser.Found(wxT("c"))||parser.Found(wxT("l1p1"))|| parser.Found(wxT("l2p1"))|| parser.Found(wxT("l3p1")))
	{
		wxString cmdFilename = parser.GetParam(0);

		// Check for a project filename
		if (parser.GetParamCount() > 0)
		{

			wxString cmdFilename = parser.GetParam(0);

			bool l_bISAndl = false;

			if (cmdFilename.AfterLast('.').IsSameAs("andl"))
			{
				l_bISAndl = true;
			}

			m_pcCanvasNormalBrush = wxTheBrushList->FindOrCreateBrush(wxColour(255, 255, 255));
			m_pcCanvasEdgeBrush = wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0));
			m_pcCanvasCoarseBrush = wxTheBrushList->FindOrCreateBrush(wxColour(64, 64, 255));

			m_pcCanvasNormalPen = wxThePenList->FindOrCreatePen(wxColour(0, 0, 0), 1);
			m_pcCanvasCoarsePen = wxThePenList->FindOrCreatePen(wxColour(64, 64, 255), 1);

			m_pcDocmanager = new SP_GM_Docmanager();
			m_pcExportManager = new SP_ExportManager();

			SP_ExportRoutine *l_pcExport = new SP_ExportEPS();


			if (!m_pcExportManager->AddExportRoutine(l_pcExport))
			{
				//wxDELETE(l_pcExport);
			}

			m_pcIAManager = new SP_IA_Manager();
			m_pcImportManager = new SP_ImportManager();
			SP_ImportRoutine *l_pcImport;

			l_pcImport = new SP_ImportANDL();
			if (!m_pcImportManager->AddImportRoutine(l_pcImport))
			{
				//wxDELETE(l_pcImport);
			}


			m_pcCanvasPrefs = new SP_GPR_Canvas();
			m_pcAnimationPrefs = new SP_GPR_Animation();
			m_pcFontPrefs = new SP_GPR_Fonts();
			m_pcElementPrefs = new SP_GPR_Elements();
			SP_Core* l_pcCore = SP_Core::Instance();

			l_pcCore->InitialiseNetclasses(m_pcDocmanager);
			ReadOptions();
			wxOGLInitialize();

			m_pcMainframe = new SP_GUI_Mainframe(
				m_pcDocmanager,
				NULL,
				SP_APP_LONG_NAME,
				wxPoint(m_nXPos, m_nYPos),
				wxSize(m_nWidth, m_nHeight));
			m_pcMainframe->Show(FALSE);
			wxDocument* l_pcDoc;

			if (!l_bISAndl)
			{
				l_pcDoc = m_pcDocmanager->CreateDocument(cmdFilename, wxDOC_SILENT);

				SP_GM_DocTemplate* l_pcDocTemplate = NULL;

				if (l_pcDoc)
				{
					l_pcDocTemplate = dynamic_cast<SP_GM_DocTemplate*>(l_pcDoc->GetDocumentTemplate());
				}

				if (l_pcDocTemplate)
				{
					SP_DS_Netclass* l_pcNS = l_pcDocTemplate->GetNetclass();
					SP_MDI_Doc* l_pcMyDoc = dynamic_cast<SP_MDI_Doc*>(l_pcDoc);
					l_pcMyDoc->InitGraph(l_pcNS);
					SP_DS_Graph* l_pcGraph = l_pcMyDoc->GetGraph();
					SP_DLG_LayoutProperties *l_pcDlg = new SP_DLG_LayoutProperties(NULL, l_pcMyDoc);


					if (l_nx == 1 || parser.Found(wxT("l1")))
					{

						l_pcDlg->SetLayoutAlgo(1);

					}
					else if (l_nx == 2 || parser.Found(wxT("l2"))) //(parser.Found(wxT("l2")))
					{

						l_pcDlg->SetLayoutAlgo(2);
					}
					else
					{
						l_pcDlg->SetLayoutAlgo(3);
					}

					l_pcDlg->Destroy();
					wxString l_sSave = cmdFilename;

					if (parser.Found(wxT("p"), &l_np) || parser.Found(wxT("p1")))//do export to eps
					{
						if (l_np == 1 || parser.Found(wxT("p1")) )
						{
							cmdFilename << wxT(".eps");
							l_pcExport->Write(l_pcMyDoc, cmdFilename);
						}
					}

					if (parser.Found(wxT("s")) )//save the orginal pn file after layout
					{
						l_pcMyDoc->OnSaveDocument(l_sSave);
					}
					wxDELETE(l_pcExport);
				}

			}
			else
			{
				SP_DS_Graph* l_pcGraph = l_pcImport->ImportToDoc(cmdFilename);

				if (!l_pcGraph) return false;

				SP_MDI_Doc* l_pcMyDoc = l_pcGraph->GetParentDoc();

				SP_DLG_LayoutProperties *l_pcDlg = new SP_DLG_LayoutProperties(NULL, l_pcMyDoc);

				if (l_nx == 1 || parser.Found(wxT("l1")))
				{

					l_pcDlg->SetLayoutAlgo(1);
				}
				else if (l_nx == 2 || parser.Found(wxT("l2")))
				{

					l_pcDlg->SetLayoutAlgo(2);
				}
				else
				{
					l_pcDlg->SetLayoutAlgo(3);
				}

				l_pcDlg->Destroy();
				wxString l_sSave = cmdFilename;

				if (parser.Found(wxT("p"), &l_np) || parser.Found(wxT("p1")))
				{
					if (l_np == 1 || parser.Found(wxT("p1")))//do export to eps
					{
						cmdFilename << wxT(".eps");
						l_pcExport->Write(l_pcMyDoc, cmdFilename);
					}
				}


				if (parser.Found(wxT("s")))//save the orginal pn file after layout
				{
					l_sSave << wxT(".");
					l_sSave << l_pcGraph->GetNetclass()->GetExtension();//get the ext of the target file

					l_pcMyDoc->OnSaveDocument(l_sSave);
				}
				wxDELETE(l_pcExport);
			}
			return false;
		}
		else
		{
			SP_MESSAGEBOX(wxT("file name is mandatroy"), wxT("Param missing"), wxOK | wxICON_EXCLAMATION);
			return false;
		}
	}

	return true;


}

// Initialise this in OnInit, not statically
bool Snoopy::OnInit()
{
	//first of all we switch to a default locale
	localeSave= wxString(setlocale(LC_ALL, NULL), wxConvUTF8);
	setlocale(LC_ALL, "C");

	// let us first handle the command line parameters
	wxArrayString l_aargs;

	if (!ProcessCommandLine( l_aargs)) // let us handle command line first
	{
		return false;
	}

	::wxInitAllImageHandlers();
	
	wxFileSystem::AddHandler(new wxZipFSHandler);

	m_pcCanvasNormalBrush = wxTheBrushList->FindOrCreateBrush(wxColour(255,	255, 255));
	m_pcCanvasEdgeBrush = wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0));
	m_pcCanvasCoarseBrush = wxTheBrushList->FindOrCreateBrush(wxColour(64, 64, 255));

	m_pcCanvasNormalPen = wxThePenList->FindOrCreatePen(wxColour(0, 0, 0), 1);
	m_pcCanvasCoarsePen = wxThePenList->FindOrCreatePen(wxColour(64, 64, 255), 1);

	m_pcDocmanager = new SP_GM_Docmanager();

	m_pcExportManager = new SP_ExportManager();
	SP_ExportRoutine *l_pcExport = new SP_ExportEPS();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportLatex();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportReport();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2LaTeX();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportMIF();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportXfig();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportStochPed2Ped();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportStochPed2ExtPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportStochPed2ContPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2StochPed;
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportANDL();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportCANDL_new();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportAPNN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColPN2ANDL();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColPN2ANDL_new();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColPN2APNN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColPN2SBML();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportSMART();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportGCM();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportINA();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportLoLA();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportMaria();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2METATOOL();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		delete l_pcExport;
	}

	l_pcExport = new SP_ExportContODE2Text();
    if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			delete l_pcExport;
	}

	l_pcExport = new SP_ExportContODE2Octave();
    if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			delete l_pcExport;
	}

	l_pcExport = new SP_ExportCPN2Matlab();
    if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			delete l_pcExport;
	}

    l_pcExport = new SP_ExportLLPEP();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportMETATOOL();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		delete l_pcExport;
	}

	l_pcExport = new SP_ExportProd();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportTINA();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2SBML();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportSyRePa();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportTIM();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportTMD();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPed2ContPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPed2ExtPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPed2TimePT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportStochPed2TimePT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPed2StochPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportExtPT2StochPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2ExtPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2Ped();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2PedConvert();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2PedRed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportContPed2PedConservative();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportExtPT2ContPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportExtPT2Ped();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportFTree2ExtFTree();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportTimePT2Ped();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPed2ModuloNets();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportModuloNets2Ped();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportMusicPed2Ped();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPed2MusicPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPNML();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportSBML();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPRISM();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColExtPT2ExtPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}	

	l_pcExport = new SP_ExportColPT2PT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}	

	l_pcExport = new SP_ExportContPed2ColCPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}	

	l_pcExport = new SP_ExportColSPN2ColCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colSPN==>colFSPN
	l_pcExport = new SP_ExportColSPN2ColFSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	
	//colCPN==>colFCPN
	l_pcExport = new SP_ExportColCPN2ColFCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
		//colHPN==>colFHPN
		l_pcExport = new SP_ExportColHPN2ColFHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colFHPN==>colHPN
	l_pcExport = new SP_ExportColFHPN2ColHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	//colFCPN==>colCPN
	l_pcExport = new SP_ExportColFCPN2ColCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colFSPN==>colSPN
	l_pcExport = new SP_ExportColFSPN2ColSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	//colFCPN==>FCPN
	l_pcExport = new SP_ExportColFCPN2FCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	

	//colFSPN==>FSPN
	l_pcExport = new SP_ExportColFSPN2FSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colFHPN==>FHPN
	l_pcExport = new SP_ExportColFHPN2FHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colFHPN==>colFSPN
	l_pcExport = new SP_ExportColFHPN2ColFSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colFHPN==>colFCPN
	l_pcExport = new SP_ExportColFHPN2ColFCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	//colFSPN ==> colFHPN
	l_pcExport = new SP_ExportColFSPN2ColFHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colFCPN ==> colFHPN
	l_pcExport = new SP_ExportColFCPN2ColFHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	//colFCPN ==> colFSPN
	l_pcExport = new SP_ExportColFCPN2ColFSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	//colFSPN ==> colFCPN
	l_pcExport = new SP_ExportColFSPN2ColFCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColCPN2ContPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}	

	l_pcExport = new SP_ExportColStPN2StochPed();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}	

	l_pcExport = new SP_ExportExtPT2ColExtPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportStochPed2ColSPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportPed2ColPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColStPN2ColExPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColCPN2ColExtPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColCPN2ColPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	
	l_pcExport = new SP_ExportColStPN2ColPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColExtPT2ColStPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColExtPT2ColCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColExtPT2ColPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColPT2ColExtPT();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColPT2ColStPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColPT2ColCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColCPN2ColSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColHPN2ColCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColHPN2ColSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColHPN2ColEPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColHPN2ColPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColHPN2HybridPN();
	if( !m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColSPN2ColHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColCPN2ColHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColEPN2ColHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColPN2ColHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportHybridPN2ColHybridPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColExtPT2CPNTool();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new SP_ExportColPT2Structure();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColSPN2CSV();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColPN2SSC;
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

/*
	l_pcExport = new SP_ExportColExtPT2CSV();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColPT2CSV();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportColCPN2CSV();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
*/

	l_pcExport = new SP_ExportCSV();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportFAU();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportModelica();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_Export2HPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportHPN2CPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}
	//SP_ExportHPN2SPN
	l_pcExport = new SP_ExportHPN2SPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportTPN2Matlab();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}
	l_pcExport = new SP_ExportSPN2Matlab();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportERODE();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}

#ifdef __WXDEBUG__
	l_pcExport = new SP_ExportCPP();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportCUDA();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
			wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportRomeo();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
#endif
	
	l_pcExport = new SP_ExportFspn2SPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	l_pcExport = new SP_ExportSPN2FSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
	l_pcExport = new  SP_ExportFCPN2CPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new  SP_ExportCPN2FCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	l_pcExport = new  SP_ExportFHPN2HPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	l_pcExport = new SP_ExportHPN2FHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

 	l_pcExport = new SP_ExportFSPN2FCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	l_pcExport = new SP_ExportFSPN2FHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	l_pcExport = new SP_ExportFCPN2FHPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportFHPN2FSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}
	
		l_pcExport = new SP_ExportFHPN2FCPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}

	l_pcExport = new SP_ExportFCPN2FSPN();
	if (!m_pcExportManager->AddExportRoutine(l_pcExport))
	{
		wxDELETE(l_pcExport);
	}





	m_pcIAManager = new SP_IA_Manager();
	m_pcImportManager = new SP_ImportManager();
	SP_ImportRoutine *l_pcImport;

	l_pcImport = new SP_ImportANDL();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}

	l_pcImport = new SP_ImportCANDL();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}

	l_pcImport = new SP_ImportPNML();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}

	l_pcImport = new SP_ImportPNML2ColPN();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}

	l_pcImport = new SP_ImportAPNN();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}

	l_pcImport = new SP_ImportCSV2ColPN();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}

	l_pcImport = new SP_ImportPED();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}
	l_pcImport = new SP_ImportTINA();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}
	l_pcImport = new SP_ImportSBML2extPN();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}
	l_pcImport = new SP_ImportSBML2cntPn();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}
	l_pcImport = new SP_ImportSBML2sPn();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}
	l_pcImport = new SP_ImportCSV2SPN();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}
	l_pcImport = new SP_ImportFTree_DNF();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}
	l_pcImport = new SP_ImportERODE();
	if (!m_pcImportManager->AddImportRoutine(l_pcImport))
	{
		wxDELETE(l_pcImport);
	}


	//create preferences (must be done before initializing any net classes
	m_pcCanvasPrefs = new SP_GPR_Canvas();
	m_pcAnimationPrefs = new SP_GPR_Animation();
	m_pcFontPrefs = new SP_GPR_Fonts();
	m_pcElementPrefs = new SP_GPR_Elements();

	// Init the nets
	SP_Core* l_pcCore = SP_Core::Instance();
	l_pcCore->InitialiseNetclasses(m_pcDocmanager);

	ReadOptions();
	wxOGLInitialize();

	m_pcMainframe = new SP_GUI_Mainframe(
			m_pcDocmanager,
			NULL,
			SP_APP_LONG_NAME,
			wxPoint(m_nXPos, m_nYPos),
			wxSize(m_nWidth, m_nHeight));

	for (size_t i = 0; i < l_aargs.Count(); i++)
	{
		m_pcDocmanager->CreateDocument(l_aargs[i], wxDOC_SILENT);
	}

	m_pcMainframe->Show(TRUE);

#ifndef __WXGTK__
	m_pcMainframe->Maximize(m_bMaximize);
#endif
	SetTopWindow(m_pcMainframe);


	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), 1); 

	// for the file history
	ReadHistory();

	//Check for new version
	SP_DLG_CheckForUpdate l_cDlg(m_pcMainframe);
	l_cDlg.CheckForUpdate(false);

	// enter mainloop
	return TRUE;
}

int Snoopy::OnExit()
{
	WriteOptions();

	wxDELETE(m_pcIAManager);
	wxDELETE(m_pcDocmanager);
	wxDELETE(m_pcExportManager);
	wxDELETE(m_pcImportManager);

	SP_Core* l_pcCore = SP_Core::Instance();
	l_pcCore->CleanUpAnimation();
	wxDELETE(l_pcCore);

	wxOGLCleanUp();

	wxDELETE(m_pcCanvasPrefs);
	wxDELETE(m_pcAnimationPrefs);
	wxDELETE(m_pcElementPrefs);
	wxDELETE(m_pcFontPrefs);

	setlocale( LC_ALL, localeSave.ToAscii());

	return 0;
}

// Read/write configuration information
bool Snoopy::ReadOptions()
{
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);

	config.Read(wxT("mainX"), &m_nXPos);
	config.Read(wxT("mainY"), &m_nYPos);
	config.Read(wxT("mainWidth"), &m_nWidth);
	config.Read(wxT("mainHeight"), &m_nHeight);
	config.Read(wxT("isMaximized"), &m_bMaximize);

	int style;
	config.Read(wxT("LogicBrush"), &style);
	SetCanvasLogicBrush(style);

	m_pcCanvasPrefs->UpdateFromConfig(config);
	m_pcAnimationPrefs->UpdateFromConfig(config);
	m_pcFontPrefs->UpdateFromConfig(config);
	m_pcElementPrefs->UpdateFromConfig(config);

	wxLog::SetLogLevel(wxLOG_Info);
	wxLog::SetVerbose(m_pcCanvasPrefs->GetLogVerbose());

	return TRUE;
}

// Read/write configuration information
bool Snoopy::ReadHistory()
{
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);
	if (m_pcDocmanager)
		m_pcDocmanager->FileHistoryLoad(config);

	return TRUE;
}

bool Snoopy::WriteOptions()
{
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);

	if (!m_bMaximize)
	{
		if (m_nXPos>0 && m_nYPos>0)
		{
			config.Write(wxT("mainX"), (long)m_nXPos);
			config.Write(wxT("mainY"), (long)m_nYPos);
			config.Write(wxT("mainWidth"), (long)m_nWidth);
			config.Write(wxT("mainHeight"), (long)m_nHeight);
		}
	}
	config.Write(wxT("isMaximized"), (bool)m_bMaximize);
	config.Write(wxT("LogicBrush"), (long)m_pcCanvasLogicBrush->GetStyle());

	//write global preferences to config
	m_pcCanvasPrefs->WriteToConfig(config);
	m_pcAnimationPrefs->WriteToConfig(config);
	m_pcElementPrefs->WriteToConfig(config);
	m_pcFontPrefs->WriteToConfig(config);

	if (m_pcDocmanager)
		m_pcDocmanager->FileHistorySave(config);

	return TRUE;
}

void Snoopy::SetCanvasLogicBrush(int style)
{
	if (style == wxCROSSDIAG_HATCH)
	{
		m_pcCanvasLogicBrush = wxTheBrushList->FindOrCreateBrush(wxColour(0, 0, 0), wxBRUSHSTYLE_CROSSDIAG_HATCH);
	}
	else
	{
		m_pcCanvasLogicBrush = wxTheBrushList->FindOrCreateBrush(wxColour(192, 192, 192), wxBRUSHSTYLE_SOLID);
	}
}
