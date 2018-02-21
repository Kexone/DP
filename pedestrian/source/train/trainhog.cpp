#include "trainhog.h"



TrainHog::TrainHog()
{
	this->maxIterations = Settings::maxIterations;
	this->termCriteria = Settings::termCriteria;
	this->kernel = Settings::kernel;
	this->type = Settings::type;
	this->epsilon = Settings::epsilon;
	this->coef0 = Settings::coef0;
	this->degree = Settings::degree;
	this->gamma = Settings::gamma;
	this->nu = Settings::paramNu;
	this->p = Settings::paramP;
	this->c = Settings::paramC;
	this->classifierName = Settings::classifierName2Train + ".yml";
	this->pedestrianSize = Settings::pedSize;
	this->blockSize = Settings::blockSize;
	this->cellSize = Settings::cellSize;
	this->strideSize = Settings::strideSize;
}

TrainHog::TrainHog(int maxIterations, int termCriteria, int kernel, int type, double epsilon, double coef0,
	int degree, double gamma, double nu, double p, double c, std::string classifierName)
{
	this->maxIterations = maxIterations;
	this->termCriteria = termCriteria;
	this->kernel = kernel;
	this->type = type;
	this->epsilon = epsilon;
	this->coef0 = coef0;
	this->degree = degree;
	this->gamma = gamma;
	this->nu = nu;
	this->p = p;
	this->c = c;
	this->classifierName = classifierName;
	this->pedestrianSize = Settings::pedSize;
	this->blockSize = Settings::blockSize;
	this->cellSize = Settings::cellSize;
	this->strideSize = Settings::strideSize;
}

void TrainHog::trainFromMat(std::string matPath, std::string labelsPath)
{
	cv::Mat trainMat;
	cv::FileStorage fs(matPath, cv::FileStorage::READ);
	fs["samples"] >> trainMat;

	std::ifstream is(labelsPath);
	std::istream_iterator < int > start(is), end;
	std::vector< int > labels = std::vector < int > (start, end);

	trainSvm(trainMat, labels);
}

void TrainHog::trainFromMat(cv::Mat trainMat, std::vector<int> labels)
{
	trainSvm(trainMat, labels);
}

void TrainHog::train(bool saveData)
{
	std::vector< cv::Mat > posSamplesLst;
	std::vector< cv::Mat > negSamplesLst;
    std::vector< cv::Mat > gradientLst;
	std::vector< int > labels;
    cv::Mat trainMat;

	Utils::fillSamples2List(Settings::samplesPos, posSamplesLst, labels, pedestrianSize);
	Utils::fillSamples2List(Settings::samplesNeg, negSamplesLst, labels, pedestrianSize, true);
	std::cout << "Positive samples: " << posSamplesLst.size() << std::endl;
	std::cout << "Negative samples: " << negSamplesLst.size() << std::endl;

    extractFeatures(posSamplesLst, gradientLst);
    extractFeatures(negSamplesLst, gradientLst);
    convertSamples2Mat(gradientLst, trainMat);

	if (saveData) saveLabeledMat(trainMat, labels);

    trainSvm(trainMat, labels);
}

void TrainHog::calcMatForTraining(std::string posSamples, std::string negSamples, cv::Mat& trainMat, std::vector<int> &labels)
{
	std::vector< cv::Mat > posSamplesLst;
	std::vector< cv::Mat > negSamplesLst;
	std::vector< cv::Mat > gradientLst;

	Utils::fillSamples2List(Settings::samplesPos, posSamplesLst, labels, pedestrianSize);
	Utils::fillSamples2List(Settings::samplesNeg, negSamplesLst, labels, pedestrianSize, true);
	std::cout << "Positive samples: " << posSamplesLst.size() << std::endl;
	std::cout << "Negative samples: " << negSamplesLst.size() << std::endl;

	extractFeatures(posSamplesLst, gradientLst);
	extractFeatures(negSamplesLst, gradientLst);
	convertSamples2Mat(gradientLst, trainMat);
}

void TrainHog::extractFeatures(const std::vector< cv::Mat > &samplesLst, std::vector< cv::Mat > &gradientLst) const
{
    cv::HOGDescriptor hog(
					pedestrianSize, //winSize
                    cv::Size(blockSize,blockSize), //blocksize
                    cv::Size(strideSize,strideSize), //blockStride,
                    cv::Size(cellSize,cellSize), //cellSize,
                    9, //nbins,
                    0, //derivAper,
                    -1, //winSigma,
                    0, //histogramNormType,
                    0.2, //L2HysThresh,
                    true //gammal corRection,
                                    //nlevels=64
                    );
    cv::Mat gr;
    std::vector< cv::Point > location;
    std::vector< float > descriptors;
    for(auto &mat : samplesLst) {
      //  cv::cvtColor(mat, gr,cv::COLOR_BGR2GRAY);

	//	gr.convertTo(gr, CV_8U);

		hog.compute(mat, descriptors,cv::Size(8, 8),cv::Size(0, 0));
		gradientLst.push_back(cv::Mat(descriptors).clone());
    }
}

void TrainHog::trainSvm(cv::Mat &trainMat, const std::vector<int> &labels)
{
  //  std::cout << "START training ..." << std::endl;
	clock_t timer = clock();
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
	//cv::Ptr<cv::ml::Boost> svm = cv::ml::Boost::create();

	svm->setCoef0(this->coef0);
	svm->setDegree(this->degree);
	svm->setTermCriteria(cv::TermCriteria(this->termCriteria, this->maxIterations, this->epsilon));
	svm->setGamma(this->gamma);
	svm->setKernel(this->kernel);//linear
	svm->setNu(this->nu);
	svm->setP(this->p);
	svm->setC(this->c);
	svm->setType(this->type);
	svm->train(trainMat, cv::ml::ROW_SAMPLE, cv::Mat(labels));
	svm->save(this->classifierName);
	timer = clock() - timer;
 //   std::cout << "training DONE ..."<< static_cast<float>(timer) / (CLOCKS_PER_SEC*60) << " min" <<  std::endl;

}

void TrainHog::convertSamples2Mat(const std::vector<cv::Mat> &trainSamples, cv::Mat &trainData)
{
    const int rows = trainSamples.size();
    const int cols = std::max( trainSamples[0].cols, trainSamples[0].rows);
    cv::Mat tmp(1,cols,CV_32FC1);
    trainData = cv::Mat(rows,cols, CV_32FC1);
    int i = 0;
    for(auto sample : trainSamples) {
        assert(sample.cols == 1 || sample.rows == 1);
        if(sample.cols == 1) {
            cv::transpose(sample,tmp);
            tmp.copyTo( trainData.row( i ));
        }
        else if(sample.rows == 1) {
            sample.copyTo(trainData.row( i ));
        }
        i++;
    }
	
}

void TrainHog::saveLabeledMat(cv::Mat data, std::vector< int > labels)
{
	cv::FileStorage fs("test.yml", cv::FileStorage::WRITE);
	fs << "samples"<< data;
	fs.release();
	std::ofstream output_file("./labels.txt");
	std::ostream_iterator<int> output_iterator(output_file, "\n");
	std::copy(labels.begin(), labels.end(), output_iterator);
}

void TrainHog::printSettings()
{
	std::cout << std::setw(10) << std::setfill('_') << std::endl;
	std::cout << "SVM SETTING" << std::endl;
	std::cout << "MAX ITER: " << this->maxIterations << std::endl;
	std::cout << "TERM CRIT: " << this->termCriteria << std::endl;
	std::cout << "KERNEL: " << this->kernel << std::endl;
	std::cout << "TYPE SVM: " << this->type << std::endl;
	std::cout << "EPSILON: " << this->epsilon << std::endl;
	std::cout << "COEF0:" << this->coef0 << std::endl;
	std::cout << "DEGREE: " << this->degree << std::endl;
	std::cout << "GAMMA: " << this->gamma << std::endl;
	std::cout << "NU: " << this->nu << std::endl;
	std::cout << "P: " << this->p << std::endl;
	std::cout << "C: " << this->c << std::endl;
	std::cout << std::setw(10) << std::setfill('_') << std::endl;
}
