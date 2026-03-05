
/**
 * Программа для вычисления площади, образованной тремя кривыми:
 * f1(x) = 0.6x + 3
 * f2(x) = (x-2)^3 - 1
 * f3(x) = 3/x
 * 
 * Площадь вычисляется как интеграл от разности верхней и нижней функций
 * на отрезке между точками пересечения кривых.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPS_DEFAULT 1e-6  /* Значение точности по умолчанию */

/* Прототипы функций */
double f1(double x);
double f2(double x);
double f3(double x);
double f1_derivative(double x);
double f2_derivative(double x);
double f3_derivative(double x);
double root(double (*f)(double), double (*g)(double), double (*df)(double), 
           double (*dg)(double), double a, double b, double eps, int *iterations);
double integral(double (*f)(double), double a, double b, double eps, int *iterations);
void print_help(void);
void run_tests(void);

/* Вспомогательные функции для тестирования */
double test_pow2(double x) { return x * x; }
double test_const4(double x) { return 4.0; }
double test_const05(double x) { return 0.5; }
double test_derivative_pow2(double x) { return 2 * x; }
double test_derivative_const(double x) { return 0.0; }

/* Основные функции */
double f1(double x) {
    return 0.6 * x + 3;
}

double f2(double x) {
    return pow(x - 2, 3) - 1;
}

double f3(double x) {
    return 3.0 / x;
}

/* Производные функций */
double f1_derivative(double x) {
    (void)x;  /* Подавляем предупреждение о неиспользуемом параметре */
    return 0.6;  /* Производная линейной функции постоянна */
}

double f2_derivative(double x) {
    return 3 * pow(x - 2, 2);  /* Производная от (x-2)^3 */
}

double f3_derivative(double x) {
    return -3.0 / (x * x);  /* Производная от 3/x */
}

/**
 * Находит корень уравнения f(x) = g(x) на отрезке [a, b] методом касательных (Ньютона)
 * 
 * Параметры:
 *   f, g - указатели на функции
 *   df, dg - указатели на производные функций
 *   a, b - границы отрезка, содержащего корень
 *   eps - требуемая точность
 *   iterations - указатель для сохранения числа итераций
 * 
 * Возвращает:
 *   приближенное значение корня
 */
double root(double (*f)(double), double (*g)(double), double (*df)(double), 
           double (*dg)(double), double a, double b, double eps, int *iterations) {
    double x0, x1;
    double f0, g0, df0, dg0;
    int iter = 0;
    
    /* Проверка, что корень действительно есть на отрезке */
    double fa = f(a) - g(a);
    double fb = f(b) - g(b);
    
    if (fa * fb > 0) {
        printf("Предупреждение: на отрезке [%f, %f] может не быть корня или их четное количество\n", a, b);
        /* Возвращаем середину отрезка */
        if (iterations) *iterations = 0;
        return (a + b) / 2;
    }
    
    /* Выбираем начальное приближение - середину отрезка */
    x0 = (a + b) / 2;
    
    do {
        /* Вычисляем значения функций и их производных в точке x0 */
        f0 = f(x0);
        g0 = g(x0);
        df0 = df(x0);
        dg0 = dg(x0);
        
        /* Проверяем, что знаменатель не равен нулю */
        if (fabs(df0 - dg0) < 1e-12) {
            /* Если производная близка к нулю, используем метод половинного деления */
            x1 = (a + b) / 2;
            if ((f(a) - g(a)) * (f(x1) - g(x1)) < 0) {
                b = x1;
            } else {
                a = x1;
            }
            x1 = (a + b) / 2;
        } else {
            /* Метод Ньютона: x1 = x0 - (f(x0)-g(x0))/(f'(x0)-g'(x0)) */
            x1 = x0 - (f0 - g0) / (df0 - dg0);
            
            /* Проверяем, что новое значение не выходит за границы отрезка */
            if (x1 < a || x1 > b) {
                x1 = (a + b) / 2;
            }
            
            /* Корректируем отрезок для гарантии сходимости */
            if ((f(a) - g(a)) * (f(x1) - g(x1)) < 0) {
                b = x1;
            } else {
                a = x1;
            }
        }
        
        iter++;
        
        /* Проверяем достижение точности */
        if (fabs(x1 - x0) < eps) {
            break;
        }
        
        x0 = x1;
        
        /* Предотвращаем бесконечный цикл */
        if (iter > 1000) {
            printf("Предупреждение: превышено максимальное число итераций в root()\n");
            break;
        }
    } while (1);
    
    if (iterations) {
        *iterations = iter;
    }
    
    return x1;
}

/**
 * Вычисляет определенный интеграл от функции f на отрезке [a, b] методом Симпсона
 * 
 * Параметры:
 *   f - указатель на интегрируемую функцию
 *   a, b - границы отрезка интегрирования
 *   eps - требуемая точность
 *   iterations - указатель для сохранения числа итераций (разбиений)
 * 
 * Возвращает:
 *   приближенное значение интеграла
 */
double integral(double (*f)(double), double a, double b, double eps, int *iterations) {
    int n = 2;  /* Начальное число разбиений */
    double h, sum, prev_sum = 0.0;
    int i, iter = 0;
    
    /* Проверка корректности границ */
    if (a >= b) {
        printf("Ошибка: левая граница должна быть меньше правой\n");
        return 0.0;
    }
    
    do {
        h = (b - a) / n;
        sum = f(a) + f(b);  /* Значения на концах отрезка */
        
        /* Суммируем значения в нечетных точках (коэффициент 4) */
        for (i = 1; i < n; i += 2) {
            sum += 4 * f(a + i * h);
        }
        
        /* Суммируем значения в четных точках (коэффициент 2) */
        for (i = 2; i < n; i += 2) {
            sum += 2 * f(a + i * h);
        }
        
        sum *= h / 3;  /* Формула Симпсона */
        
        iter++;
        
        /* Проверяем достижение точности */
        if (iter > 1 && fabs(sum - prev_sum) < eps) {
            break;
        }
        
        prev_sum = sum;
        n *= 2;  /* Удваиваем число разбиений */
        
        /* Предотвращаем бесконечный цикл */
        if (n > 1000000) {
            printf("Предупреждение: превышено максимальное число разбиений в integral()\n");
            break;
        }
    } while (1);
    
    if (iterations) {
        *iterations = iter;
    }
    
    return sum;
}

/**
 * Выводит справочную информацию по ключам командной строки
 */
void print_help(void) {
    printf("Программа для вычисления площади, образованной тремя кривыми:\n");
    printf("  f1(x) = 0.6x + 3\n");
    printf("  f2(x) = (x-2)^3 - 1\n");
    printf("  f3(x) = 3/x\n\n");
    printf("Допустимые ключи командной строки:\n");
    printf("  --help            - вывод данной справочной информации\n");
    printf("  --print-cross     - печать абсцисс точек пересечения кривых\n");
    printf("  --print-iter      - печать числа итераций для поиска корней и интегрирования\n");
    printf("  --test            - запуск тестового режима\n");
    printf("  --eps <значение>  - задание точности вычислений (по умолчанию %g)\n", EPS_DEFAULT);
}

/**
 * Тестирование функций root и integral
 */
void run_tests(void) {
    double result;
    int iterations;
    double eps = 1e-6;
    
    printf("=== ТЕСТИРОВАНИЕ ФУНКЦИЙ ===\n\n");
    
    /* Тест 1: решение уравнения x^2 = 4 на отрезке [1, 3] (корень x=2) */
    printf("Тест 1: root(x^2, 4, 2x, 0, 1, 3, eps)\n");
    printf("Ожидаемый результат: x ≈ 2.000000\n");
    result = root(
        test_pow2, 
        test_const4,
        test_derivative_pow2,
        test_derivative_const,
        1, 3, eps, &iterations);
    printf("Полученный результат: x = %f, итераций: %d\n", result, iterations);
    printf("Погрешность: %e\n\n", fabs(result - 2.0));
    
    /* Тест 2: решение уравнения sin(x) = 0.5 на отрезке [0, 1] (корень x=π/6≈0.523599) */
    printf("Тест 2: root(sin, 0.5, cos, 0, 0, 1, eps)\n");
    printf("Ожидаемый результат: x ≈ 0.523599\n");
    result = root(sin, test_const05, cos, test_derivative_const, 0, 1, eps, &iterations);
    printf("Полученный результат: x = %f, итераций: %d\n", result, iterations);
    printf("Погрешность: %e\n\n", fabs(result - M_PI/6));
    
    /* Тест 3: вычисление интеграла от x^2 на отрезке [0, 1] (точное значение 1/3 ≈ 0.333333) */
    printf("Тест 3: integral(x^2, 0, 1, eps)\n");
    printf("Ожидаемый результат: I ≈ 0.333333\n");
    result = integral(test_pow2, 0, 1, eps, &iterations);
    printf("Полученный результат: I = %f, итераций: %d\n", result, iterations);
    printf("Погрешность: %e\n\n", fabs(result - 1.0/3.0));
    
    /* Тест 4: вычисление интеграла от sin(x) на отрезке [0, π] (точное значение 2.0) */
    printf("Тест 4: integral(sin, 0, M_PI, eps)\n");
    printf("Ожидаемый результат: I = 2.000000\n");
    result = integral(sin, 0, M_PI, eps, &iterations);
    printf("Полученный результат: I = %f, итераций: %d\n", result, iterations);
    printf("Погрешность: %e\n\n", fabs(result - 2.0));
    
    printf("=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===\n");
}

/* Функции для вычисления разности между верхней и нижней функциями на разных отрезках */
double diff_func1(double x, double (*upper)(double), double (*lower)(double)) {
    return upper(x) - lower(x);
}

double diff_func2(double x, double (*upper)(double), double (*lower)(double)) {
    return upper(x) - lower(x);
}

/* Обертки для передачи в integral */
double diff_wrapper1(double x) {
    /* Эти указатели будут установлены в main */
    extern double (*g_upper1)(double);
    extern double (*g_lower1)(double);
    return g_upper1(x) - g_lower1(x);
}

double diff_wrapper2(double x) {
    extern double (*g_upper2)(double);
    extern double (*g_lower2)(double);
    return g_upper2(x) - g_lower2(x);
}

/* Глобальные переменные для передачи функций в обертки */
double (*g_upper1)(double) = NULL;
double (*g_lower1)(double) = NULL;
double (*g_upper2)(double) = NULL;
double (*g_lower2)(double) = NULL;

/* Основная функция */
int main(int argc, char *argv[]) {
    int i;
    double eps = EPS_DEFAULT;
    int print_cross = 0;
    int print_iter = 0;
    int test_mode = 0;
    
    /* Точки пересечения кривых */
    double x12, x13, x23;  /* абсциссы точек пересечения */
    double area;            /* вычисленная площадь */
    
    /* Числа итераций */
    int iter12 = 0, iter13 = 0, iter23 = 0;
    int iter_integral1 = 0, iter_integral2 = 0;
    
    /* Анализ параметров командной строки */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (strcmp(argv[i], "--print-cross") == 0) {
            print_cross = 1;
        } else if (strcmp(argv[i], "--print-iter") == 0) {
            print_iter = 1;
        } else if (strcmp(argv[i], "--test") == 0) {
            test_mode = 1;
        } else if (strcmp(argv[i], "--eps") == 0) {
            if (i + 1 < argc) {
                eps = atof(argv[++i]);
                if (eps <= 0) {
                    printf("Ошибка: точность должна быть положительным числом\n");
                    return 1;
                }
            } else {
                printf("Ошибка: после --eps должно следовать значение точности\n");
                return 1;
            }
        } else {
            printf("Неизвестный ключ: %s\n", argv[i]);
            printf("Используйте --help для получения справки\n");
            return 1;
        }
    }
    
    /* Запуск тестового режима, если запрошено */
    if (test_mode) {
        run_tests();
        return 0;
    }
    
    printf("Вычисление площади, образованной кривыми:\n");
    printf("f1(x) = 0.6x + 3\n");
    printf("f2(x) = (x-2)^3 - 1\n");
    printf("f3(x) = 3/x\n");
    printf("Точность вычислений: %g\n\n", eps);
    
    /* Находим точки пересечения кривых */
    
    /* Пересечение f1 и f2 */
    printf("Поиск точки пересечения f1 и f2...\n");
    x12 = root(f1, f2, f1_derivative, f2_derivative, -5, 5, eps, &iter12);
    
    /* Пересечение f1 и f3 */
    printf("Поиск точки пересечения f1 и f3...\n");
    x13 = root(f1, f3, f1_derivative, f3_derivative, 0.1, 5, eps, &iter13);
    
    /* Пересечение f2 и f3 */
    printf("Поиск точки пересечения f2 и f3...\n");
    x23 = root(f2, f3, f2_derivative, f3_derivative, 0.1, 5, eps, &iter23);
    
    /* Печать абсцисс точек пересечения, если запрошено */
    if (print_cross) {
        printf("\nАбсциссы точек пересечения кривых:\n");
        printf("  f1 и f2: x = %f\n", x12);
        printf("  f1 и f3: x = %f\n", x13);
        printf("  f2 и f3: x = %f\n", x23);
    }
    
    /* Печать числа итераций, если запрошено */
    if (print_iter) {
        printf("\nЧисло итераций для поиска корней:\n");
        printf("  f1 и f2: %d итераций\n", iter12);
        printf("  f1 и f3: %d итераций\n", iter13);
        printf("  f2 и f3: %d итераций\n", iter23);
    }
    
    /* Сортируем точки пересечения для определения порядка интегрирования */
    double points[3] = {x12, x13, x23};
    
    /* Простая сортировка пузырьком */
    for (i = 0; i < 2; i++) {
        for (int j = 0; j < 2 - i; j++) {
            if (points[j] > points[j + 1]) {
                double temp = points[j];
                points[j] = points[j + 1];
                points[j + 1] = temp;
            }
        }
    }
    
    printf("\nОтсортированные точки пересечения: [%f, %f, %f]\n", 
           points[0], points[1], points[2]);
    
    /**
     * Вычисление площади:
     * На каждом отрезке между точками пересечения нужно определить,
     * какая функция является верхней, а какая нижней, и вычислить
     * интеграл от их разности.
     */
    
    area = 0.0;
    
    /* Отрезок [points[0], points[1]] */
    printf("\nВычисление интеграла на отрезке [%f, %f]...\n", points[0], points[1]);
    
    /* Определяем, какая функция больше на этом отрезке */
    double mid = (points[0] + points[1]) / 2;
    double f1_mid = f1(mid);
    double f2_mid = f2(mid);
    double f3_mid = f3(mid);
    
    /* Находим максимальную и минимальную функции в средней точке */
    double (*upper)(double);
    double (*lower)(double);
    
    /* Определяем верхнюю функцию */
    if (f1_mid >= f2_mid && f1_mid >= f3_mid) {
        upper = f1;
        printf("  Верхняя функция: f1 (%.2fx+3)\n", 0.6);
    } else if (f2_mid >= f1_mid && f2_mid >= f3_mid) {
        upper = f2;
        printf("  Верхняя функция: f2 ((x-2)^3-1)\n");
    } else {
        upper = f3;
        printf("  Верхняя функция: f3 (3/x)\n");
    }
    
    /* Определяем нижнюю функцию */
    if (f1_mid <= f2_mid && f1_mid <= f3_mid) {
        lower = f1;
        printf("  Нижняя функция: f1 (%.2fx+3)\n", 0.6);
    } else if (f2_mid <= f1_mid && f2_mid <= f3_mid) {
        lower = f2;
        printf("  Нижняя функция: f2 ((x-2)^3-1)\n");
    } else {
        lower = f3;
        printf("  Нижняя функция: f3 (3/x)\n");
    }
    
    /* Устанавливаем глобальные указатели для первого отрезка */
    g_upper1 = upper;
    g_lower1 = lower;
    
    /* Вычисляем интеграл от разности верхней и нижней функций */
    area += integral(diff_wrapper1, points[0], points[1], eps, &iter_integral1);
    
    if (print_iter) {
        printf("  итераций интегрирования: %d\n", iter_integral1);
    }
    
    /* Отрезок [points[1], points[2]] */
    printf("\nВычисление интеграла на отрезке [%f, %f]...\n", points[1], points[2]);
    
    mid = (points[1] + points[2]) / 2;
    f1_mid = f1(mid);
    f2_mid = f2(mid);
    f3_mid = f3(mid);
    
    /* Определяем верхнюю функцию для второго отрезка */
    if (f1_mid >= f2_mid && f1_mid >= f3_mid) {
        upper = f1;
        printf("  Верхняя функция: f1 (%.2fx+3)\n", 0.6);
    } else if (f2_mid >= f1_mid && f2_mid >= f3_mid) {
        upper = f2;
        printf("  Верхняя функция: f2 ((x-2)^3-1)\n");
    } else {
        upper = f3;
        printf("  Верхняя функция: f3 (3/x)\n");
    }
    
    /* Определяем нижнюю функцию для второго отрезка */
    if (f1_mid <= f2_mid && f1_mid <= f3_mid) {
        lower = f1;
        printf("  Нижняя функция: f1 (%.2fx+3)\n", 0.6);
    } else if (f2_mid <= f1_mid && f2_mid <= f3_mid) {
        lower = f2;
        printf("  Нижняя функция: f2 ((x-2)^3-1)\n");
    } else {
        lower = f3;
        printf("  Нижняя функция: f3 (3/x)\n");
    }
    
    /* Устанавливаем глобальные указатели для второго отрезка */
    g_upper2 = upper;
    g_lower2 = lower;
    
    area += integral(diff_wrapper2, points[1], points[2], eps, &iter_integral2);
    
    if (print_iter) {
        printf("  итераций интегрирования: %d\n", iter_integral2);
    }
    
    printf("\n========================================\n");
    printf("РЕЗУЛЬТАТ: Площадь, образованная кривыми = %f\n", fabs(area));
    printf("========================================\n");
    
    return 0;
}