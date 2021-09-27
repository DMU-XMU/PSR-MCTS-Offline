#include <stdlib.h>
#include <fstream>
#include "symbol.h"
#include "environment.h"
//#include "psr.h"
#include "psr-pca.h"
#include "psr-sysdyn.h"
#include "psr-local.h"
#include "hand-pomdp.h"
#include "sampler.h"

int main(int argc, char *argv[]) {

  Environment*  env = new Environment();

  //int mothod;
  cout<<"POSadmin POMDP model"<<endl;

      psr_local *LASR=new psr_local(env);
      LASR->learn();
      delete LASR;

  //cin>>mothod;cout<<"mothod input="<<mothod<<endl;
  /*
  switch(mothod)
  {
    case 1:
    {
      psr_sysdyn *PSR = new psr_sysdyn(env);
      PSR->learn( );  
      delete PSR;
      break;
    }
    case 2:
    {
      psr_pca *PCA = new psr_pca(env); 
      PCA->learn( );    
      delete PCA;
      break;  
    }
    case 3:
    {
      psr_local *LASR=new psr_local(env);
      LASR->learn();
      delete LASR;
      break;
    }
    case 4:
    {
      hand_pomdp *HAND=new hand_pomdp(env);
      HAND->learn();
      delete HAND;
      break;
    }
  }


  delete env;
*/

  delete env;

  return 0;
}
