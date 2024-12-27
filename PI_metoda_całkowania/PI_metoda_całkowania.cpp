/**
* @file main.cpp
 * @brief Program do obliczania przybliżonej wartości liczby PI metodą całkowania numerycznego z użyciem wątków.
 * 
 * Program oblicza przybliżoną wartość liczby PI metodą całkowania numerycznego na podstawie funkcji 
 * \( f(x) = \frac{4}{1 + x^2} \), korzystając z metody trapezów. Obliczenia są równolegle przetwarzane 
 * za pomocą wątków, które dzielą zadanie obliczeń na mniejsze części.
 * Program pozwala użytkownikowi ustawić liczbę przedziałów oraz liczbę wątków do przetwarzania.
 * 
 * Program mierzy również czas obliczeń i wypisuje wynik na ekranie.
 *
*/


#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <iomanip>

// Funkcja, którą będziemy całkować (funkcja 4 / (1 + x^2))
/**
 * @brief Funkcja obliczająca wartość funkcji f(x) = 4 / (1 + x^2)
 * 
 * Funkcja przyjmuje argument x i zwraca wartość \( f(x) = \frac{4}{1 + x^2} \), która jest używana 
 * do obliczenia przybliżonej wartości liczby PI za pomocą całkowania numerycznego.
 * 
 * @param x Wartość argumentu funkcji.
 * @return Wartość funkcji f(x).
 */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

// Funkcja do obliczania całki numerycznej metodą trapezów
/**
 * @brief Funkcja obliczająca część całki numerycznej metodą trapezów.
 * 
 * Funkcja ta oblicza część całki numerycznej w przedziale [start, end], wykorzystując metodę trapezów.
 * Służy do obliczenia wartości przybliżonej liczby PI na podstawie funkcji f(x).
 * 
 * @param start Początkowy indeks przedziału.
 * @param end Końcowy indeks przedziału.
 * @param total_intervals Całkowita liczba przedziałów w całkowaniu.
 * @return Wartość obliczonej części całki.
 */
double integrate(int start, int end, int total_intervals) {
    double sum = 0.0;
    double step = 1.0 / total_intervals;
    for (int i = start; i < end; ++i) {
        double x0 = i * step;
        double x1 = (i + 1) * step;
        sum += (f(x0) + f(x1)) * step / 2.0;
    }
    return sum;
}

// Funkcja obliczająca przybliżoną wartość PI równolegle
/**
 * @brief Funkcja obliczająca przybliżoną wartość liczby PI równolegle.
 * 
 * Funkcja dzieli obliczenia całkowania na równoległe wątki, które obliczają różne części całki.
 * Następnie wyniki z poszczególnych wątków są sumowane, aby uzyskać ostateczną wartość PI.
 * Program mierzy również czas obliczeń.
 * 
 * @param total_intervals Liczba przedziałów, na które dzielony jest cały przedział całkowania.
 * @param num_threads Liczba wątków wykorzystywanych do równoległego obliczania całki.
 * @return Przybliżona wartość liczby PI.
 */
double calculatePi(int total_intervals, int num_threads) {
    // We will divide the work across the available threads
    int intervals_per_thread = total_intervals / num_threads;
    std::vector<std::thread> threads;
    std::vector<double> results(num_threads, 0.0);

    auto start_time = std::chrono::high_resolution_clock::now();

    // Create threads to compute different parts of the integral
    for (int i = 0; i < num_threads; ++i) {
        int start = i * intervals_per_thread;
        int end = (i == num_threads - 1) ? total_intervals : (i + 1) * intervals_per_thread;
        threads.push_back(std::thread([&results, i, start, end, total_intervals]() {
            results[i] = integrate(start, end, total_intervals);
        }));
    }

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }

    // Sum up the results from all threads
    double pi = 0.0;
    for (double result : results) {
        pi += result;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "Czas obliczen z " << num_threads << " watkami: " 
              << std::fixed << std::setprecision(6) << duration.count() << " sekund.\n";

    return pi;
}

/**
 * @brief Funkcja główna programu.
 * 
 * Funkcja uruchamia program, który pozwala użytkownikowi podać liczbę wątków oraz liczbę przedziałów 
 * do obliczenia przybliżonej wartości liczby PI. Program wykonuje obliczenia równolegle i wyświetla 
 * wynik oraz czas obliczeń.
 * 
 * @return 0, jeśli program zakończył się pomyślnie.
 */
int main() {
    int total_intervals = 100000000;  ///< Liczba przedziałów
    int num_threads;

    // Zapytaj użytkownika o liczbę wątków
    std::cout << "Podaj liczbe watkow (1-50): ";
    std::cin >> num_threads;

    // Sprawdź, czy liczba wątków mieści się w dozwolonym zakresie
    if (num_threads < 1 || num_threads > 50) {
        std::cerr << "Liczba watkow musi byc w przedziale 1-50.\n";
        return 1;
    }

    // Oblicz PI
    double pi = calculatePi(total_intervals, num_threads);

    // Wyświetl wynik
    std::cout << "Przyblizona wartosc PI wynosi: " << std::fixed << std::setprecision(8) << pi << std::endl;
    
    return 0;
}
