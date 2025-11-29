#ifndef KATAGO_HPP
#define KATAGO_HPP

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <windows.h> // The essential header
#include <atomic>

class KataGoManager {
public:
    KataGoManager();
    void set(const std::string& workingDir);
    ~KataGoManager();

    void sendCommand(const std::string& cmd);
    bool hasReply();
    std::string getReply();
    std::string waitForReply(int maxTimeoutMiliseconds);

    int get_exit_status();
private:
    // --- Windows Process Handles and Structures ---
    HANDLE hProcess = NULL;     // The main handle to the process itself
    HANDLE hThread = NULL;      // The main thread of the process
    HANDLE hReadPipe = NULL;    // The pipe handle used to READ KataGo's stdout
    HANDLE hWritePipe = NULL;   // The pipe handle used to WRITE to KataGo's stdin

    std::atomic<bool> keepRunning{ true };

    // --- C++ Threading and I/O ---
    std::thread readThread;     // Thread to continuously read stdout without blocking SFML
    std::queue<std::string> replyQueue;
    std::mutex queueMutex;
    std::string lineBuffer;

    // --- Private Methods ---
    bool createPipes(SECURITY_ATTRIBUTES* sa, HANDLE& hChildStdinRd, HANDLE& hChildStdoutWr);
    void readOutputLoop();      // The function run by readThread
};

#endif