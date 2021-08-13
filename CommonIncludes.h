//@author Chinthaka Gamanayake in 12/2017

#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <stack>
#include <cmath>

#define DBL_MAX 1.7976931348623158e+308
#define TAGSIZE 47
#define SMALLTAGSIZE 45

using namespace std;

#define ERROR(text) \
{\
    cout << "ERROR: " << __FUNCTION__ <<"\t:"<< text << "\t:" << __FILE__ <<":" << __LINE__ << endl;\
}\

#define INFO(text) \
{\
    cout << "INFO: " << __FUNCTION__ <<"\t:"<< text << endl;\
}\

