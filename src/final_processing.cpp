#include<iostream>
#include <curses.h>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<ncurses.h>
#include<string>
void process(int cvar, int cclause, string fnm)
{	
	ifstream fs;
	ofstream ft;

	fs.open("contents.txt");
	if(!fs)
	{
		cout<<"Error in opening source file..!!";
	}
	ft.open(fnm,ios::app);
	if(!ft)
	{
		cout<<"Error in opening target file..!!";
		fs.close();
	}
    
char str[255];
ft<<"p cnf "<<cvar<<" "<<cclause<<endl;
while(fs.getline(str,255))
{
    ft<<str<<endl;
}

    // now we loop back and get the next line in 'str'

	cout<<"\nFile copied successfully..!!"<<endl<<endl;
	fs.close();
	ft.close();
	getch();
}
