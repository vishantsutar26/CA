 #include <iostream>
 #include <vector>
 #include <cmath>
 #include <cstdlib>
 #include <ctime>
#include <bits/stdc++.h>
using namespace std;

struct Point {
    double x, y;
    int cluster; // Cluster assignment
};

struct Centroid {
    double x, y;
};

// Function to calculate Euclidean distance
double distance(Point a, Centroid b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Function to assign points to the nearest centroid
void assignClusters(vector<Point>& points, vector<Centroid>& centroids, int k) {
    for (auto& point : points) {
        double minDist = distance(point, centroids[0]);
        int clusterID = 0;

        for (int i = 1; i < k; i++) {
            double dist = distance(point, centroids[i]);
            if (dist < minDist) {
                minDist = dist;
                clusterID = i;
            }
        }
        point.cluster = clusterID;
    }
}

// Function to update centroid positions
void updateCentroids(vector<Point>& points, vector<Centroid>& centroids, int k) {
    vector<int> counts(k, 0);
    vector<double> sumX(k, 0), sumY(k, 0);

    for (auto& point : points) {
        int clusterID = point.cluster;
        sumX[clusterID] += point.x;
        sumY[clusterID] += point.y;
        counts[clusterID]++;
    }

    for (int i = 0; i < k; i++) {
        if (counts[i] != 0) {
            centroids[i].x = sumX[i] / counts[i];
            centroids[i].y = sumY[i] / counts[i];
        }
    }
}

int main() {
    srand(time(0));

    // Example data points
    vector<Point> points = {
        {1.0, 2.0}, {1.5, 1.8}, {5.0, 8.0}, {8.0, 8.0},
        {1.0, 0.6}, {9.0, 11.0}, {8.0, 2.0}, {10.0, 2.0}, {9.0, 3.0}
    };

    int k = 4; // Number of clusters
    int maxIterations = 100;
    vector<Centroid> centroids(k);

    // Initialize centroids randomly from points
    for (int i = 0; i < k; i++) {
        int idx = rand() % points.size();
        centroids[i] = {points[idx].x, points[idx].y};
    }

    // K-Means main loop
    for (int iter = 0; iter < maxIterations; iter++) {
        assignClusters(points, centroids, k);
        updateCentroids(points, centroids, k);
    }

    // Output the final cluster assignments
    for (auto& point : points) {
        cout << "Point (" << point.x << ", " << point.y << ") -> Cluster " << point.cluster << endl;
    }

    return 0;
}
