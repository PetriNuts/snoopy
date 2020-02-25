/***********************************/
/* $Source: $*/
/* $Author: George Assaf $  */
/* $Version: 0.0 $ */
/* $Revision: 1.0 $
/* $Date: 2018/10/16 11:20:00 $ */
/*SP_DS_UnUsedDeclaration.cpp */
/* Short Description:implementation file */
/***********************************/

#include "SP_DS_UnUsedDeclaration.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include "sp_core/SP_Core.h"
#include <sstream>
#include <iostream>

/*construvtor*/
SP_DS_UnUsedDeclaration::SP_DS_UnUsedDeclaration(SP_DS_Graph* p_pcGraph, SP_VectorString p_vUserDec) :m_pcGraph(p_pcGraph), m_vUserDec(p_vUserDec) {}


/*find un-used constants for (coloured) Petri net classes in declaration tree*/
std::set<wxString>  SP_DS_UnUsedDeclaration::FindUnusedConstants()
{
	std::set<wxString> l_setRes;
	std::set<wxString> l_setAllUsedConstants;
	bool l_bIsColoured = false;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}
	/*scan all used constants in marking, multipl., fun bodys and fun calls*/
	std::set<wxString> l_setConstantsInMarking = ReadNetMarking();
	std::set<wxString> l_setConstantsInMultiplicities;
	std::set<wxString> l_setConstantsInFunctionRates;
	std::set<wxString> l_setConstantsInObservers;
	set<wxString>     l_setConstinColorSetsDef;
	set<string>      l_setArcConstVar;
	if (l_bIsColoured == false)
	{
		l_setConstantsInMultiplicities = ReadNetMultiplicity();
		l_setConstantsInFunctionRates = ReadTransitionParameters();//read constants used in funcs calls from transitions
		l_setConstantsInObservers = ReadUsedConstantsInObservers();

	}
	else {
		l_setArcConstVar = ReadColPNVariables();
		l_setConstinColorSetsDef = ReadUsedConstantsinColorSetsDef();
	}

	std::set<wxString> l_setConstantsInFunBody = ReadConstantsInFunctionDef();
	l_setAllUsedConstants.insert(l_setConstantsInMarking.begin(), l_setConstantsInMarking.end());
	l_setAllUsedConstants.insert(l_setConstantsInMultiplicities.begin(), l_setConstantsInMultiplicities.end());
	l_setAllUsedConstants.insert(l_setConstantsInFunctionRates.begin(), l_setConstantsInFunctionRates.end());
	l_setAllUsedConstants.insert(l_setConstantsInFunBody.begin(), l_setConstantsInFunBody.end());
	l_setAllUsedConstants.insert(l_setConstantsInObservers.begin(), l_setConstantsInObservers.end());



	map<wxString, set<wxString>> l_mFun2Constants = ObtainFun2ConstantsInDEf();
	set<wxString>                l_setUnUsedFun = FindUnusedFunctions();
	set<wxString> l_ConstInFunBodyies;//which must be removed from set of used constants
	for (auto it_map = l_mFun2Constants.begin(); it_map != l_mFun2Constants.end(); ++it_map)
	{
		string l_sToFind = (it_map->first).ToStdString();
		auto findFun = l_setUnUsedFun.find(l_sToFind);
		if (findFun != l_setUnUsedFun.end())
		{
			l_ConstInFunBodyies.insert(it_map->second.begin(), it_map->second.end());
		}
	}
	for (auto it_set = l_ConstInFunBodyies.begin(); it_set != l_ConstInFunBodyies.end(); ++it_set)
	{//remove the used constants which used to define function which is not in use by the net
		auto findC = l_setAllUsedConstants.find(*it_set);
		if (findC != l_setAllUsedConstants.end())
		{
			auto findINMArking = l_setConstantsInMarking.find(*it_set);
			auto findInMulti = l_setConstantsInMultiplicities.find(*it_set);
			auto findInfunCall = l_setConstantsInFunctionRates.find(*it_set);
			if (findINMArking == l_setConstantsInMarking.end() && findInMulti == l_setConstantsInMultiplicities.end() && findInfunCall == l_setConstantsInFunctionRates.end())
			{
				l_setAllUsedConstants.erase(*it_set);
			}
		}

	}
	/***************/
	if (!l_bIsColoured) {//observers only in Uncolored PN
		map<wxString, set<wxString>> l_mObserver2Const = ObtainObservers2Constants();
		set<wxString> l_setUnusedObs = FindUnusedObservers();
		set<wxString> l_ConstInObserversBodies;//which must be removed from set of used constants
		for (auto it_map = l_mObserver2Const.begin(); it_map != l_mObserver2Const.end(); ++it_map)
		{
			string l_sToFind = (it_map->first).ToStdString();
			auto findFun = l_setUnusedObs.find(it_map->first);
			if (findFun != l_setUnusedObs.end())
			{
				l_ConstInObserversBodies.insert(it_map->second.begin(), it_map->second.end());
			}
		}

		for (auto it_set = l_ConstInObserversBodies.begin(); it_set != l_ConstInObserversBodies.end(); ++it_set)
		{//remove the used constants which used to define function which is not in use by the net
			auto findC = l_setAllUsedConstants.find(*it_set);
			if (findC != l_setAllUsedConstants.end())
			{//if it the const not in marking/multiplicities/fun call , then it is un used even it is used inside observer body
				auto findINMArking = l_setConstantsInMarking.find(*it_set);
				auto findInMulti = l_setConstantsInMultiplicities.find(*it_set);
				auto findInfunCall = l_setConstantsInFunctionRates.find(*it_set);
				if (findINMArking == l_setConstantsInMarking.end() && findInMulti == l_setConstantsInMultiplicities.end() && findInfunCall == l_setConstantsInFunctionRates.end())
				{
					l_setAllUsedConstants.erase(*it_set);
				}
			}
		}

	}


	if (l_bIsColoured)
	{

		l_setAllUsedConstants.insert(l_setArcConstVar.begin(), l_setArcConstVar.end());
		l_setAllUsedConstants.insert(l_setConstinColorSetsDef.begin(), l_setConstinColorSetsDef.end());
	}

	wxString l_sCon = wxT("Constants");
	std::map<wxString, wxString> l_vUserDec = TrimUserDec(l_sCon);//all defined decl

	for (auto it_v = l_vUserDec.begin(); it_v != l_vUserDec.end(); ++it_v)
	{
		wxString l_sDec = it_v->second.Trim();
		auto it_s = l_setAllUsedConstants.find(l_sDec);
		if (it_s == l_setAllUsedConstants.end())
		{
			l_setRes.insert(l_sDec);
		}
	}

	return l_setRes;
}

/*find unused functions in (coloured) Petri net classes*/
std::set<wxString>   SP_DS_UnUsedDeclaration::FindUnusedFunctions()
{
	std::set<wxString> l_setResult;
	wxString l_sDec = wxT("Functions");
	std::map<wxString, wxString> l_vUserDec = TrimUserDec(l_sDec);//all defined dec
	bool l_bIsColoured = false;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();

	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}

	if (!l_bIsColoured)
	{
		//uncolored PN classes
		std::set<wxString> l_setUsedfun = ReadTransitionFunctions();

		std::set<wxString> l_setMarking = ReadNetMarking();


		std::set<wxString> l_setMul = ReadNetMultiplicity();

		l_setMarking.insert(l_setMul.begin(), l_setMul.end());//merge multiplicities and markings

		 //search also if some funs used to initialise some constants
		map<wxString, wxString> l_mConst2Val = ObtainAllDefinedConstants2Valuses();

		for (auto it_mCon2Val = l_mConst2Val.begin(); it_mCon2Val != l_mConst2Val.end(); ++it_mCon2Val)
		{
			set<wxString> l_setConstantsInFunctionRates = ReadTransitionParameters();
			auto l_itISConstinParams = l_setConstantsInFunctionRates.find(it_mCon2Val->first);
			auto l_itIsConstantused = l_setMarking.find(it_mCon2Val->first);
			if (l_itIsConstantused != l_setMarking.end() || l_itISConstinParams != l_setConstantsInFunctionRates.end())
				l_setMarking.insert(it_mCon2Val->second);
		}

		set<wxString> l_setUnusedOb = FindUnusedObservers();

		//search also if some funs used to initialise some observers bodies
		map<wxString, wxString> l_mObserver2Body = ReadObserverBodies();

		for (auto it_mOb2Body = l_mObserver2Body.begin(); it_mOb2Body != l_mObserver2Body.end(); ++it_mOb2Body)
		{
			string l_sObName = it_mOb2Body->first;
			if (l_setUnusedOb.find(l_sObName) == l_setUnusedOb.end())
			{
				l_setMarking.insert(it_mOb2Body->second);
			}
		}

		//to extract function names if the fun is used to initialise marking or Arcs multiplicities
		for (auto it_setMarking = l_setMarking.begin(); it_setMarking != l_setMarking.end(); ++it_setMarking)
		{
			std::string str = (*it_setMarking).ToStdString();
			typedef boost::tokenizer<boost::char_separator<char> >
				tokenizer;
			boost::char_separator<char> sep("-|+*()[]{}/&=<>, ");
			tokenizer tokens(str, sep);
			for (tokenizer::iterator tok_iter = tokens.begin();
				tok_iter != tokens.end(); ++tok_iter)
			{
				wxString l_tkn = *tok_iter;
				bool l_bisNum = false;
				if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
				{
					l_bisNum = true;
				}
				if (!l_bisNum)
				{
					l_setUsedfun.insert(*tok_iter);
				}

			}
		}



		for (auto it_v = l_vUserDec.begin(); it_v != l_vUserDec.end(); ++it_v)
		{
			string l_sDec = (it_v->second).ToStdString();
			auto it_s = l_setUsedfun.find(l_sDec);
			if (it_s == l_setUsedfun.end())
			{
				l_setResult.insert(l_sDec);
			}
		}


	}
	else {
		//colored PN classes
		std::set<string> l_setUsedFunsColPN;

		l_setUsedFunsColPN = ReadUsedColouredFunctions();

		set<wxString> l_setMarking = ReadNetMarking();//added 06.12.19 function can be used to determine colour in initial marking 

		for (auto l_it = l_setMarking.begin(); l_it != l_setMarking.end(); ++l_it)
		{
			string l_sToFind = (*l_it).ToStdString();
			if (l_vUserDec.find(l_sToFind) != l_vUserDec.end())
			{
				l_setUsedFunsColPN.insert(l_sToFind);

			}
		}

		for (auto it_v = l_vUserDec.begin(); it_v != l_vUserDec.end(); ++it_v)
		{
			wxString l_sDec = it_v->second;
			auto it_s = l_setUsedFunsColPN.find(l_sDec);
			if (it_s == l_setUsedFunsColPN.end())
			{
				l_setResult.insert(l_sDec);
			}
		}

	}

	return l_setResult;
}

/*return set of all decalarations used in the predictaes of col PN*/
set<wxString> SP_DS_UnUsedDeclaration::GetDeclarationsInPredicates()
{
	SP_ListNode::const_iterator l_itElem;
	SP_ListEdge::const_iterator l_itEdges;
	set<wxString> l_setPredicates;
	wxString l_sNetClass = m_pcGraph->GetNetclass()->GetName();

	if ((l_sNetClass == SP_DS_COLSPN_CLASS) ||
		(l_sNetClass == SP_DS_COLCPN_CLASS) ||
		(l_sNetClass == SP_DS_COLHPN_CLASS))
	{

		SP_VectorString l_vTransNC = { SP_DS_CONTINUOUS_TRANS ,SP_DS_STOCHASTIC_TRANS ,wxT("Immediate Transition") ,wxT("Deterministic Transition") ,wxT("Scheduled Transition") };
		for (auto it_vTrns = l_vTransNC.begin(); it_vTrns != l_vTransNC.end(); ++it_vTrns)
		{
			SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(*it_vTrns);

			if (l_pcNodeclass)
			{
				for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
				{
					SP_DS_Node* l_pcTransNode = (*l_itElem);

					wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();


					SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransNode->GetAttribute(wxT("FunctionList")));

					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						wxString l_sPredicate = wxString(l_pcColList->GetCell(i, 0).c_str());
						l_setPredicates.insert(l_sPredicate);
					}
				}
			}



		}
	}
	return l_setPredicates;

}

/*return set of all unused color sets of given CS type*/
std::set<wxString>   SP_DS_UnUsedDeclaration::FindUnusedColorSets(wxString p_sCSType)
{
	std::set<wxString> l_setResult;

	std::map<wxString, wxString> l_mcolorsets;

	if (p_sCSType == wxT("Alias Color Sets"))
	{
		l_mcolorsets = TrimAliasColorSets(p_sCSType);
	}
	else {
		l_mcolorsets = TrimUserDec(p_sCSType);

	}
	set<string> l_setUsedCS = ReadUsedColorsets();

	for (auto it_v = l_mcolorsets.begin(); it_v != l_mcolorsets.end(); ++it_v)
	{
		string l_sDec = (it_v->second).ToStdString();
		auto it_s = l_setUsedCS.find(l_sDec);
		if (it_s == l_setUsedCS.end())
		{
			l_setResult.insert(l_sDec);
		}
	}


	return l_setResult;
}

/*returns set of color sets used to define vars*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadUsedColorsetsByVariables()const {
	std::set<wxString> l_setRes;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if (!l_pcMetadataclass)
		return l_setRes;

	if (l_pcMetadataclass->GetElements()->empty())
		return l_setRes;

	SP_DS_Metadata*	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute *  l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if (!l_pcColList)
		return l_setRes;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		l_setRes.insert(l_sColors.Trim());
	}

	return l_setRes;

}

/*read constants used as marking in (coloured) Petri net classes*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadNetMarking()
{
	std::set<wxString> p_vNetMarking;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	if (!l_netType.Contains(wxT("Colored")))
	{
		for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
		{
			if (nc->GetShowInElementTree())
			{
				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{

					if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
					{
						for (SP_DS_Node* node1 : *(nc->GetElements()))
						{
							wxString l_sNodeclass = nc->GetDisplayName();
							wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
							wxString l_smarking;
							if (l_sNodeclass == wxT("Continuous Place"))
							{
								//continuous place
								l_smarking = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING)->GetValueString();
							}
							else
							{
								l_smarking = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING)->GetValueString();

							}

							p_vNetMarking.insert(l_smarking.Trim());
						}
					}
				}
			}
		}
	}
	else {
		//coloured net
		for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
		{
			if (nc->GetShowInElementTree())
			{
				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{

					if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
					{
						for (SP_DS_Node* node1 : *(nc->GetElements()))
						{

							wxString l_sNodeclass = nc->GetDisplayName();
							wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
							wxString l_smarking;
							SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(node1->GetAttribute(SP_DS_CPN_MARKINGLIST));
							if (!l_pcColList) { break; }
							int count = l_pcColList->GetColCount();



							for (unsigned int j = 0; j < l_pcColList->GetColCount(); j++)
							{
								map<wxString, SP_CPN_TokenNum> l_mColor2TokenNum;


								for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
								{
									wxString l_sColorExpr = l_pcColList->GetCell(i, j);
									wxString l_sTokenNum = l_pcColList->GetCell(i, j + 1);
									if (l_sTokenNum.CompareTo(wxT("")) != 0)
									{
										std::string str = l_sTokenNum.ToStdString();
										typedef boost::tokenizer<boost::char_separator<char> >
											tokenizer;
										boost::char_separator<char> sep("-|+*()/&=<>,");
										tokenizer tokens(str, sep);
										for (tokenizer::iterator tok_iter = tokens.begin();
											tok_iter != tokens.end(); ++tok_iter)
										{
											wxString l_sTokenNum = *tok_iter;
											l_sTokenNum.Replace(" ", "");
											p_vNetMarking.insert(l_sTokenNum);
										}

									}
									if (l_sTokenNum.CompareTo(wxT("")) != 0)
									{
										std::string str = l_sColorExpr.ToStdString();
										typedef boost::tokenizer<boost::char_separator<char> >
											tokenizer;
										boost::char_separator<char> sep("-|+*()/&=<>,");
										tokenizer tokens(str, sep);
										for (tokenizer::iterator tok_iter = tokens.begin();
											tok_iter != tokens.end(); ++tok_iter)
										{
											wxString l_tkn = *tok_iter;
											bool l_bisNum = false;
											if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
											{
												l_bisNum = true;
											}
											if (!l_bisNum)
											{
												wxString l_sTk = *tok_iter;
												l_sTk.Replace(" ", "");
												p_vNetMarking.insert(l_sTk);
											}

										}
									}
								}
								j++;
							}
						}
					}
				}
			}
		}
	}
	return p_vNetMarking;
}

/*read constants used as arc multiplicities in uncoloured petri net classes*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadNetMultiplicity()//const//25.11
{
	std::set<wxString> p_vMultiplicityVector;
	for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
	{

		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{

				for (SP_DS_Node* node1 : *(nc->GetElements()))
				{

					std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

					for (SP_DS_Edge* l_pcEdge : *(node1->GetSourceEdges()))
					{
						SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
						std::string l_sTarget = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						p_vMultiplicityVector.insert(l_sMult);
					}

				}

			}
		}
	}
	return p_vMultiplicityVector;
}

/*return set of all constants used to define Color sets*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadUsedConstantsinColorSetsDef() const {

	std::set<wxString> l_setConst;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);


	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();

		std::string str = l_sColors.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("-|+*()=<>, ");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			bool l_bisNum = false;
			wxString l_tkn = *tok_iter;
			if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
			{
				l_bisNum = true;
			}
			if (!l_bisNum)
			{
				l_setConst.insert(*tok_iter);
			}

		}

	}
	return l_setConst;

}

/*returns map of defined constants 2 their values*/
map<wxString, wxString> SP_DS_UnUsedDeclaration::ObtainAllDefinedConstants2Valuses()
{

	map<wxString, wxString> l_mConst2Name;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();


	if (l_netType.Contains(wxT("Colored")))
	{
		//SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);

		//SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

		//SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));


		//for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		//{
			//wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
			//wxString l_sValue = l_pcColList->GetCell(i, 2).c_str();

		  //l_mConst2Name[l_sName] = l_sValue;
			 
		//}

		///////////////////////
		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		SP_ListMetadata::const_iterator it;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			SP_DS_Metadata* l_pcConstant = *it;
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
			wxString l_sValue = l_pcColList->GetCell(0, 1);
			int x = l_pcColList->GetRowCount();
			l_mConst2Name[l_sName] = l_sValue;
		}



		////////////////////
	}
	else {
		//uncolored pn

		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);//SP_DS_META_CONSTANT="Constant Class"
		if (l_pcMetadataclass)
		{
			SP_ListMetadata::const_iterator l_itElem;
			for (l_itElem = l_pcMetadataclass->GetElements()->begin();
				l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Metadata* l_pcMetadata = *l_itElem;
				wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));
				for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
				{
					wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
					wxString l_sValue = l_pcColList->GetCell(i, 1).c_str();

					l_mConst2Name[l_sMetadataName] = l_sValue;
				}



			}

		}
	}
	return l_mConst2Name;
}

/*returns set of all defined constants*/
set<wxString> SP_DS_UnUsedDeclaration::ObtainAllDefinedConstants() {

	set<wxString> l_setConst;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();

	if (l_netType.Contains(wxT("Colored")))
	{
			SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
			SP_ListMetadata::const_iterator it;

			for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
			{
				SP_DS_Metadata* l_pcConstant = *it;
				wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				 
				l_setConst.insert(l_sName);
			}
			 
		 
		/*
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);

		SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));


		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sName = l_pcColList->GetCell(i, 0).c_str();

			l_setConst.insert(l_sName);

		}
		*/
	}
	else {
		//uncolored pn

		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);//SP_DS_META_CONSTANT="Constant Class"
		if (l_pcMetadataclass)
		{
			SP_ListMetadata::const_iterator l_itElem;
			for (l_itElem = l_pcMetadataclass->GetElements()->begin();
				l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Metadata* l_pcMetadata = *l_itElem;
				wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				l_setConst.insert(l_sMetadataName);



			}


		}


	}
	return l_setConst;
}



/*read used constants from body of defined functions for (coloured) Petri net classes*/
std::map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ObtainFun2ConstantsInDEf()
{
	std::map<wxString, set< wxString>> l_mfun2Con;

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();

	set<wxString> l_setAllDefinedConstants = ObtainAllDefinedConstants();
	if (!l_netType.Contains(wxT("Colored")))
	{
		for (SP_DS_Metadataclass* mc : *(m_pcGraph->GetMetadataclasses()))
		{
			if (mc->GetShowInDeclarationTreeOther()
				|| mc->GetShowInDeclarationTreeColorSet())
			{
				if (mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{
					for (SP_DS_Metadata* meta : *(mc->GetElements()))
					{
						wxString l_sMetaclass = mc->GetDisplayName();
						wxString l_sName = meta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
						/**********/
						if (l_sMetaclass == wxT("Functions"))
						{
							set<wxString> l_setConstants;
							wxString l_sName = meta->GetAttribute(wxT("Name"))->GetValueString();
							wxString l_sParamList = meta->GetAttribute(wxT("Parameter"))->GetValueString();
							wxString l_sBody = meta->GetAttribute(wxT("Body"))->GetValueString();
							std::stringstream sstream1(l_sParamList);
							std::string sparam;

							while (getline(sstream1, sparam, ','))
							{
								wxString par = sparam.c_str();
								//p_setConstant.insert(par);
							}

							std::string str = l_sBody.ToStdString();
							typedef boost::tokenizer<boost::char_separator<char> >
								tokenizer;
							boost::char_separator<char> sep("-|+*()%/!&=<>");
							tokenizer tokens(str, sep);
							for (tokenizer::iterator tok_iter = tokens.begin();
								tok_iter != tokens.end(); ++tok_iter)
							{

								l_setConstants.insert(*tok_iter);

							}
							l_mfun2Con[l_sName] = l_setConstants;
						}
					}
				}
			}
		}
	}
	else {

		//coloured pn
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));

		for (int l_nRow = 0; l_nRow < l_pcColList->GetRowCount(); l_nRow++)
		{
			set<wxString> p_setConstant;
			std::set<wxString> l_setTempParams;
			std::set<wxString> l_setTempBodyConst;
			string l_sName = l_pcColList->GetCell(l_nRow, 1).ToStdString();
			string l_sParamList = l_pcColList->GetCell(l_nRow, 2).ToStdString();//parameters
			string l_sFunBody = l_pcColList->GetCell(l_nRow, 3).ToStdString();//body  

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(", ");
			tokenizer tokens(l_sParamList, sep);
			wxString result;
			for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg)
			{

				l_setTempParams.insert(*beg);
			}

			boost::char_separator<char> sep1("(|&=+-!/%)<>");
			tokenizer tokens1(l_sFunBody, sep1);
			for (tokenizer::iterator beg = tokens1.begin(); beg != tokens1.end(); ++beg)
			{
				std::string l_tkn = *beg;
				if (l_tkn.compare("|") != 0 && l_tkn.compare("&") != 0 && l_tkn.compare("=") != 0 && l_tkn.compare("(") != 0 && l_tkn.compare(")") != 0 &&
					l_tkn.compare(">") && l_tkn.compare("!") != 0 && l_tkn.compare("<") != 0 && l_tkn.compare("+") != 0 && l_tkn.compare("-") != 0 && l_tkn.compare("/") != 0 && l_tkn.compare("%") != 0
					)
				{
					bool l_bisNum = false;
					if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
					{
						l_bisNum = true;
					}
					if (!l_bisNum)
					{
						l_setTempBodyConst.insert(l_tkn);
					}
				}
			}

			for (auto it_setBody = l_setTempBodyConst.begin(); it_setBody != l_setTempBodyConst.end(); ++it_setBody)
			{
				string l_sToFind = (*it_setBody).ToStdString();
				auto pos = l_setTempParams.find(l_sToFind);
				if (pos == l_setTempParams.end())
				{
					auto csfind = l_setAllDefinedConstants.find(l_sToFind);
					if (csfind != l_setAllDefinedConstants.end()) {
						p_setConstant.insert(*it_setBody);
					}

				}
			}
			l_mfun2Con[l_sName] = p_setConstant;
		}

	}
	return l_mfun2Con;
}


/*read used constants from body of defined functions for (coloured) Petri net classes*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadConstantsInFunctionDef()
{
	std::set<wxString> p_setConstant;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();

	set<wxString> l_setAllDefinedConstants = ObtainAllDefinedConstants();
	if (!l_netType.Contains(wxT("Colored")))
	{
		for (SP_DS_Metadataclass* mc : *(m_pcGraph->GetMetadataclasses()))
		{
			if (mc->GetShowInDeclarationTreeOther()
				|| mc->GetShowInDeclarationTreeColorSet())
			{
				if (mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{
					for (SP_DS_Metadata* meta : *(mc->GetElements()))
					{
						wxString l_sMetaclass = mc->GetDisplayName();
						wxString l_sName = meta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
						/**********/
						if (l_sMetaclass == wxT("Functions"))
						{

							wxString l_sParamList = meta->GetAttribute(wxT("Parameter"))->GetValueString();
							wxString l_sBody = meta->GetAttribute(wxT("Body"))->GetValueString();
							std::stringstream sstream1(l_sParamList);
							std::string sparam;

							while (getline(sstream1, sparam, ','))
							{
								wxString par = sparam.c_str();
								//p_setConstant.insert(par);
							}

							std::string str = l_sBody.ToStdString();
							typedef boost::tokenizer<boost::char_separator<char> >
								tokenizer;
							boost::char_separator<char> sep("!-|+*()%/&=<>");
							tokenizer tokens(str, sep);
							for (tokenizer::iterator tok_iter = tokens.begin();
								tok_iter != tokens.end(); ++tok_iter)
							{

								p_setConstant.insert(*tok_iter);

							}

						}
					}
				}
			}
		}
	}
	else {
		//coloured pn
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		for (int l_nRow = 0; l_nRow < l_pcColList->GetRowCount(); l_nRow++)
		{
			std::set<wxString> l_setTempParams;
			std::set<wxString> l_setTempBodyConst;
			string l_sParamList = l_pcColList->GetCell(l_nRow, 2).ToStdString();//parameters
			string l_sFunBody = l_pcColList->GetCell(l_nRow, 3).ToStdString();//body  

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(", ");
			tokenizer tokens(l_sParamList, sep);
			wxString result;
			for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg)
			{

				l_setTempParams.insert(*beg);
			}

			boost::char_separator<char> sep1("(|&*=+-/%)<>");
			tokenizer tokens1(l_sFunBody, sep1);
			for (tokenizer::iterator beg = tokens1.begin(); beg != tokens1.end(); ++beg)
			{
				std::string l_tkn = *beg;
				if (l_tkn.compare("|") != 0 && l_tkn.compare("&") != 0 && l_tkn.compare("=") != 0 && l_tkn.compare("(") != 0 && l_tkn.compare(")") != 0 &&
					l_tkn.compare(">") != 0 && l_tkn.compare("<") != 0 && l_tkn.compare("+") != 0 && l_tkn.compare("-") != 0 && l_tkn.compare("/") != 0 && l_tkn.compare("%") != 0
					)
				{
					bool l_bisNum = false;
					if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
					{
						l_bisNum = true;
					}
					if (!l_bisNum)
					{
						l_setTempBodyConst.insert(l_tkn);
					}
				}
			}

			for (auto it_setBody = l_setTempBodyConst.begin(); it_setBody != l_setTempBodyConst.end(); ++it_setBody)
			{
				string l_sToFind = (*it_setBody).ToStdString();
				auto pos = l_setTempParams.find(l_sToFind);
				if (pos == l_setTempParams.end())
				{
					auto csfind = l_setAllDefinedConstants.find(l_sToFind);
					if (csfind != l_setAllDefinedConstants.end()) {
						p_setConstant.insert(*it_setBody);
					}

				}
			}
		}

	}
	return p_setConstant;
}

/*returns set of all called functions in transitions*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadTransitionFunctions() const
{
	std::set<wxString> p_setUsedFunctions;
	for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{


				for (SP_DS_Node* node1 : *(nc->GetElements()))
				{
					wxString l_sNodeclass = nc->GetDisplayName();
					wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
					if (l_sNodeclass.Contains(wxT("Transition")))
					{
						SP_DS_ColListAttribute* pColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(node1->GetAttribute("FunctionList"));

						if (!pColListAttr) return p_setUsedFunctions;
						wxString fun = pColListAttr->GetCell(0, 1);
						std::stringstream sstream1(fun);
						std::string oneFun;

						while (getline(sstream1, oneFun, '('))
						{
							wxString sOneFun = oneFun.c_str();
							if (!sOneFun.Contains(")"))
							{
								p_setUsedFunctions.insert(sOneFun);
							}

						}
					}

				}
			}
		}
	}
	return p_setUsedFunctions;

}

/*returns params(constants) of functions call in transitions*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadTransitionParameters()const
{
	std::vector<string> l_vFunCalls;//store funs calls from transitions 
	std::set<wxString> l_setParams;
	for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{


				for (SP_DS_Node* node1 : *(nc->GetElements()))
				{
					wxString l_sNodeclass = nc->GetDisplayName();
					wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
					if (l_sNodeclass.Contains(wxT("Transition")))
					{
						SP_DS_ColListAttribute* pColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(node1->GetAttribute("FunctionList"));
						if (!pColListAttr)
							return l_setParams;
						wxString funCall = pColListAttr->GetCell(0, 1);
						l_vFunCalls.push_back(funCall.ToStdString());

					}

				}
			}
		}
	}
	/*************************************************/
	std::set<wxString> l_funSet = ReadTransitionFunctions();//get funs calls without params e.g, fun(k,m) => fun


	for (auto it = l_vFunCalls.begin(); it != l_vFunCalls.end(); it++) {

		string l_str = *it;
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("(,)/*-+%");
		tokenizer tokens(l_str, sep);
		wxString result;
		for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg) {
			auto pos = l_funSet.find(*beg);
			if (pos == l_funSet.end())
			{
				l_setParams.insert(*beg);
			}

		}
	}

	return l_setParams;
}

/*returns constants used in observers bodies*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadUsedConstantsInObservers() const
{

	std::set<wxString> l_setPlaces;
	std::set<wxString> l_setUsedConstants;
	std::map<wxString, wxString>  l_mapObservers = ReadObserverBodies();
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	if (!l_netType.Contains(wxT("Colored")))
	{
		for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
		{
			if (nc->GetShowInElementTree())
			{
				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{

					if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
					{
						for (SP_DS_Node* node1 : *(nc->GetElements()))
						{
							wxString l_sNodeclass = nc->GetDisplayName();
							wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
							wxString l_smarking;
							l_setPlaces.insert(l_sName);
						}
					}
				}
			}
		}
	}
	/*********toknize bodies of observers*******/

	for (auto it_map = l_mapObservers.begin(); it_map != l_mapObservers.end(); ++it_map)
	{
		wxString l_sBody = it_map->second;
		std::set<string> l_setTokens;
		std::string str = l_sBody.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep("-|+*()/&,");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_sToken = *tok_iter;
			auto l_it = l_setPlaces.find(l_sToken.Trim());
			if (l_it == l_setPlaces.end())
			{
				l_setTokens.insert(*tok_iter);
			}

		}
		l_setUsedConstants.insert(l_setTokens.begin(), l_setTokens.end());


	}
	return l_setUsedConstants;
}

/*delete a given observer*/
void SP_DS_UnUsedDeclaration::DeleteUnUsedObserver(wxString p_sObserverName)
{
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			if (p_sObserverName == l_sName)
			{
				l_pcMetadataclass->RemoveElement(l_pcMetadata);
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				return;
			}


		}
	}

}

/*returns map of observers 2 related used constants*/
std::map<wxString, set< wxString>>   SP_DS_UnUsedDeclaration::ObtainObservers2Constants()
{
	std::map<wxString, set< wxString>> l_mOb2Const;
	map<wxString, wxString> L_mOb2Body = ReadObserverBodies();
	set<wxString> l_setAllConst = ObtainAllDefinedConstants();

	for (auto it_map = L_mOb2Body.begin(); it_map != L_mOb2Body.end(); ++it_map)
	{
		wxString l_Name = it_map->first;
		wxString l_sBodyOb = it_map->second;
		set<wxString> l_setConstants;
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sepList("(|&=+-/%)<>* ");
		string l_sBody = l_sBodyOb.ToStdString();
		tokenizer Obtokens(l_sBody, sepList);
		for (tokenizer::iterator beg = Obtokens.begin(); beg != Obtokens.end(); ++beg)
		{
			std::string l_tkn = *beg;
			if (l_tkn.compare("|") != 0 && l_tkn.compare("&") != 0 && l_tkn.compare("=") != 0 && l_tkn.compare("(") != 0 && l_tkn.compare(")") != 0 &&
				l_tkn.compare(">") != 0 && l_tkn.compare("<") != 0 && l_tkn.compare("+") != 0 && l_tkn.compare("-") != 0 && l_tkn.compare("/") != 0 && l_tkn.compare("%") != 0
				)
			{
				bool l_bisNum = false;
				if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
				{
					l_bisNum = true;
				}
				if (!l_bisNum)
				{
					auto find_c = l_setAllConst.find(l_tkn);
					if (find_c != l_setAllConst.end())
					{
						l_setConstants.insert(l_tkn);
					}
				}
			}
		}
		l_mOb2Const[l_Name] = l_setConstants;
	}

	return l_mOb2Const;
}

/*returns observer 2 Body*/
std::map<wxString, wxString>   SP_DS_UnUsedDeclaration::ReadObserverBodies()const
{
	std::map<wxString, wxString>  l_setObservers;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();

			l_setObservers[l_sName] = l_sBody;
		}
	}
	return l_setObservers;

}

/*Find unused observers*/
std::set<wxString> SP_DS_UnUsedDeclaration::FindUnusedObservers()const
{
	std::set<wxString> l_setResult, l_setUsedObservers;

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_netType == SP_DS_SIMPLEGRAPH_CLASS | l_netType == SP_DS_BIPARTGRAPH_CLASS | l_netType == SP_DS_PN_CLASS | l_netType == SP_DS_EXTPN_CLASS)
	{
		return l_setResult;
	}
	std::map<wxString, wxString> l_mDefinedObs = ReadObserverBodies();



	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));
	SP_DS_Metadata* m_pcCurrentTablePlot;
	const SP_ListMetadata* l_pcViewList = l_pcMetadataclass->GetElements();
	if (!l_pcViewList->empty())
		//return l_setResult;
		if (l_pcViewList) {


			for (SP_ListMetadata::const_iterator l_itElem = l_pcViewList->begin(); l_itElem != l_pcViewList->end(); ++l_itElem)
			{
				wxString l_sName = (*l_itElem)->GetAttribute(wxT("Name"))->GetValueString();
				m_pcCurrentTablePlot = (*l_itElem);
				SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
				for (unsigned long i = 0; i < l_pcPlaceIdList->GetRowCount(); i++)
				{

					l_sName = l_pcPlaceIdList->GetCell(i, 6);
					l_setUsedObservers.insert(l_sName);

				}
			}
		}

	/**********************check all views for used observers***********************************/
	wxString l_sName;

	//get plot list. We ignore tables in old implementation
	SP_DS_Metadataclass* l_pcMetadataclass1 = m_pcGraph->GetMetadataclass(wxT("Plot"));


	const SP_ListMetadata* l_pcViewList1 = l_pcMetadataclass1->GetElements();

	//read all views
	for (SP_DS_Metadata* l_pcMeta : *l_pcViewList1)
	{
		l_sName = l_pcMeta->GetAttribute(wxT("Name"))->GetValueString();

		SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
		for (unsigned long i = 0; i < l_pcPlaceIdList->GetRowCount(); i++)
		{

			l_sName = l_pcPlaceIdList->GetCell(i, 6);
			l_setUsedObservers.insert(l_sName);

		}

	}

	/*********************************************************/


	for (auto it_map = l_mDefinedObs.begin(); it_map != l_mDefinedObs.end(); ++it_map)
	{
		string l_sToFind = (it_map->first).ToStdString();
		auto it_found = l_setUsedObservers.find(l_sToFind);
		if (it_found == l_setUsedObservers.end())
		{
			l_setResult.insert(it_map->first);
		}
	}



	return l_setResult;
}

/*returns map of Tree naming declarations to origin names*/
std::map<wxString, wxString> SP_DS_UnUsedDeclaration::TrimUserDec(wxString& p_sDec)
{
	 
	std::map<wxString, wxString> l_mTrimUserDec;
	wxString l_sdec;
	auto it_v = m_vUserDec.begin();
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	bool l_bIsColoured = false;
	if (l_netType.Contains(wxT("Colored"))) {
		l_bIsColoured = true;
	}

	///////////////////////////////////////
	while (*it_v != p_sDec)
	{
		++it_v;
	}
	while (++it_v != m_vUserDec.end())
	{
		//	++it_v;
		wxString	l_sdec = *it_v;
		if (l_sdec != wxT("Constants"))
		{
			if (p_sDec == wxT("Constants"))
			{
				if (l_sdec == wxT("Functions") | l_sdec == wxT("Observers"))
				{
					break;
				}
				wxString constant;
				if (!l_sdec.Contains("("))
					constant = l_sdec.Before(':');
				else
				{
					constant = l_sdec.Before('(');
					if (constant.Contains(':'))
						constant = l_sdec.Before(':');
				}
				l_mTrimUserDec[l_sdec] = constant;

			}
			else if (p_sDec == wxT("Functions"))
			{
				if (l_sdec == wxT("Observers") | l_sdec == wxT("Variables"))
				{
					break;
				}
				wxString l_sfun;
				if (!l_netType.Contains(wxT("Colored")))
				{
					l_sfun = l_sdec.Before('(');
					l_mTrimUserDec[l_sdec] = l_sfun.Trim();
				}
				else {
					l_mTrimUserDec[l_sdec] = l_sdec.Trim();
				}


			}
			else if (p_sDec == wxT("Simple Color Sets"))
			{
				if (l_sdec == wxT("Compound Color Sets"))
				{
					break;
				}

				l_mTrimUserDec[l_sdec] = l_sdec;

			}
			else if (p_sDec == wxT("Compound Color Sets"))
			{
				if (l_sdec == wxT("Alias Color Sets"))
				{
					break;
				}

				l_mTrimUserDec[l_sdec] = l_sdec;

			}
			else if (p_sDec == wxT("Alias Color Sets"))
			{


				while (l_sdec != wxT("Constants")) {
					l_mTrimUserDec[l_sdec] = l_sdec;

					l_sdec = *(++it_v);

				}
				break;
			}
			else if (p_sDec == wxT("Variables"))
			{

				l_mTrimUserDec[l_sdec] = l_sdec;

			}
			else {
				wxString obs = (*it_v).Before('=');
				// l_vTrimUserDec.push_back(obs.Trim());
				l_mTrimUserDec[l_sdec] = obs.Trim();

			}
		}
		else if (l_sdec == wxT("Constants")) {
			//all declarations

			while (*(++it_v) != wxT("Functions"))
			{
				l_sdec = *(it_v);
				wxString constant;
				if (!l_sdec.Contains("("))
					constant = l_sdec.Before(':');
				else
					constant = l_sdec.Before('(');
				l_mTrimUserDec[l_sdec] = constant;
			}
			while (*(++it_v) != wxT("Observers"))
			{
				wxString l_sfun = (*it_v).Before('(');
				l_mTrimUserDec[*it_v] = l_sfun.Trim();

			}
			while (++it_v != m_vUserDec.end())
			{
				wxString l_sOb = (*it_v).Before('=');
				l_mTrimUserDec[*it_v] = l_sOb.Trim();

			}
			break;
		}
		else {//color sets

		}
	}

	return l_mTrimUserDec;
}


/*delete a given variable*/
void SP_DS_UnUsedDeclaration::DeleteUnusedVariable(wxString p_sVarName)
{

	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());//l_pcMetadataclass->NewElement(1);
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sVar = l_pcColList->GetCell(i, 0);
		if (p_sVarName == l_sVar)
		{
			l_pcColList->RemoveRow(i);
			return;
		}
	}
}

/*returns CsType of a given variable*/
wxString SP_DS_UnUsedDeclaration::ObtainVarType(wxString p_sVarName)
{
	wxString l_sCS;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		if (p_sVarName == l_sName)
			l_sCS = l_sColors;
	}
	return l_sCS;
}

/*returns set of all defined variables*/
set<wxString> SP_DS_UnUsedDeclaration::ObtainAllDefinedVars()
{
	set<wxString> l_setVar;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();

		l_setVar.insert(l_sName);
	}


	return l_setVar;

}

/*returns map of defined vars 2 data type*/
map<wxString, wxString> SP_DS_UnUsedDeclaration::ObtainAllVar2Cs()const
{
	map<wxString, wxString> l_mResult;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		l_mResult[l_sName] = l_sColors;
	}


	return l_mResult;
}

/*returns set of all used variable in the colored net*/
std::set<string>   SP_DS_UnUsedDeclaration::ReadColPNVariables()
{
	std::set<string> l_setVar;

	SP_ListNode::const_iterator l_itElem;
	SP_ListEdge::const_iterator l_itEdges;
	SP_VectorString l_vTransNC = { SP_DS_CONTINUOUS_TRANS ,SP_DS_STOCHASTIC_TRANS ,wxT("Immediate Transition") ,wxT("Deterministic Transition") ,wxT("Scheduled Transition") };
	for (auto it_vTrns = l_vTransNC.begin(); it_vTrns != l_vTransNC.end(); ++it_vTrns)
	{
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(*it_vTrns);

		if (l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcTransNode = (*l_itElem);

				//get arc expressions
				for (l_itEdges = l_pcTransNode->GetTargetEdges()->begin(); l_itEdges != l_pcTransNode->GetTargetEdges()->end(); ++l_itEdges)
				{
					if ((*l_itEdges)->GetSource())
					{
						SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetSource());
						SP_DS_Edge* l_pcEdge = *l_itEdges;
						wxString l_sexparc = ObtainArcExp(l_pcPlaceNode, l_pcEdge, l_pcTransNode);

						std::set<string> l_settoks = ToknizeArcExp(l_sexparc);
						for (auto it_s = l_settoks.begin(); it_s != l_settoks.end(); ++it_s)
						{
							wxString l_sVar = *it_s;
							l_sVar.Replace(wxT(" "), wxT(""));
							l_sVar.Replace(wxT("`"), wxT(""));
							l_setVar.insert(l_sVar);
						}

					}
				}

				//get arc expressions
				for (l_itEdges = l_pcTransNode->GetSourceEdges()->begin(); l_itEdges != l_pcTransNode->GetSourceEdges()->end(); ++l_itEdges)
				{
					if ((*l_itEdges)->GetTarget())
					{
						SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetTarget());
						SP_DS_Edge* l_pcEdge = *l_itEdges;
						wxString l_sexparc = ObtainArcExp(l_pcPlaceNode, l_pcEdge, l_pcTransNode);
						std::set<string> l_settoks = ToknizeArcExp(l_sexparc);
						for (auto it_s = l_settoks.begin(); it_s != l_settoks.end(); ++it_s)
						{
							wxString l_sVar = *it_s;
							l_sVar.Replace(wxT(" "), wxT(""));
							l_sVar.Replace(wxT("`"), wxT(""));
							l_setVar.insert(l_sVar);
						}

					}
				}
				std::set<string> l_setGvars = ObtainGuardVar(l_pcTransNode);
				l_setVar.insert(l_setGvars.begin(), l_setGvars.end());

			}
		}

	}

	//search variable if found in Predicates of transitions
	set<wxString> l_setPredicates = GetDeclarationsInPredicates();
	for (auto it_set = l_setPredicates.begin(); it_set != l_setPredicates.end(); ++it_set)
	{
		std::string str = (*it_set).ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("!-|+*(){}[]%/&=<> ");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_sTkn = *tok_iter;
			bool l_bisNum = false;
			if (!l_sTkn.empty() && std::all_of(l_sTkn.begin(), l_sTkn.end(), ::isdigit))
			{
				l_bisNum = true;
			}
			if (!l_bisNum)
			{
				l_sTkn.Replace(" ", "");
				l_setVar.insert(l_sTkn);
			}
		}

	}

	return l_setVar;
}

/*return col expression of a given arc*/
wxString SP_DS_UnUsedDeclaration::ObtainArcExp(SP_DS_Node* p_pcPlaceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* p_pcTransNode) const
{
	wxString l_sExp;
	if (!p_pcPlaceNode || !p_pcEdge) {
		return l_sExp;
	}


	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
	if (!l_pcColList)
		return l_sExp;

	l_sExp = l_pcColList->GetCell(0, 1);
	return l_sExp;
}

std::set<string>   SP_DS_UnUsedDeclaration::ToknizeArcExp(wxString& exp) const
{
	std::set<string> l_setVars;
	std::string str = exp.ToStdString();
	typedef boost::tokenizer<boost::char_separator<char> >
		tokenizer;
	boost::char_separator<char> sep("-|+*()/&,[ ]=><`! ");
	tokenizer tokens(str, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
	{
		l_setVars.insert(*tok_iter);
	}

	return l_setVars;
}

/*return the used gaured of a given transition*/
string SP_DS_UnUsedDeclaration::ObtainUsedGuard(SP_DS_Node* p_pcTransNode) const
{

	wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	SP_DS_Node* l_pcTransNode = p_pcTransNode;
	wxString l_sGuard;
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));

	l_sGuard = l_pcColList->GetCell(0, 1);
	wxString l_sExpressionUnique = l_sGuard;


	if (l_sExpressionUnique.Contains("("))
	{
		l_sExpressionUnique = l_sExpressionUnique.BeforeFirst('(');
		//l_sExpressionUnique = l_sExpressionUnique.AfterFirst('(');
	}
	else
	{
		l_sExpressionUnique = l_sExpressionUnique.BeforeFirst(' ');
		l_sExpressionUnique.Replace(wxT(" "), wxT(""));
	}


	return l_sExpressionUnique;
}

/*obtain varible used in the guard*/
std::set<string> SP_DS_UnUsedDeclaration::ObtainGuardVar(SP_DS_Node* p_pcTransNode) const {
	std::set<string> l_setGuardVars;

	SP_DS_Node* l_pcTransNode = p_pcTransNode;
	// To get the name of the transition
	wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

	wxString l_sGuard;
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));


	l_sGuard = l_pcColList->GetCell(0, 1);
	wxString l_s = l_pcColList->GetCell(0, 0);

	wxString l_sExpressionUnique = l_sGuard;


	if (l_sExpressionUnique.Contains("("))
	{
		l_sExpressionUnique = l_sExpressionUnique.BeforeFirst(')');
		l_sExpressionUnique = l_sExpressionUnique.AfterFirst('(');
	}
	else
	{
		//	l_sExpressionUnique = l_sExpressionUnique.AfterFirst(' ');
		l_sExpressionUnique.Replace(wxT(" "), wxT(""));
	}

	std::string str = l_sExpressionUnique.ToStdString();
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("-|+*()/&,><!= ");
	tokenizer tokens(str, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
	{
		l_setGuardVars.insert(*tok_iter);
	}


	return l_setGuardVars;
}

/*returns set of all used colored functions in the net*/
std::set<string> SP_DS_UnUsedDeclaration::ReadUsedColouredFunctions()
{
	std::set<string> l_setfuns, l_setusedFunInColExpr;

	SP_ListNode::const_iterator l_itElem;
	SP_ListEdge::const_iterator l_itEdges;
	SP_VectorString l_vTransNC = { SP_DS_CONTINUOUS_TRANS ,SP_DS_STOCHASTIC_TRANS ,wxT("Immediate Transition") ,wxT("Deterministic Transition") ,wxT("Scheduled Transition") };
	for (auto it_vTrns = l_vTransNC.begin(); it_vTrns != l_vTransNC.end(); ++it_vTrns)
	{
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(*it_vTrns);

		if (l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcTransNode = (*l_itElem);

				string l_sUsedGaurd = ObtainUsedGuard(l_pcTransNode);
				if (!l_sUsedGaurd.empty())
				{
					l_setfuns.insert(l_sUsedGaurd);
				}

			}
		}

	}

	//search function if found in Predicates of transitions
	set<wxString> l_setPredicates = GetDeclarationsInPredicates();
	for (auto it_set = l_setPredicates.begin(); it_set != l_setPredicates.end(); ++it_set)
	{
		std::string str = (*it_set).ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("!-|+*(){}[]%/&=<> ");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_sTkn = *tok_iter;
			bool l_bisNum = false;
			if (!l_sTkn.empty() && std::all_of(l_sTkn.begin(), l_sTkn.end(), ::isdigit))
			{
				l_bisNum = true;
			}
			if (!l_bisNum)
			{
				l_sTkn.Replace(" ", "");
				l_setfuns.insert(l_sTkn);
			}
		}

	}

	l_setusedFunInColExpr = ReadColPNVariables();
	l_setfuns.insert(l_setusedFunInColExpr.begin(), l_setusedFunInColExpr.end());

	return l_setfuns;
}

/*returns all used Cs by the colred net*/
set<wxString> SP_DS_UnUsedDeclaration::ReadAllUsedCSByGraph() {
	set<wxString> l_setResults;
	set<wxString> l_setUsedByPlaces = ReadUsedColorsetsbyPlaces();//assigned by places
	l_setResults.insert(l_setUsedByPlaces.begin(), l_setUsedByPlaces.end());

	/*next step*/
	map<wxString, set<wxString>> l_mAllfun2SimpleCompoundCs = ReadFunctionsWithUsedCS();//get all defined funs with their CSs(simple +compound)
	std::set<wxString>   l_setUnUsedFuns = FindUnusedFunctions();//unused functions

	for (auto it_m = l_mAllfun2SimpleCompoundCs.begin(); it_m != l_mAllfun2SimpleCompoundCs.end(); ++it_m)
	{
		string l_sToFind = (it_m->first).ToStdString();
		auto it_findfun = l_setUnUsedFuns.find(l_sToFind);
		if (it_findfun == l_setUnUsedFuns.end())//this fun is used by the graph
		{
			std::set<wxString> l_setCs = it_m->second;//color sets used by this fun
			l_setResults.insert(l_setCs.begin(), l_setCs.end());

		}
	}


	//find all cs used by the vars and these vars are used by the grap
	std::set<string>   l_setUsedVarsbytheGraph = ReadColPNVariables();
	map<wxString, wxString> l_mVar2CS = ObtainAllVar2Cs();
	for (auto it_map = l_mVar2CS.begin(); it_map != l_mVar2CS.end(); ++it_map)
	{
		string l_sToFind = it_map->first.ToStdString();
		auto findVar = l_setUsedVarsbytheGraph.find(l_sToFind);
		if (findVar != l_setUsedVarsbytheGraph.end())
		{
			l_setResults.insert(it_map->second);
		}
	}

	/*consider all compound cs which defined by used Simple cs as used also*/
	/*consider compound cs which used to define subset Cs as used*/
	map<wxString, set<wxString>> l_mComp2SimplCS = ReadDefinedCompoundCS();
	set<wxString> l_setCSComponents;
	for (auto it_map = l_mComp2SimplCS.begin(); it_map != l_mComp2SimplCS.end(); ++it_map)
	{
		string l_sToFind = (it_map->first).ToStdString();
		auto foundComp = l_setResults.find(l_sToFind);
		if (foundComp != l_setResults.end())
		{
			l_setCSComponents = it_map->second;
			l_setResults.insert(l_setCSComponents.begin(), l_setCSComponents.end());
		}

	}

	//check for SUBSET CSs, consider the cs which depend on as used also
	map<wxString, wxString> l_mCs2Type = ObtainAllCs2Type();
	for (auto it = l_mCs2Type.begin(); it != l_mCs2Type.end(); ++it)
	{
		string l_sToFind = (it->first).ToStdString();
		auto findsubcs = l_setResults.find(l_sToFind);
		if (findsubcs != l_setResults.end())
		{
			if (it->second != wxT("Compound") && it->second != wxT("Simple"))
			{
				l_setResults.insert(it->second);
			}
		}
	}


	return l_setResults;
}

/*returns color sets used by the places*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadUsedColorsetsbyPlaces()const
{
	std::set<wxString> l_setCS;

	for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{

				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
				{
					for (SP_DS_Node* node1 : *(nc->GetElements()))
					{
						//cs assigned by places
						wxString l_sNodeclass = nc->GetDisplayName();
						wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
						SP_DS_Attribute* l_pcAttr = node1->GetAttribute(SP_DS_CPN_COLORSETNAME);
						string l_sCSName = l_pcAttr->GetValueString();
						l_setCS.insert(l_sCSName);

					}
				}
			}
		}
	}
	return l_setCS;
}

/*return a set of all constants that compound cs depend on*/
std::set<wxString> SP_DS_UnUsedDeclaration::ObtainAllCompoundCSConstantDependent(set<wxString> p_setSimple)
{
	std::set<wxString> l_setCompCS;
	std::map<wxString, set<wxString>> l_mCompoundCs = ReadDefinedCompoundCS();

	for (auto it_s = p_setSimple.begin(); it_s != p_setSimple.end(); ++it_s)
	{
		for (auto it_map = l_mCompoundCs.begin(); it_map != l_mCompoundCs.end(); ++it_map)
		{
			std::set<wxString> l_setCs = it_map->second;
			string l_sToFind = (*it_s).ToStdString();
			auto findcs = l_setCs.find(l_sToFind);
			if (findcs != l_setCs.end())
			{
				l_setCompCS.insert(it_map->first);
			}
		}
	}
	return l_setCompCS;

}


/*return all defined simple cs*/
set<wxString> SP_DS_UnUsedDeclaration::ObtainSimpleCS() {


	SP_DS_Metadataclass* l_pcMetadataclass1 = NULL;

	SP_DS_Metadata* l_pcNewMetadata1 = NULL;

	l_pcMetadataclass1 = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);

	l_pcNewMetadata1 = l_pcMetadataclass1->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata1->GetAttribute(wxT("ColorsetList")));

	set<wxString> l_setSimpleCS;
	for (unsigned int i = 0; i < l_pcColList1->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList1->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList1->GetCell(i, 2).c_str();
		l_setSimpleCS.insert(l_sName);
	}
	return l_setSimpleCS;
}

bool SP_DS_UnUsedDeclaration::ObtainCompBassics(wxString p_comp, set<wxString> p_setSimple, map < wxString, set<wxString>> p_mComp2setSimple, set<wxString> &p_setResSimple, set<wxString> &p_setCom)
{
	set<wxString> l_setComponents = p_mComp2setSimple[p_comp];
	bool l_bIsAllSimple = true;
	for (auto it_set = l_setComponents.begin(); it_set != l_setComponents.end(); ++it_set)
	{
		if (p_setSimple.find(*it_set) != p_setSimple.end())
		{
			p_setResSimple.insert(*it_set);
		}
		else {
			l_bIsAllSimple = false;
			p_setCom.insert(*it_set);
		}
	}
	return l_bIsAllSimple;
}

void  SP_DS_UnUsedDeclaration::FindSimpleCsChain(wxString p_sSimplecs, map<wxString, set<wxString>> p_mC2S, wxString &Res)
{
	bool l_bHasComp = false;
	wxString l_sComp;
	for (auto it_m = p_mC2S.begin(); it_m != p_mC2S.end(); ++it_m)
	{
		set<wxString> l_setComponents = it_m->second;
		string l_sToFind = p_sSimplecs.ToStdString();
		auto it_found = l_setComponents.find(l_sToFind);
		if (it_found != l_setComponents.end())
		{
			l_bHasComp = true;
			l_sComp = it_m->first;
			Res << wxT("+") << l_sComp;
			break;
		}
	}
	if (!l_bHasComp) { return; }
	else {
		return FindSimpleCsChain(l_sComp, p_mC2S, Res);
	}
}


void SP_DS_UnUsedDeclaration::FindCompCsChain(wxString p_sCompcs, set<wxString> p_setSimple, map < wxString, set<wxString>> p_mComp2setSimple, wxString &p_sRes)
{
	set<wxString> l_setComponents = p_mComp2setSimple[p_sCompcs];


	bool b = false;
	for (auto it_setComponents = l_setComponents.begin(); it_setComponents != l_setComponents.end(); ++it_setComponents)
	{
		if (p_setSimple.find(*it_setComponents) == p_setSimple.end())
		{
			p_sRes << "+" << *it_setComponents;
			b = true;
		}
		else {
			p_sRes << "+" << *it_setComponents;
		}
	}
	if (!b) { return; }
	else {
		for (auto it_setComponents = l_setComponents.begin(); it_setComponents != l_setComponents.end(); ++it_setComponents)
		{
			//for (auto it_setComponents = l_setComponents.begin(); it_setComponents != l_setComponents.end(); ++it_setComponents)
			if (p_setSimple.find(*it_setComponents) == p_setSimple.end())
			{
				p_sRes << "+" << *it_setComponents;
				return FindCompCsChain(*it_setComponents, p_setSimple, p_mComp2setSimple, p_sRes);
			}
		}

	}
}

/*return set of all simple cs used to define compound cs*/
set<wxString>  SP_DS_UnUsedDeclaration::ObtainSimple2Compound(set<wxString> p_setSimple, map<wxString, set<wxString>> p_mall, set<wxString> p_setChildren, set<wxString> &p_setAllRes, wxString p_sProd)
{


	for (auto it_s = p_setChildren.begin(); it_s != p_setChildren.end(); ++it_s)
	{
		p_setAllRes.insert(*it_s);
		if (p_setSimple.find(*it_s) == p_setSimple.end())
		{
			set<wxString> l_setChild = p_mall[*it_s];
			p_setAllRes.insert(l_setChild.begin(), l_setChild.end());
			for (auto it_ss = l_setChild.begin(); it_ss != l_setChild.end(); ++it_ss)
			{
				set<wxString> l_setChild1 = p_mall[*it_ss];
				p_setAllRes.insert(l_setChild1.begin(), l_setChild1.end());
				if (p_setSimple.find(*it_ss) == p_setSimple.end())
				{
					for (auto it_sss = l_setChild1.begin(); it_sss != l_setChild1.end(); ++it_sss)
					{
						set<wxString> l_setChild2 = p_mall[*it_sss];
						p_setAllRes.insert(l_setChild2.begin(), l_setChild2.end());
					}
				}
			}
		}
	}

	return p_setAllRes;
}

map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ObtainCompCs2SimpleOneLevel()
{
	std::map<wxString, set<wxString>> l_mResult;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	for (int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i, 0);
		wxString l_sCSType = l_pcColList->GetCell(i, 1);
		//set<wxString>  l_setCS;
		wxString l_sCSComp = l_pcColList->GetCell(i, 2);
		set<wxString> l_setCS;
		std::string str = l_sCSComp.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep("-|+*()/&, ");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_sCS = *tok_iter;
			l_sCS.Replace(" ", "");
			l_setCS.insert(l_sCS);
		}

		l_mResult[l_sCSName] = l_setCS;
	}
	return l_mResult;
}

/*returns map of compound cs 2 its other CSs dependencies*/
std::map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ObtainCompoundCS2OtherCSDependency()
{
	std::map<wxString, set<wxString>> l_mResult, l_m;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	for (int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i, 0);
		wxString l_sCSType = l_pcColList->GetCell(i, 1);
		//set<wxString>  l_setCS;
		wxString l_sCSComp = l_pcColList->GetCell(i, 2);
		set<wxString> l_setCS;
		std::string str = l_sCSComp.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep("-|+*()/&,");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			l_setCS.insert(*tok_iter);
		}

		l_mResult[l_sCSName] = l_setCS;
	}
	/**********************************/

	SP_DS_Metadataclass* l_pcMetadataclass1 = NULL;
	SP_DS_Metadata* l_pcNewMetadata1 = NULL;

	l_pcMetadataclass1 = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);



	l_pcNewMetadata1 = l_pcMetadataclass1->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata1->GetAttribute(wxT("ColorsetList")));

	set<wxString> l_setSimpleCS;
	for (unsigned int i = 0; i < l_pcColList1->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList1->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList1->GetCell(i, 2).c_str();
		l_setSimpleCS.insert(l_sName);
	}
	/*****************************/
	map<wxString, set<wxString>> l_mFinal;
	for (auto it_m = l_mResult.begin(); it_m != l_mResult.end(); it_m++)
	{
		set<wxString> children = it_m->second;
		set<wxString> l_setAll;
		set<wxString> setset = ObtainSimple2Compound(l_setSimpleCS, l_mResult, children, l_setAll, it_m->first);
		l_mFinal[it_m->first] = setset;
	}


	return l_mFinal;
}

/*read all compound CS with their simple cs components*/
std::map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ReadDefinedCompoundCS() const
{
	std::map<wxString, set<wxString>> l_mCompoundCs;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	for (int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i, 0);
		wxString l_sCSType = l_pcColList->GetCell(i, 1);
		if (l_sCSType == wxT("product") | l_sCSType == wxT("union"))
		{
			wxString l_sCSComp = l_pcColList->GetCell(i, 2);
			set<wxString> l_setCS;
			std::string str = l_sCSComp.ToStdString();
			typedef boost::tokenizer<boost::char_separator<char> >
				tokenizer;
			boost::char_separator<char> sep("-|+*()/&, ");
			tokenizer tokens(str, sep);
			for (tokenizer::iterator tok_iter = tokens.begin();
				tok_iter != tokens.end(); ++tok_iter)
			{
				wxString l_sCs = *tok_iter;
				l_sCs.Replace(" ", "");
				l_setCS.insert(l_sCs);
			}

			l_mCompoundCs[l_sCSName] = l_setCS;
		}
		else {
			string l_sToFind = l_sCSType.ToStdString();
			auto find = l_mCompoundCs.find(l_sToFind);
			if (find != l_mCompoundCs.end())
			{
				l_mCompoundCs[l_sCSName] = l_mCompoundCs[l_sCSType];
			}

		}
	}
	return l_mCompoundCs;
}

/* Read Colour sets used by the net*/
std::set<string> SP_DS_UnUsedDeclaration::ReadUsedColorsets()const
{
	std::set<string> l_setCS;

	for (SP_DS_Nodeclass* nc : *(m_pcGraph->GetNodeclasses()))
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{

				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
				{
					for (SP_DS_Node* node1 : *(nc->GetElements()))
					{
						//cs assigned by places
						wxString l_sNodeclass = nc->GetDisplayName();
						wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
						SP_DS_Attribute* l_pcAttr = node1->GetAttribute(SP_DS_CPN_COLORSETNAME);
						string l_sCSName = l_pcAttr->GetValueString();
						l_setCS.insert(l_sCSName);

					}
				}
			}
		}
	}
	/****check the color sets components of structured color sets - those must be marked as used also******/
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	std::set<wxString> l_setCompoundCs;
	for (int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i, 0);
		wxString l_sCSComp = l_pcColList->GetCell(i, 2);
		l_setCompoundCs.insert(l_sCSName);//store compound color sets
	}

	for (int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i, 0);
		wxString l_sCSComp = l_pcColList->GetCell(i, 2);

		std::string str = l_sCSComp.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("-|+*()/&,");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			l_setCS.insert(*tok_iter);
		}


	}
	std::set<wxString> l_setColorsetsForVariables = ReadUsedColorsetsByVariables();
	for (auto it_s = l_setColorsetsForVariables.begin(); it_s != l_setColorsetsForVariables.end(); ++it_s)
	{
		string l_sStrCs = (*it_s).ToStdString();
		l_setCS.insert(l_sStrCs);
	}


	//merge cs used as data type for functions arguments
	std::set<wxString> l_setCSAasArguments = ReadColorSetsFromFunctionsArguments(l_setCS);
	for (auto it_s = l_setCSAasArguments.begin(); it_s != l_setCSAasArguments.end(); ++it_s)
	{
		string l_sStrCS = (*it_s).ToStdString();
		l_setCS.insert(l_sStrCS);
	}


	return l_setCS;
}

/*return color sets used to define a color function*/
std::set<wxString> SP_DS_UnUsedDeclaration::ReadColorSetsFromFunctionsArguments(std::set<string> p_set) const
{
	std::set<wxString> l_setCs;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if (!l_pcMetadataclass)
		return l_setCs;

	if (l_pcMetadataclass->GetElements()->empty())
		return l_setCs;

	SP_DS_Metadata*	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if (!l_pcColList)
		return l_setCs;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2);

		//tokenize argument List 
		std::string str = l_sColors.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(" ,");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{

			auto l_itfound = p_set.find(*tok_iter);
			if (l_itfound != p_set.end()) {
				wxString l_sTr = (*tok_iter);
				l_sTr.Replace(" ", "");
				l_setCs.insert(l_sTr);
			}
		}

	}
	return l_setCs;
}

std::map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ReadUsedCSByCompundCS()
{
	std::map<wxString, set<wxString>> l_mComp2CS;

	wxString l_sGroupSimple = wxT("Simple Color Sets");
	wxString l_sGroupCompound = wxT("Compound Color Sets");
	wxString l_sGroupAlias = wxT("Alias Color Sets");
	map<wxString, wxString> l_mtrimeddecSimplCS = TrimUserDec(l_sGroupSimple);
	map<wxString, wxString> l_mtrimeddecCompoundCS = TrimUserDec(l_sGroupCompound);

	map<wxString, wxString> l_vtrimeddecAliasCS = TrimAliasColorSets(l_sGroupAlias);

	map<wxString, wxString> l_mtrimeddecALLCs;
	l_mtrimeddecALLCs.insert(l_mtrimeddecSimplCS.begin(), l_mtrimeddecSimplCS.end());
	l_mtrimeddecALLCs.insert(l_mtrimeddecCompoundCS.begin(), l_mtrimeddecCompoundCS.end());
	l_mtrimeddecALLCs.insert(l_vtrimeddecAliasCS.begin(), l_vtrimeddecAliasCS.end());

	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));


	for (int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i, 0);
		wxString l_sCSComp = l_pcColList->GetCell(i, 2);
		set<wxString> l_setUsedCS;

		std::string str = l_sCSComp.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep("-|+*()/&,% ");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_sCs = (*tok_iter);
			l_sCs.Replace(" ", "");
			l_setUsedCS.insert(l_sCs);
		}


		l_mComp2CS[l_sCSName] = l_setUsedCS;
	}

	return l_mComp2CS;
}

std::set<wxString>  SP_DS_UnUsedDeclaration::ReadColorSetsUsedByFunction(wxString p_sFunName)
{

	wxString l_sGroupSimple = wxT("Simple Color Sets");
	wxString l_sGroupCompound = wxT("Compound Color Sets");
	wxString l_sGroupAlias = wxT("Alias Color Sets");
	map<wxString, wxString> l_mtrimeddecSimplCS = TrimUserDec(l_sGroupSimple);
	map<wxString, wxString> l_mtrimeddecCompoundCS = TrimUserDec(l_sGroupCompound);

	map<wxString, wxString> l_vtrimeddecAliasCS = TrimAliasColorSets(l_sGroupAlias);

	map<wxString, wxString> l_mtrimeddecALLCs;
	l_mtrimeddecALLCs.insert(l_mtrimeddecSimplCS.begin(), l_mtrimeddecSimplCS.end());
	l_mtrimeddecALLCs.insert(l_mtrimeddecCompoundCS.begin(), l_mtrimeddecCompoundCS.end());
	l_mtrimeddecALLCs.insert(l_vtrimeddecAliasCS.begin(), l_vtrimeddecAliasCS.end());

	std::set<wxString> l_setCs;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if (!l_pcMetadataclass)
		return l_setCs;

	if (l_pcMetadataclass->GetElements()->empty())
		return l_setCs;

	SP_DS_Metadata*	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if (!l_pcColList)
		return l_setCs;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
		if (l_sName == p_sFunName)
		{
			wxString l_sColors = l_pcColList->GetCell(i, 2);

			//tokenize argument List
			std::string str = l_sColors.ToStdString();
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(" ,");
			tokenizer tokens(str, sep);
			for (tokenizer::iterator tok_iter = tokens.begin();
				tok_iter != tokens.end(); ++tok_iter)
			{

				auto l_itfound = l_mtrimeddecALLCs.find(*tok_iter);
				if (l_itfound != l_mtrimeddecALLCs.end()) {
					wxString l_sTr = (*tok_iter);
					l_sTr.Replace(" ", "");
					l_setCs.insert(l_sTr);
				}
			}
		}
	}
	return l_setCs;
}

std::set<wxString> SP_DS_UnUsedDeclaration::ReadColorSetsUsedFunctionsArguments()
{

	wxString l_sGroupSimple = wxT("Simple Color Sets");
	wxString l_sGroupCompound = wxT("Compound Color Sets");
	wxString l_sGroupAlias = wxT("Alias Color Sets");
	map<wxString, wxString> l_mtrimeddecSimplCS = TrimUserDec(l_sGroupSimple);
	map<wxString, wxString> l_mtrimeddecCompoundCS = TrimUserDec(l_sGroupCompound);

	map<wxString, wxString> l_vtrimeddecAliasCS = TrimAliasColorSets(l_sGroupAlias);

	map<wxString, wxString> l_mtrimeddecALLCs;
	l_mtrimeddecALLCs.insert(l_mtrimeddecSimplCS.begin(), l_mtrimeddecSimplCS.end());
	l_mtrimeddecALLCs.insert(l_mtrimeddecCompoundCS.begin(), l_mtrimeddecCompoundCS.end());
	l_mtrimeddecALLCs.insert(l_vtrimeddecAliasCS.begin(), l_vtrimeddecAliasCS.end());

	std::set<wxString> l_setCs;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if (!l_pcMetadataclass)
		return l_setCs;

	if (l_pcMetadataclass->GetElements()->empty())
		return l_setCs;

	SP_DS_Metadata*	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if (!l_pcColList)
		return l_setCs;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2);

		//tokenize argument List 
		std::string str = l_sColors.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(" ,");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{

			auto l_itfound = l_mtrimeddecALLCs.find(*tok_iter);
			if (l_itfound != l_mtrimeddecALLCs.end()) {
				wxString l_sStr = *tok_iter;
				l_sStr.Replace(" ", "");
				l_setCs.insert(l_sStr);
			}
		}

	}
	return l_setCs;
}

/*obtain all unused variable in the net*/
std::set<wxString>   SP_DS_UnUsedDeclaration::FindUnusedVariables()
{
	set<wxString> l_setResult;

	set<string> l_setUsedVariables = ReadColPNVariables();//read all used variables
	wxString l_sCon = wxT("Variables");
	std::map<wxString, wxString> l_vUserDec = TrimUserDec(l_sCon);//all defined decl

	for (auto it_v = l_vUserDec.begin(); it_v != l_vUserDec.end(); ++it_v)
	{
		wxString l_sDec = it_v->second.Trim();
		string l_sToFind = l_sDec.ToStdString();

		auto it_s = l_setUsedVariables.find(l_sToFind);
		if (it_s == l_setUsedVariables.end())
		{
			l_setResult.insert(l_sDec);
		}
	}



	return l_setResult;
}

/*delete a color set*/
void SP_DS_UnUsedDeclaration::DeleteUnusedColorSet(wxString p_sCsName)
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_VectorString l_vCSType = { SP_DS_CPN_BASICCOLORSETCLASS,SP_DS_CPN_STRUCTUREDCOLORSETCLASS,SP_DS_CPN_ALIASCOLORSETCLASS };
	SP_VectorString l_vCSAtt = { wxT("ColorsetList"),wxT("StructuredColorsetList"),wxT("AliasColorsetList") };
	for (int cs = 0; cs < l_vCSType.size(); cs++)
	{
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(l_vCSType[cs]);

		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(l_vCSAtt[cs]));


		for (int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sCSName = l_pcColList->GetCell(i, 0);
			wxString l_sCSComp = l_pcColList->GetCell(i, 2);
			if (l_sCSName == p_sCsName)
			{
				l_pcColList->RemoveRow(i);
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				break;
			}
		}
	}

}

/*delete a given constant*/
void SP_DS_UnUsedDeclaration::DeleteUnusedConstant(wxString p_sName)
{
	bool l_bIsColored = false;
	wxString l_sNet = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_sNet.Contains(wxT("Colored")))
	{
		l_bIsColored = true;
	}
	if (l_bIsColored) {
		SP_DS_Metadataclass* l_pcConstants= m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	
		for (SP_DS_Metadata* l_pcMeta : *(l_pcConstants)->GetElements())
		{
			wxString l_sNameCon = l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
			if (l_sNameCon == p_sName)
			{
				l_pcConstants->RemoveElement(l_pcMeta);
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				break;
			}
		}

		//SP_DS_Metadataclass* l_pcMetadataclass;
		//l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);

		//SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());//l_pcMetadataclass->NewElement(1);
		//SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));

		/**
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sName = l_pcColList->GetCell(i, 0);
			if (l_sName == p_sName) {
				l_pcColList->RemoveRow(i);
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				return;
			}
		}
		*/
	}
	else {

		SP_DS_Metadataclass* l_pcConstants = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
		for (SP_DS_Metadata* l_pcMeta : *(l_pcConstants)->GetElements())
		{
			wxString l_sNameCon = l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
			if (l_sNameCon == p_sName)
			{
				l_pcConstants->RemoveElement(l_pcMeta);
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				break;
			}
		}
	}
}

/*delete a given function*/
void SP_DS_UnUsedDeclaration::DeleteUnusedFunction(wxString p_sName)
{

	SP_DS_Metadataclass* l_pcMetadataclass;
	bool l_bIsColored = false;
	wxString l_sNet = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_sNet.Contains(wxT("Colored")))
	{
		l_bIsColored = true;
	}
	if (l_bIsColored)
	{
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{

			wxString l_sName = l_pcColList->GetCell(i, 1);
			if (p_sName == l_sName)
			{
				l_pcColList->RemoveRow(i);
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				break;
			}
		}

	}
	else {
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	}



	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			if (p_sName == l_sName)
			{
				l_pcMetadataclass->RemoveElement(l_pcMetadata);
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				set<wxString> l_setConstantsDependency = FindUnusedConstants();

				break;
			}
		}
	}

}

std::map<wxString, wxString> SP_DS_UnUsedDeclaration::TrimAliasColorSets(wxString p_sDec)
{
	std::map<wxString, wxString> l_mtrimDec;
	wxString l_sdec;
	auto it_v = m_vUserDec.begin();

	while (*it_v != p_sDec)
	{
		++it_v;
	}

	while (++it_v != m_vUserDec.end())
	{
		l_sdec = *it_v;
		if (p_sDec == wxT("Alias Color Sets"))
		{
			while (l_sdec != wxT("Constants")) {

				l_mtrimDec[l_sdec] = l_sdec;

				l_sdec = *(++it_v);

			}
			break;
		}
	}

	return l_mtrimDec;
}

/*compute all pairs cs <=> Constant*/
set<pair<wxString, wxString>> SP_DS_UnUsedDeclaration::ObtainSimpleCsConstantsDependent()
{
	set<pair<wxString, wxString>> l_setCs2ConstPair;
	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;

	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);


	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();


		std::string str = l_sColors.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep("-|+*()/&=<>,");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_tkn = *tok_iter;
			bool l_bisNum = false;
			if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
			{
				l_bisNum = true;
			}
			if (!l_bisNum)
			{
				wxString l_sToken = *tok_iter;
				l_sToken.Replace(" ", "");
				pair<wxString, wxString> l_pair(l_sName, l_sToken);
				l_setCs2ConstPair.insert(l_pair);

			}



		}
	}
	return l_setCs2ConstPair;
}

map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ObtainObserversConstantsDependent()
{
	map < wxString, set<wxString >> l_mObserver2Constants;

	std::map<wxString, wxString>   l_mOb2Body = ReadObserverBodies();
	set<wxString> l_setAllConstants = ObtainAllDefinedConstants();

	for (auto it_map = l_mOb2Body.begin(); it_map != l_mOb2Body.end(); ++it_map)
	{
		wxString l_sObsName = it_map->first;
		wxString l_sTempBody = it_map->second;
		set<wxString> l_setCon;
		//
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sepList("(|&=+-/%)<>* ");
		string l_sBody = l_sTempBody.ToStdString();
		tokenizer Obtokens(l_sBody, sepList);
		for (tokenizer::iterator beg = Obtokens.begin(); beg != Obtokens.end(); ++beg)
		{
			std::string l_tkn = *beg;
			if (l_tkn.compare("|") != 0 && l_tkn.compare("&") != 0 && l_tkn.compare("=") != 0 && l_tkn.compare("(") != 0 && l_tkn.compare(")") != 0 &&
				l_tkn.compare(">") != 0 && l_tkn.compare("<") != 0 && l_tkn.compare("+") != 0 && l_tkn.compare("-") != 0 && l_tkn.compare("/") != 0 && l_tkn.compare("%") != 0
				)
			{
				bool l_bisNum = false;
				if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
				{
					l_bisNum = true;
				}
				if (!l_bisNum)
				{
					auto find_c = l_setAllConstants.find(l_tkn);
					if (find_c != l_setAllConstants.end())
					{
						l_setCon.insert(l_tkn);
					}

				}
			}
		}
		l_mObserver2Constants[l_sObsName] = l_setCon;

	}
	return l_mObserver2Constants;
}

/*returns map of function name,used constants*/
set<pair<wxString, set<wxString>>> SP_DS_UnUsedDeclaration::ObtainFunctionsConstantsDependent()
{
	set<pair<wxString, set<wxString>>> l_setUsedConstantsInFuns;

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	set<wxString> l_setAllDefinedConstants = ObtainAllDefinedConstants();
	if (!l_netType.Contains(wxT("Colored")))
	{
		for (SP_DS_Metadataclass* mc : *(m_pcGraph->GetMetadataclasses()))
		{
			if (mc->GetShowInDeclarationTreeOther()
				|| mc->GetShowInDeclarationTreeColorSet())
			{
				if (mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{
					for (SP_DS_Metadata* meta : *(mc->GetElements()))
					{
						wxString l_sMetaclass = mc->GetDisplayName();
						wxString l_sName = meta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
						/**********/
						if (l_sMetaclass == wxT("Functions"))
						{
							wxString l_sName = meta->GetAttribute(wxT("Name"))->GetValueString();
							wxString l_sParamList = meta->GetAttribute(wxT("Parameter"))->GetValueString();
							wxString l_sBody = meta->GetAttribute(wxT("Body"))->GetValueString();
							std::stringstream sstream1(l_sParamList);
							std::string sparam;
							set<wxString> p_setConstant;
							while (getline(sstream1, sparam, ','))
							{
								wxString par = sparam.c_str();

							}

							std::string str = l_sBody.ToStdString();
							typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
							boost::char_separator<char> sep("!-|+*()%/&=<> ");
							tokenizer tokens(str, sep);
							for (tokenizer::iterator tok_iter = tokens.begin();
								tok_iter != tokens.end(); ++tok_iter)
							{
								wxString l_sTkn = *tok_iter;
								bool l_bisNum = false;
								if (!l_sTkn.empty() && std::all_of(l_sTkn.begin(), l_sTkn.end(), ::isdigit))
								{
									l_bisNum = true;
								}
								if (!l_bisNum)
								{
									string l_sToFind = l_sTkn.ToStdString();
									auto findCon = l_setAllDefinedConstants.find(l_sToFind);
									if (findCon != l_setAllDefinedConstants.end())

									{
										wxString l_sToken = l_sTkn;
										l_sToken.Replace(" ", "");
										p_setConstant.insert(l_sToken);
									}
								}


							}
							pair<wxString, set<wxString>> l_pair(l_sName, p_setConstant);
							l_setUsedConstantsInFuns.insert(l_pair);
						}
					}
				}
			}
		}
		return l_setUsedConstantsInFuns;
	}

	/*****/
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	for (int l_nRow = 0; l_nRow < l_pcColList->GetRowCount(); l_nRow++)
	{
		std::set<wxString> l_setTempParams;
		std::set<wxString> l_setTempBodyConst;
		string l_sName = l_pcColList->GetCell(l_nRow, 1).ToStdString();//parameters
		string l_sParamList = l_pcColList->GetCell(l_nRow, 2).ToStdString();//parameters
		string l_sFunBody = l_pcColList->GetCell(l_nRow, 3).ToStdString();//body  

		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(" ,");
		tokenizer tokens(l_sParamList, sep);
		wxString result;
		for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg)
		{

			l_setTempParams.insert(*beg);
		}

		boost::char_separator<char> sep1("(|&=+-/%)<>*! ");
		tokenizer tokens1(l_sFunBody, sep1);
		for (tokenizer::iterator beg = tokens1.begin(); beg != tokens1.end(); ++beg)
		{
			std::string l_tkn = *beg;
			if (l_tkn.compare("|") != 0 && l_tkn.compare("&") != 0 && l_tkn.compare("=") != 0 && l_tkn.compare("(") != 0 && l_tkn.compare(")") != 0 &&
				l_tkn.compare(">") != 0 && l_tkn.compare("!") && l_tkn.compare("<") != 0 && l_tkn.compare("+") != 0 && l_tkn.compare("-") != 0 && l_tkn.compare("/") != 0 && l_tkn.compare("%") != 0
				)
			{
				bool l_bisNum = false;
				if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
				{
					l_bisNum = true;
				}
				if (!l_bisNum)
				{
					wxString l_sToken = l_tkn;
					l_sToken.Replace(" ", "");
					l_setTempBodyConst.insert(l_sToken);
				}
			}
		}
		set<wxString> l_setCon;
		for (auto it_setBody = l_setTempBodyConst.begin(); it_setBody != l_setTempBodyConst.end(); ++it_setBody)
		{
			string l_sToFind = (*it_setBody).ToStdString();
			auto pos = l_setTempParams.find(l_sToFind);
			if (pos == l_setTempParams.end())
			{

				l_setCon.insert(*it_setBody);
			}
		}
		pair<wxString, set< wxString>> l_pair(l_sName, l_setCon);
		l_setUsedConstantsInFuns.insert(l_pair);
	}
	return	l_setUsedConstantsInFuns;
}
/*return set of all used constants in the net*/
set<wxString> SP_DS_UnUsedDeclaration::ObtainUsedConstantsInGraph()
{
	std::set<wxString> l_setUsedConst;
	std::set<wxString> l_setConstAsMarking = ReadNetMarking();

	bool l_bIsColoured = false;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}
	if (!l_bIsColoured)
	{
		set<wxString>  l_setConst = ObtainAllDefinedConstants();
		set<wxString> l_setUnUsed = FindUnusedConstants();
		for (auto it_set = l_setConst.begin(); it_set != l_setConst.end(); ++it_set)
		{
			string l_sToFind = (*it_set).ToStdString();
			auto foundcon = l_setUnUsed.find(l_sToFind);
			if (foundcon == l_setUnUsed.end())
			{
				l_setUsedConst.insert(*it_set);
			}
		}

		return l_setUsedConst;
	}


	std::set<string> l_setConstInExp = ReadColPNVariables();
	std::set<wxString> l_setmarking;
	std::set<wxString> l_setArc;
	set<wxString>  l_setConst = ObtainAllDefinedConstants();
	for (auto it_set = l_setConstAsMarking.begin(); it_set != l_setConstAsMarking.end(); ++it_set)
	{
		string l_sTofind = (*it_set).ToStdString();
		auto foundcon = l_setConst.find(l_sTofind);
		if (foundcon != l_setConst.end())
		{
			l_setmarking.insert(*it_set);
		}
	}
	for (auto it_set = l_setConstInExp.begin(); it_set != l_setConstInExp.end(); ++it_set)
	{

		auto foundcon = l_setConst.find(*it_set);
		if (foundcon != l_setConst.end())
		{
			l_setArc.insert(*it_set);
		}
	}
	l_setUsedConst.insert(l_setmarking.begin(), l_setmarking.end());
	l_setUsedConst.insert(l_setArc.begin(), l_setArc.end());

	set<wxString> l_setConstantCSRelated;

	set<wxString> l_setUsedCS = ReadAllUsedCSByGraph();

	map<wxString, wxString> l_mapCs2Type = ObtainAllCs2Type();

	set<pair<wxString, wxString>> l_setSimple2Const = ObtainSimpleCsConstantsDependent();

	for (auto it_set = l_setUsedCS.begin(); it_set != l_setUsedCS.end(); ++it_set)
	{
		if (l_mapCs2Type[*it_set] == wxT("Simple"))
		{
			for (auto it_con = l_setSimple2Const.begin(); it_con != l_setSimple2Const.end(); ++it_con)
			{
				pair<wxString, wxString> l_pair = *it_con;
				if (l_pair.first == *it_set)
				{
					l_setConstantCSRelated.insert(l_pair.second);
				}
			}
		}
		else if (l_mapCs2Type[*it_set] == wxT("Compound"))
		{
			wxString l_setName = *it_set;
			set<wxString> l_setIndirectconst = ObtainConstantsUsed2DefineCompoundCS(l_setName);
			l_setConstantCSRelated.insert(l_setIndirectconst.begin(), l_setIndirectconst.end());
		}
	}

	//search constants if found in Predicates of transitions
	set<wxString> l_setPredicates = GetDeclarationsInPredicates();
	for (auto it_set = l_setPredicates.begin(); it_set != l_setPredicates.end(); ++it_set)
	{
		std::string str = (*it_set).ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("!-|+*(){}[]%/&=<> ");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_sTkn = *tok_iter;
			bool l_bisNum = false;
			if (!l_sTkn.empty() && std::all_of(l_sTkn.begin(), l_sTkn.end(), ::isdigit))
			{
				l_bisNum = true;
			}
			if (!l_bisNum)
			{
				l_sTkn.Replace(" ", "");
				l_setUsedConst.insert(l_sTkn);
			}
		}

	}

	l_setUsedConst.insert(l_setConstantCSRelated.begin(), l_setConstantCSRelated.end());
	return l_setUsedConst;
}

/*return set of Simple CS which depend on a given constant*/
set<wxString> SP_DS_UnUsedDeclaration::ObtainSimpleCsConstantsDependent(wxString p_sConstantName)
{
	set<wxString> l_setCs;
	set<pair<wxString, wxString>> l_setSimple2Constants = ObtainSimpleCsConstantsDependent();

	for (auto it_set = l_setSimple2Constants.begin(); it_set != l_setSimple2Constants.end(); ++it_set)
	{
		pair<wxString, wxString> l_pair = *it_set;
		if (l_pair.second == p_sConstantName)
		{
			l_setCs.insert(l_pair.first);
		}
	}
	return l_setCs;
}

/*returns map  <SimpleCS,ConstantDependent> which not used by the graph*/
set<pair<wxString, wxString>> SP_DS_UnUsedDeclaration::ObtainSimpleCS2ContantsDependentOutofGraph()
{
	set<pair<wxString, wxString>> l_setSimpleCs2Const;
	set<wxString> l_setUsedConstInGraph = ObtainUsedConstantsInGraph();
	set<pair<wxString, wxString>> l_setSimple2Constants = ObtainSimpleCsConstantsDependent();

	for (auto it_set = l_setSimple2Constants.begin(); it_set != l_setSimple2Constants.end(); ++it_set)
	{
		wxString l_sConst = (*it_set).second;
		string l_sToFind = l_sConst.ToStdString();
		auto find_const = l_setUsedConstInGraph.find(l_sToFind);
		if (find_const == l_setUsedConstInGraph.end())
		{
			l_setSimpleCs2Const.insert(*it_set);
		}
	}
	return l_setSimpleCs2Const;
}

/*returns all unused declaration of sub cs if this subcs is unused*/
SubSetCsDependencyInfo  SP_DS_UnUsedDeclaration::FindSubCSDependency(wxString p_sCSName)
{
	SubSetCsDependencyInfo l_subcsInfo;
	bool l_bIsUsed = false;


	set<wxString> l_setallUsedCSBygraph = ReadAllUsedCSByGraph(); //check if the cs isused by the graph
	string l_sToFind = p_sCSName.ToStdString();
	auto it_foundcs = l_setallUsedCSBygraph.find(l_sToFind);

	if (it_foundcs != l_setallUsedCSBygraph.end())
	{
		l_bIsUsed = true;
	}
	l_subcsInfo.m_bIsUSed = l_bIsUsed;

	l_subcsInfo.m_sName = p_sCSName;

	if (!l_bIsUsed)
	{
		set<wxString> l_setVar;
		set<wxString> l_setFuns;
		set<wxString> l_setSimpleCs;
		set<wxString> l_setConst;

		map<wxString, wxString> l_mCs2Type = ObtainAllCs2Type();
		for (auto itmap = l_mCs2Type.begin(); itmap != l_mCs2Type.end(); itmap++)
		{
			if (itmap->first == p_sCSName)
			{
				CompoundCsDependencyInfo l_compoundCSInf = FindCompoundCSDependencies(itmap->second);
				if (l_compoundCSInf.m_bIsUSed == false) {
					l_subcsInfo.m_CompoundCS = l_compoundCSInf.m_sName;
					l_setVar.insert(l_compoundCSInf.m_setVar.begin(), l_compoundCSInf.m_setVar.end());
					l_setConst.insert(l_compoundCSInf.m_setConst.begin(), l_compoundCSInf.m_setConst.end());
					l_setSimpleCs.insert(l_compoundCSInf.m_setSimpleCs.begin(), l_compoundCSInf.m_setSimpleCs.end());
					l_setFuns.insert(l_compoundCSInf.m_setFun.begin(), l_compoundCSInf.m_setFun.end());
				}


			}

		}
		//check if the expression which determines the colors of subset cs contains some variables/contants which not in use any more 
		map<wxString, set<wxString>> l_msubcs2colors = ObtainSubCs2ColorComponents();
		set<wxString> l_setColors = l_msubcs2colors[p_sCSName];
		set<wxString>                l_setConstdef = ObtainAllDefinedConstants();
		set<wxString> l_setVarColor;
		set<wxString> l_setConstants;
		for (auto itset = l_setColors.begin(); itset != l_setColors.end(); ++itset)
		{
			string l_sToFind = (*itset).ToStdString();
			auto constfound = l_setConstdef.find(l_sToFind);
			if (constfound != l_setConstdef.end())
			{
				l_setConstants.insert(*itset);
			}
			else {
				l_setVarColor.insert(*itset);
				l_subcsInfo.m_setForwardVar.insert(*itset);
			}
		}
		//constant only used by this subset cs
		set<wxString> l_setallusedconstants = ObtainUsedConstantsInGraph();
		for (auto itsetcons = l_setConstants.begin(); itsetcons != l_setConstants.end(); ++itsetcons)
		{
			string l_sToFind = (*itsetcons).ToStdString();
			auto isUsed = l_setallusedconstants.find(l_sToFind);
			if (isUsed == l_setallusedconstants.end())
			{
				l_setConst.insert(*itsetcons);
			}
		}

		//variable only used by this subset cs
		std::set<string>   l_setusedvars = ReadColPNVariables();//all used var in pn

		for (auto itsetvar = l_setVarColor.begin(); itsetvar != l_setVarColor.end(); ++itsetvar)
		{
			string l_sToFind = (*itsetvar).ToStdString();
			auto isUsed = l_setusedvars.find(*itsetvar);
			if (isUsed == l_setusedvars.end())
			{
				l_setVar.insert(*itsetvar);
			}
		}

		map<wxString, wxString> l_setAllvars = ObtainAllVar2Cs();
		for (auto it_setv = l_setAllvars.begin(); it_setv != l_setAllvars.end(); ++it_setv)
		{
			if (it_setv->second == p_sCSName)
			{
				string l_sToFind = (it_setv->first).ToStdString();
				auto isUsedInPN = l_setusedvars.find(l_sToFind);
				if (isUsedInPN == l_setusedvars.end())
				{
					l_setVar.insert(it_setv->first);
				}
			}
		}
		//remain to check if this subcs is used by Functions which are not in use by the graph
		map<wxString, set<wxString>> l_mfun2cs = ReadFunctionsWithUsedCS();//all defined funs
		for (auto itm = l_mfun2cs.begin(); itm != l_mfun2cs.end(); ++itm)
		{
			set<wxString> l_setCs = itm->second;
			string l_sToFind = p_sCSName.ToStdString();
			auto foundSubcs = l_setCs.find(l_sToFind);
			if (foundSubcs != l_setCs.end())
			{
				FunctionDependencyInfo l_funInfo = FindFunctionDependencies(itm->first);
				if (l_funInfo.m_bIsUSed == false)
				{
					l_setFuns.insert(itm->first);

				}
			}
		}
		l_subcsInfo.m_setConst = l_setConst;
		l_subcsInfo.m_setFun = l_setFuns;
		l_subcsInfo.m_setSimpleCs = l_setSimpleCs;
		l_subcsInfo.m_setVar = l_setVar;

	}
	return l_subcsInfo;
}


CompoundCsDependencyInfo SP_DS_UnUsedDeclaration::FindCompoundCSDependForSimpleCs(wxString p_sCSName)
{
	CompoundCsDependencyInfo l_compcsInf;
	bool l_bIsUsed = false;

	set<wxString> l_setallUsedCSBygraph = ReadAllUsedCSByGraph(); //check if the cs isused by the graph

	string l_sToFind = p_sCSName.ToStdString();
	auto it_foundcs = l_setallUsedCSBygraph.find(l_sToFind);

	if (it_foundcs != l_setallUsedCSBygraph.end())
	{
		l_bIsUsed = true;
	}
	l_compcsInf.m_bIsUSed = l_bIsUsed;

	l_compcsInf.m_sName = p_sCSName;

	if (!l_bIsUsed)
	{
		set<wxString> l_setVar;
		set<wxString> l_setFuns;
		set<wxString> l_setSimpleCs;
		set<wxString> l_setConst;

		map<wxString, wxString> l_mVar2Cs = ObtainAllVar2Cs();
		//add var defined on this cs  to the info as unused
		for (auto it_mapVar = l_mVar2Cs.begin(); it_mapVar != l_mVar2Cs.end(); ++it_mapVar)
		{
			if (it_mapVar->second == p_sCSName)
			{
				l_setVar.insert(it_mapVar->first);
			}

		}

		//add funs which use this cs as unuesed dependency
		set<wxString> l_setUnusedFun = FindUnusedFunctions();
		map<wxString, set< wxString>> l_mfun2Cs = ReadFunctionsWithUsedCS();
		map<wxString, wxString> l_mCs2Type = ObtainAllCs2Type();
		for (auto it_mapfun = l_mfun2Cs.begin(); it_mapfun != l_mfun2Cs.end(); ++it_mapfun)
		{
			set<wxString> l_setCs = it_mapfun->second;

			//let FunctionDependencyInfo check
			string l_sToFind = p_sCSName.ToStdString();
			auto findCS = l_setCs.find(l_sToFind);
			if (findCS != l_setCs.end())
			{
				//check this fun whether it is in use or not, if not in use, then add its dependencies
				string l_sToFind = (it_mapfun->first).ToStdString();
				auto it_checkFun = l_setUnusedFun.find(l_sToFind);
				if (it_checkFun != l_setUnusedFun.end())
				{
					l_setFuns.insert(it_mapfun->first);

					set<pair<wxString, set<wxString>>> l_setFun2Const = ObtainFunctionsConstantsDependent();
					set<wxString> l_setConstUsedByFun;
					for (auto it_set = l_setFun2Const.begin(); it_set != l_setFun2Const.end(); ++it_set)
					{
						pair<wxString, set<wxString>> l_pair = *it_set;
						if (l_pair.first == it_mapfun->first)
						{
							l_setConstUsedByFun = l_pair.second;
							break;
						}

					}
					for (auto it_con = l_setConstUsedByFun.begin(); it_con != l_setConstUsedByFun.end(); ++it_con)
					{
						ConstantDependencyInfo l_checkConst = FindConstantsDependencies(*it_con);
						if (l_checkConst.m_bIsUsed == false)
						{
							l_setConst.insert(*it_con);
							l_setSimpleCs.insert(l_checkConst.m_SetSimpleCS.begin(), l_checkConst.m_SetSimpleCS.end());
							l_compcsInf.m_setIndirectCompCs.insert(l_checkConst.m_SetCompoundCS.begin(), l_checkConst.m_SetCompoundCS.end());
							l_setVar.insert(l_checkConst.m_setVar.begin(), l_checkConst.m_setVar.end());
						}

					}
					//check the cs used by this fun whther if they are in use or not
					for (auto it_mCs2Type = l_mCs2Type.begin(); it_mCs2Type != l_mCs2Type.end(); ++it_mCs2Type)
					{
						string l_sToFind = (it_mCs2Type->first).ToStdString();
						auto it_findCs = l_setCs.find(l_sToFind);
						if (it_findCs != l_setCs.end())
						{
							if (it_mCs2Type->second == wxT("Simple"))
							{
								SimpleCsDependencyInfo l_checkSimple = FindSimpleCSDependencies(it_mCs2Type->first);
								if (l_checkSimple.m_bIsUSed == false)
								{

									l_setSimpleCs.insert(it_mCs2Type->first);
									l_setVar.insert(l_checkSimple.m_setVar.begin(), l_checkSimple.m_setVar.end());
									l_setConst.insert(l_checkSimple.m_setConst.begin(), l_checkSimple.m_setConst.end());
									l_setFuns.insert(l_checkSimple.m_setFun.begin(), l_checkSimple.m_setFun.end());
									l_setSimpleCs.insert(l_checkSimple.m_setIndirectCs.begin(), l_checkSimple.m_setIndirectCs.end());

								}
							}
							else {
								//indirect compound cs
								set<wxString> l_setUsedCS = ReadAllUsedCSByGraph();//ReadUsedColorsets();
								string l_sToFind = it_mCs2Type->first.ToStdString();
								auto it_foundCS = l_setUsedCS.find(l_sToFind);//check the compound cs if it is in use or not
								if (it_foundCS == l_setUsedCS.end())
								{
									l_compcsInf.m_setIndirectCompCs.insert(it_mCs2Type->first);
									//auto l_mvar2cs=ObtainAllVar2Cs();
									for (auto it_m = l_mVar2Cs.begin(); it_m != l_mVar2Cs.end(); ++it_m)
									{
										if (it_m->second == it_mCs2Type->first)
										{
											l_setVar.insert(it_m->first);

										}
									}

									for (auto it_mCs2Type1 = l_mCs2Type.begin(); it_mCs2Type1 != l_mCs2Type.end(); ++it_mCs2Type1)
									{
										if (it_mCs2Type1->first == it_mCs2Type->first &&it_mCs2Type1->second == wxT("Simple"))
										{

											SimpleCsDependencyInfo l_check2 = FindSimpleCSDependencies(it_mCs2Type1->first);
											if (l_check2.m_bIsUSed == false)
											{
												l_setSimpleCs.insert(it_mCs2Type1->first);
												l_setVar.insert(l_check2.m_setVar.begin(), l_check2.m_setVar.end());
												l_setConst.insert(l_check2.m_setConst.begin(), l_check2.m_setConst.end());
												l_setFuns.insert(l_check2.m_setFun.begin(), l_check2.m_setFun.end());
												l_setSimpleCs.insert(l_check2.m_setIndirectCs.begin(), l_check2.m_setIndirectCs.end());

											}
										}

										if (it_mCs2Type1->first == it_mCs2Type->first &&it_mCs2Type1->second == wxT("Compound"))
										{
											l_compcsInf.m_setIndirectCompCs.insert(it_mCs2Type1->first);

										}
									}

								}


							}
						}
					}

				}
				/************/

			}
		}
		/************simple cs test**************/
		map<wxString, set<wxString>> l_mComp2CS = ReadDefinedCompoundCS();
		string l_sToFind = p_sCSName.ToStdString();
		auto findcs = l_mComp2CS.find(l_sToFind);
		if (findcs != l_mComp2CS.end())
		{
			set<wxString> l_setSimple = l_mComp2CS[p_sCSName];
			for (auto it_m = l_setSimple.begin(); it_m != l_setSimple.end(); ++it_m)
			{
				SimpleCsDependencyInfo l_simplCheck = FindSimpleCSDependencies(*it_m);
				if (l_simplCheck.m_bIsUSed == false)
				{
					l_setSimpleCs.insert(*it_m);
					l_setVar.insert(l_simplCheck.m_setVar.begin(), l_simplCheck.m_setVar.end());
					l_setConst.insert(l_simplCheck.m_setConst.begin(), l_simplCheck.m_setConst.end());
					l_setFuns.insert(l_simplCheck.m_setFun.begin(), l_simplCheck.m_setFun.end());
				}
			}
		}
		l_compcsInf.m_setSimpleCs = l_setSimpleCs;
		l_compcsInf.m_setConst = l_setConst;
		l_compcsInf.m_setFun = l_setFuns;
		l_compcsInf.m_setVar = l_setVar;



	}

	return l_compcsInf;
}

pair<wxString, wxString> SP_DS_UnUsedDeclaration::ObtainAlias2Orginal(wxString p_sAlias)
{
	pair<wxString, wxString> l_pair;

	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;
	map<wxString, wxString> l_mCs2Type = ObtainAllCs2Type();
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	if (!l_pcMetadataclass)
		return l_pair;

	if (l_pcMetadataclass->GetElements()->empty())
		return l_pair;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
	if (!l_pcColList)
		return l_pair;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		if (l_sName == p_sAlias)
		{
			wxString l_sCs = l_mCs2Type[l_sColors];
			l_pair.first = l_sColors;
			l_pair.second = l_sCs;
		}
	}

	return l_pair;
}


AliasCsDependencyInfo   SP_DS_UnUsedDeclaration::FindAliasCSDependency(wxString p_sAliasName)
{
	AliasCsDependencyInfo l_AliasInfo;
	bool l_bIsUsed = false;
	pair<wxString, wxString> l_pair = ObtainAlias2Orginal(p_sAliasName);
	l_AliasInfo.m_sName = p_sAliasName;
	l_AliasInfo.m_bIsUSed = true;
	set<wxString> l_setallUsedCSBygraph = ReadAllUsedCSByGraph(); //check if the cs isused by the graph

	if (l_setallUsedCSBygraph.find(p_sAliasName) == l_setallUsedCSBygraph.end())
	{
		l_AliasInfo.m_bIsUSed = false;
	}
	if (l_pair.second == wxT("Simple") && l_AliasInfo.m_bIsUSed == false)
	{
		SimpleCsDependencyInfo l_simpleCheck = FindSimpleCSDependencies(l_pair.first);
		//	l_AliasInfo.m_bIsUSed = l_simpleCheck.m_bIsUSed;
		l_AliasInfo.m_setCompCs = l_simpleCheck.m_setCompCs;
		l_AliasInfo.m_setConst = l_simpleCheck.m_setConst;
		l_AliasInfo.m_setFun = l_simpleCheck.m_setFun;
		l_AliasInfo.m_setSimpleCs = l_simpleCheck.m_setIndirectCs;
		l_AliasInfo.m_setVar = l_simpleCheck.m_setVar;
	}
	else {
		if (l_AliasInfo.m_bIsUSed == false)
		{
			CompoundCsDependencyInfo l_compoundCheck = FindCompoundCSDependencies(l_pair.first);
			//l_AliasInfo.m_bIsUSed = l_compoundCheck.m_bIsUSed;
			l_AliasInfo.m_setCompCs = l_compoundCheck.m_setIndirectCompCs;
			l_AliasInfo.m_setConst = l_compoundCheck.m_setConst;
			l_AliasInfo.m_setFun = l_compoundCheck.m_setFun;
			l_AliasInfo.m_setSimpleCs = l_compoundCheck.m_setSimpleCs;
			l_AliasInfo.m_setVar = l_compoundCheck.m_setVar;
		}
	}
	if (l_AliasInfo.m_bIsUSed == false) {

		set<wxString> l_setUnusedvars = FindUnusedVariables();
		for (auto it_set = l_setUnusedvars.begin(); it_set != l_setUnusedvars.end(); ++it_set)
		{
			wxString l_sType = ObtainVarType(*it_set);
			if (l_sType == p_sAliasName)
			{
				l_AliasInfo.m_setVar.insert(*it_set);
			}
		}
		set<wxString> l_setunUsedfuns = FindUnusedFunctions();
		for (auto it_fun = l_setunUsedfuns.begin(); it_fun != l_setunUsedfuns.end(); ++it_fun)
		{
			set<wxString> l_setCs = ReadColorSetsUsedByFunction(*it_fun);
			if (l_setCs.find(p_sAliasName) != l_setCs.end())
			{
				l_AliasInfo.m_setFun.insert(*it_fun);
			}

		}

	}

	return l_AliasInfo;
}

/*returns all decl dependency info of a given compound cs if it is unused*/
CompoundCsDependencyInfo SP_DS_UnUsedDeclaration::FindCompoundCSDependencies(wxString p_sCSName)
{
	CompoundCsDependencyInfo l_compcsInf;
	bool l_bIsUsed = false;

	map<wxString, set<wxString>> l_mComp2Simples = ObtainCompoundCS2OtherCSDependency();

	set<wxString> l_setallUsedCSBygraph = ReadAllUsedCSByGraph(); //check if the cs isused by the graph
	set<wxString> l_setALL = l_setallUsedCSBygraph;
	for (auto it_used = l_setallUsedCSBygraph.begin(); it_used != l_setallUsedCSBygraph.end(); ++it_used)
	{
		if (l_mComp2Simples.find(*it_used) != l_mComp2Simples.end())
		{
			l_setALL.insert(*it_used);
			set<wxString> l_setChild = l_mComp2Simples[*it_used];
			l_setALL.insert(l_setChild.begin(), l_setChild.end());
		}
	}
	string l_sToFind = p_sCSName.ToStdString();
	auto it_foundcs = l_setALL.find(l_sToFind);

	if (it_foundcs != l_setALL.end())
	{
		l_bIsUsed = true;
	}
	l_compcsInf.m_bIsUSed = l_bIsUsed;

	l_compcsInf.m_sName = p_sCSName;

	if (!l_bIsUsed)
	{
		set<wxString> l_setVar;
		set<wxString> l_setFuns;
		set<wxString> l_setSimpleCs;
		set<wxString> l_setConst;

		map<wxString, wxString> l_mVar2Cs = ObtainAllVar2Cs();
		//add var defined on this cs  to the info as unused
		for (auto it_mapVar = l_mVar2Cs.begin(); it_mapVar != l_mVar2Cs.end(); ++it_mapVar)
		{
			if (it_mapVar->second == p_sCSName)
			{
				l_setVar.insert(it_mapVar->first);
			}

		}

		//add funs which use this cs as unuesed dependency
		map<wxString, set< wxString>> l_mfun2Cs = ReadFunctionsWithUsedCS();
		for (auto it_mapfun = l_mfun2Cs.begin(); it_mapfun != l_mfun2Cs.end(); ++it_mapfun)
		{
			set<wxString> l_setCs = it_mapfun->second;

			//let FunctionDependencyInfo check
			string l_STr = p_sCSName.ToStdString();
			auto findCS = l_setCs.find(l_STr);
			if (findCS != l_setCs.end())
			{
				FunctionDependencyInfo l_funcheck = FindFunctionDependencies(it_mapfun->first);
				if (l_funcheck.m_bIsUSed == false) {
					l_setFuns.insert(it_mapfun->first);
					//to add fun dependencies
					l_setSimpleCs.insert(l_funcheck.m_setSimpleCs.begin(), l_funcheck.m_setSimpleCs.end());
					l_setConst.insert(l_funcheck.m_setConst.begin(), l_funcheck.m_setConst.end());
					l_setVar.insert(l_funcheck.m_setVars.begin(), l_funcheck.m_setVars.end());
				}

			}
		}
		/************simple cs test**************/
		map<wxString, set<wxString>> l_mComp2CS = ReadDefinedCompoundCS();
		l_sToFind = p_sCSName.ToStdString();
		auto findcs = l_mComp2CS.find(l_sToFind);
		if (findcs != l_mComp2CS.end())
		{
			set<wxString> l_setSimple = l_mComp2CS[p_sCSName];
			for (auto it_m = l_setSimple.begin(); it_m != l_setSimple.end(); ++it_m)
			{
				SimpleCsDependencyInfo l_simplCheck = FindSimpleCSDependencies(*it_m);
				if (l_simplCheck.m_bIsUSed == false)
				{
					l_setSimpleCs.insert(*it_m);
					l_setVar.insert(l_simplCheck.m_setVar.begin(), l_simplCheck.m_setVar.end());
					l_setConst.insert(l_simplCheck.m_setConst.begin(), l_simplCheck.m_setConst.end());
					l_setFuns.insert(l_simplCheck.m_setFun.begin(), l_simplCheck.m_setFun.end());
				}
			}
		}

		//compute other indirect Compound/simple cs Chain (dependency)
		wxString l_sRes;
		set<wxString> l_setSimple = ObtainSimpleCS();
		map < wxString, set<wxString>> l_mComp2setSimple = ObtainCompoundCS2OtherCSDependency();
		FindCompCsChain(p_sCSName, l_setSimple, l_mComp2setSimple, l_sRes);
		string str = l_sRes.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(" +");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			if (l_setSimple.find(*tok_iter) != l_setSimple.end())
			{

				SimpleCsDependencyInfo l_simplCheck = FindSimpleCSDependencies(*tok_iter);
				if (l_simplCheck.m_bIsUSed == false)
				{
					l_setSimpleCs.insert(*tok_iter);
					l_setConst.insert(l_simplCheck.m_setConst.begin(), l_simplCheck.m_setConst.end());
					l_setFuns.insert(l_simplCheck.m_setFun.begin(), l_simplCheck.m_setFun.end());
					l_setVar.insert(l_simplCheck.m_setVar.begin(), l_simplCheck.m_setVar.end());
				}
			}
			else {
				l_compcsInf.m_setIndirectCompCs.insert(*tok_iter);
				set<wxString> l_setUnusedvars = FindUnusedVariables();
				for (auto it_set = l_setUnusedvars.begin(); it_set != l_setUnusedvars.end(); ++it_set)
				{
					wxString l_sType = ObtainVarType(*it_set);
					if (l_sType == *tok_iter)
					{
						l_setVar.insert(*it_set);
					}
				}
				set<wxString> l_setunUsedfuns = FindUnusedFunctions();
				for (auto it_fun = l_setunUsedfuns.begin(); it_fun != l_setunUsedfuns.end(); ++it_fun)
				{
					set<wxString> l_setCs = ReadColorSetsUsedByFunction(*it_fun);
					if (l_setCs.find(*tok_iter) != l_setCs.end())
					{
						l_setFuns.insert(*it_fun);
					}

				}

			}

		}

		l_compcsInf.m_setSimpleCs = l_setSimpleCs;
		l_compcsInf.m_setConst = l_setConst;
		l_compcsInf.m_setFun = l_setFuns;
		l_compcsInf.m_setVar = l_setVar;

	}

	return l_compcsInf;
}

/*returns all decl dependency info of a given simple cs if it is unused*/
SimpleCsDependencyInfo SP_DS_UnUsedDeclaration::FindSimpleCSDependencies(wxString p_sCSName)
{
	SimpleCsDependencyInfo l_CsInfo;
	bool l_bISUsed = false;

	map<wxString, set<wxString>> l_mComp2Simples = ObtainCompoundCS2OtherCSDependency();

	set<wxString> l_setallUsedCSBygraph = ReadAllUsedCSByGraph(); //check if the cs isused by the graph
	set<wxString> l_setALL = l_setallUsedCSBygraph;
	for (auto it_used = l_setallUsedCSBygraph.begin(); it_used != l_setallUsedCSBygraph.end(); ++it_used)
	{
		if (l_mComp2Simples.find(*it_used) != l_mComp2Simples.end())
		{
			l_setALL.insert(*it_used);
			set<wxString> l_setChild = l_mComp2Simples[*it_used];
			l_setALL.insert(l_setChild.begin(), l_setChild.end());
		}
	}


	auto it_foundcs = l_setALL.find(p_sCSName);

	if (it_foundcs != l_setALL.end())
	{
		l_bISUsed = true;
	}
	l_CsInfo.m_bIsUSed = l_bISUsed;

	l_CsInfo.m_sName = p_sCSName;

	if (!l_bISUsed)
	{
		set<wxString> l_setVar;
		set<wxString> l_setFuns;
		set<wxString> l_setCompCs;
		set<wxString> l_setConst;

		map<wxString, wxString> l_mVar2Cs = ObtainAllVar2Cs();
		//add var defined on this cs  to the info as unused
		for (auto it_mapVar = l_mVar2Cs.begin(); it_mapVar != l_mVar2Cs.end(); ++it_mapVar)
		{
			if (it_mapVar->second == p_sCSName)
			{
				l_setVar.insert(it_mapVar->first);
			}

		}
		//add funs which use this cs as unuesed dependency
		map<wxString, set< wxString>> l_mfun2Cs = ReadFunctionsWithUsedCS();
		for (auto it_mapfun = l_mfun2Cs.begin(); it_mapfun != l_mfun2Cs.end(); ++it_mapfun)
		{
			set<wxString> l_setCs = it_mapfun->second;

			//let FunctionDependencyInfo check
			auto findCS = l_setCs.find(p_sCSName);
			if (findCS != l_setCs.end())
			{
				set<wxString> l_setUnusedFuns = FindUnusedFunctions();
				auto it_found = l_setUnusedFuns.find(it_mapfun->first);
				if (it_found != l_setUnusedFuns.end()) {
					l_setFuns.insert(it_mapfun->first);
					//to add fun dependencies later
				}

			}
		}

		//add constant (used to define this cs and this constant is not used )dependency info to this simple cs info
		wxString l_sLinkedCompCs;
		map<wxString, set<wxString>> l_mCom2Simple = ObtainCompCs2SimpleOneLevel();
		FindSimpleCsChain(p_sCSName, l_mCom2Simple, l_sLinkedCompCs);

		string str = l_sLinkedCompCs.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep(" +");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			l_setCompCs.insert(*tok_iter);
		}

		set<pair<wxString, wxString>> l_setCs2Const = ObtainSimpleCS2ContantsDependentOutofGraph();
		for (auto it_setcs2con = l_setCs2Const.begin(); it_setcs2con != l_setCs2Const.end(); ++it_setcs2con)
		{
			pair<wxString, wxString> l_pair = *it_setcs2con;
			if (l_pair.first == p_sCSName)
			{
				ConstantDependencyInfo l_constantInf = FindConstantsDependencies(l_pair.second);
				if (l_constantInf.m_bIsUsed == false)
				{
					l_setConst.insert(l_pair.second);
					l_setVar.insert(l_constantInf.m_setVar.begin(), l_constantInf.m_setVar.end());
					l_setFuns.insert(l_constantInf.m_setFun.begin(), l_constantInf.m_setFun.end());
					l_setCompCs.insert(l_constantInf.m_SetCompoundCS.begin(), l_constantInf.m_SetCompoundCS.end());
				}
			}

		}
		l_CsInfo.m_setVar = l_setVar;
		l_CsInfo.m_setFun = l_setFuns;
		l_CsInfo.m_setCompCs = l_setCompCs;
		l_CsInfo.m_setConst = l_setConst;
	}
	return l_CsInfo;
}

/*return all unused declarations of a given function if this function is unused*/
FunctionDependencyInfo  SP_DS_UnUsedDeclaration::FindFunctionDependencies(wxString p_sfunName)
{
	FunctionDependencyInfo l_funInfo;
	bool                 l_bIsFunUsed = true;
	l_funInfo.m_bIsUSed = l_bIsFunUsed;
	set<wxString> l_setUnusedSimplCSDep;
	set<wxString> l_setUnusedCompCSDep;
	set<wxString> l_setUnusedCSs;
	bool l_bIsColoured = false;

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	set<wxString> l_setAllDefinedConstants = ObtainAllDefinedConstants();
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}


	std::set<wxString>   l_setUnUsedFunscheck = FindUnusedFunctions();
	auto it_find = l_setUnUsedFunscheck.find(p_sfunName);
	if (it_find != l_setUnUsedFunscheck.end())
	{
		l_bIsFunUsed = false;
		l_funInfo.m_bIsUSed = l_bIsFunUsed;
	}
	l_funInfo.m_sFunName = p_sfunName;
	if (!l_bIsFunUsed && l_bIsColoured)
	{//check dependencies
		map<wxString, set<wxString>> l_mAllfun2SimpleCompoundCs = ReadFunctionsWithUsedCS();//get all defined funs with their CSs(simple +compound)

		set<wxString> l_setUsedCsByFun = l_mAllfun2SimpleCompoundCs[p_sfunName];//CSs used by given function

		set<wxString> allUsedCSBygraph = ReadAllUsedCSByGraph();//all used CSs in graph

		for (auto it_setfun = l_setUsedCsByFun.begin(); it_setfun != l_setUsedCsByFun.end(); ++it_setfun)

		{
			auto findcs = allUsedCSBygraph.find(*it_setfun);
			if (findcs == allUsedCSBygraph.end())
			{
				l_setUnusedCSs.insert(*it_setfun);
			}
		}
		/*********store unused cs*******/

		map<wxString, wxString> l_mapAllCS2Type = ObtainAllCs2Type();
		for (auto it_map = l_mapAllCS2Type.begin(); it_map != l_mapAllCS2Type.end(); ++it_map)
		{
			auto foundtype = l_setUnusedCSs.find(it_map->first);
			if (foundtype != l_setUnusedCSs.end())
			{
				if (it_map->second == wxT("Simple"))
				{
					SimpleCsDependencyInfo l_checkSimpleCs = FindSimpleCSDependencies(it_map->first);
					if (l_checkSimpleCs.m_bIsUSed == false)
					{
						l_setUnusedSimplCSDep.insert(it_map->first);
						l_setUnusedSimplCSDep.insert(l_checkSimpleCs.m_setIndirectCs.begin(), l_checkSimpleCs.m_setIndirectCs.end());
						l_setUnusedCompCSDep.insert(l_checkSimpleCs.m_setCompCs.begin(), l_checkSimpleCs.m_setCompCs.end());
						l_funInfo.m_setVars.insert(l_checkSimpleCs.m_setVar.begin(), l_checkSimpleCs.m_setVar.end());
						l_funInfo.m_setConst.insert(l_checkSimpleCs.m_setConst.begin(), l_checkSimpleCs.m_setConst.end());
					}



				}
				else if (it_map->second == wxT("Compound"))
				{
					CompoundCsDependencyInfo l_checkComp = FindCompoundCSDependencies(it_map->first);
					if (l_checkComp.m_bIsUSed == false)
					{
						l_setUnusedCompCSDep.insert(it_map->first);
						l_setUnusedCompCSDep.insert(l_checkComp.m_setIndirectCompCs.begin(), l_checkComp.m_setIndirectCompCs.end());
						l_setUnusedSimplCSDep.insert(l_checkComp.m_setSimpleCs.begin(), l_checkComp.m_setSimpleCs.end());
						l_funInfo.m_setVars.insert(l_checkComp.m_setVar.begin(), l_checkComp.m_setVar.end());
						l_funInfo.m_setConst.insert(l_checkComp.m_setConst.begin(), l_checkComp.m_setConst.end());
					}

				}
			}
		}

		//l_funInfo.m_bIsUSed = true;
		l_funInfo.m_setSimpleCs = l_setUnusedSimplCSDep;
		l_funInfo.m_setCompoundCs = l_setUnusedCompCSDep;


		/***********check constants if dependent or not by un-used function*********/
		set<pair<wxString, set<wxString>>> l_setFun2Const = ObtainFunctionsConstantsDependent();
		set<wxString> l_setConstUsedByFun;
		for (auto it_set = l_setFun2Const.begin(); it_set != l_setFun2Const.end(); ++it_set)
		{
			pair<wxString, set<wxString>> l_pair = *it_set;
			if (l_pair.first == p_sfunName)
			{
				l_setConstUsedByFun = l_pair.second;
				break;
			}

		}
		for (auto it_con = l_setConstUsedByFun.begin(); it_con != l_setConstUsedByFun.end(); ++it_con)
		{
			ConstantDependencyInfo l_checkConst = FindConstantsDependencies(*it_con);
			if (l_checkConst.m_bIsUsed == false)
			{
				l_funInfo.m_setConst.insert(*it_con);
				l_funInfo.m_setSimpleCs.insert(l_checkConst.m_SetSimpleCS.begin(), l_checkConst.m_SetSimpleCS.end());
				l_funInfo.m_setCompoundCs.insert(l_checkConst.m_SetCompoundCS.begin(), l_checkConst.m_SetCompoundCS.end());
				l_funInfo.m_setVars.insert(l_checkConst.m_setVar.begin(), l_checkConst.m_setVar.end());
			}

		}

	}
	else {
		//uncolored pn
		if (!l_bIsFunUsed)
		{
			set<pair<wxString, set<wxString>>> l_setFun2Const = ObtainFunctionsConstantsDependent();
			set<wxString> l_setAllUnusedsedConStantsInNet = FindUnusedConstants();
			set<wxString> l_setConstUsedByUnusedFun;
			for (auto it_set = l_setFun2Const.begin(); it_set != l_setFun2Const.end(); ++it_set)
			{
				pair<wxString, set<wxString>> l_pair = *it_set;
				if (l_pair.first == p_sfunName)
				{
					l_setConstUsedByUnusedFun.insert(l_pair.second.begin(), l_pair.second.end());
					//break;
				}

			}
			for (auto it_con = l_setConstUsedByUnusedFun.begin(); it_con != l_setConstUsedByUnusedFun.end(); ++it_con)
			{
				ConstantDependencyInfo l_checkConst = FindConstantsDependencies(*it_con);
				if (l_checkConst.m_bIsUsed == false)
				{
					l_funInfo.m_setConst.insert(*it_con);
					l_funInfo.m_setObservers.insert(l_checkConst.m_setObservers.begin(), l_checkConst.m_setObservers.end());
				}

			}

			//look for constants which use this function to init its values and set them as backword dependency
			map<wxString, wxString> l_mConst2Val = ObtainAllDefinedConstants2Valuses();
			for (auto it_m = l_mConst2Val.begin(); it_m != l_mConst2Val.end(); it_m++)
			{
				wxString l_sConst = it_m->first;
				wxString l_sVal = it_m->second;

				std::string str = l_sVal.ToStdString();
				typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
				boost::char_separator<char> sep("!-|+*(){}[]%/&=<> ");
				tokenizer tokens(str, sep);
				for (tokenizer::iterator tok_iter = tokens.begin();
					tok_iter != tokens.end(); ++tok_iter)
				{
					wxString l_sTkn = *tok_iter;
					bool l_bisNum = false;
					if (!l_sTkn.empty() && std::all_of(l_sTkn.begin(), l_sTkn.end(), ::isdigit))
					{
						l_bisNum = true;
					}
					if (!l_bisNum)
					{
						if (p_sfunName == l_sTkn)
						{
							l_funInfo.m_setBackwordConst.insert(l_sConst);
						}
					}
				}


			}
			//look for observers which use this function to init its values, add them as backword dep.
			set<wxString> l_setUnusedObservers = FindUnusedObservers();
			std::map<wxString, wxString>   l_mObserver2Body = ReadObserverBodies();
			for (auto it_m = l_mObserver2Body.begin(); it_m != l_mObserver2Body.end(); ++it_m)
			{
				auto l_itFound = l_setUnusedObservers.find(it_m->first);
				if (l_itFound != l_setUnusedObservers.end())
				{
					if (it_m->second.Contains(p_sfunName))
					{
						l_funInfo.m_setBackwordObservers.insert(it_m->first);

					}

				}
				else {
					if (it_m->second.Contains(p_sfunName))
					{
						l_funInfo.m_bIsUSed = true;

					}
				}
			}
		}

	}
	return l_funInfo;
}

/*returns all decl dependency info of a given variable if it is unused*/
VariableDependencyInfo  SP_DS_UnUsedDeclaration::FindVariableDependencies(wxString p_sVarName)
{
	VariableDependencyInfo l_varDepInfo;
	bool l_bIsUsed = false;
	std::set<string> l_setAllusedVars = ReadColPNVariables();

	set<wxString> l_setVarasMarking = ReadNetMarking();//25.11

	l_setAllusedVars.insert(l_setVarasMarking.begin(), l_setVarasMarking.end());//25.11
	auto foundvar = l_setAllusedVars.find(p_sVarName);
	if (foundvar != l_setAllusedVars.end())
	{
		l_bIsUsed = true;

	}

	set<wxString> l_setAllusedCs = ReadAllUsedCSByGraph();
	//scan vars in sub cs color components, then check if this sub cs is used by the graph
	//then this var (sub cs color components) it must bbe marked as used
	set<wxString> l_setAllDefinedVars = ObtainAllDefinedVars();
	map<wxString, set<wxString>> l_mSub2Colors = ObtainSubCs2ColorComponents();

	set<wxString> l_setColors;
	set<wxString> l_setVarColor;
	for (auto itmap = l_mSub2Colors.begin(); itmap != l_mSub2Colors.end(); ++itmap)
	{
		l_setColors = itmap->second;
		for (auto itset = l_setColors.begin(); itset != l_setColors.end(); ++itset)
		{
			auto found = l_setAllDefinedVars.find(*itset);
			if (found != l_setAllDefinedVars.end())
			{
				auto foundcs = l_setAllusedCs.find(itmap->first);
				if (foundcs != l_setAllusedCs.end())
				{
					l_setVarColor.insert(*itset);
				}
			}
		}
	}

	auto findType = l_setVarColor.find(p_sVarName);
	if (findType != l_setVarColor.end())
	{
		l_bIsUsed = true;
	}

	l_varDepInfo.m_sName = p_sVarName;
	l_varDepInfo.m_bIsUSed = l_bIsUsed;
	if (!l_bIsUsed)
	{
		wxString l_sVarType = ObtainVarType(p_sVarName);
		map<wxString, wxString> allcs2type = ObtainAllCs2Type();
		SimpleCsDependencyInfo l_simplCsCheck;
		CompoundCsDependencyInfo l_compoundCsCheck;
		SubSetCsDependencyInfo   l_subCScheck;
		set<wxString> l_setConst;
		set<wxString> l_setSimpleCS;
		set<wxString> l_setCompCs;
		set<wxString> l_setFun;
		wxString l_sCsType = allcs2type[l_sVarType];
		if (l_sCsType == wxT("Simple"))
		{
			l_simplCsCheck = FindSimpleCSDependencies(l_sVarType);
			if (l_simplCsCheck.m_bIsUSed == false)
			{
				l_setSimpleCS.insert(l_sVarType);
				l_setConst.insert(l_simplCsCheck.m_setConst.begin(), l_simplCsCheck.m_setConst.end());
				l_setCompCs.insert(l_simplCsCheck.m_setCompCs.begin(), l_simplCsCheck.m_setCompCs.end());
				l_setFun.insert(l_simplCsCheck.m_setFun.begin(), l_simplCsCheck.m_setFun.end());

			}
		}
		else if (l_sCsType == wxT("Compound"))
		{
			l_compoundCsCheck = FindCompoundCSDependencies(l_sVarType);
			if (l_compoundCsCheck.m_bIsUSed == false)
			{
				l_setCompCs.insert(l_sVarType);
				l_setConst.insert(l_compoundCsCheck.m_setConst.begin(), l_compoundCsCheck.m_setConst.end());
				l_setSimpleCS.insert(l_compoundCsCheck.m_setSimpleCs.begin(), l_compoundCsCheck.m_setSimpleCs.end());
				l_setFun.insert(l_compoundCsCheck.m_setFun.begin(), l_compoundCsCheck.m_setFun.end());


			}


		}
		else {
			l_subCScheck = FindSubCSDependency(l_sVarType);
			if (l_subCScheck.m_bIsUSed == false)
			{
				l_setCompCs.insert(l_sVarType);
				l_setConst.insert(l_subCScheck.m_setConst.begin(), l_subCScheck.m_setConst.end());
				l_setSimpleCS.insert(l_subCScheck.m_setSimpleCs.begin(), l_subCScheck.m_setSimpleCs.end());
				l_setFun.insert(l_subCScheck.m_setFun.begin(), l_subCScheck.m_setFun.end());

			}
		}
		l_varDepInfo.m_setConst = l_setConst;
		l_varDepInfo.m_CompoundCS = l_setCompCs;
		l_varDepInfo.m_setFun = l_setFun;
		l_varDepInfo.m_setSimpleCs = l_setSimpleCS;

	}
	return l_varDepInfo;

}

/*returns all decl dependency info of a given observer if it is unused*/
ObserverDependencyInfo  SP_DS_UnUsedDeclaration::FindObserverDependency(wxString p_sObserverName)
{
	ObserverDependencyInfo l_ObserverDepIfo;
	bool l_bIsConstUsed = false;
	std::set<wxString>   l_setUnusedObservers = FindUnusedObservers();
	auto foundObs = l_setUnusedObservers.find(p_sObserverName);
	if (foundObs == l_setUnusedObservers.end())
	{
		l_bIsConstUsed = true;
	}
	l_ObserverDepIfo.m_bIsUsed = l_bIsConstUsed;
	l_ObserverDepIfo.m_observerName = p_sObserverName;
	if (!l_bIsConstUsed)
	{
		set<wxString> l_setUnusedCon;
		set<wxString> l_setUnusedFun;
		map<wxString, set< wxString>> l_mObs2Constants = ObtainObserversConstantsDependent();
		set<wxString> l_setConst = l_mObs2Constants[p_sObserverName];
		for (auto it = l_setConst.begin(); it != l_setConst.end(); ++it)
		{
			ConstantDependencyInfo l_ConstInf = FindConstantsDependencies(*it);
			if (l_ConstInf.m_bIsUsed == false)
			{
				l_setUnusedCon.insert(*it);
				l_setUnusedFun.insert(l_ConstInf.m_setFun.begin(), l_ConstInf.m_setFun.end());
			}

		}

		set<wxString> l_setUnusedFun1 = FindUnusedFunctions();
		map<wxString, wxString> l_mObserver2Body = ReadObserverBodies();
		for (auto it_m = l_mObserver2Body.begin(); it_m != l_mObserver2Body.end(); ++it_m)
		{
			if (it_m->first == p_sObserverName)
			{
				wxString l_sBody = it_m->second;
				for (auto it_set = l_setUnusedFun1.begin(); it_set != l_setUnusedFun1.end(); ++it_set)
				{
					if (l_sBody.Contains(*it_set))
					{
						l_ObserverDepIfo.m_setForwardFuns.insert(*it_set);
						FunctionDependencyInfo l_FunInf = FindFunctionDependencies(*it_set);
						l_setUnusedCon.insert(l_FunInf.m_setBackwordConst.begin(), l_FunInf.m_setBackwordConst.end());
					}
				}
			}
		}
		l_ObserverDepIfo.m_setFuns = l_setUnusedFun;
		l_ObserverDepIfo.m_setConstants = l_setUnusedCon;
	}


	return l_ObserverDepIfo;

}

void   SP_DS_UnUsedDeclaration::FindConChain(wxString p_sConst, set<wxString> &p_setVals, map<wxString, wxString>&p_mname2val, wxString &p_sRes)
{
	if (p_setVals.find(p_sConst) == p_setVals.end())
	{
		return;
	}
	else
	{
		wxString l_sVal;
		for (auto it_m = p_mname2val.begin(); it_m != p_mname2val.end(); it_m++)
		{
			if (p_sConst == it_m->second | it_m->second.Contains(p_sConst))//26.11
			{
				p_sRes << wxT("+") << (it_m->first);
				l_sVal = it_m->first;
				p_mname2val.erase(it_m->first);
				p_mname2val.erase(p_sConst);

				break;
			}

		}
		return	FindConChain(l_sVal, p_setVals, p_mname2val, p_sRes);

	}
	return;
}

set<wxString> SP_DS_UnUsedDeclaration::FindOppositeConstanChain(wxString p_scnst, set<wxString> &p_setChain)
{
	set<wxString> l_setResult, l_setValues;
	map<wxString, wxString> l_mDefinedConst = ObtainAllDefinedConstants2Valuses();
	for (auto it_set = l_mDefinedConst.begin(); it_set != l_mDefinedConst.end(); it_set++)
	{
		l_setValues.insert(it_set->second);
		wxString l_sVal = it_set->second;// p_mname2val[p_sConst];
		std::string str = l_sVal.ToStdString(); bool f1 = false;
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep("-|+*()/% ");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			wxString l_tkn = *tok_iter;
			bool l_bisNum = false;
			if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
			{
				l_bisNum = true;
			}
			if (!l_bisNum)
			{
				l_setValues.insert(l_tkn);
			}
		}
	}
	wxString l_sRes;
	FindConChain(p_scnst, l_setValues, l_mDefinedConst, l_sRes);

	string str = l_sRes.ToStdString();
	typedef boost::tokenizer<boost::char_separator<char> >
		tokenizer;
	boost::char_separator<char> sep(" +");
	tokenizer tokens(str, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
	{
		p_setChain.insert(*tok_iter);
	}

	return l_setResult;

}

set<wxString> SP_DS_UnUsedDeclaration::FindOppositeConstantDependChain(wxString p_scnst, set<wxString> &p_setChain)
{
	set<wxString> l_setRes;
	set<wxString> l_setValues;

	map<wxString, wxString> l_mDefinedConst = ObtainAllDefinedConstants2Valuses();
	for (auto it_set = l_mDefinedConst.begin(); it_set != l_mDefinedConst.end(); it_set++)
	{
		l_setValues.insert(it_set->second);
	}

	auto it_found = l_setValues.find(p_scnst);
	if (it_found == l_setValues.end())
	{

		return l_setRes;

	}
	else {
		wxString l_sVal;

		for (auto it_set = l_mDefinedConst.begin(); it_set != l_mDefinedConst.end(); it_set++)
		{
			if (it_set->second == p_scnst)
			{
				l_sVal = it_set->first;
				p_setChain.insert(l_sVal);

				break;
			}
		}
		l_setRes.insert(p_setChain.begin(), p_setChain.end());
		return FindOppositeConstantDependChain(l_sVal, p_setChain);

	}


}

set<wxString> SP_DS_UnUsedDeclaration::FindConstantDependChain(wxString p_scnst, set<wxString> &p_setChain)
{
	set<wxString> l_setRes, l_setValues;
	map<wxString, wxString> l_mDefinedConst = ObtainAllDefinedConstants2Valuses();
	//wxString l_sVal = l_mDefinedConst[p_scnst];
	for (auto it_m = l_mDefinedConst.begin(); it_m != l_mDefinedConst.end(); ++it_m)
	{
		l_setValues.insert(it_m->second);
	}
	wxString l_sRes;
	FindForwardDepChain(p_scnst, l_setValues, l_mDefinedConst, l_sRes);

	string str = l_sRes.ToStdString();
	typedef boost::tokenizer<boost::char_separator<char> >
		tokenizer;
	boost::char_separator<char> sep(" ++");
	tokenizer tokens(str, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
	{
		p_setChain.insert(*tok_iter);
	}

	return l_setRes;

}

void   SP_DS_UnUsedDeclaration::FindForwardDepChain(wxString p_sConst, set<wxString> &p_setVals, map<wxString, wxString>&p_mname2val, wxString &p_sRes)
{
	wxString l_sVal = p_mname2val[p_sConst];
	std::string str = l_sVal.ToStdString(); bool f1 = false;
	typedef boost::tokenizer<boost::char_separator<char> >
		tokenizer;
	boost::char_separator<char> sep("-|+*()/% ");
	tokenizer tokens(str, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
	{
		wxString l_tkn = *tok_iter;
		bool l_bisNum = false;
		if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
		{
			l_bisNum = true;
		}
		if (!l_bisNum)
		{
			p_sRes << wxT("++") << (l_tkn);
			f1 = true;
		}
	}
	if (p_mname2val.find(l_sVal) == p_mname2val.end() && f1 == false)
	{
		return;
	}
	else
	{
		//wxString l_sVal;
		for (auto it_m = p_mname2val.begin(); it_m != p_mname2val.end(); it_m++)
		{
			if (l_sVal == it_m->second)
			{
				p_sRes << wxT("++") << (it_m->second);
				l_sVal = it_m->second;


				break;
			}

		}
		return	FindForwardDepChain(l_sVal, p_setVals, p_mname2val, p_sRes);

	}

}

/*find all dependencies of an unused constant*/
ConstantDependencyInfo SP_DS_UnUsedDeclaration::FindConstantsDependencies(wxString p_sConstantName)
{
	ConstantDependencyInfo l_ConstInfo;
	l_ConstInfo.m_bIsUsed = true;
	set<wxString> l_setUsedConstInGraph = ObtainUsedConstantsInGraph();
	set<wxString> l_setUnusedDependentFun;
	set<wxString> l_setPredicates;
	bool l_bIsConstUsed = false;

	bool l_bIsColoured = false;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;

	}
	//search UsedConstantInGraph for the input constant, if it is not find then continue checking
	auto it_foundConstInGraph = l_setUsedConstInGraph.find(p_sConstantName);

	//search used functions for the intended constants if it is found then the const is used
	set<pair<wxString, set< wxString>>> l_setFun2Const = ObtainFunctionsConstantsDependent();

	std::set<wxString>   l_setUnUsedFunscheck = FindUnusedFunctions();

	for (auto it_setfun = l_setFun2Const.begin(); it_setfun != l_setFun2Const.end(); ++it_setfun)
	{
		pair<wxString, set< wxString>> l_pair = *it_setfun;

		auto findf = l_setUnUsedFunscheck.find(l_pair.first);
		if (findf == l_setUnUsedFunscheck.end())
		{
			//fun is used, check if it uses the given constant or not
			auto findConst = l_pair.second.find(p_sConstantName);
			if (findConst != l_pair.second.end())
			{
				l_bIsConstUsed = true;
				break;
			}
		}
	}


	if (it_foundConstInGraph == l_setUsedConstInGraph.end() && !l_bIsConstUsed && l_bIsColoured)
	{
		set<wxString> l_setSimpleCs = ObtainSimpleCsConstantsDependent(p_sConstantName);//all simple cs used by the given constant
		set<wxString> l_setCompundCS = ObtainAllCompoundCSConstantDependent(l_setSimpleCs); //all compound cs used by the given constant
		set<wxString> l_setAllcsDepofConst;

		l_setAllcsDepofConst.insert(l_setSimpleCs.begin(), l_setSimpleCs.end());
		l_setAllcsDepofConst.insert(l_setCompundCS.begin(), l_setCompundCS.end());

		set<wxString> l_setAllusedCSBytheGraph = ReadAllUsedCSByGraph();
		map<wxString, set<wxString>> l_mAllfun2SimpleCompoundCs = ReadFunctionsWithUsedCS();//get all defined funs with their CSs(simple +compound)
		std::set<wxString>   l_setUnUsedFuns = FindUnusedFunctions();//unused functions

		for (auto it_m = l_mAllfun2SimpleCompoundCs.begin(); it_m != l_mAllfun2SimpleCompoundCs.end(); ++it_m)
		{
			auto it_findfun = l_setUnUsedFuns.find(it_m->first);
			if (it_findfun != l_setUnUsedFuns.end())
			{
				std::set<wxString> l_setCs = it_m->second;
				for (auto it_s = l_setCs.begin(); it_s != l_setCs.end(); ++it_s)
				{
					auto itfoundcs = l_setAllcsDepofConst.find(*it_s);
					auto itfoundcsIngraph = l_setAllusedCSBytheGraph.find(*it_s);//cs of the fun must not used by the graph
					if (itfoundcs != l_setAllcsDepofConst.end() && itfoundcsIngraph == l_setAllusedCSBytheGraph.end())
					{
						l_setUnusedDependentFun.insert(it_m->first);
					}
				}

			}
		}

		//unused dependent simple and compound  cs
		set<wxString> l_setUnUsedSimpleCs;
		set<wxString> l_setUnUsedCompoundCs;
		set<wxString> l_setUnUsedVar;
		for (auto it_simplcs = l_setSimpleCs.begin(); it_simplcs != l_setSimpleCs.end(); ++it_simplcs)
		{
			auto find = l_setAllusedCSBytheGraph.find(*it_simplcs);
			if (find == l_setAllusedCSBytheGraph.end())
			{
				l_setUnUsedSimpleCs.insert(*it_simplcs);
			}
		}

		for (auto it_compcs = l_setCompundCS.begin(); it_compcs != l_setCompundCS.end(); ++it_compcs)
		{
			auto find = l_setAllusedCSBytheGraph.find(*it_compcs);
			if (find == l_setAllusedCSBytheGraph.end())
			{
				l_setUnUsedCompoundCs.insert(*it_compcs);
			}
		}

		map<wxString, wxString> l_mvar2cs = ObtainAllVar2Cs();
		std::set<string>   l_setUsedVar = ReadColPNVariables();

		for (auto it_mVar = l_mvar2cs.begin(); it_mVar != l_mvar2cs.end(); ++it_mVar)
		{
			auto findVar = l_setUsedVar.find(it_mVar->first);
			if (findVar == l_setUsedVar.end())
			{
				auto finccofVar = l_setAllusedCSBytheGraph.find(it_mVar->second);
				auto fintype = l_setAllcsDepofConst.find(it_mVar->second);
				if (finccofVar == l_setAllusedCSBytheGraph.end() && fintype != l_setAllcsDepofConst.end())
				{
					l_setUnUsedVar.insert(it_mVar->first);
				}

			}
		}

		set<wxString> l_setChain, l_setOppChain;
		FindConstantDependChain(p_sConstantName, l_setChain);
		set<wxString> kk = FindOppositeConstanChain(p_sConstantName, l_setOppChain);
		l_ConstInfo.m_sConstName = p_sConstantName;
		l_ConstInfo.m_setConstants = l_setChain;
		l_ConstInfo.m_setOppositeDepConstants = l_setOppChain;
		l_ConstInfo.m_SetSimpleCS = l_setUnUsedSimpleCs;
		l_ConstInfo.m_SetCompoundCS = l_setUnUsedCompoundCs;
		l_ConstInfo.m_setFun = l_setUnusedDependentFun;
		l_ConstInfo.m_setVar = l_setUnUsedVar;
		l_ConstInfo.m_bIsUsed = false;
	}
	else {
		//uncolored PN
		if (!l_bIsConstUsed && it_foundConstInGraph == l_setUsedConstInGraph.end())//not used in functions
		{
			set<wxString> l_setFunctions;
			set<wxString> l_setObservers;
			set<pair<wxString, set< wxString>>>    l_setFun2Const = ObtainFunctionsConstantsDependent();
			std::set<wxString>   l_setUnusedObservers = FindUnusedObservers();
			map<wxString, set< wxString>> l_mObs2Constants = ObtainObserversConstantsDependent();

			for (auto it_map = l_mObs2Constants.begin(); it_map != l_mObs2Constants.end(); ++it_map)
			{
				wxString l_sObName = it_map->first;
				set<wxString> l_setConst = it_map->second;

				auto findCon = l_setConst.find(p_sConstantName);
				if (findCon != l_setConst.end())
				{
					l_setObservers.insert(l_sObName);
				}

			}

			for (auto it_set = l_setFun2Const.begin(); it_set != l_setFun2Const.end(); ++it_set)
			{
				pair<wxString, set< wxString>>  l_pairFun2Consts = *it_set;
				auto findCon = l_pairFun2Consts.second.find(p_sConstantName);
				if (findCon != l_pairFun2Consts.second.end())
				{
					l_setFunctions.insert(l_pairFun2Consts.first);
				}
			}

			set<wxString> l_setChain, l_setOppChain;
			FindConstantDependChain(p_sConstantName, l_setChain);
			set<wxString> kk = FindOppositeConstanChain(p_sConstantName, l_setOppChain);

			//check if the unused constant depends on unused function

			map<wxString, wxString> l_mConst2Val = ObtainAllDefinedConstants2Valuses();

			wxString l_sConstVal = l_mConst2Val[p_sConstantName];

			std::string str = l_sConstVal.ToStdString();
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep("!-|+*(){}[]%/&=<> ");
			tokenizer tokens(str, sep);
			for (tokenizer::iterator tok_iter = tokens.begin();
				tok_iter != tokens.end(); ++tok_iter)
			{
				wxString l_sTkn = *tok_iter;
				bool l_bisNum = false;
				if (!l_sTkn.empty() && std::all_of(l_sTkn.begin(), l_sTkn.end(), ::isdigit))
				{
					l_bisNum = true;
				}
				if (!l_bisNum)
				{
					auto l_itFindFun = l_setUnUsedFunscheck.find(l_sTkn);
					if (l_itFindFun != l_setUnUsedFunscheck.end())
					{
						l_ConstInfo.m_setForwardFun.insert(l_sTkn);
					}
				}
			}

			l_ConstInfo.m_setConstants = l_setChain;
			l_ConstInfo.m_setOppositeDepConstants = l_setOppChain;
			l_ConstInfo.m_sConstName = p_sConstantName;
			l_ConstInfo.m_setFun = l_setFunctions;
			l_ConstInfo.m_setObservers = l_setObservers;
			l_ConstInfo.m_bIsUsed = false;

		}

	}

	return l_ConstInfo;
}

/*return set of all defined functions*/
set<wxString> SP_DS_UnUsedDeclaration::ReadAllDefinedFunctions()
{
	set<wxString> l_setFunList;

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();


	if (!l_netType.Contains(wxT("Colored")))
	{

		SP_DS_Metadataclass*	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
		if (l_pcMetadataclass)
		{
			SP_ListMetadata::const_iterator l_itElem;
			for (l_itElem = l_pcMetadataclass->GetElements()->begin();
				l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Metadata* l_pcMetadata = *l_itElem;
				wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();

				l_setFunList.insert(l_sName);
			}
		}


		return l_setFunList;
	}

	SP_DS_Metadataclass*	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);


	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		set<wxString> l_setCS;
		wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
		l_setFunList.insert(l_sName);
		//wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();
	}

	return l_setFunList;
}

/*returns map of functions 2 Cs*/
map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ReadFunctionsWithUsedCS()
{
	map<wxString, set<wxString>> l_mFun2UsedCs;

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();

	SP_DS_Metadataclass*	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);


	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));

	set<wxString> l_setAllDefinedCS = ObtainAllDefinedCS();
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		set<wxString> l_setCS;
		wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();
		std::string str = l_sColors.ToStdString();
		typedef boost::tokenizer<boost::char_separator<char> >
			tokenizer;
		boost::char_separator<char> sep(" ,");
		tokenizer tokens(str, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
		{
			auto csfind = l_setAllDefinedCS.find(*tok_iter);
			if (csfind != l_setAllDefinedCS.end())
			{
				l_setCS.insert(*tok_iter);
			}
		}
		l_mFun2UsedCs[l_sName] = l_setCS;
	}
	return l_mFun2UsedCs;
}

/*returns set of all defined cs*/
set<wxString> SP_DS_UnUsedDeclaration::ObtainAllDefinedCS()
{
	set <wxString> l_setAllCs;
	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;

	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);


	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		l_setAllCs.insert(l_sName);
	}


	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);



	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		l_setAllCs.insert(l_sName);
	}


	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);


	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		l_setAllCs.insert(l_sName);

	}
	return l_setAllCs;

}
/*returns set of constants used to define compound cs indirectly*/
set<wxString> SP_DS_UnUsedDeclaration::ObtainConstantsUsed2DefineCompoundCS(wxString p_sCompCS)
{
	set<wxString> l_setInDirectConst;
	set<wxString> l_setCoresspondingSimple;
	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;

	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		if (l_sName == p_sCompCS)
		{
			wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();

			std::string str = l_sColors.ToStdString();
			typedef boost::tokenizer<boost::char_separator<char> >
				tokenizer;
			boost::char_separator<char> sep("-, ");
			tokenizer tokens(str, sep);
			for (tokenizer::iterator tok_iter = tokens.begin();
				tok_iter != tokens.end(); ++tok_iter)
			{
				l_setCoresspondingSimple.insert(*tok_iter);
			}

		}

	}

	set<pair<wxString, wxString>> l_setSimple2Const = ObtainSimpleCsConstantsDependent();

	for (auto it_set = l_setCoresspondingSimple.begin(); it_set != l_setCoresspondingSimple.end(); ++it_set)
	{
		for (auto it_con = l_setSimple2Const.begin(); it_con != l_setSimple2Const.end(); ++it_con)
		{
			pair<wxString, wxString> l_pair = *it_con;
			if (l_pair.first == *it_set)
			{
				l_setInDirectConst.insert(l_pair.second);
			}
		}
	}
	return l_setInDirectConst;
}

/*returns the colors of SubCs */
map<wxString, set<wxString>> SP_DS_UnUsedDeclaration::ObtainSubCs2ColorComponents()
{
	map<wxString, set<wxString>> l_mSubCs2Colors;
	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;



	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);



	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	map<wxString, wxString> l_mVar2CS = ObtainAllVar2Cs();
	set<wxString>           l_setConst = ObtainAllDefinedConstants();

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sType = l_pcColList->GetCell(i, 1).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();
		set<wxString> l_setColors;
		if (!l_sType.Contains(wxT("product")) && !l_sType.Contains(wxT("union")))
		{

			std::string str = l_sColors.ToStdString();
			typedef boost::tokenizer<boost::char_separator<char> >
				tokenizer;
			boost::char_separator<char> sep("-|+*()/&=<>,% ");
			tokenizer tokens(str, sep);
			for (tokenizer::iterator tok_iter = tokens.begin();
				tok_iter != tokens.end(); ++tok_iter)
			{
				wxString l_tkn = *tok_iter;
				bool l_bisNum = false;
				if (!l_tkn.empty() && std::all_of(l_tkn.begin(), l_tkn.end(), ::isdigit))
				{
					l_bisNum = true;
				}
				if (!l_bisNum)
				{
					auto findInvar = l_mVar2CS.find(*tok_iter);
					auto findinCons = l_setConst.find(*tok_iter);
					if (findinCons != l_setConst.end() | findInvar != l_mVar2CS.end())
					{
						l_setColors.insert(*tok_iter);
					}

				}

			}

		}

		l_mSubCs2Colors[l_sName] = l_setColors;
	}


	return l_mSubCs2Colors;
}

/*returns map of CS 2 Type*/
map<wxString, wxString> SP_DS_UnUsedDeclaration::ObtainAllCs2Type()
{
	map<wxString, wxString> l_mCs2Type;


	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;

	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);




	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		l_mCs2Type[l_sName] = wxT("Simple");
	}


	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);



	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sType = l_pcColList->GetCell(i, 1).c_str();
		if (!l_sType.Contains(wxT("product")) && !l_sType.Contains(wxT("union")))
		{
			l_mCs2Type[l_sName] = l_sType;
		}
		else {
			l_mCs2Type[l_sName] = wxT("Compound");
		}

	}


	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);


	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));


	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		l_mCs2Type[l_sName] = wxT("Alias");

	}
	return l_mCs2Type;
}
