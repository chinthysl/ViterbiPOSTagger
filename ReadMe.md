# POS Tagger implementation using Viterbi Algorithm
## Introduction
This report describes an implementation of POS bigram tagger which is developed in C++ using Penn Treebank tag set. Viterbi algorithm is implemented in this application to find the most probable sequence of the POS tags. For more information about the algorithm please refer the file ViterbiAlgorithm.pdf.

Mainly this report demonstrates the implemented modules, data structures used and algorithms implemented. This application takes two files as inputs, a training data file (train.data) with words and POS tags and a test file (test.data) with word sequences. An output file with word and tag pairs is generated as the result of the decoding process of the application.

## Modules and Thread architecture

This is a single threaded C++ application with two modules named as “TrainingModule” and “DecordingModule”. As a future development plan, iterative independent work flows such as Viterbi trellis traversal in the DecordingModule can be implemented in a multi-threaded architecture or parallel computer architecture to gain better performance and also to balance the load in the testing data set.

### The application execution steps are as follows,
1. TrainingModule read the train.data file and extract the word/tag related counts and information.
2. TrainingModule stores the above information in its data structures.
3. TrainingModule does smoothing on extracted data and process them, find transition and emission probability values need to the Viterbi algorithm.
4. DecodingModule is instantiated with the TrainingModule instance.
5. DecodingModule read the test.data file, extracts word sequences and store in its data structures.
6. DecodignModule launches Viterbi algorithm and compute the most probable tag sequences using the probabilities inside the TrainingModule.
7. DecodignModule writes the computed word/tag pair to the output.data file.

## Training Data
The training data used by the program (train.data) consists of sentences which contains words and the corresponding tags (from Penn Treebank tag set). Each word with its corresponding tags is placed in a line. Sentences are separated from empty lines. Apart from the 45 Penn Treebank POS tags, a special start state and end (final) state POS tags, “\<s\>” and “\<\/s\>”, are added.
Attributes in the TrainingModule
The required attributes are as follows,
1. The set of words in the training set (words are case-sensitive).
2. The set of POS tags in the training set.
3. The transition probability to have a POS tag t_i given the previous POS tag t_i-1.
4. The probability to have a word, given a POS tag.

## Data structures maintained to extract information
To obtain the required attributes, the following data structures are maintained.

- POS Tag Map - A map to translate a POS tag to its assigned integer index and vice versa. (two std::map structures)
- Word Map - A map to translate a word to its assigned integer index and vice versa. (two std::map structures)
- Tag Count - A table to store the number of occurrences of every POS tag. (an array)
- Word Count - A table to store the number of occurrences of every word. (an std::vector)
- Bigram Tag Matrix - A matrix to store the number of occurrences of a POS tag preceding another POS tag (two consecutive POS tags). (an 2D array)
- Word Tag Matrix - A matrix to store the number of occurrences of a word being assigned with a specific tag. (a std::vector containing std::vectors)

## Information Processing and Smoothing
The raw counts obtained from the training data, are then processed, computed, and stored in the respective probability matrices (bigram tag matrix and word tag matrix). To calculate the probability, smoothing is required to handle zero raw count. Thus one is added to every data structure to get rid of zero probabilities.
The above smoothing method is also used to handle unknown (unseen) words in the test data. One word “<UNK>” is added into the word set with one occurrence, without any tags, and it represents all unknown words encountered. In other words, all unknown words are treated in equal manner.

## Usage
- C++ 11 has to be installed in your environment to compile this application.
- ViterbiPOSTagger.cpp source file contains the main method of the application.
- Compilation commands,
Linux : ``` g++ -std=c++11ViterbiPOSTagger.cpp –o ViterbiPOSTagger ```
Windows : ``` g++ -std=c++11ViterbiPOSTagger.cpp –o ViterbiPOSTagger.exe```
- Training file name : train.data
- Testing file name : test.data
- Execution commands,
Linux : ``` ViterbiPOSTagger <training_filename> <testing_filename> <output_filename> ```
Windows : ``` ViterbiPOSTagger.exe <training_filename> <testing_filename> <output_filename> ```