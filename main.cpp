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

int getToolNum (string line,float &drillsize);

int getToolNum (string line,float &drillsize)       //Liefert die Nummer bzw die groesse des Bohrers
{
    int i = line.length()-1,ival;
    string sval;
    float fval;
    char buf;
    bool loop = true;

    drillsize = 0;

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
    fval = roundf(fval * 10) / 10; //Auf die 1. Dezimalstelle runden

    if(fval > 1.5)      //Die Sonderfälle treten erst ab 1.6 mm Durchmesser auf
    {
        if(fval == 1.6)
        {
            fval = 1.8;
        }
        else if (fval == 2.0)
            {
                fval = 2.5;
            }

        if(fval == 1.8) //Die Formel von unten gilt hier nicht mehr da die Bohrer nicht mehr in einem Abstand von 0.1 mm sind
            ival = 13;
        else if (fval == 2.5)
            ival = 14;
        else if (fval == 3.0)
            ival = 15;
    }
    else        //Bei einer Abstufung von 0.1 mm zwischen den Bohrern gilt meißt diese Formel
    {
        ival = (fval-0.3) * 10;
    }

    drillsize = fval; //Zurückliefern des Bohrdurchmessers
    return ival;      //Zurückliefern der Bohrernummer
}

int main(const int argc, const char** argv)
{
    string infile, outfile, buf;
    int i = 0,j;
    float drillsize;
    bool loop = true;
    vector<int> tools;

    cout<<"CNC-Tool-Swapper, Julian Kandlhofer 2BHEL, 2017\n\n";

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

        for(j = 0; j< 4; j++)
            getline(ifile,buf);

        cout<<"Tools werden erkannt...\n";
        i= 0;
        do
        {
            getline(ifile, buf);
            if(buf.size() > 1)
            {
                tools.resize(i+2);

                tools.at(i) = getToolNum(buf,drillsize);
                cout<<"T"<<i+1<<" ==> T"<<tools.at(i)<<", durchm.: "<<fixed<<setprecision(2)<<drillsize<<"mm"<<endl;

                ofile <<'T'<<tools.at(i)<<"F00S00C"<<fixed<<setprecision(2)<<float(drillsize)<<endl;
                i++;
            }
            else
            {
                loop = false;
            }
        }
        while(loop);

        cout<<"Toolerkennung erfolgreich!\n";
        i= 0;

        do
        {

            getline(ifile,buf);
            if(buf.length() > 0)
            {

            if(buf.at(0)=='T')
            {
                ofile<<'T'<<tools.at(i)<<endl;
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
