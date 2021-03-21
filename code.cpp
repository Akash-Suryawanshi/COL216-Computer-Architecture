#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

// For storing values in a register
map<string, int> registers = {
	{"$zero", 0}, {"$at", 0}, {"$v0", 0}, {"$v1", 0}, {"$a0", 0}, {"$a1", 0}, {"$a2", 0}, {"$a3", 0}, {"$t0", 0},
	{"$t1", 0}, {"$t2", 0}, {"$t3", 0}, {"$t4", 0}, {"$t5", 0}, {"$t6", 0}, {"$t7", 0}, {"$s0", 0}, {"$s1", 0},
	{"$s2", 0}, {"$s3", 0}, {"$s4", 0}, {"$s5", 0}, {"$s6", 0}, {"$s7", 0}, {"$t8", 0}, {"$t9", 0}, {"$k0", 0},
	{"$k1", 0}, {"$gp", 0}, {"$sp", 0}, {"$fp", 0}, {"$ra", 0}
};
//For storing number of times given instruction has been executed. 
map<string, int> instr_num = {
	{"add", 0}, {"sub", 0}, {"mul", 0}, {"beq", 0}, {"bne", 0}, {"slt", 0}, {"j", 0}, {"lw", 0}, {"sw", 0}, {"addi", 0}
};
int n = (int)(pow(2.0, 10.0));
vector<vector<string>> instructions; // For storing instructions
vector<vector<int>> dram_memory(n,vector<int>(n, 0));
vector<string> line_inst;
map<string, int> jump_check; //For storing the instruction number in case of jump
vector<int> row_buffer;
int total_inst = 0, counter = 0, inst_count = 0, cycles_clock = 0;
int row_number = -1;
int row_access_delay;
int column_access_delay;
string register_in_buffer;
int wait_time;
bool buffer_in_use = false;
int number_of_buffer_updates = 0;

//Integer is 2^5 bits, total memory available is 2^23 bits 
//so total size of array to be created for memory is 2^18=262144.
// Data will be stored after storing instructions. 
//So size of memory array is 262144-(number of instructions)
// Assumption: each instruction and data stored takes 2^5 bits. 

//Function for converting number from decimal to hexadecimal form
string dectohex(int num)
{
	stringstream ss;
	ss << hex << num;
	string hex_str = ss.str();
	return hex_str;
}

void throw_error(int counter)
{
	cout << "Error at line number " << counter + 1 << endl;
	cout << "Program Terminated";
	exit(0);
}

void decrement_time()
{
		wait_time = wait_time - 1;
		if (wait_time == 0)
		{
			buffer_in_use = false;
		}
		cycles_clock--;
}

void executed_ins(string ins)
{
	cout << "executed instruction in cycle number "<< cycles_clock <<": " << ins << endl;
	cout << "\n\n";
}
//Function for printing the value of map of registers
void print_reg(int cycles_clock)
{
	cout << "Cycle Number: " << cycles_clock << " => ";
	for (auto i : registers)
	{
		cout << "{" << i.first << ": " << dectohex(i.second) << "} ";
	}
	cout << endl;
}

void modified_register(string reg, int new_val) {
	cout << "modified register: " << reg << ", " << "new value: " << new_val << endl;
}

int main(int argc, char const* argv[])
{
	string mytext;
	ifstream myfile(argv[1]);
	row_access_delay = strtol(argv[2], nullptr, 0);
	column_access_delay = strtol(argv[3], nullptr, 0);

	//Reading the file
	while (getline(myfile, mytext))
	{
		if (mytext.empty()) continue;
		bool comment = false;
		line_inst = {};
		string temp;
		istringstream abc(mytext);
		while (abc >> temp)
		{
			if (temp[0] == '#')
			{
				comment = true;
				break;
			}
			char c = temp[temp.length() - 1];
			if (c == ',') temp.erase(temp.length() - 1, 1); // Removing the comma
				// Check if it's a function in MIPS
			else if (c == ':')
			{
				temp.erase(temp.length() - 1, 1);
				jump_check[temp] = counter; // Storing the instruction number in case of jump
			}
			line_inst.push_back(temp);
		}
		if (line_inst.size() == 0) continue; // If it's an empty line then simply move ahead
		if (line_inst.size() > 4) throw_error(counter); // Syntax error
		instructions.push_back(line_inst);
		counter++;
	}
	counter = 0;

	total_inst = instructions.size();
	vector<int> memory(262144 - total_inst, 0);

	int t = 0;
	while (counter < total_inst)
	{
		line_inst = instructions[counter];
		if (line_inst[0] == "sub")
		{
			if ((line_inst.size() != 4) || (registers.find(line_inst[1]) == registers.end())
				|| (registers.find(line_inst[2]) == registers.end()) || (registers.find(line_inst[3]) == registers.end()
				))
				throw_error(counter);
			instr_num[line_inst[0]]++;
			
			registers[line_inst[1]] = registers[line_inst[2]] - registers[line_inst[3]];
			cycles_clock++;
			print_reg(cycles_clock);
			modified_register(line_inst[1], registers[line_inst[1]]);
			executed_ins("sub");
		}
		else if (line_inst[0] == "mul")
		{
			if ((line_inst.size() != 4) || (registers.find(line_inst[1]) == registers.end())
				|| (registers.find(line_inst[2]) == registers.end()) || (registers.find(line_inst[3]) == registers.end()
				))
				throw_error(counter);
			instr_num[line_inst[0]]++;
			
			registers[line_inst[1]] = registers[line_inst[2]] * registers[line_inst[3]];
			cycles_clock++;
			print_reg(cycles_clock);
			modified_register(line_inst[1], registers[line_inst[1]]);
			executed_ins("mul");
		}
		else if (line_inst[0] == "add")
		{
			if ((line_inst.size() != 4) || (registers.find(line_inst[1]) == registers.end())
				|| (registers.find(line_inst[2]) == registers.end()) || (registers.find(line_inst[3]) == registers.end()
				))
				throw_error(counter);
			instr_num[line_inst[0]]++;
			
			registers[line_inst[1]] = registers[line_inst[2]] + registers[line_inst[3]];
			cycles_clock++;
			print_reg(cycles_clock);
			modified_register(line_inst[1], registers[line_inst[1]]);
			executed_ins("add");
		}
		else if (line_inst[0] == "slt")
		{
			if ((line_inst.size() != 4) || (registers.find(line_inst[1]) == registers.end())
				|| (registers.find(line_inst[2]) == registers.end()) || (registers.find(line_inst[3]) == registers.end()
				))
				throw_error(counter);
			instr_num[line_inst[0]]++;
			int reg1 = registers[line_inst[2]], reg2 = registers[line_inst[3]];
			
			if (reg1 < reg2) registers[line_inst[1]] = 1;
			else registers[line_inst[1]] = 0;
			cycles_clock++;
			print_reg(cycles_clock);
			executed_ins("slt");
		}
		else if (line_inst[0] == "addi")
		{
			if ((line_inst.size() != 4) || (registers.find(line_inst[1]) == registers.end())
				|| (registers.find(line_inst[2]) == registers.end()))
				throw_error(counter);
			if (line_inst.size() != 4) throw_error(counter);
			instr_num[line_inst[0]]++;
			
			int reg1 = registers[line_inst[2]], reg2 = stoi(line_inst[3]);
			if (line_inst[2] == "$sp") registers[line_inst[1]] = reg1 + reg2 / 4;
			else registers[line_inst[1]] = reg1 + reg2;
			cycles_clock++;
			print_reg(cycles_clock);
			modified_register(line_inst[1], registers[line_inst[1]]);
			executed_ins("addi");
		}
		else if (line_inst[0] == "j")
		{
			if (line_inst.size() != 2) throw_error(counter);
			instr_num[line_inst[0]]++;
			if (jump_check.find(line_inst[1]) == jump_check.end()) throw_error(counter);
			counter = jump_check[line_inst[1]];
			cycles_clock++;
			print_reg(cycles_clock);
		}
		else if (line_inst[0] == "bne")
		{
			if ((line_inst.size() != 4) || (registers.find(line_inst[1]) == registers.end())
				|| (registers.find(line_inst[2]) == registers.end()))
				throw_error(counter);
			instr_num[line_inst[0]]++;
			
			int reg1 = registers[line_inst[1]], reg2 = registers[line_inst[2]];
			if (jump_check.find(line_inst[3]) == jump_check.end()) throw_error(counter);
			if (reg1 != reg2)
			{
				counter = jump_check[line_inst[3]];
			}
			cycles_clock++;
			print_reg(cycles_clock);
			executed_ins("j");
		}
		else if (line_inst[0] == "beq")
		{
			if ((line_inst.size() != 4) || (registers.find(line_inst[1]) == registers.end())
				|| (registers.find(line_inst[2]) == registers.end()))
				throw_error(counter);
			instr_num[line_inst[0]]++;
			
			int reg1 = registers[line_inst[1]], reg2 = registers[line_inst[2]];
			if (jump_check.find(line_inst[3]) == jump_check.end()) throw_error(counter);
			if (reg1 == reg2)
			{
				counter = jump_check[line_inst[3]];
			}
			cycles_clock++;
			print_reg(cycles_clock);
			executed_ins("beq");
		}
		else if (line_inst[0] == "sw")
		{
			cycles_clock++;
			cout << cycles_clock << ": " << "DRAM request issued" << endl << endl;
			if ((line_inst.size() != 3) || (registers.find(line_inst[1]) == registers.end())) throw_error(counter);
			instr_num[line_inst[0]]++;
			string base_address = "", offset_address = "", mem = line_inst[2];
			int flag = 0;
			for (char c : mem)
			{
				if (flag)
				{
					if (c == ')') break;
					base_address += c;
				}
				else
				{
					if (c == '(') flag = 1;
					else offset_address += c;
				}
			}
			if ((registers.find(base_address) == registers.end())) throw_error(counter);
			int base_value = registers[base_address];
			memory[base_value + stoi(offset_address) / 4] = registers[line_inst[1]];
			int rw, clw;
			rw = static_cast<int>(floor((base_value + stoi(offset_address)) / 1024));
			register_in_buffer = line_inst[1];
			if(row_number!= rw && row_number != -1){
				//copy to dram and then copy another row and change
				dram_memory[row_number] = row_buffer;
				row_buffer = dram_memory[rw];
				clw = (base_value + stoi(offset_address)) % 1024;
				if (clw % 4 != 0) throw_error(counter);
				row_buffer[clw] = registers[line_inst[1]];
				row_number = rw;
				wait_time = 2*row_access_delay + column_access_delay;
				buffer_in_use = true;
				cycles_clock += wait_time;
				number_of_buffer_updates += 2;
			}
			else if (row_number == rw){
				clw = (base_value + stoi(offset_address)) % 1024;
				if (clw % 4 != 0) throw_error(counter);
				row_buffer[clw] = registers[line_inst[1]];
				wait_time = 0*row_access_delay + column_access_delay;
				buffer_in_use = true;
				cycles_clock += wait_time;
			}
			else{
				row_buffer = dram_memory[rw];
				clw = (base_value + stoi(offset_address)) % 1024;
				if (clw % 4 != 0) throw_error(counter);
				row_buffer[clw] = registers[line_inst[1]];
				row_number = rw;
				wait_time = row_access_delay + column_access_delay;
				buffer_in_use = true;
				cycles_clock += wait_time;
				number_of_buffer_updates += 1;
			}
			print_reg(cycles_clock);
			cout << "memory location: " << rw * 1024 + clw << "-" << rw * 1024 + clw + 3 << ", value = " << registers[line_inst[1]] << endl;
			executed_ins("sw");
		}
		else if (line_inst[0] == "lw")
		{
			cycles_clock++;
			cout << cycles_clock << ": " << "DRAM request issued" << endl << endl;
			if ((line_inst.size() != 3) || (registers.find(line_inst[1]) == registers.end())) throw_error(counter);
			instr_num[line_inst[0]]++;
			string base_address = "", offset_address = "", mem = line_inst[2];
			int flag = 0;
			for (char c : mem)
			{
				if (flag)
				{
					if (c == ')') break;
					base_address += c;
				}
				else
				{
					if (c == '(') flag = 1;
					else offset_address += c;
				}
			}
			if ((registers.find(base_address) == registers.end())) throw_error(counter);
			register_in_buffer = line_inst[1];
			int base_value = registers[base_address];
			//registers[line_inst[1]] = memory[base_value + stoi(offset_address)];
			int rw, clw;
			rw = static_cast<int>(floor((base_value + stoi(offset_address)) / 1024));
			if(rw == row_number){
				clw = (base_value + stoi(offset_address)) % 1024;
				if (clw % 4 != 0) throw_error(counter);
				registers[line_inst[1]] = row_buffer[clw];
				wait_time = 0*row_access_delay + column_access_delay;
				buffer_in_use = true;
				cycles_clock += wait_time;
			}
			else if (row_number == -1) {
				row_buffer = dram_memory[rw];
				clw = (base_value + stoi(offset_address)) % 1024;
				if (clw % 4 != 0) throw_error(counter);
				registers[line_inst[1]] = row_buffer[clw];
				wait_time = 2 * row_access_delay + column_access_delay;
				buffer_in_use = true;
				cycles_clock += wait_time;
				row_number = rw;
				number_of_buffer_updates += 1;
			}
			else{
				dram_memory[row_number] = row_buffer;
				row_buffer = dram_memory[rw];
				clw = (base_value + stoi(offset_address)) % 1024;
				if (clw % 4 != 0) throw_error(counter);
				registers[line_inst[1]] = row_buffer[clw];
				wait_time = 2*row_access_delay + column_access_delay;
				buffer_in_use = true;
				cycles_clock += wait_time;
				row_number = rw;
				number_of_buffer_updates += 2;
			}
			print_reg(cycles_clock);
			modified_register(line_inst[1], registers[line_inst[1]]);
			executed_ins("lw");
		}
		else if (line_inst.size() > 1)
		{
			cout << "Syntax Error at line number " << counter + 1 << endl;
			cout << "Program Terminated";
			exit(0);
		}
		counter++;
	}
	dram_memory[row_number] = row_buffer;
	cout << "Number of Clock Cycles: " << cycles_clock << endl;
	for (auto i : instr_num)
	{
		cout << "{" << i.first << ": " << dectohex(i.second) << "} ";
	}
	cout << endl;

	for (int i = 0; i < dram_memory.size(); i++) {
		for (int j = 0; j < dram_memory[0].size(); j++) {
			if (dram_memory[i][j]!=0){
				cout << 1024*i + j << ", " << dram_memory[i][j] << endl;
			}
		}
	}
	cout << "Number of row buffer updates: " << number_of_buffer_updates << endl;

	return 0;
}
