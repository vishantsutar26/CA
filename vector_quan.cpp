#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

typedef vector<double> Vector;
typedef vector<Vector> Matrix;

// Compute Euclidean distance between two vectors
double euclideanDistance(const Vector& a, const Vector& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

// Compute the mean vector of a cluster
Vector computeMean(const Matrix& cluster) {
    if (cluster.empty()) return {};

    Vector mean(cluster[0].size(), 0.0);
    for (const auto& vec : cluster)
        for (size_t i = 0; i < vec.size(); ++i)
            mean[i] += vec[i];

    for (auto& val : mean)
        val /= cluster.size();

    return mean;
}

// LBG / k-means-like vector quantization algorithm
Matrix vectorQuantization(const Matrix& data, int numCentroids, int maxIter = 100, double epsilon = 1e-5) {
    srand(time(0));
    Matrix codebook;

    // Initialize with one centroid (mean of all data)
    Vector initialCentroid = computeMean(data);
    codebook.push_back(initialCentroid);

    // Expand to desired number of centroids
    while (codebook.size() < numCentroids) {
        // Splitting step
        Matrix newCodebook;
        for (const auto& c : codebook) {
            Vector plus(c), minus(c);
            for (auto& val : plus) val *= (1 + epsilon);
            for (auto& val : minus) val *= (1 - epsilon);
            newCodebook.push_back(plus);
            newCodebook.push_back(minus);
        }
        codebook = newCodebook;

        // K-means refinement
        for (int iter = 0; iter < maxIter; ++iter) {
            vector<Matrix> clusters(codebook.size());

            // Assign vectors to nearest centroid
            for (const auto& vec : data) {
                double minDist = numeric_limits<double>::max();
                int bestIdx = 0;
                for (size_t i = 0; i < codebook.size(); ++i) {
                    double dist = euclideanDistance(vec, codebook[i]);
                    if (dist < minDist) {
                        minDist = dist;
                        bestIdx = i;
                    }
                }
                clusters[bestIdx].push_back(vec);
            }

            // Update centroids
            Matrix newCodebook;
            for (size_t i = 0; i < codebook.size(); ++i) {
                if (!clusters[i].empty()) {
                    newCodebook.push_back(computeMean(clusters[i]));
                } else {
                    // Keep old centroid if cluster is empty
                    newCodebook.push_back(codebook[i]);
                }
            }

            // Check for convergence
            double totalChange = 0.0;
            for (size_t i = 0; i < codebook.size(); ++i)
                totalChange += euclideanDistance(codebook[i], newCodebook[i]);

            codebook = newCodebook;
            if (totalChange < epsilon) break;
        }
    }

    return codebook;
}

// Quantize data using the codebook
Matrix quantize(const Matrix& data, const Matrix& codebook) {
    Matrix quantized;
    for (const auto& vec : data) {
        double minDist = numeric_limits<double>::max();
        int bestIdx = 0;
        for (size_t i = 0; i < codebook.size(); ++i) {
            double dist = euclideanDistance(vec, codebook[i]);
            if (dist < minDist) {
                minDist = dist;
                bestIdx = i;
            }
        }
        quantized.push_back(codebook[bestIdx]);
    }
    return quantized;
}

// Example usage
int main() {
    // Generate synthetic 2D data
    Matrix data;
    for (int i = 0; i < 1000; ++i) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        data.push_back({x, y});
    }

    int numCentroids = 8;
    Matrix codebook = vectorQuantization(data, numCentroids);
    Matrix quantizedData = quantize(data, codebook);

    // Print codebook
    cout << "Codebook vectors:\n";
    for (const auto& vec : codebook) {
        for (auto val : vec)
            cout << val << " ";
        cout << endl;
    }

    return 0;
}
