# Square Sum Problem
Solve the famous “square sum” problem with an elegant searching method.

## Problem Description
Find a permutation of sequence _[1...N]_, where every two adjacent numbers sum to a square number. 

For example, when _N=15_, we have: 
```
[9, 7, 2, 14, 11, 5, 4, 12, 13, 3, 6, 10, 15, 1, 8]
```

I have tried backtracking algorithm, but it will always stuck at searching 100+, 
even with heuristic methods and some technics like "branch and cut". 

So I design a new approach: 

## Algorithm
We define an array as "valid", when every two adjacent numbers of the array sum to a square number. 

We always maintain two valid arrays named _A_ and _B_, where _concat(A, B)_ is a permutation of sequence _[1...N]_.
This serves as our invariant in all iterations. 

Supposed we have find a solution array to sequence _[1...(N-1)]_. 
By definition, this array is valid, so we assign it as inital value of _A_. 
This solution can be found with brute-force algorithm when _N_ is small enough. 

Then we assign singleton _[N]_ as inital value of _B_, to satisfy the invariant. 

Next, here comes the loop: 

Obviously, we can do zero or more operations as follows. 
```
1. swap A and B or not
2. reverse A or not
3. reverse B or not
```

and won't break the invariant. After a random combination of that, we
```
4. split A into C, D,  where the last number of C and the first number of B sum to a square number. 
```
Thus, let _A' = concat(C, B), B' = D_, we got a new pair of valid arrays. After this operation, we keep the invariant. 

Finally, check if any endpoint of _A_ and any endpoint of _B_ can sum to a square number. 
If so, concat them accordingly, and return the result. 
if not, go on for the next iteration. 

At step 4, chances are there is no valid move, e.g., _A_ is too small. 
In that case, we can do another combination of step 1/2/3. 
In most situations, there are many valid moves at step 4, and we just pick one randomly. 

After several iterations, we can always grow the valid permutation of _[1...(N-1)]_ into a valid permutation of _[1...N]_. 

Applying this algorithm repeatedly, we can get the solution to an arbitarily large _N_. 

Though in theory termination of the algorithm is not guaranteed, in practice it is surprisingly fast. 
Benchmark results show that we can grow _N_ into 40000 in seconds, way faster than backtracing algorithms. 
