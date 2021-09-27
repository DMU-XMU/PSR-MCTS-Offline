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
  cout<<"tiger POMDP model"<<endl;

      psr_local *LASR=new psr_local(env);
      LASR->learn();
      delete LASR;

  delete env;

  return 0;
}
