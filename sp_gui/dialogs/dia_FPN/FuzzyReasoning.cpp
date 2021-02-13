/*/////////////////////////////////
*$implementation file: $
*$Author: George Assaf $
*$Date: 2018$
*@Description: fuzzy manupilation e.g., alpha-cut, membership, fuzzy band, sampling algos computation
*/

#include "FuzzyReasoning.h"
#include <wx/wx.h>
#include <wx/busyinfo.h>
#include "SP_DS_Fuzzy_MinMaxThread.h"

fuzzification::FuzzyReasoning::FuzzyReasoning(unsigned long alphaLevels, unsigned long nDiscPoints, std::vector<vector<double>> paramMat, unsigned long FN,unsigned long pcount, SAMPLING_TYPE p_Type)
{
	m_nAlphaLevels = alphaLevels;
	m_nDiscPoints = nDiscPoints;
	m_paramMatrix = paramMat;
	m_bIsAbortCalculation = false;
	m_tSample_type = p_Type;
	//m_compMatrix = compMat;
	m_compressedBand =new SP_Compressed_Fuzzy_Band( );
	m_alphaStepSize =(double) 1 / m_nAlphaLevels;
	double answer = static_cast<float>(static_cast<int>(m_alphaStepSize * 10.)) / 10.;
	m_nFuzzyNum = FN;
	m_pCount = pcount;
	long LL = pow(m_nDiscPoints, m_nFuzzyNum);
	 m_compMatrix.resize  (LL, std::vector<double>(m_paramMatrix.size(), 0));

	m_vSamplingProperties = new SP_SamplingPropertyVector();
	FillOutSamplingOptionsVector();
}
 
fuzzification::FuzzyReasoning::FuzzyReasoning(unsigned long l, unsigned long s) {
	m_nAlphaLevels = l;
	m_nDiscPoints = s;
}
void   fuzzification::FuzzyReasoning::SetAbort(bool bAbort)
{
	m_bIsAbortCalculation = bAbort;
}
fuzzification::FuzzyReasoning::FuzzyReasoning() {

}

SP_Vector2DDouble fuzzification::FuzzyReasoning::GetCombinationMatrix()
{
	return m_compMatrix;
}

std::vector<double>& fuzzification::FuzzyReasoning::GetAlphaSet()
{

	return m_valphaLevels;
}

 


unsigned long fuzzification::FuzzyReasoning::GetNumFuzzyLevels()
{

	return m_nAlphaLevels;
}

unsigned long fuzzification::FuzzyReasoning::GetNumSamplePoints()
{

	return m_nDiscPoints;
}

unsigned long fuzzification::FuzzyReasoning::GetNumFuzzyNum()
{
	return m_nFuzzyNum;
}
/*Calculate vector of alpha levels from 0 to 1*/
void fuzzification::FuzzyReasoning::CalculateAlphaLevels()
{
	
	m_valphaLevels.clear();
	double alpha = 0.0;
	m_valphaLevels.push_back(alpha);
	int nlevels = 0;
	
	while (alpha < 1 && nlevels != m_nAlphaLevels)
	{
		alpha += m_alphaStepSize;
		m_valphaLevels.push_back(alpha);
		nlevels = nlevels + 1;
	}
	m_valphaLevels[nlevels ] = 1;
}

SP_Vector2DDouble fuzzification::FuzzyReasoning::GetAlphaCutForAllLevels()
{
	
	 
	for (int i = 0; i < m_paramMatrix.size(); i++)
	{
		double left = m_paramMatrix[i][0];
		double center = m_paramMatrix[i][1];
		double right = m_paramMatrix[i][2];
		if (!(center - left) < pow(10, -10))
		{
			TriangularFN t(left, right, center);
			for (int j = 0; j < m_valphaLevels.size()-1; j++)
			{
			
			AlphaCutSet set1 = t.getAlphaCutSet(m_valphaLevels[j], m_nDiscPoints);

			m_alphaCutSetAllLevels.push_back(set1);

			m_mAlphaLevel2AlphaCuts[m_valphaLevels[j]].push_back(set1);
		    }
			m_fuzzyParamPos.push_back(i);
		}
		 
	}
	return m_alphaCutSetAllLevels;

}


bool fuzzification::FuzzyReasoning:: DoLHSSampleMapping(const double& p_dAlphaLevel)
{
	SP_Vector2DDouble l_mLHSMatrix;
	SP_Vector2DInt l_mLHSIntMatrix;
	//initialise combination matrix
	l_mLHSMatrix.resize(m_nDiscPoints , std::vector<double>(m_paramMatrix.size(), 0));//n
	l_mLHSIntMatrix.resize(m_nDiscPoints, std::vector<int>(m_paramMatrix.size(), 0));//n

	//global LHS options of all LHS variants
	int  dup, maxsweeps, recordLength, pop, gen;
	std::string l_sCriterium;
	double eps, pmut;
	bool verbos, preserve= false;
	std::string criterium;

	dup = m_OptionStruct.dup;
	maxsweeps = m_OptionStruct.maxsweeps;
	recordLength = m_OptionStruct.recordLength;
	pop = m_OptionStruct.pop;
	gen = m_OptionStruct.gen;
	eps = m_OptionStruct.eps;
	pmut = m_OptionStruct.pmut;
	l_sCriterium = m_OptionStruct.criterium;
	verbos = m_OptionStruct.verbos;
	preserve = m_OptionStruct.preseve;

	if (l_sCriterium.empty())
	{
		l_sCriterium = "S";
	}

	//compute LHS sample matrix
	if (m_tSample_type ==fuzzification::SAMPLING_TYPE::RANDOM_LHS)
	{

		ComputeLHSMatrix(m_nDiscPoints, m_nFuzzyNum, preserve, l_mLHSMatrix);//n,k

	}
	else if (m_tSample_type ==fuzzification:: SAMPLING_TYPE::IMPROVED_LHS )
	{
		ComputeImprovedLHSMatrix(m_nDiscPoints, m_nFuzzyNum, (long)dup, l_mLHSIntMatrix);

		for (unsigned i = 0; i < l_mLHSIntMatrix.size(); i++)
		{
			for (unsigned j = 0; j < l_mLHSIntMatrix[0].size(); j++)
			{
				l_mLHSMatrix[i][j] = l_mLHSIntMatrix[i][j];
			}
		}
	}
	else if (m_tSample_type == fuzzification::SAMPLING_TYPE::OPTIMUM_LHS)
	{
		bool l_bIsSucced=ComputeOptimumLHSMatrix(m_nDiscPoints, m_nFuzzyNum, l_mLHSIntMatrix,maxsweeps,eps,recordLength,verbos);

		if (!l_bIsSucced) return false;

		for (unsigned i = 0; i < l_mLHSIntMatrix.size(); i++)
		{
			for (unsigned j = 0; j < l_mLHSIntMatrix[0].size(); j++)
			{
				l_mLHSMatrix[i][j] = l_mLHSIntMatrix[i][j];
			}
		}
	}
	else if (m_tSample_type == fuzzification::SAMPLING_TYPE::GENETIC_LHS)
	{
		ComputeGeneticLHSMatrix(m_nDiscPoints, m_nFuzzyNum, l_mLHSMatrix,pop,gen,pmut,criterium,verbos);

		for (unsigned i = 0; i < l_mLHSIntMatrix.size(); i++)
		{
			for (unsigned j = 0; j < l_mLHSIntMatrix[0].size(); j++)
			{
				l_mLHSMatrix[i][j] = l_mLHSIntMatrix[i][j];
			}
		}
	}
	else if (m_tSample_type == fuzzification::SAMPLING_TYPE::MAXIMINI_LHS)
	{
		ComputeMaximiniLHSMatrix(m_nDiscPoints, m_nFuzzyNum, l_mLHSIntMatrix,dup);

		for (unsigned i = 0; i < l_mLHSIntMatrix.size(); i++)
		{
			for (unsigned j = 0; j < l_mLHSIntMatrix[0].size(); j++)
			{
				l_mLHSMatrix[i][j] = l_mLHSIntMatrix[i][j];
			}
		}
	}

	if (m_alphaCutSetAllLevels.size() == 0)
	{
		GetAlphaCutForAllLevels();
	}

	//get crisp parameters
	SP_VectorDouble l_vCrisp;
	SP_VectorLong   l_vParamPos;
	for (int i = 0; i < m_paramMatrix.size(); i++)
	{
		double left = m_paramMatrix[i][0];
		double center = m_paramMatrix[i][1];
		double right = m_paramMatrix[i][2];
		if ((center - left) < pow(10, -10))
		{
			l_vCrisp.push_back(left);
			l_vParamPos.push_back(i);
		}
	}
	//add crisp parameter first to the combination matrix
	for (long i = 0; i < l_mLHSMatrix.size(); i++)
	{
			for (long j = 0; j < l_vCrisp.size(); j++)
			{

					l_mLHSMatrix[i][l_vParamPos[j]] = l_vCrisp[j];//k

			}

	}

	//look for the input alpha level, and assign it to the combination matrix
	auto itFind = m_mAlphaLevel2AlphaCuts.find(p_dAlphaLevel);

	if ((m_paramMatrix.size()- l_vParamPos.size()) != m_fuzzyParamPos.size()) return false;//should not occur

	if (itFind != m_mAlphaLevel2AlphaCuts.end())
	{
		//mapping  alpha-cut sets to LHS samples
		for (long i = 0; i < itFind->second.size(); i++)
		{
			int l_nIndex = m_fuzzyParamPos[i];
			for (long j = 0; j < itFind->second[i].size(); j++)

			{

					l_mLHSMatrix[j][l_nIndex] = itFind->second[i][j];

			}

		}
	}
	else
	{
		wxString l_sInputLevel;
		l_sInputLevel << p_dAlphaLevel;
		wxString l_sError = wxT("The Alpha level ") + l_sInputLevel + wxT(" is out of range!");
		SP_LOGERROR_(l_sError);
		return false;
	}

	m_compMatrix = l_mLHSMatrix;

	return true;
}

bool  fuzzification::FuzzyReasoning ::ComputeLHSMatrix(const int& p_n, const int& p_k, const bool& p_bPreserve, SP_Vector2DDouble& p_mResult)
{
	if (p_n < p_k || p_n <= 0 || p_k<0) return false;

	bclib::matrix<double> result = bclib::matrix<double>(p_n, p_k);

	bclib::CRandomStandardUniform oRandom = bclib::CRandomStandardUniform();

	lhslib::randomLHS(p_n, p_k, p_bPreserve, result, oRandom);


	for (long i = 0; i <p_n ; i++)
	{
		SP_VectorDouble l_vOneRow= result.getrow(i);

		for (unsigned long j = 0; j < l_vOneRow.size(); j++)
		{
			p_mResult[i][j] = l_vOneRow[j];
		}

	 }


	return true;
}


bool  fuzzification::FuzzyReasoning::ComputeImprovedLHSMatrix(const int& p_n, const int& p_k, const long& p_nDub, SP_Vector2DInt& p_mResult)
{
	 if (p_n < p_k || p_n <= 0 || p_k<0) return false;

	 bclib::matrix<int> result = bclib::matrix<int>(p_n, p_k);

	 bclib::CRandomStandardUniform oRandom = bclib::CRandomStandardUniform();

	// oRandom.setSeed(p_iSeed1, p_iSeed2);


	 lhslib::improvedLHS(p_n, p_k, 1, result, oRandom);


	 for (long i = 0; i <p_n; i++)
	 {
		  std::vector<int> l_vOneRow = result.getrow(i);

		 for (unsigned long j = 0; j < l_vOneRow.size(); j++)
		 {
			 p_mResult[i][j] = l_vOneRow[j];
		 }

	 }


	 return true;
}

bool  fuzzification::FuzzyReasoning::ComputeGeneticLHSMatrix(const int& p_n, const int& p_k, SP_Vector2DDouble& p_mResult ,const int& pop, const int& gen, const double& pMut , const std::string& p_scriterium , const bool& l_bVerbos )
{
	 if (p_n < p_k || p_n <= 0 || p_k<0) return false;

	 bclib::matrix<double> result = bclib::matrix<double>(p_n, p_k);

	 bclib::CRandomStandardUniform oRandom = bclib::CRandomStandardUniform();


	 lhslib::geneticLHS(p_n, p_k, pop,gen, pMut, p_scriterium, false, result, oRandom);

	 for (long i = 0; i <p_n; i++)
	 {
		 SP_VectorDouble l_vOneRow = result.getrow(i);

		 for (unsigned long j = 0; j < l_vOneRow.size(); j++)
		 {
			 p_mResult[i][j] = l_vOneRow[j];
		 }

	 }


	 return true;
}

bool fuzzification::FuzzyReasoning::ComputeMaximiniLHSMatrix(const int& p_n, const int& p_k, SP_Vector2DInt& p_mResult, const int& dup )
 {
	  if (p_n < p_k || p_n <= 0 || p_k<0) return false;

	  bclib::matrix<int> result = bclib::matrix<int>(p_n, p_k);

	  bclib::CRandomStandardUniform oRandom = bclib::CRandomStandardUniform();


	  lhslib::maximinLHS(p_n, p_k, dup, result, oRandom);

	  for (int i = 0; i <p_n; i++)
	  {
		  std::vector<int> l_vOneRow = result.getrow(i);

		  for (int j = 0; j < l_vOneRow.size(); j++)
		  {
			  p_mResult[i][j] = l_vOneRow[j];
		  }

	  }

	  return true;
 }

bool  fuzzification::FuzzyReasoning::ComputeOptimumLHSMatrix(const int& p_n, const int& p_k, SP_Vector2DInt& p_mResult, const int& p_nMaxSeeps , const double& p_eps , const int& p_nObtimalityRecordLength , const bool& p_bVerbos )
 {

	if (p_n < p_k || p_n <= 0 || p_k<0) return false;

	bclib::matrix<int> result = bclib::matrix<int>(p_n, p_k);

	bclib::CRandomStandardUniform oRandom = bclib::CRandomStandardUniform();


	try {
		//seems to be a bug in the lhs lib, TODO check what is goining on
		lhslib::optimumLHS(p_n, p_k, p_nMaxSeeps, p_eps, result, p_nObtimalityRecordLength, oRandom, true);



	for (int i = 0; i <p_n; i++)
	{
		std::vector<int> l_vOneRow = result.getrow(i);

		for (int j = 0; j < l_vOneRow.size(); j++)
		{
			p_mResult[i][j] = l_vOneRow[j];
		}

	}
	}
	catch (std::exception& ex) {
		SP_MESSAGEBOX(ex.what());
		return false;
	}
	return true;
 }

/*Do ampling for the current(inputed) alpha level of all TFNs */
void fuzzification::FuzzyReasoning::DoSamplePointsCombination(double calpha)
{

	if (m_tSample_type == SAMPLING_TYPE::BASIC)
		{
			for (int y = 0; y < m_compMatrix.size(); y++)
			{
				for (int x = 0; x < m_compMatrix[0].size(); x++)
				{
					m_compMatrix[y][x] = 0.0;
				}
			}

			long ll = pow(m_nDiscPoints, m_nFuzzyNum);
			int count = 1;
			for (int i = 0; i < m_paramMatrix.size(); i++)
			{
				double left = m_paramMatrix[i][0];
				double center = m_paramMatrix[i][1];
				double right = m_paramMatrix[i][2];
				if ((center - left) < pow(10, -10))// crisp value
					for (int j = 0; j < pow(m_nDiscPoints, m_nFuzzyNum); j++)
						m_compMatrix[j][i] = left;
				else
				{
					/* the parameter is fuzzy number*/
					TriangularFN t(left, right, center);
					/*calculate the alpha cut set (m_nDiscPoints sample-size) for the input alpha level*/
					AlphaCutSet set = t.getAlphaCutSet(calpha, m_nDiscPoints);
					int k = 0;
					while (k < pow(m_nDiscPoints, m_nFuzzyNum))
					{// do combinations
						for (int m = 0; m < set.size(); m++)
						{
							long g = pow(m_nDiscPoints, count - 1);
							for (int nn = 0; nn < g; nn++)
							{

								m_compMatrix[k][i] = set[m];
								k++;


							}
						}
					}
					count = count + 1;
				}

			}
		}
		else if (m_tSample_type == SAMPLING_TYPE::RANDOM_LHS || m_tSample_type == SAMPLING_TYPE::IMPROVED_LHS
			|| SAMPLING_TYPE::OPTIMUM_LHS || SAMPLING_TYPE::MAXIMINI_LHS|| SAMPLING_TYPE::GENETIC_LHS)
		{
			if (calpha == 0.0)
			{
				GetLHSOptions();
			}


			DoLHSSampleMapping(calpha);
		}
		else
		{
			//TODO
		}
	/*
	for (int y = 0; y<m_compMatrix.size(); y++)
	{
		for (int x = 0; x < m_compMatrix[0].size(); x++)
		{
			m_compMatrix[y][x] = 0.0;
		}
	}

	long ll = pow(m_nDiscPoints,m_nFuzzyNum );
 	int count = 1;
	for (int i = 0; i < m_paramMatrix.size(); i++)
	{
		double left = m_paramMatrix[i][0];
		double center = m_paramMatrix[i][1];
		double right = m_paramMatrix[i][2];
		if ((center - left) < pow(10, -10))// crisp value
			for (int j = 0; j < pow(m_nDiscPoints, m_nFuzzyNum); j++)
				m_compMatrix[j][i] = left;
		else
		{
			// the parameter is fuzzy number
			TriangularFN t(left, right, center);
			//calculate the alpha cut set (m_nDiscPoints sample-size) for the input alpha level
			AlphaCutSet set = t.getAlphaCutSet(calpha, m_nDiscPoints);
			int k = 0;
			while (k < pow(m_nDiscPoints, m_nFuzzyNum))
			{// do combinations
				for (int m = 0; m < set.size(); m++)
				{
					long g = pow(m_nDiscPoints, count - 1);
					for (int nn = 0; nn < g; nn++)
					{
						
						m_compMatrix[k][i] = set[m];
						k++;
						
						
					}
				}
			}
			count = count + 1;
		}

	}
	*/
}


void fuzzification::FuzzyReasoning::GetMinMaxTraceThreadV2(ResultFuzzyBand fuzzyBand, long dataColumnNum)
{
	long ltime = fuzzyBand[0].fuzzyTrace.size();
	m_nFinalMinResultFuzzyMatrix.resize(dataColumnNum, std::vector<double>(ltime));
	m_nFinalMaxResultFuzzyMatrix.resize(dataColumnNum, std::vector<double>(ltime));
	vector<SP_DS_Fuzzy_MinMaxThread*> threadVector;
	long lcounter = 0;
	for (int i = 0; i < dataColumnNum; i++)
	{
		if (m_bIsAbortCalculation)
		{
			break;
		}
		threadVector.push_back(new SP_DS_Fuzzy_MinMaxThread(m_nFinalMaxResultFuzzyMatrix, m_nFinalMinResultFuzzyMatrix, i, fuzzyBand, 0, ltime));
		if(threadVector[lcounter]->Create() != wxTHREAD_NO_ERROR)
		{
			
		}
		threadVector[lcounter]->SetPriority(WXTHREAD_MAX_PRIORITY);
		threadVector[lcounter]->Run();
		threadVector[lcounter]->Wait();
		wxYieldIfNeeded();
		wxDELETE(threadVector[lcounter]);
		lcounter++;
	}
	 

}

/*Calculate minimum and maximum traces over time*/
void fuzzification::FuzzyReasoning::GetMinMaxTraceThread(ResultFuzzyBand fuzzyBand, long dataColumnNum)
{
	/*divide the time into two slots, and run 2*spieces threads to speed up the calculations*/
	long ltime = fuzzyBand[0].fuzzyTrace.size();
	long lnumThreads = ltime / 2;
	vector<std::thread> threadVector(dataColumnNum*2);
	vector<std::thread> minthreads(dataColumnNum * 2);
	m_nFinalMaxResultFuzzyMatrix.clear();
	m_nFinalMinResultFuzzyMatrix.clear();
	std::vector<double> finalMinVector;
	std::vector<double> finalMaxVector;
	long lDataCol = 0; long li = 0; 

	m_nFinalMinResultFuzzyMatrix.resize(dataColumnNum, std::vector<double>(ltime));
	m_nFinalMaxResultFuzzyMatrix.resize(dataColumnNum, std::vector<double>(ltime));
	 
	while (lDataCol < dataColumnNum)
	{
		
		minthreads[li]=std::thread([&](SP_Vector2DDouble& m_nFinalMaxResultFuzzyMatrix, SP_Vector2DDouble& m_nFinalMinResultFuzzyMatrix,ResultFuzzyBand& fuzzyBand, long dataColumn, long start, long end) {
		        long lastTime = fuzzyBand[0].fuzzyTrace.size();
				for (long pos = start; pos < end; pos++)
				{
					if(m_bIsAbortCalculation)
					{
						break;
					}
					double minVal = fuzzyBand[0].fuzzyTrace[pos][dataColumn];
					double maxVal = fuzzyBand[0].fuzzyTrace[pos][dataColumn];
					for (unsigned long tracePos = 0; tracePos < fuzzyBand.size(); tracePos++)
					{
						if (m_bIsAbortCalculation)
						{
							break;
						}
						SP_Vector2DDouble elementTrace = fuzzyBand[tracePos].fuzzyTrace;
						double dataValue = elementTrace[pos][dataColumn];
						if (dataValue < minVal)
						{
							minVal = dataValue;

						}
						(dataValue > maxVal) ? maxVal = dataValue : maxVal = maxVal;

					}
					m_nFinalMinResultFuzzyMatrix[dataColumn][pos] = minVal;
					m_nFinalMaxResultFuzzyMatrix[dataColumn][pos] = maxVal;
				}
		},std::ref(m_nFinalMaxResultFuzzyMatrix),std::ref(m_nFinalMinResultFuzzyMatrix),std::ref(fuzzyBand), lDataCol, 0, lnumThreads);

		li = li + 1;

 

		minthreads[li] = std::thread([&](SP_Vector2DDouble& m_nFinalMaxResultFuzzyMatrix,SP_Vector2DDouble& m_nFinalMinResultFuzzyMatrix, ResultFuzzyBand& fuzzyBand, long dataColumn, long start, long end) {

			long lastTime = fuzzyBand[0].fuzzyTrace.size();
			for (long pos = start; pos < end; pos++)
			{
				if (m_bIsAbortCalculation)
				{
					break;
				}
				double minVal = fuzzyBand[0].fuzzyTrace[pos][dataColumn];
				double maxVal = fuzzyBand[0].fuzzyTrace[pos][dataColumn];
				for (unsigned long tracePos = 0; tracePos < fuzzyBand.size(); tracePos++)
				{
					if (m_bIsAbortCalculation)
					{
						break;
					}
					SP_Vector2DDouble elementTrace = fuzzyBand[tracePos].fuzzyTrace;
					double dataValue = elementTrace[pos][dataColumn];
					if (dataValue < minVal)
					{
						minVal = dataValue;

					}
				 	(dataValue > maxVal) ? maxVal = dataValue : maxVal = maxVal;

				}
				m_nFinalMinResultFuzzyMatrix[dataColumn][pos] = minVal;
				m_nFinalMaxResultFuzzyMatrix[dataColumn][pos] = maxVal;
			}

		},std::ref(m_nFinalMaxResultFuzzyMatrix), std::ref(m_nFinalMinResultFuzzyMatrix), std::ref(fuzzyBand), lDataCol, lnumThreads, ltime);

		lDataCol = lDataCol + 1;
			li = li + 1;
	}

	
	for (std::vector<std::thread>::iterator it = minthreads.begin(); it != minthreads.end(); ++it)
	{
		if ((*it).joinable())
		{
			(*it).join();
		}

	}
	 

}


 std::vector<SP_Vector2DDouble> fuzzification::FuzzyReasoning::GetMinMaxFuzzyTraceMatrix(ResultFuzzyBand& fuzzyBand)
 {
	 
	 SP_Vector2DDouble m_nResultMinMatrix;
	 SP_Vector2DDouble m_nResultMaxMatrix;
	 std::vector<SP_Vector2DDouble> m_nMinMaxResultMats;
	 long l_placeCount;
	 long l_lastTimePoint;
	 SP_Vector2DDouble mytrace;
	 mytrace = fuzzyBand[0].fuzzyTrace;
	 l_placeCount = mytrace[0].size();
	 l_lastTimePoint = fuzzyBand[0].fuzzyTrace.size();
	
	 //GetMinMaxTraceThread(fuzzyBand, l_placeCount);
	 GetMinMaxTraceThreadV2(fuzzyBand, l_placeCount);

	 long ss = m_nFinalMinResultFuzzyMatrix.size();
	 //transform the matrix so that each row represents time point with the corrosponding values of places
	 for (long pos = 0; pos < l_lastTimePoint; pos++)
	 {
		 SP_VectorDouble row;//min matrix
		 SP_VectorDouble maxRow;
		 for (int col = 0; col < m_nFinalMinResultFuzzyMatrix.size(); col++)// minResultFuzzyMatrix.size()
		 {
			 double dataVal = m_nFinalMinResultFuzzyMatrix[col][pos];// minResultFuzzyMatrix[col][pos]
			 double dataValmax = m_nFinalMaxResultFuzzyMatrix[col][pos];
			 row.push_back(dataVal);
			 maxRow.push_back(dataValmax);
			 
		 }
		 m_nResultMinMatrix.push_back(row);
		 m_nResultMaxMatrix.push_back(maxRow);
	 }

	 m_nMinMaxResultMats.push_back(m_nResultMinMatrix);
	 m_nMinMaxResultMats.push_back(m_nResultMaxMatrix);

	 return m_nMinMaxResultMats;

 }

 /*Calculate the Triangular membership function over time for specific data column and time point*/
 TimedTFN fuzzification::FuzzyReasoning::CalculateTfnAt( long timePoint,  long dataColumn,const ResultFuzzyBand& fuzzyband)
 {
	 std::map<double, int> m_levels2PointsMap;/*store each level with its corrosponding trace count */
	 long lnumTraces = fuzzyband.size();
	 SP_Vector2DDouble   m_nMinMaxResult;
	 TimedTFN            timedMembershipFunc;

	 /*calculating the number of traces per level*/
 	 for (int i = 0; i < m_valphaLevels.size(); i++)//m_nAlphaLevels
	 {
		 int point_count = 0;
		 for (int t = 0; t < lnumTraces; t++)
		 {
			 if (fuzzyband[t].currentLevel == m_valphaLevels[i])
			 {
				 point_count++;
			 }
		 }
		 m_levels2PointsMap[m_valphaLevels[i]] = point_count;
	 }

	 /* store each sample result in its according level*/
	 SP_Vector2DDouble m_nFormedMatrix;
	 SP_Vector2DDouble m_oneTrace;
	 SP_VectorDouble   m_nValuePerlevel;/*used to store result values of one level*/
	 vector<double> levels; levels.clear();
	 int i_Level = 0; /*refers to number of samples per level*/
	 int j = 0;
	 for (long i = 0; i < lnumTraces; i++)
	 {
		 if (m_levels2PointsMap[m_valphaLevels[j]] != 0)
		 {
			 if (i_Level == 0)
			 {
				 levels.push_back(m_valphaLevels[j]);
			 }

			 TraceElement traceElement = fuzzyband[i];
			 m_oneTrace = traceElement.fuzzyTrace;

			 m_nValuePerlevel.push_back(m_oneTrace[timePoint][dataColumn]);

			 i_Level++;
			 if (i_Level == m_levels2PointsMap[levels[j]])
			 {/*complete one level, so store it in the final matrix*/
				 i_Level = 0;
				 j++;
				 m_nFormedMatrix.push_back(m_nValuePerlevel);
				 m_nValuePerlevel.clear();
			 }
			
		 }
		 else { j++; }
	 }
	 /*sort and store  the final results, min and max of each level*/
	 if (m_nFormedMatrix.size() > 0)
	 {
		 for (int i = 0; i < m_nFormedMatrix.size(); i++)
		 {

			 std::vector<double> smv = m_nFormedMatrix[i];
			 std::sort(smv.begin(), smv.end());
			 double low = smv[0];
			 double up = smv[smv.size() - 1];
			 SP_VectorDouble minMaxvector;
			 minMaxvector.push_back(low);
			 minMaxvector.push_back(up);
			 m_nMinMaxResult.push_back(minMaxvector);

		 }
		 
	 }

	 timedMembershipFunc.l_timePoint = timePoint;
	 timedMembershipFunc.m_nTFNMemberShip = m_nMinMaxResult;


	 return timedMembershipFunc;


 }
 /*Calculate all TFNs for all data columns(places)  for all time points*/
void fuzzification::FuzzyReasoning::CalcAllTFN( long dataColumn,  ResultFuzzyBand& band)
 {
	 TimedTFN         m_timeTfn;
	 unsigned long l_lastTime = band[0].fuzzyTrace.size();

	 for (long pos = 0; pos < l_lastTime; pos++)
	 {
		 if (m_bIsAbortCalculation)
		 {
			 break;
		 }
		 m_timeTfn = CalculateTfnAt(pos, dataColumn, band);
		 m_membershipList.StoreTFNMembership(dataColumn, pos, m_timeTfn.m_nTFNMemberShip);
		 
	 }
	 
	 
 }
/*Calculate the min max fuzzy band with TFNs over time*/
SP_Compressed_Fuzzy_Band* fuzzification::FuzzyReasoning::CompressResults(  ResultFuzzyBand  fuzzyBand)
 {
	
	std::vector<SP_Vector2DDouble> m_minMax;

	 std::vector<vector<double>> minTrace;
	 std::vector<vector<double>> maxTrace;

	 m_minMax = GetMinMaxFuzzyTraceMatrix(fuzzyBand);
	
	 minTrace = m_minMax[0];
	 maxTrace =  m_minMax[1];
	  long lnumPlaces = fuzzyBand[0].fuzzyTrace[0].size();
	  long lDataColumn;
	  unsigned long l_lastTime = fuzzyBand[0].fuzzyTrace.size();
	  for (long dataCol = 0; dataCol < lnumPlaces; dataCol++)
	  {
		 if (m_bIsAbortCalculation)
		  {
			  break;
		  }
		  CalcAllTFN(dataCol, fuzzyBand);
	  }
	  Membership_List tfnList = m_membershipList.GetMembershipList();
	  vector<double> levls = GetAlphaSet();
	  SP_Compressed_Fuzzy_Band* band;
	  if (!m_bIsAbortCalculation)
	  {
		   band = new SP_Compressed_Fuzzy_Band(tfnList, minTrace, maxTrace, levls);
	  }
	  else {
		  minTrace.clear();
		  maxTrace.clear();
		  levls.clear();
		  tfnList.clear();
		   band = new SP_Compressed_Fuzzy_Band(tfnList, minTrace, maxTrace, levls);
	  }

	  return band;
 }


void  fuzzification::FuzzyReasoning::FillOutSamplingOptionsVector()
{

	switch (m_tSample_type)
	{
	case  fuzzification::SAMPLING_TYPE::BASIC:  return;
	case  fuzzification::SAMPLING_TYPE::REDUCED: return;
	case  fuzzification::SAMPLING_TYPE::RANDOM_LHS:
	{
		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyPreview = new  fuzzification::SAMPLING_OPTION_PROPERTY();
     	l_propertyPreview->m_sDisplayText = wxT("preserve Draw");
		l_propertyPreview->m_sValue = wxT("0");
		l_propertyPreview->m_tGuiType =  fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_CHECKBOX;
	     m_vSamplingProperties->push_back(l_propertyPreview);

		break;
	}
	case  fuzzification::SAMPLING_TYPE::IMPROVED_LHS:
	{
		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyDup1 = new  fuzzification::SAMPLING_OPTION_PROPERTY();

	l_propertyDup1->m_sDisplayText = wxT("dup");
	l_propertyDup1->m_sValue = wxT("1");
	l_propertyDup1->m_dMin = 1;
	l_propertyDup1->m_dMax = 100;
	l_propertyDup1->m_tGuiType = fuzzification:: GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
	m_vSamplingProperties->push_back(l_propertyDup1);
	break;
	}
	case fuzzification::SAMPLING_TYPE::OPTIMUM_LHS:
	{
		fuzzification:: SAMPLING_OPTION_PROPERTY* l_propertyEps = new  fuzzification::SAMPLING_OPTION_PROPERTY();

		l_propertyEps->m_sDisplayText = wxT("eps");
		l_propertyEps->m_sValue = wxT("0.01");
		l_propertyEps->m_dMin = 0.01;
		l_propertyEps->m_dMax = 100;
		l_propertyEps->m_tGuiType = fuzzification:: GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertyEps);

		fuzzification:: SAMPLING_OPTION_PROPERTY* l_propertyMaxSweeps = new fuzzification:: SAMPLING_OPTION_PROPERTY();

		l_propertyMaxSweeps->m_sDisplayText = wxT("MaxSweeps");
		l_propertyMaxSweeps->m_sValue = wxT("2");
		l_propertyMaxSweeps->m_dMin = 1;
		l_propertyMaxSweeps->m_dMax = 100;
		l_propertyMaxSweeps->m_tGuiType =  fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertyMaxSweeps);


		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyOptimalityRecordLength = new  fuzzification::SAMPLING_OPTION_PROPERTY();

		l_propertyOptimalityRecordLength->m_sDisplayText = wxT("optimality record length");
		l_propertyOptimalityRecordLength->m_sValue = wxT("2");
		l_propertyOptimalityRecordLength->m_dMin = 1;
		l_propertyOptimalityRecordLength->m_dMax = 1000;
		l_propertyOptimalityRecordLength->m_tGuiType =  fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertyOptimalityRecordLength);

		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyVerbos = new  fuzzification::SAMPLING_OPTION_PROPERTY();
		 //SAMPLING_OPTION_PROPERTY* l_propertyPreview = new  fuzzification::SAMPLING_OPTION_PROPERTY();
		l_propertyVerbos->m_sDisplayText = wxT("verbos");
		l_propertyVerbos->m_sValue = wxT("0");
		l_propertyVerbos->m_tGuiType =  fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_CHECKBOX;
		m_vSamplingProperties->push_back(l_propertyVerbos);
		break;
	}
	case  fuzzification::SAMPLING_TYPE::GENETIC_LHS:
	{
		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyPop = new  fuzzification::SAMPLING_OPTION_PROPERTY();
		l_propertyPop->m_sDisplayText = wxT("pop");
		l_propertyPop->m_sValue = wxT("100");
		l_propertyPop->m_dMin = 100;
		l_propertyPop->m_dMax = 10000;
		l_propertyPop->m_tGuiType = fuzzification:: GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertyPop);

		fuzzification:: SAMPLING_OPTION_PROPERTY* l_propertygen = new  fuzzification::SAMPLING_OPTION_PROPERTY();
		l_propertygen->m_sDisplayText = wxT("gen");
		l_propertygen->m_sValue = wxT("4");
		l_propertygen->m_dMin = 1;
		l_propertygen->m_dMax = 100;
		l_propertygen->m_tGuiType =fuzzification::  GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertygen);


		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyPMut = new  fuzzification::SAMPLING_OPTION_PROPERTY();
		l_propertyPMut->m_sDisplayText = wxT("PMut");
		l_propertyPMut->m_sValue = wxT("0.1");
		l_propertyPMut->m_dMin = 0.00001;
		l_propertyPMut->m_dMax = 1;
		l_propertyPMut->m_tGuiType = fuzzification:: GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertyPMut);


		fuzzification:: SAMPLING_OPTION_PROPERTY* l_propertyCriterium = new  fuzzification::SAMPLING_OPTION_PROPERTY();
		l_propertyCriterium->m_sDisplayText = wxT("criterium");
		l_propertyCriterium->m_sValue = wxT("S");
		l_propertyCriterium->m_tGuiType =  fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertyCriterium);


		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyVerbos = new fuzzification:: SAMPLING_OPTION_PROPERTY();
		l_propertyVerbos->m_sDisplayText = wxT("verbos");
		l_propertyVerbos->m_sValue = wxT("0");
		l_propertyVerbos->m_tGuiType = fuzzification:: GUI_PROPORTY_TYPE::GUI_TYPE_CHECKBOX;
		m_vSamplingProperties->push_back(l_propertyVerbos);
		break;
	}
	case fuzzification::SAMPLING_TYPE::MAXIMINI_LHS:
	{
		fuzzification::SAMPLING_OPTION_PROPERTY* l_propertyDup = new fuzzification:: SAMPLING_OPTION_PROPERTY();

		l_propertyDup->m_sDisplayText = wxT("dup");
		l_propertyDup->m_sValue = wxT("1");
		l_propertyDup->m_dMin = 1;
		l_propertyDup->m_dMax = 100;
		l_propertyDup->m_tGuiType =fuzzification::  GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX;
		m_vSamplingProperties->push_back(l_propertyDup);
		break;

	}
	}

}


double fuzzification::FuzzyReasoning::GetDoubleProperty(const wxString& p_sPropert)
{
	double l_ndValue;
	fuzzification::SP_SamplingPropertyVector::iterator l_itProperty;

	for (l_itProperty = m_vSamplingProperties->begin(); l_itProperty != m_vSamplingProperties->end(); l_itProperty++)
	{
		if ((*l_itProperty)->m_sDisplayText.IsSameAs(p_sPropert))
		{
			wxString l_sVAlue = (*l_itProperty)->m_sValue;

			if (l_sVAlue.ToDouble(&l_ndValue))
			{
				return l_ndValue;
			}
			else
			{
				return -1;
			}
		}
	}
	return -1;
}

int fuzzification::FuzzyReasoning::GetIntProperty(const wxString& p_sPropert)
{
	long l_ndValue;
	fuzzification::SP_SamplingPropertyVector::iterator l_itProperty;

	for (l_itProperty = m_vSamplingProperties->begin(); l_itProperty != m_vSamplingProperties->end(); l_itProperty++)
	{
		if ((*l_itProperty)->m_sDisplayText.IsSameAs(p_sPropert))
		{
			wxString l_sVAlue = (*l_itProperty)->m_sValue;

			if (l_sVAlue.ToLong(&l_ndValue))
			{
				return l_ndValue;
			}
			else
			{
				return -1;
			}
		}
	}
	return -1;
}

wxString fuzzification::FuzzyReasoning::GetStringProperty(const wxString& p_sPropert)
{

	fuzzification:: SP_SamplingPropertyVector::iterator l_itProperty;

	for (l_itProperty = m_vSamplingProperties->begin(); l_itProperty != m_vSamplingProperties->end(); l_itProperty++)
	{
		if ((*l_itProperty)->m_sDisplayText.IsSameAs(p_sPropert))
		{
			wxString l_sVAlue = (*l_itProperty)->m_sValue;

			return l_sVAlue;

		}
	}
	return _T("");
}



bool fuzzification::FuzzyReasoning::GetBoolProperty(const wxString& p_sPropert)
{
	fuzzification:: SP_SamplingPropertyVector::iterator l_itProperty;

	for (l_itProperty = m_vSamplingProperties->begin(); l_itProperty != m_vSamplingProperties->end(); l_itProperty++)
	{
		if ((*l_itProperty)->m_sDisplayText.IsSameAs(p_sPropert))
		{
			wxString l_sVAlue = (*l_itProperty)->m_sValue;

			if (l_sVAlue == wxT("1") || l_sVAlue == wxT("True"))

			{
				return true;
			}

		}
	}
	return false;
}

void fuzzification::FuzzyReasoning::GetLHSOptions()
{

	bool prserve,verbos = false;
	verbos=   GetBoolProperty(wxT("verbos"));
	prserve=  GetBoolProperty(wxT("preserve Draw"));
	long dup =  GetIntProperty(wxT("dup"));
	int maxsweeps =  GetIntProperty(wxT("MaxSweeps"));
	double eps = GetDoubleProperty(wxT("eps"));
	int recordLength = GetIntProperty(wxT("optimality record length"));//GetIntProperty(wxT("optimality record length"));

	int pop = GetIntProperty(wxT("pop"));
	int gen = GetIntProperty(wxT("gen"));
	double pmut = GetDoubleProperty(wxT("PMut"));
	std::string criterium = GetStringProperty(wxT("criterium"));
	m_OptionStruct.criterium = criterium;
	m_OptionStruct.pmut = pmut;
	m_OptionStruct.gen = gen;
	m_OptionStruct.pop = pop;
	m_OptionStruct.recordLength = recordLength;
	m_OptionStruct.eps = eps;
	m_OptionStruct.maxsweeps = maxsweeps;
	m_OptionStruct.dup = dup;
	m_OptionStruct.verbos = verbos;
	m_OptionStruct.preseve = prserve;
}

