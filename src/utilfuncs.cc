#include "utilfuncs.h"


template<class T> T** new_2DArray(T** arr, int x, int y){
  arr = new T*[x];
  for (int i=0; i<x; i++){
    arr[i] = new T[y];
  }
  return arr;
}

template<class T> T*** new_3DArray(T*** arr, int x, int y, int z){
  arr = new T**[x];
  for (int i=0; i<x; i++){
    arr[i] = new_2DArray(arr[i], y, z);
  }
  return arr;
}

template<class T> T**** new_4DArray(T**** arr, int w, int x, int y, int z){
  arr = new T***[w];
  for (int i=0; i<w; i++){
    arr[i] = new_3DArray(arr[i], x, y, z);
  }
  return arr;
}

template<class T> void delete_2DArray(T** arr, int x, int y){
  for (int i=0; i<x; i++){
    delete[] arr[i];
  }
  delete[] arr;
}

template<class T> void delete_3DArray(T*** arr, int x, int y, int z){
  for (int i=0; i<x; i++){
    delete_2DArray(arr[i], y, z);
  }
  delete[] arr;
}

template<class T> void delete_4DArray(T**** arr, int w, int x, int y, int z){
  for (int i=0; i<w; i++){
    delete_3DArray(arr[i], x, y, z);
  }
  delete[] arr;
}

