//@author Chinthaka Gamanayake in 12/2017

#pragma once

#include "CommonIncludes.h"

class TrainingModule 
{
public:
    //initialize the necessary attributes in the constructor
    TrainingModule()
    {
        memset(tagPairProbMatrix, 0, sizeof(tagPairProbMatrix));
        memset(tagUnigramProbArray, 0, sizeof(tagUnigramProbArray));

        setTags();
        memset(transitionTagPairCountMatrix, 0, sizeof(transitionTagPairCountMatrix));
        wordTagPairCountTable.clear();

        vector<int> emptyVec(TAGSIZE, 0);
        wordTagPairCountTable.push_back(emptyVec);

        totalNumberofUniqueWords = 0;
        totalNumberOfWords = 0;

        //add one word "<UNK>" to handle unknown wordMap.
        int wordIndex = insertWord("<UNK>");
        wordCountVector[wordIndex] += 1;
        totalNumberOfWords += 1;
        
        setTags();
    }

    ~TrainingModule()
    {
        
    }

    //split a word and a tag from a combined format of "word tag1 tag2"
    //this method will also be used by the Decording module
    void splitWordAndTag(string wordAndTags, string &word, string &tag)
    {
        int delimIndex1 = getDelimiterIndex(wordAndTags);
        word = wordAndTags.substr(0, delimIndex1);

        string tags = wordAndTags.substr(delimIndex1 + 1);
        int delimIndex2 = getDelimiterIndex(tags);
        tag = tags.substr(0, delimIndex2);
        return;
    }

    //read data from Training data file and store in the data structures
    bool countData(string filename)
    {
        ifstream infile;
        infile.open(filename.c_str(), ios::in);
        if (infile.fail())
        {
            ERROR("FileOpen failed for counting data");
            std::exit(EXIT_FAILURE);
        }

        string startTag = "<s>";
        string endTag = "</s>";
        string prevTag = startTag;
        tagCountArray[45] += 1;

        string wordAndTags;
        while (getline(infile, wordAndTags))
        {
            if (wordAndTags == "")
            {
                //update for the sentence close tag
                int tagIndex = getTagIndex(endTag);
                int prevTagIndex = getTagIndex(prevTag);
                transitionTagPairCountMatrix[tagIndex][prevTagIndex] += 1;
                //add tag count for last character </s>, mapped to index 46
                tagCountArray[46] += 1;
                prevTag = startTag;
                //add tag count for start character <s>, mapped to index 45
                //since new sentence begin
                tagCountArray[45] += 1;

                INFO("Sentence End");
                continue;
            }

            string word, tag;

            //general case, counting the occurences of t(i),t(i-1)
            splitWordAndTag(wordAndTags, word, tag);
            INFO("Word&Tags:" << wordAndTags);
            INFO(word << "/" << tag);

            int tagIndex = getTagIndex(tag);
            int prevTagIndex = getTagIndex(prevTag);
            INFO("PrevTagIndex:" << prevTagIndex << " TagIndex:" << tagIndex);
            transitionTagPairCountMatrix[tagIndex][prevTagIndex] += 1;
            tagCountArray[tagIndex] += 1;

            int wordIndex = insertWord(word);
            INFO("WordIndex:" << wordIndex);
            wordTagPairCountTable[wordIndex][tagIndex] += 1;
            wordCountVector[wordIndex] += 1;
            prevTag = tag;
            totalNumberOfWords++;
        }
        //remove the additionaly added start character count
        tagCountArray[45] -= 1;

        totalNumberofUniqueWords = wordMap.size();

        infile.close();
        return true;
    }

    //process the input data smooth them and calc probabilities
    void processDataWithSmoothing()
    {
        vector<double> emptyVec(TAGSIZE, 0);

        for (int i = 0; i<totalNumberofUniqueWords; i++)
        {
            wordTagPairProbTable.push_back(emptyVec);
        }

        INFO("Applying Add-One Smoothing and processing unigram probability");
        addOneSmoothing();
        processUnigramProbability();

        return;
    }


private:

    //data structures to store count of the test data
    int transitionTagPairCountMatrix[TAGSIZE][TAGSIZE];
    int tagCountArray[TAGSIZE];
    vector< vector<int> > wordTagPairCountTable;
    vector<int> wordCountVector;

    //data structures to store the transition and emission probabilities 
    double tagPairProbMatrix[TAGSIZE][TAGSIZE];
    double tagUnigramProbArray[TAGSIZE];
    vector< vector<double> > wordTagPairProbTable;
    vector<double> wordTagUnigramProbVector;

    //varibales to store the number of words and unique words
    int totalNumberOfWords;
    int totalNumberofUniqueWords;

    //these maps will assign integers to tags and words for simplycity of 
    //the usage rather than using std::strings
    map<string, int> tagMap;
    map<int, string> tagIndexMap;

    map<string, int> wordMap;
    map<int, string> wordIndexMap;

    //have to make the DecodingModule access the private data structures of the
    //TrainingModule to getrid of getter methods
    friend class DecodingModule;

    //***********************************private methods************************************

    //initialize the tagMap map with Penn Treebank POS Tags
    //there are two additional characters, "<s>" and "</s>" to
    //to indicate the boundry of a sentence(start and end)
    void setTags()
    {
        string pennTags[] = 
        { 
            "CC", "CD", "DT", "EX", "FW", "IN", "JJ", "JJR", "JJS", "LS", "MD", "NN", "NNS", 
            "NNP", "NNPS", "PDT", "POS", "PRP", "PRP$", "RB", "RBR", "RBS", "RP", "SYM", 
            "TO", "UH", "VB", "VBD", "VBG", "VBN", "VBP", "VBZ", "WDT", "WP", "WP$", "WRB", 
            "$", "#", "``", "''", ")", "(", ",", ".", ":", "<s>", "</s>" 
        };

        for (int i = 0; i<TAGSIZE; i++) 
        {
            tagMap.insert(pair<string, int>(pennTags[i], i));
            tagIndexMap.insert(pair<int, string>(i, pennTags[i]));
        }
        return;
    }

    //return tag index given a string of POS tag
    int getTagIndex(string tag) 
    {
        auto it = tagMap.find(tag);
        if (it == tagMap.end()) 
        {
            ERROR("Tag not found in the tagsMap");
            return -1;
        }
        else 
        {
            return it->second;
        }
    }

    //insert a word into the word map
    int insertWord(string word)
    {
        auto it = wordMap.find(word);
        if (it == wordMap.end())
        {
            int index = wordMap.size();
            wordMap.insert(pair<string, int>(word, index));
            wordIndexMap.insert(pair<int, string>(index, word));
            vector<int> emptyVec(TAGSIZE, 0);
            wordTagPairCountTable.push_back(emptyVec);
            wordCountVector.push_back(0);
            return index;
        }
        else 
        {
            return it->second;
        }
    }

    //try to find a word from the word map, and return its index
    //if it is not found, return the index of "<UNK>" (unknown word)
    int getWordIndex(string word) 
    {
        auto it = wordMap.find(word);
        //if word is found, return index straight away
        if (it != wordMap.end())
        {
            return it->second;
        }

        //if word not found
        //return the index of "<UNK>" aka unknown wordMap
        it = wordMap.find("<UNK>");
        return it->second;
    }

    //get the index of the slash ' ' which separates a word and its POS Tags
    int getDelimiterIndex(string wordAndTags)
    {
        int stringLength = wordAndTags.length();
        for (int i = 0; i < stringLength; i++)
        {
            if (wordAndTags[i] == ' ')
            {
                return i;
            }
        }
        ERROR("Word tag boundry not found");
        std::exit(EXIT_FAILURE);
    }

    //smoothing is done to eliminate the zero probability value generation in
    //unigram probability generation method
    void addOneSmoothing() 
    {
        //ADD ONE to all tables
        for (int i = 0; i<TAGSIZE; i++) 
        {
            for (int j = 0; j<TAGSIZE; j++) 
            {
                transitionTagPairCountMatrix[i][j] += 1;
                tagCountArray[i] += 1;
                tagCountArray[j] += 1;
            }
        }

        for (int i = 0; i<totalNumberofUniqueWords; i++) 
        {
            for (int j = 0; j<TAGSIZE; j++) 
            {
                wordTagPairCountTable[i][j] += 1;
                wordCountVector[i] += 1;
                tagCountArray[j] += 1;
            }
        }

        //END ADD ONE
        for (int i = 0; i<TAGSIZE; i++) 
        {
            for (int j = 0; j<TAGSIZE; j++) 
            {
                //probability table(i|j) = count(j,i) / count(j)
                //probability table(t(k)|t(k-1)) = count(t(k-1),t(k)) / count(t(k-1))
                tagPairProbMatrix[i][j] = static_cast<double>(transitionTagPairCountMatrix[i][j]) / static_cast<double>(tagCountArray[j]);
            }
        }

        for (int i = 0; i<totalNumberofUniqueWords; i++) 
        {
            for (int j = 0; j<TAGSIZE; j++) 
            {
                //probability table(i|j) = count(j,i) / count(j)
                //probability table(w(k)|t(k)) = count(t(k),w(k)) / count(t(k))
                wordTagPairProbTable[i][j] = static_cast<double>(wordTagPairCountTable[i][j]) / static_cast<double>(tagCountArray[j]);
            }
        }
        return;
    }

    void processUnigramProbability() 
    {
        for (int i = 0; i<TAGSIZE; i++)
        {
            tagUnigramProbArray[i] = tagCountArray[i] / static_cast<double>(totalNumberOfWords);
        }
        for (int i = 0; i<totalNumberofUniqueWords; i++)
        {
            wordTagUnigramProbVector.push_back(wordCountVector[i] / static_cast<double>(totalNumberOfWords));
        }
        return;
    }

};
