/*///////////////////////////////// 
* $Source: $
*$Author: G.A $
* 
* @Description
*/
/////////////////////////////////// 
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

typedef std::vector<double>  VectorDouble;
typedef std::vector< VectorDouble >  Matrix2DDouble;
 
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
 
	unsigned long m_pCount;


public:
 
	virtual void CalculateAlphaLevels();

	virtual void DoSamplePointsCombination(double calpha);

	virtual SP_Vector2DDouble GetAlphaCutForAllLevels();
	 
	
	virtual TimedTFN CalculateTfnAt( long timePoint,  long dataColumn, const ResultFuzzyBand& fuzzyband);
	virtual void CalcAllTFN( long dataColumn,  ResultFuzzyBand& band);
	
	  
public:
	FuzzyReasoning(unsigned long alphaLevels, unsigned long nDiscPoint, SP_Vector2DDouble paramMat,unsigned long FN,unsigned long pcount);
	FuzzyReasoning(unsigned long levels,unsigned long samples);
	FuzzyReasoning( );

	virtual SP_Vector2DDouble GetCombinationMatrix();

	virtual std::vector<double>& GetAlphaSet();

	virtual unsigned long GetNumFuzzyLevels();
	
	virtual unsigned long GetNumSamplePoints();

	virtual unsigned long GetNumFuzzyNum();

	void FuzzyReasoning::GetMinMaxTraceThread(ResultFuzzyBand fuzzyBand, long dataColumn);

	virtual std::vector<SP_Vector2DDouble> GetMinMaxFuzzyTraceMatrix(ResultFuzzyBand& fuzzyBand);

	virtual SP_Compressed_Fuzzy_Band* CompressResults( ResultFuzzyBand  fuzzyBand);

};

#endif // SP_FUZZY_REASONONG_
