#include <stdio.h>
#include <stdlib.h>

#define SIZE 100000  // 1e5 elements

void quickSort(int arr[], int low, int high);
int partition(int arr[], int low, int high);

// Quick Sort function
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Partition function
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            // Swap arr[i] and arr[j]
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    // Swap arr[i+1] and pivot
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return i + 1;
}

int main() {
    int *arr = (int *)malloc(SIZE * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Generate random values
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand();
    }

    // Sort the array
    quickSort(arr, 0, SIZE - 1);

    // Print the first 100 sorted elements for checking
    // printf("First 100 elements of sorted array:\n");
    // for (int i = 0; i < 100; i++) {
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");

    free(arr);
    return 0;
}
