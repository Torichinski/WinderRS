#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace std;

#define GET_NAME(var) #var

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

fstream file("parametrs.txt", std::ios::in | std::ios::out | std::ios::app);

string line;
bool runProg = true;

struct characteristics{
	int num;
	double diametr;
	double kc;
	double paz_leng;     
  	double wire_width_limit;      
  	double stator_height;  
  	double stator_diametr;   
  	double angle_motor_speed;  
  	double wire_diametr;   
  	int coils_quality; 
  	double winder_radius; 
  	double extra_wire_len;
  };



void append_value(fstream& file, double value) {
    if (!file.is_open()){
	cerr << "File not found. Entry of parametrs impossible ! \n";
	return;
}
    file << value << '\t';
}

void fifty_dashes(){
	for(int i = 0; i <= 50; i++){
		cout << "-";
	}
	cout << '\n';
}

void setConsoleFontSize(int width, int height) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = width; // Øèðèíà ñèìâîëà
    cfi.dwFontSize.Y = height; // Âûñîòà ñèìâîëà
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    //std::wcscpy_s(cfi.FaceName, L"Terminal "); // Èìÿ øðèôòà

    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void ConsoleSetup(){
	system("title WinderRS Menege Panel");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

class panel{
	public:
		characteristics motor;

	public:
		int num;
		double diametr;
		double kc;
		
		bool runInput = true;
		bool withExtraPars = false;
		int row_num;
		int comb[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		std::vector <double> characteristics;
		string directoryPath = "C:\\WinderRS_Sistem";
		
		double paz_leng;     
      	double wire_width_limit;      
      	double stator_height;  
      	double stator_diametr;   
      	double angle_motor_speed;  
      	double wire_diametr;   
      	int coils_quality; 
      	double winder_radius; 
      	double extra_wire_len;
      	
      	//characteristics.push_back(0);
      	//characteristics.insert(characteristics.begin() + 1, paz_leng);
		
		panel(int num, double diametr, double kc) : num(num), diametr(diametr), kc(kc){	}
		
		void error(){
			throw invalid_argument("Error of input");
			cin.ignore(numeric_limits<streamsize>::max(), '\n');	
		}
		
		void Make_Directory(){
		    if (CreateDirectory(directoryPath.c_str(), NULL)) {
		        cout << "WinderRS program has loaded " << std::endl;
		        fifty_dashes(); 
		    } 
		    else{
		    	fifty_dashes(); 
			}
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
			int pars_num;
			cout << "Enter num of parametrs from the rule, that your need:" << endl;
			cin >> pars_num;

		}
			
		void setParametrs(){
			
			try{
				while(runInput == true){
					
				cout << "Enter num of coils :" << endl;
				cin >> num;
			
				if(num > 10000 or cin.fail() or num != (int)num){
					cout << "Num of coils can't be more that 10.000 and must be an integer!" << endl << '\n';
					cin.clear();
					cin.ignore(10000, '\n');
					continue;
					}
						
				cout << "Enter diametr (in mm) : " << endl;
				cin >> diametr;
				
				if(diametr > 80 or cin.fail()){
					cout << "Diametr can't be more that 80 mm and must be a float !" << endl << '\n';
					cin.clear();
					cin.ignore(10000, '\n');
					continue;
					}
			
				cout << "Enter the kontrol coefficient :" << endl;
				cin >> kc;
				// autopointer
			if(kc < 0.25 || kc > 2 || cin.fail() || kc != (float)kc){
				cout << "Kontrol coefficient is a float for 0.25 to 2 involved !" << endl << '\n';
				cin.ignore(10000, '\n');
				cin.clear();
				continue;
				}
				
				cout << "Enter more parametrs? Print 0 or 1 :" << endl;
				cin >> runInput;
				
			if(num, diametr, kc != 0 && runInput == true){
				
				cout << "Choose extra parametrs from the rule :" << endl;
				string line;
				
				ifstream Extra_pars("Extra parametrs.txt");
				SetConsoleOutputCP(CP_UTF8);
				
				if (!Extra_pars.is_open()) {
	            throw std::runtime_error("Îøèáêà îòêðûòèÿ ôàéëà!");
	        	}        
				while (getline(Extra_pars, line)){
	        	cout << line << endl;
	        	}
	        	Extra_pars.close();
	        	
	        	cout << '\n' << "Write the number of the parameter you need (set 0 to skip parameter) :" << endl;
	        	
	        	for(int i = 0; i <= 8; ++i){
	        	cout << i + 1 << ". ";
				cin >> comb[i];
				
				if (comb[i] != (int)comb[i] || comb[i] > 10000){
					error();
					}
					
				if (comb[i] != 0){
					withExtraPars = true;
					int idx = i + 1;
						switch(idx) {
							case 1:
								paz_leng = comb[i];
							case 2:
								wire_width_limit = comb[i];
							case 3:
								stator_height = comb[i];
							case 4:
								stator_diametr = comb[i];
							case 5:
								angle_motor_speed = comb[i];
							case 6:
								wire_diametr = comb[i];
							case 7:
								coils_quality = comb[i];
							case 8:
								winder_radius = comb[i];
							case 9:	
								extra_wire_len = comb[i];
						}
				}
			}
			
			cout << "You choose this parametr configuration:" << endl;
                for(int i = 0; i <= 9; ++i){
				cout << comb[i] << ' ';
			}			
				
	        cout << '\n' << wire_width_limit << endl;
	        runInput = false;
	        
	        while(withExtraPars == true){
				motor = {
				.num = num,
				.diametr = diametr, 
				.kc = kc,
				.paz_leng = paz_leng,
				.wire_width_limit = wire_width_limit,
				.stator_height = stator_height,
				.stator_diametr = stator_diametr,
				.angle_motor_speed = angle_motor_speed,
				.wire_diametr = wire_diametr,
				.coils_quality = coils_quality,
				.winder_radius = winder_radius,
				.extra_wire_len = extra_wire_len
				};
			}
	        
			}
		
			if (num, diametr, kc != 0 && runInput == false){
				motor = {.num = num, .diametr = diametr, .kc = kc};
			
				this -> num = num;
				this -> diametr = diametr;
				this -> kc = kc;
				
				row_index(file);
				append_value(file, num);
				append_value(file, diametr);
				append_value(file, kc);
				
				file << '\n';
				cout << "Input was finished" << endl;
				}
		}
	}
		
		catch (const invalid_argument& e) {
	            cerr << "Error: Invalid input." << e.what() << endl;
	            cin.clear();
	            cin.ignore(numeric_limits<streamsize>::max(), '\n');
	    }
	        
		catch (const runtime_error& e) {
	        cerr << "Error: " << e.what() << std::endl;
	    }
	
		}
		
		void load_parametrs(){ 
			
		}
};

int main(int argc, char** argv) {
	
setlocale(LC_ALL, "RUS");
system("color 1F");

setConsoleFontSize(12, 24);
ConsoleSetup();

	
int num;
double diametr, kc;	
panel Panel(num, diametr, kc);

// runProg
Panel.Make_Directory();
Panel.setParametrs();

return 0;
}
