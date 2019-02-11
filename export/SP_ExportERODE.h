//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/10/26 $
// Short Description: export to ANDL
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTERODE__
#define __SP_EXPORTERODE__

class SP_ExportPT;
class SP_DLG_ExportProperties;

class SP_ExportERODE: public SP_ExportPT {
	private:
		std::string massActionValue(const std::string &strFun);

	protected:
		virtual bool DoWrite();

	public:
		SP_ExportERODE();
		virtual ~SP_ExportERODE();

		virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg,
								 SP_MDI_Doc* p_pcDoc);
		virtual bool OnDialogOk();


		virtual bool AcceptsDoc(SP_MDI_Doc*);
		//  virtual bool Write(SP_MDI_Doc*, const wxString&);

		inline virtual wxString GetName()
		{ return wxT("Export to ERODE");}
		inline virtual wxString GetDescr()
		{ return wxT("Export a Petri Net in ODE RN Format");}
		virtual wxString GetExtension()
		{ return wxT("ode");}
		virtual wxString GetFileDescr()
		{ return wxT("ERODE Files");}

	// Teamplates
	private:
		template <
					class MapTransitionsId,
					class MapPreTranstions,
					class MapPostTranstions,
					class MapPlacesId,
					class Arc
				>
		void appendTransition(
				const MapTransitionsId& mapTransiations,
				const MapPreTranstions& mapPreTransiations,
				const MapPostTranstions& mapPostTransiations,
				const MapPlacesId& mapPlacesId,
				const std::string &INDENT1,
				const std::string &INDENT2,
				std::ostringstream &outStream
				) {

			std::list<Arc*>* pListPreArc = NULL;
			std::list<Arc*>* pListPostArc = NULL;


			for (auto transitionPair : mapTransiations) {
				bool bFirstPlace = true;
				auto* pTransition = transitionPair.second;
				/*
				 * map.at() because operator [] is not const and allow
				 * modify the map
				 */
				try {
					pListPreArc = mapPreTransiations.at(pTransition->m_id);
					pListPostArc = mapPostTransiations.at(pTransition->m_id);
				} catch(const std::out_of_range& oorExcpeption) {
					// no pre or post
					continue;
				}
				// reagents
				outStream << INDENT2;
				for(auto pPreArc : *pListPreArc) {
//					SP_ContPlace *prePlace = m_contPlaceIdMap[pPreArc->m_id];
					auto* pPrePlace = mapPlacesId.at(pPreArc->m_id);
					//TODO Multiplicity
					outStream << (bFirstPlace ? "" : " + ") << pPrePlace->m_name;
					bFirstPlace = false;
				}
				// product
				bFirstPlace = true;
				outStream << " -> ";
				for(auto pPostArc : *pListPostArc) {
//					SP_ContPlace* postPlace = m_contPlaceIdMap[pPostArc->m_id];
					auto* pPostPlace = mapPlacesId.at(pPostArc->m_id);
					//TODO Multiplicity
					outStream << (bFirstPlace ? "" : " + ") << pPostPlace->m_name;
					bFirstPlace = false;
				}

				outStream << ", " << massActionValue(pTransition->m_function)
						  << " [" << pTransition->m_name << "]"<< std::endl;
			}
		}


};

#endif // __SP_EXPORTERODE__


