#ifndef _ThreadManager_HeaderFile
#define _ThreadManager_HeaderFile

namespace parallel {

class ThreadManager {
public:
    ThreadManager() = default;

    // ������ ������ ������
    static void StartOneThread();

    // ������ n �������
    static void StartNThreads (size_t N);

    // ������ ��������� ������� ������� � ���������
    static void EstimateThreads (bool aLogging);

    // ������ ������� � ������� ����������
    static void StartThreadsWithAttr();

    // ������ ������ �� ������� ����������
    static void StartThreadWithParams();
};

}

#endif
