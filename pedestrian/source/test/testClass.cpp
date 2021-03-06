﻿#include "testClass.h"
#include "dlibSvmTest.h"

#define PARAMETER_NU  1
#define PARAMETER_C  2
#define PARAMETER_P  3

#define TYPE_TEST_RANDOM 1
#define TYPE_TEST_DIFF_EVO 2
#define TYPE_TEST_NESTED_ITER 3

TestClass::TestClass()
{
	
}

void TestClass::initTesting()
{
	int type;
	std::cout << " 1) CROSS VALIDATION OpenCV SVM  \n 2) CROSS VALIDATION Dlib SVM \n 3) TESTING OpenCV SVM" << std::endl;
	std::cout << "Selection: ";
	std::cin >> type;

	if(type == 1)
		crossTestingSvm();
	if (type == 2)
		crossTestingDlibSvm();
	if (type == 3)
		testingSvm();
	std::cout << "DONE" << std::endl;
}

void TestClass::crossTestingSvm()
{
	int typeTest;
	std::cout << "\n\n******************" << std::endl;
	std::cout << "***  SVM TEST  ***" << std::endl;
	std::cout << "******************" << std::endl;
	std::cout << "DIFFERENTIAL EVOLUTION (1)\nRANDOM TESTING (2)\nNESTED ITERATION (3)\nCHOOSE TYPE: ";
	std::cin >> typeTest;


	if (typeTest == 1)
	{
		initLog(TYPE_TEST_DIFF_EVO);
		SvmTest::initResultFile(ss);
		diffEvoTest();
	}
	else if (typeTest == 2)
		randomTest();
	else if (typeTest == 3)
		iterationCycle();
}

void TestClass::crossTestingDlibSvm()
{
	int type;
	std::cout << " 1) NU SVM \n 2) C SVM" << std::endl;
	std::cin >> type;

	std::vector< int > labels;
	cv::Mat trainMat;

	TrainHog hog;
	hog.calcMatForTraining(trainMat, labels, true);
	std::vector < double > fLabels(labels.begin(), labels.end());
	DlibSvmTest test = DlibSvmTest(trainMat, fLabels);

	std::cout << "\n\n*************************" << std::endl;
	std::cout << "***  DLIB SVM TEST    ***" << std::endl;
	std::cout << "*************************" << std::endl;
	std::cout << "DEFAULT CROSS VALIDATION" << std::endl;
	test.process(type);
}

void TestClass::testingSvm()
{
		std::string svmPath =  "KONF_15.yml" ;
		std::string samples[] = { "bad/fHD.txt" };
		Hog hog = Hog(svmPath);
		std::vector < int > predict;
		std::vector < float > distances;

		for (auto typeSample : samples) {
			cv::Mat frame;
			std::fstream sampleFile(typeSample);
			std::string oSample;
			while (sampleFile >> oSample) {
				frame = cv::imread(oSample);
				if (frame.empty()) {
					std::cout << "eerr " << oSample << std::endl;
					sampleFile.close();
					break;
				}
				//frame.convertTo(frame, CV_32FC3);
				int value = 0;
				float distance = 0.0;
				cv::resize(frame, frame, Settings::pedSize);
				value = cvRound(hog.predict(frame));
				distance = hog.getDistance(frame);
				predict.push_back(value);
				distances.push_back(distance);
				frame.release();
			}
		}
		std::string output = "bad/results/predicted_fHD.txt"; //PREDICT
		std::string output2 = "bad/results/distances_fHD.txt"; //DISTANCES
		std::ofstream output_file(output);
		std::ofstream output_file2(output2);
		for (int a = 0; a < predict.size(); a++)	{
			output_file << predict[a] << std::endl;
			output_file2 << distances[a] << std::endl;
		}
		output_file.close();
		output_file2.close();
		std::cout << "RESULT FOR: " << output << std::endl;
		evaluate("bad/GT_fHD.txt", output);
}

void TestClass::randomTest()
{
	int maxRepeatTest, typeIncr, typeTest;
	
	std::cout << "NUMBER OF REPEATS: ";
	std::cin >> maxRepeatTest;

	if (maxRepeatTest <= 0)
	{
		std::cout << "NUMBER OF ITERATIONS MUST BE GREATER THAN ZERO!\nEND" << std::endl;
		return;
	}
	std::cout << "SOFT (1) OR GROSS (2) ITERATION: ";
	std::cin >> typeTest;

	if (typeTest == 1)
	{
		iterChange = 50;
		parChange = 0.001f;
	}
	else
	{
		iterChange = 100;
		parChange = 0.01f;
	}

	std::cout << "ITERATE ALL AT ONCE (1) \nITERATION ONE BY ONE (2) \nONLY ITERATION(3)\nCHOOSE TYPE: ";
	std::cin >> typeIncr;
	if (typeIncr == 2) {
		std::cout << "TESTING WILL TAKE PLACE MAX ITER * VARIABLES (" << maxRepeatTest * 3 << " ITER)" << std::endl;
		maxRepeatTest *= 3;
	}
	else if (typeIncr == 3)
	{
		std::cout << "NUMBER OF ITERATIONS PER CYCLE:";
		std::cin >> iterChange;
	}
	initLog(typeTest-1,typeIncr,maxRepeatTest);
	nu = 0.0;
	p = 0.0;
	c = 0.0;
	SvmTest svm;
	svm.preprocessing();
	svm.initResultFile(ss);

	std::cout << std::endl << "*** TESTING HAS STARTED ***" << std::endl << std::endl;

	while (maxRepeatTest != 0)
	{
		incrementSvmValues(typeIncr, maxRepeatTest);

		svm.setParams(maxIterations, nu, c, p);
		svm.process();

		maxRepeatTest--;
	}
}
void TestClass::incrementSvmValues(int typeIncr, int maxRepTest)
{
	if (typeIncr == 1)
	{
		maxIterations += iterChange;
		nu += parChange;
		p += parChange;
		c += parChange;
	}
	else if (typeIncr == 2)
	{
		int par = (int) (maxRepTest / (float)(maxRepTest / 3));
		if (par == PARAMETER_NU)
		{
			nu += parChange;
			p = 0.1;
			c = 0.1;
		}
		if (par == PARAMETER_C)
		{
			nu = 0.1;
			p = 0.1;
			c += parChange;
		}
		if (par == PARAMETER_P)
		{
			nu = 0.1;
			p += parChange;
			c = 0.1;
		}
		maxIterations += iterChange;
	}
	else if (typeIncr == 3)	maxIterations += iterChange;


}

void TestClass::iterationCycle()
{
	std::cout << "This testing is only for type C_SVC" << std::endl;
	initLog(TYPE_TEST_NESTED_ITER);
	SvmTest svm;
	svm.preprocessing();
	svm.initResultFile(ss);
	for (int iter = 50; iter < 1500; iter += 50) {
		for (double gamma = 0.0001; gamma < 1; gamma *= 5)
		{
			for (double c = 0.0001; c < 1; c *= 5)
			{
				svm.setParams(iter, c, gamma);
				std::cout << "TESTING PARAMS C: " << c << " GAMMA: " << gamma << " iter: " << iter << "\t" << svm.process() << std::endl;
			}
		}
	}
}

void TestClass::diffEvoTest()
{
	int dimsCount = 4;
	std::cout << "DIMENSIONS ARE DEFAULT SET TO 4." << std::endl;
	std::cout << "POPULATION IS DEFAULT SET TO 50." << std::endl;

	DiffEvoTest dims(dimsCount);
	de::DifferentialEvolution de(dims, 50);

	de.Optimize(1000, true);
}

void TestClass::initLog(int typeTest, int typeIncr, int maxRepeatTest)
{
	std::string incrType = "SOFT";
	std::string iterType = "SINGLE";
	std::string testName = "SVM";
	if (typeTest == TYPE_TEST_RANDOM) incrType = "GROSS";
	if (typeTest == TYPE_TEST_DIFF_EVO) incrType = "DIFFERENTIAL EVOLUTION";
	if (typeTest == TYPE_TEST_NESTED_ITER) incrType = "NESTED ITERATION";
	else if (typeIncr == 2) iterType = "ALL AT ONCE";
	else if (typeIncr == 3) iterType = "ONLY ITERATION";


	ss << "__________________________________" << std::endl;
	ss << "\t\t******************" << std::endl;
	ss << "\t\t***  "<< testName <<" TEST  ***" << std::endl;
	ss << "\t\t******************" << std::endl;
	if (typeTest != TYPE_TEST_DIFF_EVO && typeTest != TYPE_TEST_NESTED_ITER) {
		ss << "NUMBER OF REPEATS: " << maxRepeatTest << std::endl;
		ss << "REGRESSION TYPE: " << incrType << std::endl;
		ss << "INCREMENT TYPE: " << iterType << std::endl;
	}
	else
	{
		ss << "TYPE: " << incrType << std::endl;
	}
	ss << "__________________________________" << std::endl;
}

void TestClass::evaluate(std::string groundTruthFile, std::string resultsFilePath) {
	// Load files
	std::ifstream groundTruthStream, resultsStream;
	groundTruthStream.open(groundTruthFile);
	resultsStream.open(resultsFilePath);
	assert(groundTruthStream.is_open());
	assert(resultsStream.is_open());

	int detectorLine, groundTruthLine;
	int falsePositives = 0;
	int falseNegatives = 0;
	int truePositives = 0;
	int trueNegatives = 0;

	while (true) {
		if (!(resultsStream >> detectorLine)) break;
		groundTruthStream >> groundTruthLine;

		int detect = detectorLine;
		int ground = groundTruthLine;

		//false positives
		if ((detect == 1) && (ground == 0)) {
			falsePositives++;
		}

		//false negatives
		if ((detect == 0) && (ground == 1)) {
			falseNegatives++;
		}

		//true positives
		if ((detect == 1) && (ground == 1)) {
			truePositives++;
		}

		//true negatives
		if ((detect == 0) && (ground == 0)) {
			trueNegatives++;
		}
	}

	groundTruthStream.close();
	resultsStream.close();

	std::cout << "falsePositives " << falsePositives << std::endl;
	std::cout << "falseNegatives " << falseNegatives << std::endl;
	std::cout << "truePositives " << truePositives << std::endl;
	std::cout << "trueNegatives " << trueNegatives << std::endl;
	float acc = (float)(truePositives + trueNegatives) /
		(float)(truePositives + trueNegatives + falsePositives + falseNegatives);
	std::cout << "Accuracy " << acc << std::endl;
}