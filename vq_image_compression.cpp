#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace cv;

typedef vector<float> Vector;
typedef vector<Vector> Matrix;

// Parameters
const int BLOCK_SIZE = 4;          // 4x4 blocks
const int CODEBOOK_SIZE = 64;      // Number of clusters
const int MAX_ITER = 50;

// Convert image block to vector
Vector blockToVector(const Mat& block) {
    Vector vec;
    for (int i = 0; i < block.rows; ++i)
        for (int j = 0; j < block.cols; ++j)
            vec.push_back(static_cast<float>(block.at<uchar>(i, j)));
    return vec;
}

// Convert vector to image block
Mat vectorToBlock(const Vector& vec) {
    Mat block(BLOCK_SIZE, BLOCK_SIZE, CV_8U);
    for (int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; ++i)
        block.at<uchar>(i / BLOCK_SIZE, i % BLOCK_SIZE) = static_cast<uchar>(vec[i]);
    return block;
}

// Compute Euclidean distance
float euclidean(const Vector& a, const Vector& b) {
    float sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

// Compute mean of cluster
Vector computeMean(const Matrix& cluster) {
    if (cluster.empty()) return Vector(BLOCK_SIZE * BLOCK_SIZE, 0.0f);

    Vector mean(BLOCK_SIZE * BLOCK_SIZE, 0.0f);
    for (const auto& v : cluster)
        for (size_t i = 0; i < v.size(); ++i)
            mean[i] += v[i];

    for (auto& val : mean)
        val /= cluster.size();

    return mean;
}

// K-means clustering to create codebook
Matrix createCodebook(const Matrix& vectors, int k) {
    srand(time(0));
    Matrix codebook;
    int n = vectors.size();

    // Random initialization
    for (int i = 0; i < k; ++i)
        codebook.push_back(vectors[rand() % n]);

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        vector<Matrix> clusters(k);

        // Assign vectors to nearest codebook vector
        for (const auto& v : vectors) {
            float minDist = 1e9;
            int best = 0;
            for (int i = 0; i < k; ++i) {
                float d = euclidean(v, codebook[i]);
                if (d < minDist) {
                    minDist = d;
                    best = i;
                }
            }
            clusters[best].push_back(v);
        }

        // Recalculate centroids
        for (int i = 0; i < k; ++i) {
            if (!clusters[i].empty())
                codebook[i] = computeMean(clusters[i]);
        }
    }

    return codebook;
}

// Compress image using VQ
vector<int> compress(const vector<Vector>& vectors, const Matrix& codebook) {
    vector<int> indices;
    for (const auto& v : vectors) {
        float minDist = 1e9;
        int best = 0;
        for (int i = 0; i < codebook.size(); ++i) {
            float d = euclidean(v, codebook[i]);
            if (d < minDist) {
                minDist = d;
                best = i;
            }
        }
        indices.push_back(best);
    }
    return indices;
}

// Decompress using indices and codebook
Mat decompress(const vector<int>& indices, const Matrix& codebook, int h, int w) {
    Mat result(h, w, CV_8U);
    int idx = 0;

    for (int i = 0; i < h; i += BLOCK_SIZE) {
        for (int j = 0; j < w; j += BLOCK_SIZE) {
            Mat block = vectorToBlock(codebook[indices[idx++]]);
            block.copyTo(result(Rect(j, i, BLOCK_SIZE, BLOCK_SIZE)));
        }
    }
    return result;
}

int main() {
    // Load grayscale image
    Mat image = imread("input.png", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cerr << "Image not found!" << endl;
        return -1;
    }

    // Crop to be divisible by block size
    int h = image.rows - image.rows % BLOCK_SIZE;
    int w = image.cols - image.cols % BLOCK_SIZE;
    Mat cropped = image(Rect(0, 0, w, h));

    vector<Vector> blocks;
    for (int i = 0; i < h; i += BLOCK_SIZE) {
        for (int j = 0; j < w; j += BLOCK_SIZE) {
            Mat block = cropped(Rect(j, i, BLOCK_SIZE, BLOCK_SIZE));
            blocks.push_back(blockToVector(block));
        }
    }

    cout << "Creating codebook..." << endl;
    Matrix codebook = createCodebook(blocks, CODEBOOK_SIZE);

    cout << "Compressing..." << endl;
    vector<int> indices = compress(blocks, codebook);

    cout << "Decompressing..." << endl;
    Mat reconstructed = decompress(indices, codebook, h, w);

    // Save output
    imwrite("compressed.png", reconstructed);

    cout << "Done. Saved to compressed.png" << endl;
    return 0;
}
