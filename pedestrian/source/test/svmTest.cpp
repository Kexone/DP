/* 
 * 
 * Testing trained SVM  on samples data
 * 
 */

#include <iostream>
#include <fstream>
#include "../train/trainhog.h"
#include "svmTest.h"
#include "../alg/hog.h"

// The number of clock ticks per minute
#define CLOCKS_PER_MIN  ((clock_t)60000)

void SvmTest::runSvmTest()
{
	int maxIterTest;
	std::cout << "\n\n******************" << std::endl;
	std::cout << "***  SVM TEST  ***" << std::endl;
	std::cout << "******************" << std::endl;
	std::cout << "COUNT OF ITERATION: ";
	std::cin >> maxIterTest;
	if(maxIterTest <= 0)
	{
		std::cout << "COUNT OF ITERATION MUST BE GREATER THEN ZERO!\nEND" << std::endl;
		return;
	}
	std::cout << "SOFT (1) OR GROSS (2) REGRESSION TEST?";
	int typeTest;
	std::cin >> typeTest;
	if(typeTest == 1)
	{
		iterChange = 10;
		parChange = 0.01;
	}
	else
	{
		iterChange = 100;
		parChange = 0.1;
	}
	std::string samplesPath = "samples/";
	std::string posSamples = samplesPath + "listPosMin.txt";
	std::string negSamples = samplesPath + "listNegMin.txt";
	std::string posTest = samplesPath + "listTestPosMin.txt";
	std::string negTest = samplesPath + "listTestNegMin.txt";
	std::string classifierName = "test.yml";
	
	std::vector< cv::Mat > posTestLst;
	std::vector< cv::Mat > negTestLst;
	loadMats(posSamples, posTestLst);
	loadMats(negSamples, negTestLst);
	std::cout << "*** TESTING HAS STARTED ***" << std::endl << std::endl;

	int actualIter = 1;
	

	this->maxIterations = 300;
	this->termCriteria = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS;
	this->kernel = cv::ml::SVM::LINEAR;
	this->type = cv::ml::SVM::NU_SVC;
	this->epsilon = 1.e-6;
	this->coef0 = 0.0;
	this->degree = 3;
	this->gamma = 0.1;
	this->nu = 0.1;
	this->p = 0.1;
	this->c = 0.1;
	while(actualIter != maxIterTest+1)
	{
		nTruePos = 0;
		nFalsePos = 0;
		nTrueNeg = 0;
		nFalseNeg = 0;

		std::cout << actualIter << ". ITERATION OF TESTING" << std::endl;

		TrainHog th = TrainHog(maxIterations,termCriteria,kernel,type,epsilon,coef0,degree,gamma,nu,p,c,classifierName);

		trainTime = clock();
		th.fillVectors(posSamples);
		th.fillVectors(negSamples, true);
		th.train(false);
		trainTime = clock() - trainTime;

		Hog h = Hog(classifierName);

		classTime = clock();
		h.detect(posTestLst, nTruePos, nFalsePos);
		h.detect(negTestLst, nTrueNeg, nFalseNeg, false);
		classTime = clock() - classTime;

		print2File(actualIter);
		iterateValues();

		actualIter++;
	}
}

void SvmTest::print2File(int actualIter)
{
	std::ofstream file;
	file.open("result.txt", std::ios::app);

	file << "\n\t<< START" << actualIter << ".ITERATION>>" << std::endl;
	file << "\t__SVM SETTINGS__" << std::endl;
	file << "MAX ITER: " << this->maxIterations << std::endl;
	file << "TERM CRIT: " << this->termCriteria << std::endl;
	file << "KERNEL: " << this->kernel << std::endl;
	file << "TYPE SVM: " << this->type << std::endl;
	file << "EPSILON: " << this->epsilon << std::endl;
	file << "COEF0:" << this->coef0 << std::endl;
	file << "DEGREE: " << this->degree << std::endl;
	file << "GAMMA: " << this->gamma << std::endl;
	file << "NU: " << this->nu << std::endl;
	file << "P: " << this->p << std::endl;
	file << "C: " << this->c << std::endl;
	file << "TRAIN TIME: " << static_cast<float>(trainTime / CLOCKS_PER_MIN ) << " MIN" << std::endl;

	file << "\n\t__SVM RESULTS__" << std::endl;
	file << "POS GOOD: " << nTruePos << " POS BAD: " << nFalsePos << std::endl;
	file << "NEG GOOD: " << nTrueNeg << " NEG BAD: " << nFalseNeg << std::endl;
	file << "CLASS TIME: " << static_cast<float>(classTime / CLOCKS_PER_MIN) << " MIN" << std::endl;
	file << "\t<< END" << actualIter << ".ITERATION>>" << std::endl;

	file.close();
}

void SvmTest::loadMats(std::string& samplesPath, std::vector<cv::Mat>& samples)
{
	assert(!samplesPath.empty());

	cv::Mat frame;
	std::fstream sampleFile(samplesPath);
	std::string oSample;
	while (sampleFile >> oSample) {

		//	frame = cv::imread(oSample, CV_32FC3);
		samples.push_back(cv::imread(oSample, CV_32FC3));
	}
}

void SvmTest::iterateValues()
{
	maxIterations += iterChange;
	this->nu += parChange;
	this->p += parChange;
	this->c += parChange;
}

SvmTest::SvmTest()
{
	
}
