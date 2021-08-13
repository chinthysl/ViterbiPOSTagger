//@author Chinthaka Gamanayake in 12/2017

#pragma once

#include "TrainingModule.h"

class DecodingModule 
{
public:

    DecodingModule(TrainingModule trainingModule)
    {
        dataTrainingModule = trainingModule;
    }

    ~DecodingModule()
    {

    }
    
    //read input from file, sentences and words without POS tags
    bool readInput(string filename)
    {
        ifstream infile;
        infile.open(filename.c_str(), ios::in);
        if (infile.fail()) 
        {
            return false;
        }

        vector<string> wordsInSentence;
        string wordAndTags;
        while (getline(infile, wordAndTags))
        {
            if (wordAndTags == "")
            {
                INFO("Sentence End");
                inputSentences.push_back(wordsInSentence);
                wordsInSentence.clear();
                continue;
            }

            string word, tag;
            dataTrainingModule.splitWordAndTag(wordAndTags, word, tag);
            INFO(word);
            wordsInSentence.push_back(word);
        }

        infile.close();
        return true;
    }

    //run viterbi algorithm for each sentence
    void processData()
    {
        for (auto z = 0; z<inputSentences.size(); z++)
        {
            vector<string> lineInput = inputSentences[z];
            vector<string> tagResult = viterbiAlgorithm(lineInput);
            tagOutput.push_back(tagResult);
        }
    }

    //print output to file after assigning POS Tags with viterbi algorithm.
    bool printOutput(string filename)
    {
        FILE* outfile;
        outfile = fopen(filename.c_str(), "w+");
        if (outfile == NULL)
        {
            return false;
        }
        for (auto z = 0; z<inputSentences.size(); z++)
        {
            vector<string> lineInput = inputSentences[z];
            vector<string> lineTagOutput = tagOutput[z];

            //subsequent word and tag outputs
            for (auto i = 0; i<lineInput.size(); i++)
            {
                fprintf(outfile, " %s %s\n", lineInput[i].c_str(), lineTagOutput[i].c_str());
            }
            fprintf(outfile, "\n");
        }
        fclose(outfile);
        return true;
    }


private:
    
    TrainingModule dataTrainingModule;
    vector< vector<string> > inputSentences;
    vector< vector<string> > tagOutput;
    
    //reset/refresh tables and matrices to be used by viterbi algorithm
    void initViterbi(vector< vector<double> > &dp, vector< vector<int> > &parent, int inputSize) 
    {
        vector<double> emptyVec(TAGSIZE, 0);
        for (int i = 0; i<inputSize; i++)
        {
            dp.push_back(emptyVec);
        }

        parent.clear();
        vector<int> emptyVecInt(SMALLTAGSIZE, 0);
        for (int i = 0; i <= inputSize + 1; i++)
        {
            parent.push_back(emptyVecInt);
        }
        return;
    }

    //use viterbi algorithm to assign POS tags for each word
    vector<string> viterbiAlgorithm(vector<string> &input)
    {
        int inputSize = input.size();
        //initializing dp table and parent/backpointer table
        vector< vector<double> > dp;
        vector< vector<int> > parent;

        initViterbi(dp, parent, inputSize);

        //first loop to initialize the first state
        //i.e. connecting the start node to the first state nodes.
        //45 is the index for <s>
        for (int i = 0; i<SMALLTAGSIZE; i++) 
        {
            parent[0][i] = 45;
        }

        int wordIndex = dataTrainingModule.getWordIndex(input[0]);
        for (int i = 0; i<SMALLTAGSIZE; i++) 
        {
            dp[0][i] = log2(dataTrainingModule.tagPairProbMatrix[i][45]) + log2(dataTrainingModule.wordTagPairProbTable[wordIndex][i]);
        }

        //Dynamic programming to set rest of the states
        for (int d = 1; d<inputSize; d++)
        {
            wordIndex = dataTrainingModule.getWordIndex(input[d]);
            for (int i = 0; i<SMALLTAGSIZE; i++) 
            {
                double maxPrev = -DBL_MAX;
                //get the maximum from the previous nodes
                for (int j = 0; j<SMALLTAGSIZE; j++)
                {
                    double tempDouble = dp[d - 1][j] + log2(dataTrainingModule.tagPairProbMatrix[i][j]);
                    //INFO(tempDouble);
                    if (maxPrev<tempDouble)
                    {
                        maxPrev = tempDouble;
                        parent[d][i] = j;
                    }
                }
                //store the result of the maximum/optimum probability in the state node
                dp[d][i] = maxPrev + log2(dataTrainingModule.wordTagPairProbTable[wordIndex][i]);
            }
        }

        //last loop to get the maximum/optimum value of the last state
        //i.e. connecting the last state nodes to the end node
        //46 is the index of </s>
        double maxPrev = -DBL_MAX;
        for (int j = 0; j<SMALLTAGSIZE; j++) 
        {
            double tempDouble = dp[input.size() - 1][j] + log2(dataTrainingModule.tagPairProbMatrix[46][j]);
            if (maxPrev<tempDouble)
            {
                maxPrev = tempDouble;
                for (int i = 0; i<SMALLTAGSIZE; i++)
                {
                    parent[inputSize][i] = j;
                }
            }
        }

        vector<string> tagResult;

        //obtain tag indexes from viterbi backpointer (i named it parent) and store in tagOutput
        stack<int> s;
        int prevBestTag = parent[inputSize][0];
        s.push(prevBestTag);
        for (int d = inputSize - 1; d>0; d--)
        {
            s.push(parent[d][prevBestTag]);
            prevBestTag = parent[d][prevBestTag];
        }
        while (!s.empty())
        {
            tagResult.push_back(dataTrainingModule.tagIndexMap[s.top()]);
            s.pop();
        }
        return tagResult;
    }

};
