#include "../Lib/commonLib.h"


int findSide(float x1, float y1, float x2, float y2, float x3, float y3){
  float val = ((y3 - y1)*(x2 - x1) - (y2 - y1) * (x3 - x1));
  //printf("x1 = %f, y1 = %f, x2 = %f, y2 = %f, x3 = %f, y3 = %f,val = %f\n", x1, y1, x2, y2, x3, y3, val);
  if (val > 0){
    return 1;
  }
  if (val < 0){
    return -1;
  }
  return 0;
}

float lineDist(float x1, float y1, float x2, float y2, float x3, float y3){
  return fabsf((y3 - y1)*(x2 - x1) - (y2 - y1) * (x3 - x1));
}


void quickhull(int n,float* x, float* y, float x_max, float y_max, float x_min, float y_min, int side, int* n_h, float* x_h, float* y_h){
  int ind = -1;
  float max_dist = 0;

  for(int i = 0; i < n;i++){
    //printf("x[%d] = %f\n", i, x[i]);
    float temp = lineDist(x_max, y_max, x_min, y_min, x[i], y[i]);
    //printf("temp = %f\n", temp);
    int side_val = findSide(x_max, y_max, x_min, y_min, x[i], y[i]);
    //printf("side_val = %d\n", side_val);
    if(side_val == side && temp > max_dist){
      ind = i;
      max_dist = temp;
    }
  }

  if(ind == -1){
    int found1 = 0;
    int found2 = 0;
    for(int j = 0; j < *n_h; j++){
      if(x_h[j] == x_max && y_h[j]==y_max){
	found1 = 1;
      }
      if(x_h[j] == x_min && y_h[j] ==y_min){
	found2 = 1;
      }
    }
    //printf("pre\n");
    if(!found1){
      //printf("adding (%f, %f)\n", x_max, y_max);
      x_h[*n_h] = x_max;
      y_h[*n_h] = y_max;
      *n_h = *n_h + 1;
    }
    if(!found2){
      //printf("adding (%f, %f)\n", x_min, y_min);
      x_h[*n_h] = x_min;
      y_h[*n_h] = y_min;
      *n_h = *n_h + 1;
    }
    //printf("n_h = %d\n", *n_h);
    return;
  }
 
  //printf("Temp1\n");
  //printf("ind = %d\n", ind);
  quickhull(n, x, y,  x[ind], y[ind], x_max, y_max, -findSide(x[ind], y[ind], x_max, y_max, x_min, y_min), n_h, x_h, y_h);
  quickhull(n, x, y,  x[ind], y[ind], x_min, y_min, -findSide(x[ind], y[ind], x_min, y_min, x_max, y_max), n_h, x_h, y_h);   
}


/*
  A Quick Hull algorithm to get the outer points of a set
 */
void quickhull_pre(int n, float* x, float* y, int* n_h, float* x_h, float* y_h){
  int max_x = 0;
  int min_x = 0;
  for(int i =0; i < n; i++){
    if(x[max_x] < x[i]){
      max_x = i;
    }
    if(x[min_x] > x[i]){
      min_x = i;
    }
  }
  quickhull(n, x, y, x[min_x], y[min_x], x[max_x], y[max_x], 1, n_h, x_h, y_h);

  printf("Next\n");
  quickhull(n, x, y, x[min_x], y[min_x], x[max_x], y[max_x], -1, n_h, x_h, y_h);
}

int main(){
  int n = 1000;
  float x[n];
  float y[n];

  int upper = 100;
  int lower = -100;

  
  
  for(int i = 0; i < n; i++){
    x[i] = (1.0*(rand() % (upper - lower + 1)) + lower)/ (upper - lower);
    y[i] = (1.0*(rand() % (upper - lower + 1)) + lower)/ (upper - lower);

    printf("x[%d] = %f\n", i, x[i]);
    printf("y[%d] = %f\n\n", i, y[i]);
    
  }

  int n_h = 0;
  float x_h[n];
  float y_h[n];

  quickhull_pre(n, x, y , &n_h, x_h, y_h);

  printf("n_h = %d \n", n_h);
  
  for(int i = 0; i < n_h; i++){
    printf("x_h[%d] = %f \t",i, x_h[i]);
    printf("y_h[%d] = %f\n",i, y_h[i]);
  }
  
}
