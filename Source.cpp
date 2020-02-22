// Program: Facets
// Author: Janez Ster
// Date: February 22, 2020
//
// Program description:
// Input: nonempty subsets A_1,...,A_n of the set {0,...,63} (written in a file).
// Output: for each k, the number of all sets X of order k such that X is a subset of at least one
// A_i is given (written in a file).
//
// Note 1: Sets A_i are presented as 64-bit integers,
// so the program will be fastest on 64-bit processors and using 64-bit compilers.
// When compiling the program, make sure to choose the "x64" option on your compiler,
// otherwise the program will be slower.
//
// Note 2: Unfortunately, the program uses (static) arrays rather than vectors. But that doesn't
// affect performance, at least when the number of sets A_i is low.
//
// Example of usage: Given A_1={1,3,5,6}, A_2={2,4,5,16,20}, A_3={0,2,5}, we want to compute
// the number of sets X of size k such that X is a subset of at least one A_i.
//
// Input txt file:
// 1,3,5,6
// 2,4,5,16,20
// 0,2,5
//
// Output txt file:
// 1
// 9
// 18
// 15
// 6
// 1
//
// (means that there is 1 subset of size 0, 9 subsets of size 1, 18 subsets of size 2 etc.)

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

constexpr auto MAX_NUMBER = 1000; // maximal allowed number of sets A_i

// Every set A is presented by the following data:
// - unsigned long long int set (a set in binary form);
// - unsigned long long int sequence[] (ascending sequence of subsets of A,
//   a technical data that makes the algorithm faster)
//
// Example: The data for A = {1, 5, 2, 0} is:
// set = 100111 (bin) = 1 + 2 + 4 + 32 = 39 (dec)
// sequence[0] = 000010 (bin) - presenting the set {1}
// sequence[1] = 100010 (bin) - presenting the set {1,5}
// sequence[2] = 100110 (bin) - presenting the set {1,5,2}
// sequence[3] = 100111 (bin) - presenting the set {1,5,2,0}

// Allocating memory for input and output data:
int number; // number of sets A_i
unsigned long long int sets[MAX_NUMBER]; // the sets
unsigned long long int sequences[MAX_NUMBER][64]; // the sequences of subsets
int numbers_of_subsets[65] = {}; // output numbers, initialized to 0

// Returns the first element in a comma-separated string and erases that
// element in the string.
int First_element(std::string& csstring) {
	std::string x_string;
	if (csstring.find(",") == std::string::npos) { // no comma in csstring
		x_string = csstring;
		csstring = "";
	}
	else {
		x_string = csstring.substr(0, csstring.find(",")); // comma in csstring
		csstring.erase(0, x_string.length() + 1);
	}
	std::stringstream x_stream(x_string); // save the element as a stream
	int x;
	x_stream >> x; // convert to int
	return x;
}

// Reads the input data from the file.
// Returns false if not successful.
bool Read_input(std::string filename) {
	std::ifstream file(filename);
	if (!file.is_open()) return false;
	std::string line;
	number = 0;
	while (getline(file, line)) {
		unsigned long long int set = 0; // the set we add elements to
		int index = 0;
		while (line != "") {
			int x = First_element(line);
			set |= (1ULL << x); // add x to the set (i.e. add 2^x)
			sequences[number][index] = set;
			index++;
		}
		sets[number] = set;
		number++;
	}
	file.close();
	return true;
}

// Writes the output data to the file.
// Returns false if not successful.
bool Write_output(std::string filename) {
	std::ofstream file(filename);
	if (!file.is_open()) return false;
	for (int i = 0; i <= 64; i++) {
		if (numbers_of_subsets[i] == 0) break; // if the number equals 0 then we are finished
		file << numbers_of_subsets[i] << "\n"; // otherwise write the number
	}
	file.close();
	return true;
}

// Computes the numbers of subsets and saves them to the array numbers_of_subsets[].
// Algorithm: Go over all sets A_1,...,A_n, and for each fixed A_i go over
// all subsets of A_i. For each subset, check if it is a subset of any
// of the previous A_i's. If not, increase numbers_of_subsets[].
void Compute_numbers() {
	for (int i = 0; i < number; i++) {
		unsigned long long int set = sets[i]; // the set A_i
		unsigned long long int subset = 0; // the set that runs through subsets of A_i
		int size = 0; // size of the subset
		while (subset != set) {
			bool contained = false; // Is subset contained in any of the previous sets A_i?
			for (int j = 0; j < i; j++)
				if ((subset & sets[j]) == subset) { // If it is contained ...
					contained = true;
					break; // ... exit loop!
				}
			if (!contained) numbers_of_subsets[size]++; // increment number_of_subsets[]
			// The key part is to find the successor of subset in the while loop.
			// The answer is: find the smallest set in sequences[i] which
			// is not contained in subset.
			int index = 0;
			while ((subset | sequences[i][index]) == subset) index++;
			// Then take subset xor that "smallest set":
			subset = subset ^ sequences[i][index];
			size -= index - 1; // refresh size
		}
		// Finally consider the remaining case subset == set:
		bool contained = false;
		for (int j = 0; j < i; j++)
			if ((set & sets[j]) == set) {
				contained = true;
				break;
			}
		if (!contained) numbers_of_subsets[size]++;
	}
}

int main() {
	std::cout << "Input file: ";
	std::string filename;
	std::cin >> filename;
	if (Read_input(filename)) {
		std::cout << "Computing ...";
		Compute_numbers();
		std::cout << " Finished.\n";
		std::cout << "Output file: ";
		std::cin >> filename;
		if (Write_output(filename))
			std::cout << "Finished writing output data.";
		else
			std::cout << "Can't write file " << filename << ".";
	}
	else
		std::cout << "Can't read file " << filename << ".";
	std::cout << "\nPress enter to exit ...";
	std::cin.get();
	std::cin.get();
	return 0;
}