#include <inttypes.h>

#define DATATYPE uint8_t
#define INDEXTYPE uint32_t
#define KEY(x)  (x)

void heapSort(DATATYPE a[], INDEXTYPE n);
void buildMaxHeap(DATATYPE a[], INDEXTYPE n);
void heapSort(DATATYPE a[], INDEXTYPE n);
void maxHeapify(DATATYPE a[], INDEXTYPE i, INDEXTYPE end);
void selectionSort(DATATYPE a[], INDEXTYPE n);
void selectionSort_range(DATATYPE a[], INDEXTYPE start, INDEXTYPE end);

void quickSort(DATATYPE a[], INDEXTYPE n);
void quickSort_range(DATATYPE a[], INDEXTYPE start, INDEXTYPE end);

void bubbleSort(DATATYPE a[], INDEXTYPE n);
void insertionSort(DATATYPE a[], INDEXTYPE n);
void mergeSort_recursive(DATATYPE a[], INDEXTYPE n);
void mergeSort(DATATYPE a[], INDEXTYPE n);
void merge(DATATYPE dst[], DATATYPE lsrc[], INDEXTYPE llen, DATATYPE rsrc[], INDEXTYPE rlen);

