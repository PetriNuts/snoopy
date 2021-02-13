/*///////////////////////////////// 
*$Header file: $
*$Author: George Assaf $
*$Date: 2018$
*@Description: fuzzy manupilation e.g., alpha-cut, membership, fuzzy band, sampling algos computation
*/

#ifndef SP_FUZZY_REASONONG_
#define SP_FUZZY_REASONONG_
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "TriangularFN.h"
#include "FuzzyBasics.h"
#include "sp_core/SP_Vector.h"
#include "sp_gui/dialogs/dia_FPN/SP_TfnMembershipList.h"
#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"
#include <vector>
#include <thread>

//for lhs sampling
#include "lhslib/LHSCommonDefines.h"
#include "lhslib/utilityLHS.h"
#include<math.h>
using namespace::bclib;


typedef std::vector<double>  VectorDouble;
typedef std::vector< VectorDouble >  Matrix2DDouble;

namespace fuzzification {

enum SAMPLING_TYPE{
BASIC,
REDUCED,
RANDOM_LHS,
IMPROVED_LHS,
MAXIMINI_LHS,
OPTIMUM_LHS,
GENETIC_LHS,
INVALID
};

//property types of sampling algo options
enum GUI_PROPORTY_TYPE {
	GUI_TYPE_TXTBOX,
	GUI_TYPE_CHECKBOX,
	GUI_TYPE_COMBOBOX
};

struct SAMPLING_OPTION_PROPERTY {
	wxString m_sDisplayText;
	GUI_PROPORTY_TYPE m_tGuiType;
	wxString m_sValue;
	double m_dMin;
	double m_dMax;
};

struct LHS_SAMPLING_OPTIONS {
	bool preseve;
	long dup;
	int maxsweeps;
	double eps;
	int recordLength;
	bool verbos;
	int pop;
	int gen;
	double pmut;
	std::string criterium;

};

typedef std::vector< SAMPLING_OPTION_PROPERTY* >  SP_SamplingPropertyVector;

class FuzzyReasoning  
{
private:

protected:
	unsigned long             m_nAlphaLevels;               //number of alpha levels
	double                    m_alphaStepSize;
	unsigned long             m_nDiscPoints;               // number of Discritization points per level
	unsigned long             m_nFuzzyNum;                //number of fuzzy numbers involved
	SP_VectorDouble           m_valphaLevels;            //vector of the values of alpha levels
	SP_Vector2DDouble         m_paramMatrix;            //Matrix of the parameters to be combined
	SP_Vector2DDouble         m_compMatrix;            // compined parameter matrix
	SP_VectorDouble           m_lowLevelsampels;//
	SP_Vector2DDouble         m_alphaCutSetAllLevels;//
	SP_TfnMembershipList      m_membershipList;
	SP_Compressed_Fuzzy_Band* m_compressedBand;          // object to store the needed results min/max with membership funs
	SP_Vector2DDouble         m_nFinalMinResultFuzzyMatrix;  // Minimum values of places with time
	SP_Vector2DDouble         m_nFinalMaxResultFuzzyMatrix;  // Maximum values of places with time
	bool                      m_bIsAbortCalculation;
	unsigned long m_pCount;
	std::map<double, SP_Vector2DDouble> m_mAlphaLevel2AlphaCuts;
	SAMPLING_TYPE             m_tSample_type;
	SP_VectorLong             m_fuzzyParamPos;
	SP_SamplingPropertyVector*   m_vSamplingProperties;
	LHS_SAMPLING_OPTIONS      m_OptionStruct;


private:


	virtual SP_Vector2DDouble GetAlphaCutForAllLevels();
	 
	
	virtual TimedTFN CalculateTfnAt( long timePoint,  long dataColumn, const ResultFuzzyBand& fuzzyband);

	/*
			*@param p_dAlphaLevel :alpha level
			*Output: Combination matrix of LHS sampleing which has the follwing parametrs ordering
			*Col0...ColN-1 ColF0....ColFN-1
			*Column from 0 toColN are the crisp parameters
			*Column from ColF0 toColFN-1 are the fuzzy parameters(numbers)
	*/
	virtual bool DoLHSSampleMapping(const double& p_dAlphaLevel);


	virtual void CalcAllTFN( long dataColumn,  ResultFuzzyBand& band);
	
	/*
			*@param p_n: number of sampling points
			*@param p_k: number of fuzzy kineteic parameters
			*@param p_bPreserve:ordering flag
			*@param p_mResult:refrenve to result matrix
	*/

	virtual bool ComputeLHSMatrix(const int& p_n, const int& p_k, const bool& p_bPreserve, SP_Vector2DDouble& p_mResult);

	/*
			*@param p_n: number of sampling points
			*@param p_k: number of fuzzy kineteic parameters
			*@param dup:A factor that determines the number of candidate points used in the search. A multiple of the number of remaining points than can be added.
			*@param p_mResult:refrenve to result matrix(Integer)
	*/
	virtual bool ComputeImprovedLHSMatrix(const int& p_n, const int& p_k, const long& p_nDub, SP_Vector2DInt& p_mResult);

	/*
			*@param p_n: number of sampling points
			*@param p_k: number of fuzzy kineteic parameters
			*@param p_mResult: refrence to result matrix(Integer)
			*@param p_pop: number of populations
			*@param p_gen: number of generations
			*@param p_Mut: The probability with which a mutation occurs in a column of the progeny
			*@param p_scriterium: The optimality criterium of the algorithm. Default is S. Maximin is also supported
			*@param p_verbos: Print informational messages. Default is FALSE
	*/
	virtual bool ComputeGeneticLHSMatrix(const int& p_n, const int& p_k, SP_Vector2DDouble& p_mResult, const int& pop=100,const int& gen=4,const double&pMut=0.1,const std::string& p_scriterium="S",const bool& l_bVerbos=false);



	/*
			*@param p_n: number of sampling points
			*@param p_k: number of fuzzy kineteic parameters
			*@param p_mResult: refrence to result matrix(Integer)
			*@param p_dup: A factor that determines the number of candidate points used in the search. A multiple of the number of remaining points than can be added.
	*/
	virtual bool ComputeMaximiniLHSMatrix(const int& p_n, const int& p_k, SP_Vector2DInt& p_mResult, const int& dup = 1);

	/*
			*@param p_n: number of sampling points
			*@param p_k: number of fuzzy kineteic parameters
			*@param p_mResult:refrence to result matrix(Integer)
			*@param p_nMaxSweeps: The maximum number of times the CP algorithm is applied to all the columns.
			*@param p_eps:The minimum percent change in the minimum distance used in the iterative method
			*@param p_nObtimalityRecordLength
			*@param verbos:show verbos messages, default false
	*/
	virtual bool ComputeOptimumLHSMatrix(const int& p_n,const int& p_k, SP_Vector2DInt& p_mResult,const int& p_nMaxSeeps=2,const double& p_eps=0.1,const int& p_nObtimalityRecordLength=2,const bool& p_bVerbos=false);

	/*fill out the sampling options of LHS algos*/
	virtual void FillOutSamplingOptionsVector();

	virtual void GetLHSOptions();

	virtual double GetDoubleProperty(const wxString& p_sPropert);

	virtual int  GetIntProperty(const wxString& p_sPropert);

	virtual wxString GetStringProperty(const wxString& p_sPropert);

	virtual bool GetBoolProperty(const wxString& p_sPropert);


public:
	FuzzyReasoning(unsigned long alphaLevels, unsigned long nDiscPoint, SP_Vector2DDouble paramMat,unsigned long FN,unsigned long pcount,SAMPLING_TYPE p_Type=SAMPLING_TYPE::BASIC);
	FuzzyReasoning(unsigned long levels,unsigned long samples);
	FuzzyReasoning( );

	virtual void DoSamplePointsCombination(double calpha);

	/*
	* Clear current LHS algorihm sampling options
	*/
	virtual void ClearSamplingOptions()
	{
		if(m_vSamplingProperties!=nullptr)
			m_vSamplingProperties->clear();
	}

	virtual SP_SamplingPropertyVector* GetSamplingOptions() { return m_vSamplingProperties; }

	virtual SAMPLING_TYPE GetSamplingType() { return m_tSample_type; }

	void SetSamplingMethod(SAMPLING_TYPE p_type)
	{
			ClearSamplingOptions();
			m_tSample_type = p_type;
			FillOutSamplingOptionsVector();
	}

	virtual void CalculateAlphaLevels();

	void InitialiseCombinationMatrix()
	{
		long LL = pow(m_nDiscPoints, m_nFuzzyNum);
		m_compMatrix.resize(LL, std::vector<double>(m_paramMatrix.size(), 0));
		m_alphaStepSize = (double)1 / m_nAlphaLevels;
	}

	void SetPlaceNum(const long& p_npCount)
	{
		m_pCount = p_npCount;
	}

	void SetAlphaLevels(const long& p_nAlphaLevels)
	{
		m_nAlphaLevels = p_nAlphaLevels;

	}

	void SetDiscretisingPoints(const long& p_nDiscPoints)
	{
		m_nDiscPoints = p_nDiscPoints;
	}

	void SetParamMatrix(SP_Vector2DDouble p_mParamMatrix)
	{
		m_paramMatrix = p_mParamMatrix;
	}

	void SetNumberFuzzyParams(const long& p_nNum)
	{
		m_nFuzzyNum = p_nNum;
	}

	long GetNumberOfRuns()
	{
		if (m_tSample_type == fuzzification::SAMPLING_TYPE::BASIC)
		{
				return  pow(m_nDiscPoints, m_nFuzzyNum) * (m_nAlphaLevels)+1;
		}
		else if (m_tSample_type == fuzzification::SAMPLING_TYPE::REDUCED)
		{
			return pow(m_nDiscPoints, m_nFuzzyNum) + (m_nAlphaLevels - 1) * 2 + 1;

		}
		else
		{//one of LHS algos
		return (m_nDiscPoints * m_nAlphaLevels) + 1;
		}
	}

	virtual SP_Vector2DDouble GetCombinationMatrix();

	virtual std::vector<double>& GetAlphaSet();

	virtual unsigned long GetNumFuzzyLevels();
	
	virtual unsigned long GetNumSamplePoints();

	virtual unsigned long GetNumFuzzyNum();

	void GetMinMaxTraceThread(ResultFuzzyBand fuzzyBand, long dataColumn);

	void GetMinMaxTraceThreadV2(ResultFuzzyBand fuzzyBand, long dataColumn);

	virtual std::vector<SP_Vector2DDouble> GetMinMaxFuzzyTraceMatrix(ResultFuzzyBand& fuzzyBand);

	virtual SP_Compressed_Fuzzy_Band* CompressResults( ResultFuzzyBand  fuzzyBand);

	virtual void SetAbort(bool bAbort=true);

	virtual bool GetWorkingStatus() { return m_bIsAbortCalculation; }

	virtual void ClearData()
	{
		m_membershipList.ClearData();

	    if (m_compressedBand)
		{
			m_compressedBand->ClearData();
		}

	}
};
}//end namespace

#endif // SP_FUZZY_REASONONG_
