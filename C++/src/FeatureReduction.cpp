#include "FeatureReduction.h"
#include "pca.h" 

#define NUM_REDUCED_FEATURES 3

vector<vector<float>> ReduceFeaturesUsingPCA(vector<vector<float>> &feature_set)
{
	stats::pca pca(feature_set.front().size());

	vector<double> v3(feature_set.front().size(), 0);
	vector<vector<double>> double_feature_set(feature_set.size(), v3);

	for(int i = 0; i < feature_set.size(); ++i) {
		for(int j = 0; j < feature_set.front().size(); ++j) {
			double_feature_set[i][j] = (double) feature_set[i][j];
		}
	}

	for(int i = 0; i < double_feature_set.size(); ++i) {
		pca.add_record(double_feature_set[i]);
	}

	pca.solve();

	vector<float> v1 (NUM_REDUCED_FEATURES, 0);
	vector<vector<float>> princ_comp(double_feature_set.size(), v1);

	vector<double> v2 (double_feature_set.size(), 0);
	vector<vector<double>> transpose_princ_comp(NUM_REDUCED_FEATURES, v2);

	for(int i = 0; i < NUM_REDUCED_FEATURES; ++i) {
		transpose_princ_comp[i] = pca.get_principal(i);
	}

	for(int i = 0; i < double_feature_set.size(); ++i) {

		for(int j = 0; j < NUM_REDUCED_FEATURES; ++j) {

			princ_comp[i][j] = (float)transpose_princ_comp[j][i];
		}
	}

	return princ_comp;
}