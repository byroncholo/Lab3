
/*
This program solves the problem of the Tower of Hanoi
using an recursive apprach and an iterative apprach

Input:
Output:

Name: Byron Cholo Zambonino
Date: 3/1/2026
*/

// Libraries
#include <iostream>
#include <vector>
#include <stack>
#include <chrono>
#include <cmath>


using namespace std;
using namespace std::chrono;

//  towers_of_hanoi_recursive with printing
void towers_of_hanoi_recursive(int n, char source, char target, char auxiliary){
  if (n==0) return;
  if (n==1){
    cout << "Move disk 1 from " << source << " to " << target << endl;
    return;
  }
    towers_of_hanoi_recursive(n-1, source, auxiliary, target);
    cout << "Move disk " << n << " from " << source << " to " << target << endl;
    towers_of_hanoi_recursive(n-1, auxiliary, target, source);
}

//  towers_of_hanoi_recursive without printing



// with printing
void towers_of_hanoi_iterative(int n, char source, char target, char auxiliary){
  stack<tuple<int, char, char, char, int>> st;
  st.push({n, source, target, auxiliary, 0});

  while (!st.empty()){
    auto [k, s, t, a, stage] = st.top();
    st.pop();

    if (k==0) continue;
    
    if (k == 1){
      cout << "Move disk 1 from " << s << " to " << t << endl;
      continue;
    }

    if (stage == 0){
      // Simulating step
      // call hanoi(k-1, source, auxiliary, target)
      // move k: source->target
      // hanoi(k-1, auxiliary, target, source)
      
      // push in reverse order (stack LIFO)
      st.push({k, source, target, auxiliary, 1}); // after first recursive call, do middle + second
      st.push({k-1, source, auxiliary, target, 0}); // first recursive call 
    }
    
    else{
      // middle move
      cout << "Move disk " << k << " from " << source << " to " << target << endl;
      // second recursive call
      st.push({k-1, auxiliary, target, source, 0});
    }
  }
}

//  towers_of_hanoi_iterative without printing









int main(){
  int n = 3;
  char source = 'A';
  char target = 'C';
  char auxiliary = 'B';


  // Recursive algorithm
  auto start = high_resolution_clock::now();
  towers_of_hanoi_recursive(n, source, target, auxiliary);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by recursive algorithm: " << duration.count() << " microseconds" << endl;
  
  // Iterative algorithm
  start = high_resolution_clock::now();
  towers_of_hanoi_iterative(n, source, target, auxiliary);
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by iterative algorithm: " << duration.count() << " microseconds" << endl;


  return 0;
}