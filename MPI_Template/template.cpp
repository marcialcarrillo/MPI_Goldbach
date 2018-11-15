﻿
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

vector<int> primes_generated;
vector<int> global_solutions_vector;
vector<int> local_solutions_vector;
vector<bool> marked;
int process_id;
int ammount_of_digits_for_each_process;

// foos prototypes
void generate_primes_up_to_requested_number(int requested_number);
bool check_if_prime(int prime_to_check);
vector<int>::iterator prime_iterator_under_requested_number(int requested_number);
int goldbach_generator_even(int number);
void goldbach_generator_odd(vector<int> output, int number);
// end of foos

int main(int argc, char* argv[]) {
	int input_number;
	int process_quantity;

	/* Arrancar ambiente MPI */
	MPI_Init(&argc, &argv);             		/* Arranca ambiente MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id); 		/* El comunicador le da valor a id (rank del proceso) */
	MPI_Comm_size(MPI_COMM_WORLD, &process_quantity);  /* El comunicador le da valor a p (número de procesos) */

	//if (process_id == 0)
	//	cin.ignore();
	//MPI_Barrier(MPI_COMM_WORLD);
	get_args(argv, input_number);

	int numbers_to_add_for_input_to_be_divisible = process_quantity - ((input_number - 3) % process_quantity);

	if (numbers_to_add_for_input_to_be_divisible != process_quantity)
	{
		input_number += numbers_to_add_for_input_to_be_divisible;
	}

	ammount_of_digits_for_each_process = (input_number - 3) / process_quantity;

	generate_primes_up_to_requested_number(input_number);
	global_solutions_vector.resize(input_number * 3);
	local_solutions_vector.resize(ammount_of_digits_for_each_process * 3);

	for (int i = ammount_of_digits_for_each_process; i > 0; i--)
	{

	}

	//auto it = prime_iterator_under_requested_number(26457);
	//cout << *it;


	MPI_Finalize();
	return 0;
}


void generate_primes_up_to_requested_number(int requested_number)
{
	marked.resize(requested_number / 2 + 100);
	fill(marked.begin(), marked.end(), false);

	for (int i = 1; i <= (sqrt(requested_number) - 1) / 2; i++)
		for (int j = (i*(i + 1)) << 1; j <= requested_number / 2; j = j + 2 * i + 1)
			marked[j] = true;

	// Since 2 is a prime number 
	primes_generated.push_back(2);

	// Print other primes. Remaining primes are of the 
	// form 2*i + 1 such that marked[i] is false. 
	for (int i = 1; i <= requested_number / 2; i++)
		if (marked[i] == false)
			primes_generated.push_back(2 * i + 1);

}

bool check_if_prime(int prime_to_check)
{

	if (find(primes_generated.begin(), primes_generated.end(), prime_to_check) == primes_generated.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

vector<int>::iterator prime_iterator_under_requested_number(int requested_number) //returns iterator to the vector that holds the generated primes
{
	auto iterator_under_requested_number = lower_bound(primes_generated.begin(), primes_generated.end(), requested_number);
	return iterator_under_requested_number - 1;
}

int goldbach_generator_even(int number)
{
	bool exit = false;
	if (number < 3)
	{
		return 0;
	}
	while (!exit)
	{
		auto current_prime_iterator = prime_iterator_under_requested_number(number);
		int residue = number - *current_prime_iterator;
		if (check_if_prime(residue))
		{
			return residue;
		}
		if (*current_prime_iterator == 2)
		{
			return 0;
		}
		current_prime_iterator++;
	}
}

void goldbach_generator_odd_v1(vector<int> output, int number)
{
	bool exit = false;
	vector<int>::iterator current_prime_iterator;
	int residue;
	int second_residue;
	if (number < 3)
	{
		//do nothing
	}
	else
	{
		while (!exit)
		{
			current_prime_iterator = prime_iterator_under_requested_number(number);
			residue = number - *current_prime_iterator;
			second_residue = goldbach_generator_even(residue);
			if (second_residue != 0)
			{
				output[number - (process_id * ammount_of_digits_for_each_process)] = *current_prime_iterator;
				output[number - ((process_id * ammount_of_digits_for_each_process)) + 1] = second_residue;
				output[number - ((process_id * ammount_of_digits_for_each_process)) + 2] = residue - second_residue;
				exit = true;
			}
			if (*current_prime_iterator == 2)
			{
				exit = true;
			}
			current_prime_iterator++;
		}
	}

}

void get_args(char* argv[], int& input_number)
{
	input_number = strtol(argv[1], NULL, 10);
}