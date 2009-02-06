#ifndef UTILFUNCS_H
#define UTILFUNCS_H


template<class T> T** new_2DArray(T**, int x, int y);
template<class T> T*** new_3DArray(T***, int x, int y, int z);
template<class T> void delete_2DArray(T** arr, int x, int y);
template<class T> void delete_3DArray(T*** arr, int x, int y, int z);



#endif
