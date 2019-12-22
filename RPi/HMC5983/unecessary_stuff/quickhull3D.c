#include "../Lib/commonLib.h"

#define PI 3.141592654

/*
  The 3D Quick Hull Algorithm Example. Hopefully this could be used 
  for magnetometer calibration. 
 */


struct Point{
  float x, y, z;
};

/*
  The euclidean norm of a vector
 */
float euclNorm(struct Point p){
  return sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(p.z,2));
}

float dotProd(struct Point p1, struct Point p2){
  return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

/*
  The cross product with eculidean norm
 */
float lineDist(struct Point p1, struct Point p2){
  float term1 = p1.y * p2.z - p1.z * p2.y;
  float term2 = p1.z * p2.x - p1.x * p2.z;
  float term3 = p1.x * p2.y - p1.y * p2.x;

  struct Point p = {term1, term2, term3};
  
  return euclNorm(p);
}



float planeDist(struct Point p1,struct Point p2,struct Point p3, struct Point p4){
  struct Point p21 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
  struct Point p31 = {p3.x - p1.x, p3.y - p1.y, p3.z - p1.z};

  
  float term1 = p21.y * p31.z - p21.z * p31.y;
  float term2 = p21.z * p31.x - p21.x * p31.z;
  float term3 = p21.x * p31.y - p21.y * p31.x;
  
  struct Point pn = {term1, term2, term3};

  float norm = euclNorm(pn);
  
  pn.x = pn.x/norm;
  pn.y = pn.y/norm;
  pn.z = pn.z/norm;

  struct Point pi = {p4.x - p1.x, p4.y - p1.y, p4.z - p1.z};

  return dotProd(pn, pi);
}


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

void quickhull(int n,float* x, float* y, float x_max, float y_max, float x_min, float y_min, int side, int* n_h, float* x_h, float* y_h){
  int ind = -1;
  float max_dist = 0;
  float temp;
  for(int i = 0; i < n;i++){
    //printf("x[%d] = %f\n", i, x[i]);
    //float temp = lineDist(x_max, y_max, x_min, y_min, x[i], y[i]);
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
void quickhull_pre(int n, struct Point* p,int* n_h, float* x_h, float* y_h){
  int max_x = 0;
  int min_x = 0;
  for(int i =0; i < n; i++){
    if(p[max_x].x < p[i].x){
      max_x = i;
    }
    if(p[min_x].x > p[i].x){
      min_x = i;
    }
  }

  printf("The largest x is (%f,%f,%f)\n", p[max_x].x, p[max_x].y, p[max_x].z);
  printf("The smallest x is (%f,%f,%f)\n", p[min_x].x, p[min_x].y, p[min_x].z);
  
  int ind = -1;
  float max_dist = 0;
  float temp;
  float x1 = p[max_x].x - p[min_x].x;
  float y1 = p[max_x].y - p[min_x].y;
  float z1 = p[max_x].z - p[min_x].z;
  struct Point p1 = {x1, y1, z1};
  float norm1 = euclNorm(p1);
  float x2, y2, z2;
  for(int j = 0; j < n; j++){
    //temp = lineDist(x[max_x], y[max_x], x[min_x], y[min_x], x[j], y[j]);
    x2  = p[min_x].x - p[j].x;
    y2 = p[min_x].y - p[j].y;
    z2 = p[min_x].z - p[j].z;
    struct Point p2 = {x2, y2, z2};
    temp = lineDist(p1, p2)/ norm1;
    if(temp > max_dist){
      ind = j;
      max_dist = temp;
    }
  }

  printf("Maximum distance from line: %f\n", max_dist);
  printf("Farthest point from line: (%f, %f, %f)\n", p[ind].x, p[ind].y, p[ind].z);

  int ind2 = 0;
  max_dist = 0;
  for (int j= 0; j < n; j++){
    float pD = planeDist(p[max_x], p[min_x], p[ind], p[j]);
    if (max_dist < pD){
      ind2 = j;
      max_dist = pD;
    }
  }

  printf("Maximum distance from plane: %f\n", max_dist);
  printf("Farthest point from plane: (%f, %f, %f)\n", p[ind2].x, p[ind2].y, p[ind2].z);
  
  //quickhull(n, x, y, x[min_x], y[min_x], x[max_x], y[max_x], 1, n_h, x_h, y_h);
  //quickhull(n, x, y, x[min_x], y[min_x], x[max_x], y[max_x], -1, n_h, x_h, y_h);
}

int main(){
  int n = 10000;
  float x[n];
  float y[n];
  float z[n];

  struct Point p[n];
  
  float radius = 1.0;
  time_t t;
  srand((unsigned) time(&t));
  
  for(int i = 0; i < n; i++){
    float r = (float)rand()/(float)(RAND_MAX/radius);
    float psi = (float) rand()/ (float)(RAND_MAX/(2*PI));
    float theta = (float) rand()/ (float)(RAND_MAX/(PI));

    p[i].x = r * sin(theta) * cos(psi);
    p[i].y = r * sin(theta) * sin(psi);
    p[i].z = r * cos(theta);
      }

  int n_h = 0;
  float x_h[n];
  float y_h[n];
  float z_h[n];

  quickhull_pre(n, p, &n_h, x_h, y_h);

  printf("n_h = %d \n", n_h);
  
  for(int i = 0; i < n_h; i++){
    printf("x_h[%d] = %f \t",i, x_h[i]);
    printf("y_h[%d] = %f\n",i, y_h[i]);
  }
  
}
