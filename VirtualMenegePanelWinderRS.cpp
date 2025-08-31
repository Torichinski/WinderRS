#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <limits>
#include <stdexcept>

using namespace std;

#define GET_NAME(var) #var

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

fstream file("parametrs.txt", std::ios::in | std::ios::out | std::ios::app);
string line;

void append_value(fstream& file, double value) {
    if (!file.is_open()){
	cerr << "File not found. Entry of parametrs impossible ! \n";
	return;
}
    file << value << '\t';
}

void setConsoleFontSize(int width, int height) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = width; // Ширина символа
    cfi.dwFontSize.Y = height; // Высота символа
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    //std::wcscpy_s(cfi.FaceName, L"Terminal "); // Имя шрифта

    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}



class panel{
	public:
		int num;
		double diametr;
		double kc;
		
		bool runInput = true;
		int row_num;
		
		double paz_leng;
    	double stator_height;
    	double angle_motor_speed;
    	double wire_diametr;
    	int coils_quality;
		
		panel(int num, double diametr, double kc) : num(num), diametr(diametr), kc(kc){	}
		
		void error(){
			throw invalid_argument("Error of input");
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			
		}
		
		void row_index(fstream& file) {
        if (!file.is_open()) {
            cerr << "File not open. Cannot get row index." << endl;
            return;
        }

        file.seekg(0, std::ios::end);
        streampos endPos = file.tellg();

        if (endPos == 0) {
            row_num = 1;
            file << row_num << ")\t";
            return;
        }

        file.seekg(-1, std::ios::cur);

        while (file.get() != '\n' && file.tellg() > 0) {
            file.seekg(-2, std::ios::cur);
        }

        if (file.tellg() > 0) {
            file.seekg(1, std::ios::cur);
        } else {
            file.seekg(0, std::ios::beg);
        }

        string rows;
        getline(file, rows);
        istringstream iss(rows);
        iss >> row_num;

        row_num++;
        file << row_num << ")\t";
    }
		
	void readParametrs(double& val){
		
	}
		
	void setParametrs(){
		
		try{
			while(runInput == true){
			cout << "Enter num of coils :" << endl;
			cin >> num;
		
		if(num > 10000 or cin.fail()){
			error();
			cin.clear();
			}
					
			cout << "Enter diametr : " << endl;
			cin >> diametr;
			
		if(diametr > 80){
			error();
			cin.clear();
			}
		
			cout << "Enter the kontrol coefficient :" << endl;
			cin >> kc;
			
			cout << "Enter more parametrs? Print 0 or 1 :" << endl;
			cin >> runInput;
			
		if(num, diametr, kc != 0 && runInput == true){
			cout << "Choose extra parametrs from the rule :" << endl;
		}
		 
	
		if (num, diametr, kc != 0 && runInput == false){
		
			this -> num = num;
			this -> diametr = diametr;
			this -> kc = kc;
			// Номер запуска
			
			row_index(file);
			append_value(file, num);
			append_value(file, diametr);
			append_value(file, kc);
			
			file << '\n';

			}
	}
}
	
	catch (const invalid_argument& e) {
            cerr << "Error: Invalid input." << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

	}
	
	void load_parametrs(){
		
	}
};

int main(int argc, char** argv) {
	
setlocale(LC_ALL, "RUS");

//SetConsoleTextAttribute(h, (((2 << 4) | 14)));
system("color 1F");
setConsoleFontSize(12, 24);
	
int num;
double diametr, kc;	
panel Panel(num, diametr, kc);

Panel.setParametrs();

	return 0;
}
