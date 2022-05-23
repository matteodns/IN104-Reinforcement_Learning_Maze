#include "functions.h"

int min(int a, int b){
    return (a>b) ? b : a;
}

int max(int a, int b){
    return (a>b) ? a : b;
}

float fmaxf(float a, float b){
	return (a>b) ? a : b;
}
