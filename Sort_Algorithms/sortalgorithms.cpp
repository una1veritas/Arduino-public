#include "sortalgorithms.h"

#define MIN(a,b) ((a) < (b)? (a) : (b))
#define SWAP(a, b, t)   { (t) = (a); (a) = (b); (b) = (t); }

void maxHeapify(DATATYPE a[], INDEXTYPE i, INDEXTYPE end) {
  INDEXTYPE larger, tmp;
    
    while ( (i<<1) + 1 < end ) {
      larger = (i<<1) + 1;
      if ( ((i<<1) + 2 < end) && (a[(i<<1) + 1] < a[(i<<1) + 2]) ) {
        larger = (i<<1) + 2;
      }
      if ( a[larger] > a[i] ) {
        SWAP(a[larger], a[i], tmp);
        i = larger;
      } else {
        return;
      }
    }
    return;
    }
  
void buildMaxHeap(DATATYPE a[], INDEXTYPE n) {
  INDEXTYPE i;
    
    for (i = (n>>1) - 1 ; ! (i < 0); i--) {
      maxHeapify(a, i, n);
    }
    return;
    }
  
void heapSort(DATATYPE a[], INDEXTYPE n) {
  INDEXTYPE i;
  INDEXTYPE t;
    
    buildMaxHeap(a, n);
    for (i = n - 1; i > 0; i--) {
      // a[0] is always the maximum. 
      SWAP(a[0], a[i], t);
      maxHeapify(a, 0, i);
    }
    return;
    }
  

void selectionSort_range(DATATYPE a[], INDEXTYPE start, INDEXTYPE end){
  INDEXTYPE i, j, max;
  DATATYPE t; // i for the length (end+1) of sorted-array.
    
  for(i = end - 1; i > start; i--){
    for(j = start, max = i; j < i; j++ ){
      if(a[j] > a[max]) {
        max = j;
      }
    }
    SWAP(a[max], a[i], t);
    }
  return;
}
  
void selectionSort(DATATYPE a[], INDEXTYPE n) {
  return selectionSort_range(a, 0, n);
}


void quickSort_range(DATATYPE array[], INDEXTYPE start, INDEXTYPE end) {
  INDEXTYPE smaller, larger, mid;
  DATATYPE tmp;
    
    // do nothing if the size of array equals one. 
    if (! (end - start > 1)) {
      return;
    }
    
    // order the first, the middle and the last elements. 
    if (array[end - 1] < array[start]) {
      SWAP(array[start], array[end - 1], tmp);
    }
    if (end - start == 2) {
      return;
    }
    mid = ((end - start)>>1) + start;
    if (array[mid] < array[start]) {
      SWAP(array[start], array[mid], tmp);
    }
    if (array[end - 1] < array[mid]) {
      SWAP(array[mid], array[end - 1], tmp);
    }
    // already enough if the size is no more than three. 
    if (end - start == 3) {
      return;
    }
    // use the middle element as the pivot value. 
    SWAP(array[start], array[mid], tmp);
    smaller = start + 1;
    larger = end - 1;
    while ( smaller < larger) {
      if (array[smaller] <= array[start]) {
        smaller++;
      } else {
        // swap array[smaller] with array[larger - 1].
        larger--;
        SWAP(array[smaller], array[larger], tmp);
      }
    }
    
    // divide into two arrays; the latter always has at least one element. 
    quickSort_range(array, start, smaller);
    quickSort_range(array, smaller, end);
    return;
    }
    
void quickSort(DATATYPE array[], INDEXTYPE n) {
    quickSort_range(array, 0, n);
    }


void bubbleSort(DATATYPE a[], INDEXTYPE n){
    INDEXTYPE i, j;
    DATATYPE tmp;
    
    for(i = 1; i < n; i++){
      for(j = n - 1; j >= i; j--){
        if(a[j-1] > a[j]) {
          tmp = a[j];
          a[j] = a[j-1];
          a[j-1] = tmp;
        }
      }
    }
    return;
    }
  
void insertionSort(DATATYPE a[], INDEXTYPE n){
    INDEXTYPE i, j;
    DATATYPE t; // j for the length (end+1) of sorted-array.
    
    for(j = 1; j < n; j++) {
      t = a[j];
      for (i = j; i > 0; i--) {
        if (a[i-1] > t) {
          a[i] = a[i-1];
          continue;
        }
      }
      a[i] = t;
    }
    return;
    }
  
  

void merge(DATATYPE dst[], DATATYPE lsrc[], INDEXTYPE llen, INDEXTYPE rsrc[], INDEXTYPE rlen) {
  INDEXTYPE dstlen = llen + rlen;
  INDEXTYPE left, right, temp;
  for (left = 0, right = 0, temp = 0; temp < dstlen; temp++) {
    if (left < llen && right < rlen) {
      if (lsrc[left] < rsrc[right]) {
        dst[temp] = lsrc[left];
        left++;
      } else {
        dst[temp] = rsrc[right];
        right++;
      }
    } else {
      if (left < llen) {
        dst[temp] = lsrc[left];
        left++;
      } else {
        dst[temp] = rsrc[right];
        right++;
      }
    }
  }
}

void mergeSort_recursive(DATATYPE array[], INDEXTYPE n) {
    INDEXTYPE i, l, r;
    DATATYPE bufl[n/2];
    DATATYPE bufr[n/2+(n % 2)];
    
    if (n <= 1)
      return;
    for (i = 0; i < n/2; i++) {
      bufl[i] = array[i];
    }
    for (r = 0; i < n; i++, r++) {
      bufr[r] = array[i];
    }
    mergeSort_recursive(bufl, (n>>1) );
    mergeSort_recursive(bufr, n - (n>>1));
    for (i = 0, l = 0, r = 0; i < n; i++){
      if ( !(r < n - (n>>1)) ) {
        array[i]=bufl[l];
        l++;
        continue;
      } else if ( !(l < n) ) {
        array[i]=bufr[r];
        r++;
        continue;
      } else if ( (bufl[l] < bufr[r]) ) {
        array[i]=bufl[l];
        l++;
      } else {
        array[i]=bufr[r];
        r++;
      }
    }
    return;
    }
    
    
void mergeSort(DATATYPE array[], INDEXTYPE n) {
    DATATYPE buf[n];
    INDEXTYPE i, len, start, end; //, cleft, cright, ctemp;
    
    for (len = 1; len < n; len = len<<1) {
      for (start = 0; start < n; start += (len<<1)) {
        end = MIN(len, n - (start + len) );
        merge(buf+start, array+start, len, array+(start+len), end);
      }
      //copy back
      for (i = 0; i < n; i++)
        array[i] = buf[i];
    }
    return;
    }
    

