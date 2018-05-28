// neural.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <vector>
#include <cmath>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <windows.h>

using namespace std;

template <class T> class Pattern {
protected:
	T* t_value;
	int i_rows, i_cols;
public:
	Pattern() {
		cout << "Введите размер паттерна: ";
		cin >> i_cols;
		i_rows = static_cast<int>(0.138f * i_cols);
		t_value = new T[i_rows * i_cols];
		for (int i = 0; i < i_rows; i++) 
		{
			for (int j = 0; j < i_cols; j++)
			{
				t_value[i * i_cols + j] = rand() % 2;
				cout << t_value[i * i_cols + j];
			}
			cout << endl;
		}
		cout << endl;
	}

	int get_rows() {
		return i_rows;
	}

	int get_cols() {
		return i_cols;
	}

	T get_value(int i, int j) {
		return t_value[i * i_cols + j];
	}
};

class Neuron {
protected:
	int i_activation;
public:
	Neuron() {
		i_activation = 0;
	}

	void activation(int a) {
		i_activation = a;
	}

	int activation() {
		return i_activation;
	}
};

class Layer {
protected:
	Neuron* c_neuron;
	int i_size;
public:
	Layer() {
		i_size = 0;
		c_neuron = new Neuron[i_size];
	}

	Layer(Pattern<int> c_pattern) {
		i_size = c_pattern.get_cols();
		c_neuron = new Neuron[i_size];
	}

	int get_a(int i) {
		return c_neuron[i].activation();
	}

	void activate(int i, int a) {
		c_neuron[i].activation(a);
	}

	//void update(int, int*, int*);
};

class Axons : public Layer{
protected:
	float** f_weight;
public:
	Axons(Pattern<int> c_pattern) {
		int i, j, k;
		float i_sum;
		i_size = c_pattern.get_cols();
		f_weight = new float*[i_size];
		for (i = 0; i < i_size; i++)
			f_weight[i] = new float[i_size];

		for (j = 0; j < i_size; j++)
			for (i = j; i < i_size; i++)
				if (i == j)
					f_weight[j][i] = 0;
				else
				{
					i_sum = 0;
					for (k = 0; k < c_pattern.get_rows(); k++)
						i_sum += (c_pattern.get_value(k, i) * 2 - 1) * (c_pattern.get_value(k, j) * 2 - 1);
					f_weight[i][j] = i_sum;
					f_weight[j][i] = i_sum;
				}
	}

	float get_weight(int i, int j) {
		return f_weight[i][j];
	}

	void print() {
		cout << "Матрица весов:" << endl << endl;
		for (int j = 0; j < i_size; j++)
		{
			for (int i = 0; i < i_size; i++)
				cout << setw(2) << f_weight[i][j] << " ";
			cout << endl;
		}
		cout << endl;
	}
};

void recognition_test(Pattern<int> c_pattern, Layer c_layer, Axons c_axons) {
	int i, j, k, sum, select = 3,
		size = c_pattern.get_cols(), 
		selectedPattern = rand() % c_pattern.get_rows(),
		errorPercentage = 25;
	cout << "Выбран тестовый паттерн:" << endl;
	for (i = 0; i < size; i++)
	{
		cout << c_pattern.get_value(selectedPattern, i);
	}
	cout << endl << endl;

	cout << "Введите процент искажений: ";
	cin >> errorPercentage;
	cout << "Начальное состояние сети : " << endl;
	cout << "Тестовый паттерн искажен на w/ " << errorPercentage << "%:" << endl;

	Layer c_layer_prev(c_pattern);
	for (i = 0; i < size; i++)
	{
		c_layer.activate(i, c_pattern.get_value(selectedPattern, i));
		if (rand() % 100<errorPercentage) c_layer.activate(i, 1 - c_layer.get_a(i));
		cout << c_layer.get_a(i);
		c_layer_prev.activate(i, c_layer.get_a(i)); // initially prev state=current
	}
	cout << endl << endl;

	// if state of the network stays unchanged for ? steps
	// that means the network is converged to an answer
	// so then exit the loop and printout the last state
	int ctr_unchg = 0;

	// loop counter to ensure a stop just in case
	// if the network becomes cyclic or chaotic
	int ctr = 0;

	while (select != 1 && select != 0) {
		cout << "Выберите режим работы сети: \n0. Асинхронный\n1. Синхронный" << endl;
		cin >> select;
		cout << endl;
	}

	while (ctr_unchg < 100 && ctr < 1000) // max 1000 loops allowed
	{
		if (!select) {
			// First choice for updating the network
			for (k = 0; k < size; k++) // update the whole network ?
			{
				// Serial-Random updating:
				// Randomly select a neuron and update its value
				j = rand() % size;
				sum = 0;
				for (i = 0; i < size; i++)
					if (i != j)
						sum += c_layer.get_a(i) * c_axons.get_weight(j, i);
				if (sum >= 0)
					c_layer.activate(j, 1);
				else
					c_layer.activate(j, 0);
			}
		}
		else {
			Layer c_layer_temp(c_pattern);
			// calculate the new values of each neuron
			// but do not update immediately!
			for (j = 0; j < size; j++)
			{
				sum = 0;
				for (i = 0; i < size; i++)
					if (i != j)
						sum += c_layer.get_a(i) * c_axons.get_weight(j, i);
				if (sum >= 0)
					c_layer_temp.activate(j, 1);
				else
					c_layer_temp.activate(j, 0);
			}
			// update the neurons with the new values
			c_layer = c_layer_temp; // update the array pointer
		   // delete the old values
		};

		bool changed = false;
		for (k = 0; k < size; k++)
			if (c_layer.get_a(k) != c_layer_prev.get_a(k))
			{
				changed = true;
				break;
			}
		if (changed == false)
			ctr_unchg++;
		else
			ctr_unchg = 0;

		// update the previous network state
		for (k = 0; k < size; k++)
			c_layer_prev.activate(k, c_layer.get_a(k));

		ctr++;
	}

	if (ctr_unchg >= 100)
	{
		cout << "Сеть конвергировала к:" << endl;
		for (i = 0; i < size; i++)
			cout << c_layer.get_a(i);
		cout << endl << endl;

		// calculate the convergence error percentage
		int sumDif = 0; // total number of differences
		for (i = 0; i < size; i++)
			if (c_layer.get_a(i) != c_pattern.get_value(selectedPattern, i))
				sumDif++;
		cout << "ОТЧЕТ\nРежим: ";
		if (!select) cout << "асинхронный\n";
		else cout << "синхронный\n";			
		cout <<	"Исходная ошибочность: " << errorPercentage
			<< "%\nОшибочность результата конвергенции: " << 100 * sumDif / size << "%" << endl;
		if (100 * sumDif / size != 0)
			if (100 * sumDif / size == 100)
				cout << "Cеть конвергировала к инверсному аттрактору\n(возможно восстановление патерна)" << endl;
			else
				cout << "Cеть конвергировала к ложному аттрактору (химере)" << endl;
	}
	else
		cout << "Сеть не достигла заданного лимита конвергенции!" << endl;

	// garbage collection
	/*delete[]pattern;
	delete[]w;
	delete[]neuron;
	delete[]neuron_prev;*/
}

//class Hopfield_layer : public layer {
//	Hopfield_layer() : layer() {
//	};
//
//	Hopfield_layer(int i_cols) : layer(i_cols) {
//	};
//};
//
//class Hopfield_neuron : public neuron {
//public:
//	Hopfield_neuron() : neuron() {
//	};
//};


int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "russian");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << "ИСКУССТВЕННАЯ НЕЙРОННАЯ СЕТЬ ХОПФИЛДА" << endl << endl;
	int i_select = 1;

	while (i_select) {
		srand(time(NULL)); // use current time to seed random number generator

		Pattern<int> c_pattern;
		Layer c_layer(c_pattern);
		Axons c_axons(c_pattern);
		c_axons.print();
		recognition_test(c_pattern, c_layer, c_axons);
		cout << "\nВозобновить? 1/0 ";
		cin >> i_select;
	}
	return 0;
}