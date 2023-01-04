
#include <iostream>

#include "gu_test.hh"
#include "integer_test.hh"
#include "mu_test.hh"
#include "text_test.hh"
 
int main ()
{
    bool verbose = true;
    
    int succ=0, sum=0;
    run_gu_tests(sum, succ);
    std::cout << "Tests: gu: " << succ << "/" << sum << std::endl;
    
    succ=0; sum=0;
    run_mu_tests(sum, succ);
    std::cout << "Tests: mu: " << succ << "/" << sum << std::endl;
    
    //succ=0; sum=0;
    //run_text_tests(a,b);
    //std::cout << "Tests: text: " << succ << "/" << sum << std::endl;
    
    succ=0; sum=0;
    std::cout << "integer --------------------" << std::endl;
    run_integer_tests();
    
    return 1;
}