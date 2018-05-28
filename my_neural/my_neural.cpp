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
		cout << "Случайным образом сгенерированные паттерны:\n";
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

class Axon {
protected:
	float f_weight;
public:
	Axon() {
		f_weight = 0;
	}

	void weight(float w) {
		f_weight = w;
	}

	float weight() {
		return f_weight;
	}
};

class Layer {
protected:
	Neuron* c_neuron;
	Axon** c_axons;
	int i_size;
public:
	Layer(int i_size) {
		c_neuron = new Neuron[i_size];
		c_axons = new Axon*[i_size];
		for (int i = 0; i < i_size; i++)
			c_axons[i] = new Axon[i_size];
	}

	Layer(Pattern<int> c_pattern) {
		i_size = c_pattern.get_cols();
		c_neuron = new Neuron[i_size];
		c_axons = new Axon*[i_size];
		for (int i = 0; i < i_size; i++)
			c_axons[i] = new Axon[i_size];
		axon_calc(c_pattern);
	}

	void axon_calc(Pattern<int> c_pattern) {
		int i, j, k;
		float i_sum;
		for (j = 0; j < i_size; j++)
			for (i = j; i < i_size; i++)
				if (i == j)
					c_axons[j][i].weight(0);
				else
				{
					i_sum = 0;
					for (k = 0; k < c_pattern.get_rows(); k++)
						i_sum += (c_pattern.get_value(k, i) * 2 - 1) * (c_pattern.get_value(k, j) * 2 - 1);
					c_axons[i][j].weight(i_sum);
					c_axons[j][i].weight(i_sum);
				}
	}

	float weight(int i, int j) {
		return c_axons[i][j].weight();
	}

	void weight(int i, int j, float w) {
		c_axons[i][j].weight(w);
	}

	int activate(int i) {
		return c_neuron[i].activation();
	}

	void activate(int i, int a) {
		c_neuron[i].activation(a);
	}

	void print_weights() {
		cout << "Матрица весов:" << endl << endl;
		for (int j = 0; j < i_size; j++)
		{
			for (int i = 0; i < i_size; i++)
				cout << setw(2) << c_axons[i][j].weight() << " ";
			cout << endl;
		}
		cout << endl;
	}

	int update(int mode, Layer c_layer_prev) {
		int k, select = 3;
		int ctr_unchg = 0;
		int ctr = 0;
		while (ctr_unchg < 100 && ctr < 1000)
		{
			if (!mode) 	async();
			else sync();

			bool changed = false;
			for (k = 0; k < i_size; k++)
				if (activate(k) != c_layer_prev.activate(k))
				{
					changed = true;
					break;
				}
			if (changed == false)
				ctr_unchg++;
			else
				ctr_unchg = 0;

			for (k = 0; k < i_size; k++)
				c_layer_prev.activate(k, activate(k));
			ctr++;
		}
		return ctr_unchg;
	}

	void async() {
		int i, j, k;
		float i_sum;
		for (k = 0; k < i_size; k++) {
			j = rand() % i_size;
			i_sum = 0;
			for (i = 0; i < i_size; i++)
				if (i != j)
					i_sum += activate(i) * weight(i, j);
			if (i_sum >= 0)
				activate(j, 1);
			else
				activate(j, 0);
		}
	}

	void sync() {
		Layer c_layer_temp(i_size);
		int i, j;
		float i_sum;
		for (j = 0; j < i_size; j++)
		{
			i_sum = 0;
			for (i = 0; i < i_size; i++)
				if (i != j)
					i_sum += activate(i) * weight(i, j);
			if (i_sum >= 0)
				c_layer_temp.activate(j, 1);
			else
				c_layer_temp.activate(j, 0);
		}
		c_neuron = c_layer_temp.c_neuron;
	}
};

void recognition_test(Pattern<int> c_pattern, Layer c_layer) {
	int i, select = 3,
		size = c_pattern.get_cols(), 
		selectedPattern = rand() % c_pattern.get_rows(),
		errorPercentage = 25;
	cout << "Случайным образом выбранный тестовый паттерн:" << endl;
	for (i = 0; i < size; i++)
	{
		cout << c_pattern.get_value(selectedPattern, i);
	}
	cout << endl << endl;

	cout << "Введите процент искажений: ";
	cin >> errorPercentage;
	cout << "Тестовый паттерн искажен на w/ " << errorPercentage << "%:" << endl;

	Layer c_layer_prev(c_pattern);
	for (i = 0; i < size; i++)
	{
		c_layer.activate(i, c_pattern.get_value(selectedPattern, i));
		if (rand() % 100<errorPercentage) c_layer.activate(i, 1 - c_layer.activate(i));
		cout << c_layer.activate(i);
		c_layer_prev.activate(i, c_layer.activate(i));
	}
	cout << endl << endl;

	cout << "Выберите режим работы сети: \n0. Асинхронный\n1. Синхронный\n";
	cin >> select;
	cout << endl;

	if (c_layer.update(select, c_layer_prev) >= 100)
	{
		cout << "\nОТЧЕТ\n====================\nРежим ";
		if (!select) cout << "асинхронный\n";
		else cout << "синхронный\n";
		cout << "Выбранный паттерн:     ";
		for (i = 0; i < size; i++)
		{
			cout << c_pattern.get_value(selectedPattern, i);
		}
		cout << endl;
		cout << "Сеть конвергировала к: ";
		for (i = 0; i < size; i++)
			cout << c_layer.activate(i);
		cout << endl;

		int sumDif = 0;
		for (i = 0; i < size; i++)
			if (c_layer.activate(i) != c_pattern.get_value(selectedPattern, i))
				sumDif++;
		
		cout <<	"Исходная ошибочность: " << errorPercentage
			<< "%\nОшибочность результата конвергенции: " << 100 * sumDif / size << "%" << endl;
		if (100 * sumDif / size != 0)
			if (100 * sumDif / size == 100)
				cout << "Cеть конвергировала к инверсному аттрактору\n(возможно восстановление патерна)" << endl;
			else
				cout << "Cеть конвергировала к химере (ложному аттрактору)" << endl;
		else cout << "УСПЕШНО!" << endl;
	}
	else
		cout << "Сеть не достигла заданного лимита конвергенции!" << endl;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "russian");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << "ИСКУССТВЕННАЯ НЕЙРОННАЯ СЕТЬ ХОПФИЛДА\n" << endl;
	int i_select = 1;

	while (i_select) {
		srand(time(NULL));

		Pattern<int> c_pattern;
		Layer c_layer(c_pattern);
		c_layer.print_weights();
		recognition_test(c_pattern, c_layer);
		cout << "\nВозобновить? 1/0 ";
		cin >> i_select;
	}
	return 0;
}