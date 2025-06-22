#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char* argv[])
{
    //Read commandline argument
    string ifname{argv[argc-1]};
    cout << "The input file name is: " << ifname << endl;

    cout << endl;

    //Open input file
    ifstream ifs{ifname};
    cout << "Opening file succesful: " << ifs.is_open() << endl;

    cout << endl;

    //Open output file
    //-get output filename from ifname
    string ofname = ifname.substr(0,ifname.length()-3);
    cout << "The output file name is: " << ofname << endl;
    ofstream ofs{ofname};


    string input_holder{};
    //Process first line:               input line: n m 10  ->  output line: 'p' "tw" n m
    /*
            -write 'p' to output
            -write "tw" to output
            -write n to output
            -write m to ouput
    */
    getline(ifs,input_holder);
    input_holder = input_holder.substr(0,input_holder.find_last_of(' '));
    string s{"p tw "};
    input_holder = s + input_holder;
    cout << input_holder << endl;
    ofs << input_holder << endl;

    //Process remaining lines:          input line: w_(v) N(v)  ->
    int vertexNumber = 1;
    while(getline(ifs,input_holder))
    {
        //Disregard vertex weight
        input_holder = input_holder.substr(input_holder.find_first_of(' ')+1,input_holder.length());
        //cout << vertexNumber << ": " << input_holder << endl;
        //ofs << input_holder << endl;

       stringstream ss{input_holder};
       int t;
       while(ss >> t)
       {
           if(vertexNumber<t)
           {
                //cout << vertexNumber << ' ' << t << endl;
                ofs << vertexNumber << ' ' << t << endl;
           }
       }

        vertexNumber++;
    }
    ifs.close();
    ofs.close();

    return 0;
}
