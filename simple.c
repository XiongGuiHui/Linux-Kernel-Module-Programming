#include<stdio.h>

struct line{
  int start;
  int end;
};


int main(){
  struct line a = {};
  a.start = 12;
  a.end = 23;
  
  return 0;
}
