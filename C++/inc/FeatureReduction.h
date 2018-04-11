#include <iostream>
#include <cerrno>
#include <cstdint>
#include <cmath>
#include <vector>
#include <iterator>
#include <fstream>
#include <cstdio>


#ifndef __FEATUREREDUCTION_H__
#define __FEATUREREDUCTION_H__

using namespace std;

vector<vector<float>> ReduceFeaturesUsingPCA(vector<vector<float>> &feature_set);

#endif // #ifndef __FEATURE_REDUCTION_HPP__