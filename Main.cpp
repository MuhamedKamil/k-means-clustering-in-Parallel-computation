#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 
#include <fstream>
#include <string>
#include <vector>
#include <omp.h>
using namespace std;
int main()
{
	int lengthOfInput, StartClock, StopClock, K, NumOfIteration = 0;           //Read from file part ( Sequentional , parallel)
	cout << "Please Enter The Length Of Input : ";
	cin >> lengthOfInput;
	double *Input = new double[lengthOfInput];
	ifstream myfile(to_string(lengthOfInput) + ".txt");
	string line;
	StartClock = clock();
	for (int i = 0; getline(myfile, line); i++)
	{
		Input[i] = stod(line);
	}
	StopClock = clock();
	cout << "-----------------------------------------------------" << endl;
	cout << "Reading file is taken Time = " << (StopClock - StartClock) / double(CLOCKS_PER_SEC) << " s\n";   // calculate time of reading from file
	cout << "*****************************************************" << endl;
	cout << "Please Enter The Number Of Clusters : ";
	cin >> K;
	cout << "-----------------------------------------------------" << endl;
	double *Mean = new double[K];
	double *Sum = new double[K];
	int *Count = new int[K];
	//===========================================================================================================
	//***********************************************************************************************************
	  StartClock = clock();                                                   //K-Mean Algortithm in Sequentional way
	for (int i = 0; i < K; i++)
	{
		Mean[i] = Input[i];
		Sum[i] = 0;
		Count[i] = 0;
	}
	StopClock = clock();
	cout << "Time to  Select intial means in Sequentional Part " << (StopClock - StartClock) / double(CLOCKS_PER_SEC) << " s\n";
	cout << "-----------------------------------------------------" << endl;
	StartClock = clock();
	while (NumOfIteration < 20)
	{
		for (int i = 0; i < lengthOfInput; i++)
		{
			double absdist;
			double mintemp = std::numeric_limits<double>::max(); //big number
			double tmpdata;
			int meanIndex;
			for (int m = 0; m < K; m++)
			{
				absdist = abs(Input[i] - Mean[m]);
				if (absdist < mintemp)
				{
					mintemp = absdist;
					tmpdata = Input[i];
					meanIndex = m;
				}
			}
			Sum[meanIndex] += tmpdata;
			Count[meanIndex]++;
		}
		for (int i = 0; i < K; i++)
		{
			Mean[i] = Sum[i] / Count[i];
		}

		for (int i = 0; i < K; i++)
		{
			Sum[i] = 0;
			Count[i] = 0;
		}
		NumOfIteration++;
	}
	StopClock = clock();
	cout << "K-Mean Time in Sequentional Part " << (StopClock - StartClock) / double(CLOCKS_PER_SEC) << " s\n";
	cout << "-----------------------------------------------------" << endl;
	cout << "The Output : ";
	for (int i = 0; i < K; i++)
	{
		cout << Mean[i] << " ";
	}
	cout << endl;
	cout << "*****************************************************" << endl;
	
	//===========================================================================================================
	
	NumOfIteration = 0;                                                             //K-Mean Algortithm in Parallel way
	omp_set_num_threads(4);
	StartClock = clock();
#pragma omp parallel                                                        
	{
      #pragma omp for schedule(static)
		for (int f = 0; f < K; f++)
		{
			Mean[f] = Input[f];
			Sum[f] = 0;
			Count[f] = 0;
		}
	}
	StopClock = clock();
	cout << "Time to Select intial mean in parallel " << (StopClock - StartClock) / double(CLOCKS_PER_SEC) << " s\n";
	cout << "--------------------------------------" << endl;

	StartClock = clock();
	
	while (NumOfIteration < 20)
	{
		omp_set_num_threads(4);
#pragma omp parallel 
		{
          #pragma omp for schedule(static)
			for (int i = 0; i < lengthOfInput; i++)
			{
				double absoluteDist;
				double min = std::numeric_limits<double>::max();
				double ElementData;
				int meanIndex;
				for (int m = 0; m < K; m++)
				{
					absoluteDist = abs(Input[i] - Mean[m]);
					if (absoluteDist < min)
					{
						min = absoluteDist;
						ElementData = Input[i];
						meanIndex = m;
					}
				}
       #pragma omp atomic
				Sum[meanIndex] += ElementData;
       #pragma omp atomic
				Count[meanIndex]++;
			}
		}

	  omp_set_num_threads(4);
     #pragma omp parallel
			{
              #pragma omp for schedule(static)
				for (int j = 0; j < K; j++)
				{
					Mean[j] = Sum[j] / Count[j];

				}
			}
		  omp_set_num_threads(4);
   #pragma omp parallel
			{
               #pragma omp for schedule(static)
				for (int s = 0; s < K; s++)
				{
					Sum[s] = 0;
					Count[s] = 0;
				}
			}
		
		NumOfIteration++;
	}
	StopClock = clock();
	cout << "K-Mean Time in Parallel  " << (StopClock - StartClock) / double(CLOCKS_PER_SEC) << " s\n";
	cout << "--------------------------------------" << endl;
	cout << "The Output : ";
	omp_set_num_threads(4);
#pragma omp parallel
	{
       #pragma omp for schedule(static)
		for (int a = 0; a < K; a++)
		{
			cout << Mean[a] << "  ";
		}
	}
	cout << endl;
	cout << "*****************************************************" << endl;
}