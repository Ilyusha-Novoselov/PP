#include <iostream>
#include <cmath>
#include <mpi.h>
#include <vector>
#include <algorithm>

// Размеры глобальной сетки
constexpr size_t NX = 50;
constexpr size_t NY = 50;
constexpr size_t NZ = 80;

// Границы и длина области
constexpr double MINX = -1.0, MINY = -1.0, MINZ = -1.0;
constexpr double DX = 2.0, DY = 2.0, DZ = 2.0;

// Параметры метода
constexpr size_t MAX_ITER = 15000;
constexpr double EPS = 1e-8;

// Искомая функция или граничное условие
inline double Phi (double x, double y, double z) {
    return x * x + y * y + z * z;
}

// Функция правой части уравнения Пуассона
inline double RightSideFunction (double x, double y, double z) {
    return 6.0;
}

int main (int argc, char** argv) {
    MPI_Init (&argc, &argv);

    int aRank, aSize;
    MPI_Comm_rank (MPI_COMM_WORLD, &aRank);
    MPI_Comm_size (MPI_COMM_WORLD, &aSize);

    // Проверяем, чтобы кол-во слоёв было кратно кол-ву процессов
    if (NZ % aSize != 0) {
        if (aRank == 0) {
            std::cerr << "NZ must be divisible by the number of processes." << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Пространственные шаги
    double Hx = DX / (NX - 1);
    double Hy = DY / (NY - 1);
    double Hz = DZ / (NZ - 1);

    // Коэффициенты для формулы Якоби
    double Cx = 1.0 / (Hx * Hx);
    double Cy = 1.0 / (Hy * Hy);
    double Cz = 1.0 / (Hz * Hz);
    double Cc = 2.0 * (Cx + Cy + Cz);

    // Определим локальный размер по Z с учетом "призрачных" слоев
    size_t localNZ = NZ / aSize;
    size_t aLocalNZ = localNZ + 2; // +2 призрачных слоя

    // Лямбда-функция для подсчета индекса в линейном массиве для трехмерного пр-ва
    auto ComputeIndex = [&](size_t i, size_t j, size_t k) {
        return (i * (NY + 1) + j) * aLocalNZ + k;
    };

    // 3D массив: U[i][j][k] => U[(i * NY + j) * aLocalNZ + k]
    std::vector <double> U ((NX + 1) * (NY + 1) * aLocalNZ, 0.0);
    std::vector <double> U_New = U;

    // Инициализация граничных условий и начальных значений
    for (size_t i = 0; i <= NX; ++i) {
        for (size_t j = 0; j <= NY; ++j) {
            for (size_t k = 0; k < aLocalNZ; ++k) {
                // Определяется какой глобальный индекс соответствует текущему локальному индексу k для данного процесса
                size_t globalK = aRank * localNZ + k - 1;
                if (globalK >= NZ) {
                    continue;
                }

                double x = MINX + i * Hx;
                double y = MINY + j * Hy;
                double z = MINZ + globalK * Hz;

                // Проверка на граничное условие
                if (i == 0 || i == NX || j == 0 || j == NY || globalK == 0 || globalK == NZ - 1) {
                    U[ComputeIndex(i,j,k)] = U_New[ComputeIndex(i,j,k)] = Phi(x, y, z);
                }
            }
        }
    }

    // Буферы обмена границ
    std::vector <double> aSendUp((NX + 1)*(NY + 1)), aSendDown((NX + 1)*(NY + 1));
    std::vector <double> aRecvUp((NX + 1)*(NY + 1)), aRecvDown((NX + 1)*(NY + 1));

    double aLocalErr = 0, aGlobalErr = 1;
    size_t anIterations = 0;
    double aStart = MPI_Wtime();

    double anExchangeTime = 0.;
    double aComputeTime = 0.;

    for (; aGlobalErr > EPS && anIterations < MAX_ITER; ++anIterations) {
        double anExchangeStart = MPI_Wtime();

        MPI_Request aReqs[4];
        int reqsCount = 0;

        // Асинхронный обмен
        if (aRank != aSize - 1) {
            for (size_t i = 0; i <= NX; ++i) {
                for (size_t j = 0; j <= NY; ++j) {
                    aSendUp[i * (NY + 1) + j] = U[ComputeIndex(i, j, aLocalNZ - 2)];
                }
            }
    
            MPI_Isend (aSendUp.data(), aSendUp.size(), MPI_DOUBLE, aRank + 1, 0, MPI_COMM_WORLD, &aReqs[reqsCount++]);
            MPI_Irecv (aRecvUp.data(), aRecvUp.size(), MPI_DOUBLE, aRank + 1, 1, MPI_COMM_WORLD, &aReqs[reqsCount++]);
        }
    
        if (aRank != 0) {
            for (size_t i = 0; i <= NX; ++i) {
                for (size_t j = 0; j <= NY; ++j) {
                    aSendDown[i * (NY + 1) + j] = U[ComputeIndex(i, j, 1)];
                }
            }

            MPI_Isend (aSendDown.data(), aSendDown.size(), MPI_DOUBLE, aRank - 1, 1, MPI_COMM_WORLD, &aReqs[reqsCount++]);
            MPI_Irecv (aRecvDown.data(), aRecvDown.size(), MPI_DOUBLE, aRank - 1, 0, MPI_COMM_WORLD, &aReqs[reqsCount++]);
        }

        double anExchangeEnd= MPI_Wtime();
        anExchangeTime += (anExchangeEnd - anExchangeStart);

        double aComputeStart = MPI_Wtime();
    
        // Вычисляем внутренние точки, НЕ зависящие от границ
        aLocalErr = 0.0;
        for (size_t i = 1; i < NX; ++i) {
            for (size_t j = 1; j < NY; ++j) {
                for (size_t k = 2; k < aLocalNZ - 2; ++k) {
                    size_t globalK = aRank * localNZ + k - 1;
                    double x = MINX + i * Hx;
                    double y = MINY + j * Hy;
                    double z = MINZ + globalK * Hz;
    
                    double aPhi = (
                        Cx * (U[ComputeIndex(i+1,j,k)] + U[ComputeIndex(i-1,j,k)]) +
                        Cy * (U[ComputeIndex(i,j+1,k)] + U[ComputeIndex(i,j-1,k)]) +
                        Cz * (U[ComputeIndex(i,j,k+1)] + U[ComputeIndex(i,j,k-1)]) -
                        RightSideFunction (x,y,z)
                    ) / Cc;
    
                    aLocalErr = std::max (aLocalErr, std::abs (aPhi - U[ComputeIndex(i,j,k)]));
                    U_New[ComputeIndex (i,j,k)] = aPhi;
                }
            }
        }
    
        // Дождаться обменов
        if (reqsCount > 0) {
            MPI_Waitall (reqsCount, aReqs, MPI_STATUSES_IGNORE);
        }

        // Записать полученные граничные слои
        if (aRank != aSize - 1) {
            for (size_t i = 0; i <= NX; ++i) {
                for (size_t j = 0; j <= NY; ++j) {
                    U[ComputeIndex(i, j, aLocalNZ - 1)] = aRecvUp[i * (NY + 1) + j];
                }
            }
        }

        if (aRank != 0) {
            for (size_t i = 0; i <= NX; ++i) {
                for (size_t j = 0; j <= NY; ++j) {
                    U[ComputeIndex(i, j, 0)] = aRecvDown[i * (NY + 1) + j];
                }
            }
        }

        // Теперь считаем ГРАНИЧНЫЕ слои
        for (size_t i = 1; i < NX; ++i) {
            for (size_t j = 1; j < NY; ++j) {
                for (size_t k : {1ul, aLocalNZ - 2}) {
                    size_t globalK = aRank * localNZ + k - 1;
                    if (globalK == 0 || globalK == NZ - 1) {
                        continue;
                    }

                    double x = MINX + i * Hx;
                    double y = MINY + j * Hy;
                    double z = MINZ + globalK * Hz;
    
                    double aPhi = (
                        Cx * (U[ComputeIndex(i+1,j,k)] + U[ComputeIndex(i-1,j,k)]) +
                        Cy * (U[ComputeIndex(i,j+1,k)] + U[ComputeIndex(i,j-1,k)]) +
                        Cz * (U[ComputeIndex(i,j,k+1)] + U[ComputeIndex(i,j,k-1)]) -
                        RightSideFunction (x,y,z)
                    ) / Cc;
    
                    aLocalErr = std::max (aLocalErr, std::abs (aPhi - U[ComputeIndex(i,j,k)]));
                    U_New[ComputeIndex (i,j,k)] = aPhi;
                }
            }
        }
        double aComputeEnd = MPI_Wtime();
        aComputeTime += (aComputeEnd - aComputeStart);

        // Обмен указателей (U_New -> U, U_New заполнится на следующей итерации)
        std::swap (U, U_New);

        MPI_Allreduce (&aLocalErr, &aGlobalErr, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    }

    double anEnd = MPI_Wtime();

    // Сбор и вывод замеров времени для каждого процесса
    double aTimes[2] = { anExchangeTime, aComputeTime };
    std::vector <double> anAllTimes;

    if (aRank == 0) {
        anAllTimes.resize (aSize * 2);
    }

    MPI_Gather (aTimes, 2, MPI_DOUBLE, anAllTimes.data(), 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (aRank == 0) {
        std::cout << anIterations << " " << aGlobalErr << " " << (anEnd - aStart) << std::endl;

        for (size_t i = 0; i < aSize; ++i) {
            double anExchange = anAllTimes[i * 2];
            double aCompute = anAllTimes[i * 2 + 1];
            std::cout << i << " " << anExchange << " " << aCompute << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
