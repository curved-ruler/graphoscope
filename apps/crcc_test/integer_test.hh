
#pragma once

#include <iostream>

#include <crcc/crcc.hh>

void show (const std::string& s)
{
    int j=1;
    for (size_t i=0 ; i<s.size() ; ++i)
    {
        std::cout << s[i];
        if (j % 4 == 0) { std::cout << ' '; }
        ++j;
    }
}

void intdump(const std::string& s)
{
    cr::integer ii(s);
    
    std::cout << s << ": ";
    show(ii.str_dump());
    std::cout << std::endl;
}

void addtest (const std::string& as, const std::string& bs)
{
    cr::integer a(as);
    cr::integer b(bs);
    cr::integer c = a+b;
    std::cout << as << " + " << bs << ":  ";
    show(c.str_dump());
    std::cout << "  |  " << c.str(10);
    std::cout << std::endl;
}
void subtest (const std::string& as, const std::string& bs)
{
    cr::integer a(as);
    cr::integer b(bs);
    cr::integer c = a-b;
    std::cout << as << " - " << bs << ":  ";
    show(c.str_dump());
    std::cout << "  |  " << c.str(10);
    std::cout << std::endl;
}
void multest (const std::string& as, const std::string& bs)
{
    cr::integer a(as);
    cr::integer b(bs);
    cr::integer c = a*b;
    std::cout << as << " * " << bs << ":  ";
    show(c.str_dump());
    std::cout << "  |  " << c.str(10);
    std::cout << std::endl;
}
void divmodtest (const std::string& as, const std::string& bs)
{
    cr::integer a(as);
    cr::integer b(bs);
    cr::integer d;
    cr::integer m;
    cr::div_and_mod(a, b, d, m);
    std::cout << "divmod: " << as << " = " << bs << " x     " << d.str() << " + " << m.str() << std::endl;
    std::cout << std::endl;
}

void pptest (const std::string& s)
{
    cr::integer i(s);
    ++i;
    std::cout << "++" << s << ":  ";
    show(i.str_dump());
    std::cout << "  |  " << i.str(10);
    std::cout << std::endl;
}

void mmtest (const std::string& s)
{
    cr::integer i(s);
    --i;
    std::cout << "--" << s << ":  ";
    show(i.str_dump());
    std::cout << "  |  " << i.str(10);
    std::cout << std::endl;
}

int run_integer_tests ()
{
    /*
    intdump("");
    intdump("-");
    intdump("@");
    intdump("-@");
    intdump("0");
    intdump("-0");
    intdump(" 000000000000000000000000000");
    intdump("-000000000000000000000000000");
    intdump("123");
    intdump("4294967296");
    intdump("-4294967296");
    intdump("15@16");
    intdump("16@16");
    intdump("FF@16");
    
    int8 i8 = -4;
    cr::integer ii(i8);
    std::cout << "int8(-4)" << " - " << ii.str_dump() << "x" << std::endl;
    int32 i32 = -4;
    cr::integer ii2(i32);
    std::cout << "int32(-4)" << " - " << ii2.str_dump() << "x" << std::endl;
    
    int32 im = -10000;
    cr::integer ii3(im);
    std::cout << "int32(-10000)" << " - " << ii3.str_dump() << "x" << std::endl;
    
    cr::integer iix("1");
    int ix = 2000;
    iix = ix;
    std::cout << "int32(2000)" << " - " << iix.str_dump() << std::endl;
    
    addtest("1", "1");
    addtest("1", "255");
    addtest("10000", "20255");
    subtest("1", "1");
    subtest("1", "2");
    subtest("1", "255");
    subtest("1255", "255");
    pptest("127");     mmtest("127");
    pptest("0");       mmtest("0");
    */
    
    multest("255", "255");    // 65025
    multest("1010", "23456"); // 23690560
    multest("1010", "-23456");
    multest("-1010", "-23456");
    
    divmodtest("2000", "33");
    divmodtest("256",  "256");
    divmodtest("12345678",  "1");
    divmodtest("12345678",  "1357");
    
    cr::integer i001("123");
    std::cout << i001.str() << std::endl;
    
    cr::integer i002("21123");
    std::cout << i002.str_dump() << " | MSB: " << i002.msb() << std::endl;
    cr::integer i003;
    std::cout << i003.str_dump() << " | MSB: " << i003.msb() << std::endl;
    i002 <<= 1;
    std::cout << "21123 << 1 = " << i002.str() << std::endl;
    
    return 1;
}
