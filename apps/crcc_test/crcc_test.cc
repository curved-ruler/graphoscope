
#include <iostream>

#include "gu_test.hh"
#include "integer_test.hh"
#include "mu_test.hh"
#include "text_test.hh"
#include "szcript_test.hh"

using namespace cr;
 
int main ()
{
    //bool verbose = true;
    
    std::cout << "---- gu\n";
    int succ=0, sum=0;
    run_gu_tests(sum, succ);
    std::cout << "RES:  " << succ << "/" << sum << std::endl;
    
    std::cout << "---- mu\n";
    succ=0; sum=0;
    run_mu_tests(sum, succ);
    std::cout << "RES:  " << succ << "/" << sum << std::endl;
    
    // col
    std::cout << "---- col\n";
    vec3 rgb(0.42, 0.98, 0.89);
    vec3 hsv  = rgb2hsv(rgb); // should be (0.4733, 0.572, 0.98)
    vec3 rgb2 = hsv2rgb(hsv);
    std::cout << "hsv (" << hsv.x  << ", " << hsv.y  << ", " << hsv.z  << ")" << std::endl;
    std::cout << "rgb2(" << rgb2.x << ", " << rgb2.y << ", " << rgb2.z << ")" << std::endl;
    
    //succ=0; sum=0;
    //run_text_tests(sum, succ);
    //std::cout << "Tests: text: " << succ << "/" << sum << std::endl;
    
    std::cout << "---- szcript\n";
    succ=0; sum=0;
    run_szcript_tests(sum, succ);
    //std::cout << "RES: " << succ << "/" << sum << std::endl;
    
    succ=0; sum=0;
    std::cout << "\n--- integer --------------------\n";
    run_integer_tests();
    
    return 1;
}
