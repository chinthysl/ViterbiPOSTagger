//@author Chinthaka Gamanayake in 12/2017

#include "DecodingModule.h"
#include "TrainingModule.h"

int main(int argc, char* argv[])
{
    string trainingFilename = argv[1];
    string testFilename = argv[2];
    string outFilename = argv[3];

    //create TrainingModule instance to count data from Training file &
    //to calculate probabilities with smoothing
    TrainingModule trainingModuleInst;

    INFO("***************************");
    INFO("Reading Training file : " << trainingFilename);
    INFO("Counting attributes from the training file...");
    trainingModuleInst.countData(trainingFilename);

    INFO("***************************");
    INFO("Processing data, calculating probabilities with smoothing : ");
    trainingModuleInst.processDataWithSmoothing();

    //DecodingModule instance is constructed with the trainingModuleInst
    DecodingModule viterbiDecodingInst(trainingModuleInst);

    INFO("***************************");
    INFO("Reading input/test data from file : " << testFilename);
    viterbiDecodingInst.readInput(testFilename);

    INFO("***************************");
    INFO("Processing Data (using Viterbi Algorithm)");
    viterbiDecodingInst.processData();

    INFO("***************************");
    INFO("Writing output to file : " << outFilename);
    viterbiDecodingInst.printOutput(outFilename);

    INFO("***************************");
    INFO("Done! DecodingModule is terminated.");

    return 0;
}

