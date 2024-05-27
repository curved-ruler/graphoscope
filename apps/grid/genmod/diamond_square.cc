#include "diamond_square.hh"

#include <iostream>


namespace ter {

void ds_square_grid (float* hmap, int partsize, int pw, int ph)
{
    cr::fdice d1(120, -1.0f, 1.0f);
    
    for (int j=0 ; j<ph ; ++j)
    {
        for (int i=0 ; i<pw ; ++i)
        {
            //std::cout << i << "--" << j << std::endl;
            ds_square(hmap, partsize*pw+1, partsize*ph+1, partsize, i*partsize, j*partsize, d1);
        }
    }
}

void ds_square (float* hmap, int hw, int hh,
                int dsw, int ulx, int uly,
                cr::fdice& dice)
{
    int i = dsw;
    
    while (i >= 2)
    //while (i >= 4)
    {
        int ip2 = i/2;
        
        // diamond
        for (int y=uly ; y+i <= uly+dsw ; y+=i)
        {
            //if (y+i >= hh) { std::cerr << "DS: Index out of bounds in Y" << std::endl; break; }
            
            for (int x=ulx ; x+i <= ulx+dsw ; x+=i)
            {
                //if (x+i >= hh) { std::cerr << "DS: Index out of bounds in X" << std::endl; break; }
                
                float a = hmap[y*hw + x];
                float b = hmap[(y+i)*hw + x];
                float c = hmap[y*hw + (x+i)];
                float d = hmap[(y+i)*hw + (x+i)];
                float r = dice.next() * i * 0.3f;
                //float r = 0.0f;
                //hmap[(y+ip2) * hw + (x+ip2)] = (a + b + c + d) / 4.0f + r;
                //hmap[(y+ip2) * hw + (x+ip2)] = (a+b)/2.0f + r;
                float rnd = dice.next();
                float hhh = rnd < -0.5f ? a :
                            rnd <  0.0f ? b :
                            rnd <  0.5f ? c : d;
                hmap[(y+ip2) * hw + (x+ip2)] = hhh + r;
            }
        }
        //std::cout << "D" << std::endl;
        
        // square
        int alt = 0;
        for (int y = uly+ip2 ; y <= uly+dsw ; y+=ip2)
        {
            if (y == hh-1) continue;
            
            for (int x=(alt % 2 == 1) ? ulx+ip2 : ulx+i ; x <= ulx+dsw ; x+=i)
            {
                if (x == hw-1) continue;
                
                float p = 4.0f;
                
                float a = 0.0f;
                if (x-ip2 >= 0)
                {
                    a = hmap[y*hw + (x-ip2)];
                }
                else { p -= 1.0f; }
                
                float b = 0.0f;
                if (x+ip2 <= ulx+dsw)
                {
                    b = hmap[y*hw + (x+ip2)];
                }
                else if (x+ip2 > hw-1)
                {
                    b = hmap[y*hw + (ip2)];
                }
                else { p -= 1.0f; }
                
                float c = 0.0f;
                if (y-ip2 >= 0)
                {
                    c = hmap[(y-ip2)*hw + x];
                }
                else { p -= 1.0f; }
                
                float d = 0.0f;
                if (y+ip2 < uly+dsw)
                {
                    d = hmap[(y+ip2)*hw + x];
                }
                else if (y+ip2 > hh-1)
                {
                    d = hmap[(ip2)*hw + x];
                }
                else { p -= 1.0f; }
                
                float r = dice.next() * ip2 * 0.3f;
                //float r = 0.0f;
                
                //hmap[y*hw + x] = (a + b + c + d) / p + r;
                //hmap[y*hw + x] = (a+b)/2.0f + r;
                float rnd = dice.next();
                float hhh = rnd < -0.5f ? a :
                            rnd <  0.0f ? b :
                            rnd <  0.5f ? c : d;
                hmap[y*hw + x] = hhh + r;
            }
            ++alt;
        }
        //std::cout << "S" << std::endl;
        
        // step
        i = ip2;
    }
}

}
