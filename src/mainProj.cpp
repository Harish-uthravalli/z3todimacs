#include<iostream>
#include<map>
#include<iterator>
#include"z3++.h"
#include "convert.cpp"
#include "final_processing.cpp"
using namespace z3;
using namespace std;
map<unsigned int,int> mapping;
int var_count = 0;
int clause_count = 0;



int fun(z3::expr exp)
{   static int number=1;
    int id[50];
    if(mapping.find(exp.id())!= mapping.end())
    {
        std::cout<<exp<<":Found in map Value is: "<<exp.id()<<" with value: "<<mapping[exp.id()]<<endl;

        return mapping[exp.id()];
    }    
    else 
    {   
        mapping.insert(pair<unsigned int,int>(exp.id(),++number));
        int num= number;
        std::cout<<exp<<":Not Found in map.\n Key inserted for it: "<<exp.id()<<". Value given to ELEMENT in MAP: "<<number<<endl;
        symbol op = exp.decl().name();
        if(exp.is_const() && (exp.decl().decl_kind()==Z3_OP_UNINTERPRETED))
        {  
          std::cout<<exp<<": This expr is a constant. Returning ELEMENT value: "<<num<<endl<<endl; 
            
            
          return(num);
        }
        else
        { std::cout<<exp<<": This expression is not constant \n"<<endl;
          for(int i =0;i<exp.num_args();i++)
          {
              id[i] = fun(exp.arg(i));
              std::cout<<"value of ID["<<i<<"] is: "<<id[i]<<" .For the operator: "<<op.str()<<endl<<endl;
             
              if(exp.num_args() == 2)
                clause_count+=3;
              else
                clause_count+=2;       
          }
          var_count = mapping.size();
          
          Conversion::dimacs(num,op.str(),exp.num_args(),id,var_count);
          
          return num;
          
          
        }   
   }
    
  
}
int main()
{   map<unsigned int,int>::iterator itr;
    context c;
    expr x = c.bool_const("x");
    expr y = c.bool_const("y");
    expr z = c.bool_const("z");

    expr e = (( x || z ) && ( y || z ));
   fun(e);

   process(Conversion::var,Conversion::clause_count,"harish.qdimacs");
    
    cout << "\tKEY\tELEMENT\n"; 
    for (itr = mapping.begin(); itr != mapping.end(); ++itr) { 
        cout << '\t' << itr->first 
             << '\t' << itr->second << '\n';
}

}
