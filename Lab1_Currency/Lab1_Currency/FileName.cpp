#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#pragma comment(lib, "urlmon.lib")
using namespace std;

int main()
{
    setlocale(0, "ru");
    const int STR_SIZE = 128;
    string userDate;
    string userCurrency;
    
    while (true)
    {
        cout << "Enter date in a format: dd.mm.yy: ";
        cin >> userDate;
        if (userDate.size() != 10)
        {
            cout << "Wrong date" <<endl;
        }
        else 
        {
            break;
        }
    }

    string dateValid = userDate.substr(6, 4) + userDate.substr(3, 2) + userDate.substr(0, 2);

    cout << "Enter currency: ";
    cin >> userCurrency;

	string destFile = "file.txt";

    string valcode = "valcode=";
    valcode += userCurrency;
    string date = "&date=";
    date += dateValid;
    string srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?" + valcode + date + "&json";

    if (URLDownloadToFileA(NULL, srcURL.c_str(), destFile.c_str(), 0, NULL) != S_OK)
    {
        cout << "Error. Can't save to file"<<endl;
    }
    
    ifstream read(destFile.c_str());
    if (!read)
    {
        cout << "Error. Can't open the file"<<endl;
        return 1;
    }

    string response;
    char buf[STR_SIZE];
    while (!read.eof())
    {
        read.getline(buf, STR_SIZE);
        response += buf;
    }
    read.close();

    vector<string> keys = { "rate" };
  
    for (int i = 0; i < keys.size(); i++)
    {
        size_t found = response.find(keys[i]);

        if (found != string::npos)
        {
            int pos = found + keys[i].length() + 2;
            string extracted;
            while (response[pos] != ',' && pos < response.length())
            {
                extracted += response[pos];
                pos++;
            }
            extracted += "\n";
            cout << "Rate of 1 "<< userCurrency<< " on " << userDate << " = " << extracted;

        }
    }
}