// neural.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <windows.h>

using namespace std;

template <class T> class Pattern {
protected:
	T* t_value;
	int i_rows, i_cols,
		i_width, i_height;
public:
	Pattern() {
		cout << "Введите размерность паттерна (рекомендуется 5): ";
		cin >> i_width;
		//cout << "Введите высоту паттерна (рекомендуется =ширине): ";
		//cin >> i_height;
		i_height = i_width;
		i_cols = i_width * i_height;
		i_rows = static_cast<int>(0.138f * i_cols);
		cout << "Рассчитанное оптимальное количество паттернов (0.138): " << i_rows << endl;
		t_value = new T[i_rows * i_cols];
		generate_pattern();
	}

	Pattern(int size, int width, int height) {
		i_cols = size;
		i_rows = 1;
		i_width = width;
		i_height = width;
		t_value = new T[i_rows * i_cols];
	}

	void generate_pattern() {
		int select;
		cout << "\nВыберите способ генерации паттернов:\n0. Случайная генерация\n1. Загрузка из файла паттернов 7x7\n";
		cin >> select;

		switch (select) {
		case (0):
			for (int i = 0; i < i_rows; i++)
				for (int j = 0; j < i_cols; j++)
					t_value[i * i_cols + j] = rand() % 2;
			cout << "Случайно сгенерированные паттерны: " << endl;
			for (int i = 0; i < i_rows; i++) {
				cout << endl << "Паттерн " << i + 1;
				print_pattern(i, i_cols);
			}
			break;
		case (1):
			//случайная генерация на случай недостатка в файле
			for (int i = 0; i < i_rows; i++)
				for (int j = 0; j < i_cols; j++)
					t_value[i * i_cols + j] = rand() % 2;
			fstream ifs;
			ifs.open("file.txt", ios::in);
			for (int i = 0; i < i_rows; i++)
				for (int j = 0; j < i_cols; j++)
					ifs >> t_value[i * i_cols + j];
			cout << "Загруженные паттерны: " << endl;
			for (int i = 0; i < i_rows; i++) {
				cout << endl << "Паттерн " << i + 1;
				print_pattern(i, i_cols);
			}
			break;
		}
	}

	int get_rows() {
		return i_rows;
	}

	int get_cols() {
		return i_cols;
	}

	int get_width() {
		return i_width;
	}

	int get_height() {
		return i_height;
	}

	T value(int i, int j) {
		return t_value[i * i_cols + j];
	}

	void value(int i, int j, int v) {
		t_value[i * i_cols + j] = v;
	}

	void print_pattern(int i_num, int i_size) {
		int i, j;
		cout << endl << "=========" << endl;
		for (i = 0; i < i_height; i++)
		{
			print_string_d(i_num, i);
			cout << "  ";
			print_string_s(i_num, i);
		}
		cout << endl;
	}

	void print_string_s(int i_num, int i) {
		SetConsoleOutputCP(866);
		for (int j = 0; j < i_width; j++) {
			if (value(i_num, i * i_width + j) == 1) cout << char(219);
			else cout << char(32);
		}
		SetConsoleOutputCP(1251);
		SetConsoleCP(1251);
		cout << endl;
	}

	void print_string_d(int i_num, int i) {
		for (int j = 0; j < i_width; j++) {
			cout << value(i_num, i * i_width + j);
		}
	}
};

template <class T> class Axon {
protected:
	T f_weight;
};

template <class T> class Neuron {
protected:
	T i_activation;
};

template <class T> class Layer {
protected:
	int i_size;
	Neuron<T>* c_neuron;
	Axon<T>** c_axon;
};

class Hopfield_neuron : public Neuron<int>{
protected:	
public:
	Hopfield_neuron() {
		i_activation = 0;
	}

	void activation(int a) {
		i_activation = a;
	}

	int activation() {
		return i_activation;
	}
};

class Hopfield_axon : public Axon<int> {
protected:
public:
	Hopfield_axon() {
		f_weight = 0;
	}

	void weight(int w) {
		f_weight = w;
	}

	int weight() {
		return f_weight;
	}
};

class Hopfield_layer : virtual public Layer<int> {
protected:
	Hopfield_neuron *c_neuron;
	Hopfield_axon **c_axon;
public:
	Hopfield_layer(int i_size) {
		c_neuron = new Hopfield_neuron[i_size];
		c_axon = new Hopfield_axon*[i_size];
		for (int i = 0; i < i_size; i++)
			c_axon[i] = new Hopfield_axon[i_size];
	}

	Hopfield_layer(Pattern<int> c_pattern) {
		i_size = c_pattern.get_cols();
		c_neuron = new Hopfield_neuron[i_size];
		c_axon = new Hopfield_axon*[i_size];
		for (int i = 0; i < i_size; i++)
			c_axon[i] = new Hopfield_axon[i_size];
		axon_calc(c_pattern);
	}

	void axon_calc(Pattern<int> c_pattern) {
		int i, j, k;
		int i_sum;
		for (j = 0; j < i_size; j++)
			for (i = j; i < i_size; i++)
				if (i == j)
					c_axon[j][i].weight(0);
				else
				{
					i_sum = 0;
					for (k = 0; k < c_pattern.get_rows(); k++)
						i_sum += (c_pattern.value(k, i) * 2 - 1) * (c_pattern.value(k, j) * 2 - 1);
					c_axon[i][j].weight(i_sum);
					c_axon[j][i].weight(i_sum);
				}
	}

	int weight(int i, int j) {
		return c_axon[i][j].weight();
	}

	void weight(int i, int j, int w) {
		c_axon[i][j].weight(w);
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
				cout << setw(2) << c_axon[i][j].weight() << " ";
			cout << endl;
		}
		cout << endl;
	}

	int update(int mode, Hopfield_layer c_layer_prev) {
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
		int i_sum;
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
		Hopfield_layer c_layer_temp(i_size);
		int i, j;
		int i_sum;
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

void hopfield_recognition_test(Pattern<int> c_pattern, Hopfield_layer c_layer) {
	int i, select = 3, size = c_pattern.get_cols(), selectedPattern, errorPercentage;

	cout << "Выберите номер тестового паттерна (0 или больше " << c_pattern.get_rows() << " = случайный): ";
	cin >> select;
	if (select >= 1 && select < c_pattern.get_rows() + 1) {
		selectedPattern = select - 1;
		cout << "Выбранный тестовый паттерн:" << endl;
	}
	else {
		selectedPattern = rand() % c_pattern.get_rows();
		cout << "Случайным образом выбранный тестовый паттерн:" << endl;
	}

	c_pattern.print_pattern(selectedPattern, size);

	cout << "Введите процент искажений (рекомендуется <25%): ";
	cin >> errorPercentage;
	cout << "Тестовый паттерн искажен на " << errorPercentage << "%.\n Текущее состояние нейронов сети: " << endl;

	Hopfield_layer c_layer_prev(c_pattern);
	for (i = 0; i < size; i++)
	{
		c_layer.activate(i, c_pattern.value(selectedPattern, i));
		if (rand() % 100<errorPercentage) c_layer.activate(i, 1 - c_layer.activate(i));
		c_layer_prev.activate(i, c_layer.activate(i));
	}

	Pattern<int> temp_pattern(c_pattern.get_cols(), c_pattern.get_width(), c_pattern.get_height());
	for (i = 0; i < size; i++)
	{
		temp_pattern.value(0, i, c_layer.activate(i));
	}
	temp_pattern.print_pattern(0, size);

	cout << "Выберите режим работы сети: \n0. Асинхронный (предпочтительно)\n1. Синхронный\n";
	cin >> select;
	cout << endl;

	if (c_layer.update(select, c_layer_prev) >= 100)
	{
		cout << "\nОТЧЕТ\n====================\nРежим ";
		if (!select) cout << "асинхронный\n";
		else cout << "синхронный\n";
		cout << "Эталонный паттерн:";
		c_pattern.print_pattern(selectedPattern, size);
		cout << "Сеть конвергировала к: ";

		for (i = 0; i < size; i++)
		{
			temp_pattern.value(0, i, c_layer.activate(i));
		}
		temp_pattern.print_pattern(0, size);

		int sumDif = 0;
		for (i = 0; i < size; i++)
			if (c_layer.activate(i) != c_pattern.value(selectedPattern, i))
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
	//setlocale(LC_ALL, "russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	cout << "ИСКУССТВЕННАЯ НЕЙРОННАЯ СЕТЬ ХОПФИЛДА\n" << "=====================================" << endl;
	int i_select = 1;

	while (i_select) {
		srand(time(NULL));

		Pattern<int> c_pattern;
		Hopfield_layer c_layer(c_pattern);
		cout << "Печатать матрицу весов? 1/0 ";
		cin >> i_select;
		if (i_select) c_layer.print_weights();
		hopfield_recognition_test(c_pattern, c_layer);
		cout << "\nВозобновить? 1/0 ";
		cin >> i_select;
	}
	return 0;
}