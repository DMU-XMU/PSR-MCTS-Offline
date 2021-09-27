# PSR-MCTS-Offline
-----------------------------
# Related Algorithms
-----------------------------
 
  1. BAPOMCP 
  2. BAPOMCP-T 
  3. BAPOMCP-R 
  4. POMDP
  5. PSR-MCTS
  6. PSR-MCTS-Online
  7. PSR-MCTS-RO
  8. PSR-MCTS-Test-Reduce

-----------------------------
# Environments
-----------------------------

 1. Tiger
 2. RockSample
 3. POSyadmin
 
-----------------------------
# USAGE
-----------------------------
1.Find the corresponding environment.

2.Execut.

  $ mkdir out
  
  $ mkdir build
  
  $ cd build
  
  $ cmake ..(may need to be executed twice)
  
  $ make 
  
Some combination of parameters might fail without warning, you have to know what you are doing...

3.Please run the following command.

$./brl --ENV 11 --S 2 --A 3 --BAPOMCP --K 100 --T 20 --N 10000

K is the number of simulators.

N is the number of runs.

=> Check main.cpp for all parameter names and configurations.


There is a lot of useless code in the source code, too late to change due to time constraints.
Only this algorithm and environment assurance can run.
