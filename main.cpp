/*
Julian Kandlhofer, 2BHEL, 2017

Parameter: <exe>.exe "Path To Inputfile.TXT" "Path to Outputfile.DRL"
Oder nach dem Start eingeben.

Zukunftsplan:

Toolliste um dieses Programm auch mit anderen CNC-Maschinen verwenden zu können.

 !!!! ACHTUNG !!!!

 Dieses Programm benötigt stof() aus C++ 11!

*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>

using namespace std;
typedef struct
{
    string name;
    float diameter;
}Tool;

bool getToolset(vector<Tool> &tools);
int getTool (string line,vector<Tool>tools,float &indiam);


bool getToolset(vector<Tool> &tools)        //Loads the availible tools from "cnc.tools"
{
    tools.resize(0);
    int i= 0;
    string bufname;
    float bufdiam;
    bool ret;

    ifstream  file("cnc.tools");

    if(file.is_open())
    {
        do
        {
            tools.resize(i+1);

            file>>tools.at(i).name>>tools.at(i).diameter;

            i++;
        }
        while(!file.eof());
        ret = true;

        file.close();

    }
    else
    {
        cout<<"Fehler beim laden der Tools!\n";
        ret = false;
    }
    return ret;
}

int getTool (string line,vector<Tool>tools,float &indiam)       //Liefert den index des passenden Bohrers
{
    int i = line.length()-1,j = 0;
    string sval;
    float fval;
    char buf;
    bool loop = true;

    do      //Die letzten Zeichen eines Strings werden eingelesen
    {
        buf = line.at(i);
        if (((buf > 48) && (buf < 58))||(buf == '.'))
        {
            sval = buf + sval;  //Zeichen werden vorne angehängt
        }

        if(buf == 'C')
            loop = false;

        i--;
    }
    while (loop);



    fval = stof(sval); //ACHTUNG!! C++ 11 ONLY!  Der String wird zur Zahl konvertiert
    indiam = fval;
    fval = roundf(fval * 10) / 10; //Auf die 1. Dezimalstelle runden

    j = 0;
    loop = true;

    while(loop)
    {
        if((j>=tools.size())||(tools.at(j).diameter>=fval))
        {
            loop= false;
        }
        else
        {
            j++;
        }
    }

    return j;
}

int main(const int argc, const char** argv)
{
    string infile, outfile, buf;
    vector<Tool> toolset;
    vector<int> usedtools;
    float indiam;
    int i = 0,j,toolindex;
    bool loop = true;
    Tool buftool;

    cout<<"CNC-Tool-Swapper, Julian Kandlhofer 2BHEL, 2017\n\n";

    if (!getToolset(toolset))
        return 0;

    cout<<"Es wurden "<<toolset.size()<<" Tools erkannt.\n";

    if(argc == 3)
    {
        cout<<"Parameter wurden entdeckt, Pfade werden \x81"<<"bernommen.\n";
        infile = argv[1];
        outfile = argv[2];
    }
    else
    {
        cout<<"Achtung: Gro\xE1-/Kleinschreibung beachten!\n";
        cout<<"Bitte Pfad der Quelle + Dateinamen + Endung eingeben:\n";
        getline(cin,infile);
        cout<<"Bitte Pfad des Zieles + Dateinamen + Endung eingeben:\n";
        getline(cin,outfile);
    }

    ifstream ifile(infile);
    ofstream ofile(outfile);


    if(ifile.is_open()&& ofile.is_open())
    {
        cout<< "\nDateien erfolgreich ge\x94"<<"ffnet.\n";
        getline(ifile, buf);
        ofile<<buf<<endl;     // 1. Zeile Übernehmen

        for(j = 0; j< 4; j++) //4 Zeilen Überspringen
            getline(ifile,buf);

        cout<<"Tools werden erkannt...\n";
        i= 0;
        do
        {
            getline(ifile, buf);
            if(buf.size() > 1)
            {
                toolindex = getTool(buf,toolset,indiam);
                buftool = toolset.at(toolindex);
                cout<<fixed<<setprecision(2)<<"T"<<i+1<<" "<<indiam<<"mm ==> "<<buftool.name<<", durchm.: "<<buftool.diameter<<"mm"<<endl;

                ofile <<buftool.name<<"F00S00C"<<fixed<<setprecision(2)<<float(buftool.diameter)<<endl;
                usedtools.resize(usedtools.size()+1);
                usedtools.at(i)= toolindex;
                i++;
            }
            else
            {
                loop = false;
            }
        }
        while(loop);

        cout<<"Toolerkennung erfolgreich!\n";

        ofile<<'%'<<endl;

        i= 0;

        do
        {

            getline(ifile,buf);
            if(buf.length() > 0)
            {

            if(buf.at(0)=='T')
            {
                ofile<<toolset.at(usedtools.at(i)).name<<endl;
                i++;
            }
            else
            {
                ofile<<buf<<endl;
            }
            }
        }
        while(!ifile.eof());

        cout<<"Toolswap abgeschlossen, die Zieldatei befindet sich in\n\""<<outfile<<"\"\n";

        ifile.close();
        ofile.close();
    }
    else
    {
        cout<<"Fehler beim \x99"<<"ffnen der Dateien";
    }

    return 0;
}
