

#include "FuzzyReasoning.h"
#include <wx/wx.h>
#include <wx/busyinfo.h>

FuzzyReasoning::FuzzyReasoning(unsigned long alphaLevels, unsigned long nDiscPoints, std::vector<vector<double>> paramMat, unsigned long FN,unsigned long pcount)
{
	m_nAlphaLevels = alphaLevels;
	m_nDiscPoints = nDiscPoints;
	m_paramMatrix = paramMat;
	m_bIsAbortCalculation = false;

	//m_compMatrix = compMat;
	m_compressedBand =new SP_Compressed_Fuzzy_Band( );
	m_alphaStepSize =(double) 1 / m_nAlphaLevels;
	double answer = static_cast<float>(static_cast<int>(m_alphaStepSize * 10.)) / 10.;
	m_nFuzzyNum = FN;
	m_pCount = pcount;
	long LL = pow(m_nDiscPoints, m_nFuzzyNum);
	 	m_compMatrix.resize  (LL, std::vector<double>(m_paramMatrix.size(), 0));

	///	m_nSampleMeanMatrix.resize( m_pCount, std::vector<double>(LL));
	 
		///m_nSortXxMatrix.resize(m_pCount, std::vector<double>(2*(alphaLevels-1)+1,0));
	 
	///	m_nSortYyMatrix.resize(m_pCount, std::vector<double>(2 * (alphaLevels - 1) + 1, 0));
  
      //  m_nFinal_Sort_XXX_Result_Mat.resize(m_pCount, std::vector<double>(2 * (alphaLevels - 1) + 1, 0));
 		//m_nFinal_Sort_YYY_Result_Mat.resize(m_pCount, std::vector<double>(2 * (alphaLevels - 1) + 1, 0));
}
 
FuzzyReasoning::FuzzyReasoning(unsigned long l, unsigned long s) {
	m_nAlphaLevels = l;
	m_nDiscPoints = s;
}
void   FuzzyReasoning::SetAbort(bool bAbort)
{
	m_bIsAbortCalculation = bAbort;
}
FuzzyReasoning::FuzzyReasoning() {

}

SP_Vector2DDouble FuzzyReasoning::GetCombinationMatrix()
{
	return m_compMatrix;
}

std::vector<double>& FuzzyReasoning::GetAlphaSet()
{

	return m_valphaLevels;
}

 


unsigned long FuzzyReasoning::GetNumFuzzyLevels()
{

	return m_nAlphaLevels;
}

unsigned long FuzzyReasoning::GetNumSamplePoints()
{

	return m_nDiscPoints;
}

unsigned long FuzzyReasoning::GetNumFuzzyNum()
{
	return m_nFuzzyNum;
}
/*Calculate vector of alpha levels from 0 to 1*/
void FuzzyReasoning::CalculateAlphaLevels()
{
	
	m_valphaLevels.clear();
	double alpha = 0.0;
	m_valphaLevels.push_back(alpha);
	int nlevels = 1;
	
	while (alpha <= 1 && nlevels != m_nAlphaLevels)
	{
		alpha += m_alphaStepSize;
		m_valphaLevels.push_back(alpha);
		nlevels = nlevels + 1;
	}
	m_valphaLevels[nlevels-1 ] = 1;
}

SP_Vector2DDouble FuzzyReasoning::GetAlphaCutForAllLevels()
{
	
	 
	for (int i = 0; i < m_paramMatrix.size(); i++)
	{
		double left = m_paramMatrix[i][0];
		double center = m_paramMatrix[i][1];
		double right = m_paramMatrix[i][2];
		if (!(center - left) < pow(10, -10))
		{
			TriangularFN t(left, right, center);
			for (int i = 0; i < m_valphaLevels.size()-1; i++)
			{
			
			AlphaCutSet set1 = t.getAlphaCutSet(m_valphaLevels[i], m_nDiscPoints);

			m_alphaCutSetAllLevels.push_back(set1);
		    }
		}
		 
	}
	return m_alphaCutSetAllLevels;

}
/*Do ampling for the current(inputed) alpha level of all TFNs */
void FuzzyReasoning::DoSamplePointsCombination(double calpha) 
{
	
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

/*Calculate minimum and maximum traces over time*/
void FuzzyReasoning::GetMinMaxTraceThread(ResultFuzzyBand fuzzyBand, long dataColumnNum)
{
	/*divide the time ito two slots, and run 2*spieces threads to speed up the calculations*/
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
	//m_nMinResultMatrix.resize(dataColumnNum, std::vector<double>(ltime));
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

 /////////////////////////////////////////////////////////////

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
	//wxBusyInfo info1(ss, NULL);
	/*
	for (std::vector<std::thread>::iterator it = threadVector.begin(); it != threadVector.end(); ++it)
	{
		if ((*it).joinable() )
		{
			(*it).join();
		}

	}
	// end


	long count = 0; int j = 0;
	for (curvesIt = calcVec.begin(); curvesIt != calcVec.end(); curvesIt++)
	{
		 
		if (count == 0)
		{
			m_nFinalMinResultFuzzyMatrix.push_back((*curvesIt).m_minVector);
			m_nFinalMaxResultFuzzyMatrix.push_back((*curvesIt).m_maxVector);
			count++;
		}
		else {
			for (long lcount = 0; lcount < (*curvesIt).m_minVector.size(); lcount++)

			{
				m_nFinalMinResultFuzzyMatrix[m_nFinalMinResultFuzzyMatrix.size() - 1].push_back((*curvesIt).m_minVector[lcount]);
				m_nFinalMaxResultFuzzyMatrix[m_nFinalMaxResultFuzzyMatrix.size() - 1].push_back((*curvesIt).m_maxVector[lcount]);
			}
			count = 0;
		}

		 
	}
	 */

}


 std::vector<SP_Vector2DDouble> FuzzyReasoning::GetMinMaxFuzzyTraceMatrix(ResultFuzzyBand& fuzzyBand)
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
	
	 GetMinMaxTraceThread(fuzzyBand, l_placeCount);


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
 TimedTFN FuzzyReasoning::CalculateTfnAt( long timePoint,  long dataColumn,const ResultFuzzyBand& fuzzyband)
 {
	 std::map<double, int> m_levels2PointsMap;/*store each level with its corrosponding trace count */
	 long lnumTraces = fuzzyband.size();
	 SP_Vector2DDouble   m_nMinMaxResult;
	 TimedTFN            timedMembershipFunc;

	 /*calculating the number of traces per level*/
	 for (int i = 0; i < m_nAlphaLevels; i++)
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
void FuzzyReasoning::CalcAllTFN( long dataColumn,  ResultFuzzyBand& band)
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
SP_Compressed_Fuzzy_Band* FuzzyReasoning::CompressResults(  ResultFuzzyBand  fuzzyBand)
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