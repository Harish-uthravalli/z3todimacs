#include<iostream>
#include "z3++.h"
#include<string>
#include<fstream>
using namespace z3;
using namespace std;

class Conversion
{
public:
    static int clause_count;
    static int var;

    static void dimacs(int t, string oprt, int arg_num, int element[], int variables)
    {
        var = variables;
        std::cout << "Value of T is: " << t << "\n operator is: " << oprt << " \nNumber of arguments: " << arg_num << endl;
        if (arg_num == 2)
            std::cout << element[0] << " " << element[1] << endl;
        else
            std::cout << element[0] << endl;

        ofstream myfile;
        myfile.open("contents.txt", ios::app);
        myfile.clear();
        if (arg_num == 2)
        {
            clause_count += 3;
            if (oprt.compare("and") == 0)
            {
                myfile << "-" << t << " " << element[0] << " 0 \n";
                myfile << "-" << t << " " << element[1] << " 0 \n";
                myfile << "-" << element[0] << " -" << element[1] << " " << t << " 0 \n";
                myfile.close();
                std::cout << "printing done for AND.\n";
            }
            else if (oprt.compare("or") == 0)
            {
                myfile << t << " -" << element[0] << " 0 \n";
                myfile << t << " -" << element[1] << " 0 \n";
                myfile << "-" << t << " " << element[0] << " " << element[1] << " 0 \n";
                myfile.close();
                std::cout << "printing done for OR.\n";
            }
            else if (oprt.compare("=>") == 0)
            {
                myfile << "-" << t << " -" << element[0] << " 0 \n";
                myfile << "-" << t << " " << element[1] << " 0\n";
                myfile << "-" << element[1] << " -" << t << " " << element[0] << " 0 \n";
                myfile.close();
                std::cout << "printing done for implies.\n";
            }

        }
        else if (arg_num == 1)
        {
            clause_count += 2;
            if (oprt.compare("not") == 0)
            {
                myfile << "-" << t << " -" << element[0] << " 0\n";
                myfile << t << " " << element[0] << " 0 \n";
                myfile.close();
                std::cout << "printing done for NOT.\n";
            }
        }
    }
};
int Conversion::clause_count = 0;
int Conversion::var =0;

