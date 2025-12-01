#include <KataGo.hpp>
#include <iostream>
#include <stdexcept>
#include <tchar.h> // For strcpy_s and TCHAR functions

#include <SFML/Graphics.hpp>

// Default Constructor
KataGoManager::KataGoManager() = default;

// Parameterized Constructor / Setup Function
void KataGoManager::set(const std::string& workingDir) {
    if (hProcess) return; // Already running

    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE; // Pipes must be inherited by the child

    HANDLE hChildStdinRd, hChildStdoutWr;

    // Create pipes and get the Child's handles back
    if (!createPipes(&sa, hChildStdinRd, hChildStdoutWr)) {
        throw std::runtime_error("Pipe creation failed.");
    }

    STARTUPINFOA si; // Use A for ANSI/char* strings
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // Redirect child process I/O to our pipes
    si.hStdError = hReadPipe;   // Child stderr -> Parent read pipe
    si.hStdOutput = hReadPipe;  // Child stdout -> Parent read pipe
    si.hStdInput = hWritePipe;  // Child stdin <- Parent write pipe
    si.dwFlags |= STARTF_USESTDHANDLES; // Tell Windows to use these handles!

    // You must give the Child process the CHILD handles, not the Parent handles.
    // -------------------------------------------------------------
    si.hStdInput = hChildStdinRd;    // Child READS from this
    si.hStdOutput = hChildStdoutWr;  // Child WRITES to this
    si.hStdError = hChildStdoutWr;   // Child WRITES errors here too

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Assuming BASE_DIR is defined as "D:\\GameProjects\\GoGame\\include\\KataGo\\"

    // --- PREPARATION ---

    // 1. Define the ABSOLUTE path to the executable
    const std::string ABS_EXE_PATH = workingDir + "katago.exe";

    // 2. Create the MUTABLE buffers (Must be large enough)
    char mutableAppPath[2048];
    char mutableCmdLine[2048];

    // Copy the absolute executable path for lpApplicationName
    strcpy_s(mutableAppPath, sizeof(mutableAppPath), ABS_EXE_PATH.c_str());

    // The command line must be MUTABLE and start with the executable name
    // NOTE: It is best practice to pass the absolute path here too, or at least 
    // ensure the first token is the full path for maximum reliability.
    // Let's combine the full path with the command line args for maximum compatibility.
    const std::string FULL_COMMAND_WITH_PATH = ABS_EXE_PATH + " gtp -model model.txt.gz -config analysis.cfg";
    strcpy_s(mutableCmdLine, sizeof(mutableCmdLine), FULL_COMMAND_WITH_PATH.c_str());

    // --- LAUNCH ---

    BOOL success = CreateProcessA(
        NULL,       // 1. Application Name (Must be the ABSOLUTE path of the EXE)
        mutableCmdLine,       // 2. Command Line (Must be mutable char*, starting with the full path)
        NULL, NULL, TRUE, 0, NULL,
        workingDir.c_str(),   // 3. Working directory
        &si, &pi
    );


    CloseHandle(hChildStdinRd);
    CloseHandle(hChildStdoutWr);

    if (!success) {
        // 4. Retrieve and throw the specific Windows error code!
        DWORD errorCode = GetLastError();
        throw std::runtime_error("CreateProcess failed. Error code: " + std::to_string(errorCode));
    }

    // Store primary handles
    hProcess = pi.hProcess;
    hThread = pi.hThread;

    // Start the asynchronous reading thread
    keepRunning = true;
    readThread = std::thread(&KataGoManager::readOutputLoop, this);
}

// Destructor (Crucial for reliable shutdown and handle cleanup)
KataGoManager::~KataGoManager() {
    if (hProcess != NULL) {
        // 1. Stop the reading loop gracefully
        keepRunning = false;

        // 2. Join the thread (It will finish in ~10ms now)
        if (readThread.joinable()) {
            readThread.join();
        }

        // 3. Send quit to KataGo (optional cleanup)
        // Note: You can try sending "quit", but since we stopped reading, 
        // we won't see the reply. That's fine.
        sendCommand("quit");

        // 4. Standard Process Cleanup
        WaitForSingleObject(hProcess, 1000); // Short wait
        if (WaitForSingleObject(hProcess, 0) != WAIT_OBJECT_0) {
            TerminateProcess(hProcess, 0);
        }

        CloseHandle(hWritePipe);
        CloseHandle(hReadPipe);
        CloseHandle(hThread);
        CloseHandle(hProcess);
        hProcess = NULL;
    }
}

bool KataGoManager::createPipes(SECURITY_ATTRIBUTES* sa, HANDLE& hChildStdinRd, HANDLE& hChildStdoutWr) {
    HANDLE hParentStdoutRd, hParentStdinWr;

    // --- 1. STDOUT PIPE (Child writes, Parent reads) ---
    // Create the pipe. Both handles are currently inheritable (from 'sa').
    if (!CreatePipe(&hParentStdoutRd, &hChildStdoutWr, sa, 0)) return false;

    // FIX: The PARENT's read handle should NOT be inherited.
    if (!SetHandleInformation(hParentStdoutRd, HANDLE_FLAG_INHERIT, 0)) return false;

    // --- 2. STDIN PIPE (Parent writes, Child reads) ---
    // Create the pipe. Both handles are currently inheritable.
    if (!CreatePipe(&hChildStdinRd, &hParentStdinWr, sa, 0)) return false;

    // FIX: The PARENT's write handle should NOT be inherited.
    // If the child inherits the write handle too, the pipe never closes properly!
    if (!SetHandleInformation(hParentStdinWr, HANDLE_FLAG_INHERIT, 0)) return false;

    // Save Parent handles to class
    hReadPipe = hParentStdoutRd;
    hWritePipe = hParentStdinWr;

    return true;
}

void KataGoManager::readOutputLoop() {
    //std::cerr << "[DEBUG] Read Thread Started.\n"; // <--- Add this

    CHAR buffer[4096];
    DWORD bytesRead;
    DWORD bytesAvail = 0;

    while (keepRunning) { // Check the flag constantly
        // ReadFile blocks, but that is fine for a dedicated thread.
        // It returns TRUE if data is read, FALSE if pipe is broken/empty.

        // 1. Peek (Instant check)
        if (!PeekNamedPipe(hReadPipe, NULL, 0, NULL, &bytesAvail, NULL)) break;

        // 2. DECIDE: Read or Sleep?
        if (bytesAvail > 0) {
            // Data exists! We can ReadFile safely because we KNOW it won't block.
            BOOL success = ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

            if (!success || bytesRead == 0) break;

            buffer[bytesRead] = '\0';

            // --- OPTIMIZATION START ---

            // 1. Process string logic WITHOUT locking the mutex.
            // 'lineBuffer' is only used by this thread, so it doesn't need a lock!
            lineBuffer.append(buffer, bytesRead);

            size_t pos = 0;
            while ((pos = lineBuffer.find('\n')) != std::string::npos) {
                std::string line = lineBuffer.substr(0, pos);

                if (!line.empty()) {
                    // 2. Lock ONLY for the tiny moment we push to the shared queue.
                    // This takes nanoseconds, freeing up your Main Thread instantly.
                    std::lock_guard<std::mutex> lock(queueMutex);
                    replyQueue.push(line);
                }

                lineBuffer.erase(0, pos + 1);
            }
            // --- OPTIMIZATION END ---
        }
        else {
            // 3. NO DATA: Sleep briefly.
            // Only sleep if the pipe was actually empty.
            // Lowered to 1ms for maximum snappiness.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    //std::cerr << "[DEBUG] Read Thread Exiting.\n";
}

void KataGoManager::sendCommand(const std::string& cmd) {
    if (hWritePipe == NULL) return;

    std::string fullCmd = cmd + "\n";
    DWORD bytesWritten;

    if (!WriteFile(
        hWritePipe,
        fullCmd.c_str(),
        (DWORD)fullCmd.length(),
        &bytesWritten,
        NULL
    )) {
        std::cerr << "WriteFile failed: Error " << GetLastError() << "\n";
        // Do not throw; let the main loop handle the failure status
    }

    // 2. FORCE FLUSH (Add this line)
    FlushFileBuffers(hWritePipe);
}

bool KataGoManager::hasReply() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return !replyQueue.empty();
}

std::string KataGoManager::getReply() {
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!replyQueue.empty()) {
        std::string s = replyQueue.front();
        replyQueue.pop();

        // Check for GTP Success (=) or Failure (?). Others are unnecessary logs
        if (s.length() > 0 && (s[0] == '=' || s[0] == '?')) {
            return s; // Found the real answer!
        }
    }

    return ""; // Queue empty, no valid reply found yet
}

std::string KataGoManager::waitForReply(int maxTimeoutMiliseconds) {
    //Getting response in time limit (counted as miliseconds)
    //10 milliseconds cooldown

    sf::Clock deltaClock;
    deltaClock.restart();

    std::string response = "";
    do {
        std::string placeholder = getReply();

        if (!placeholder.empty()) {
            response = placeholder;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (response.empty() && deltaClock.getElapsedTime().asMilliseconds() < maxTimeoutMiliseconds);

    return response;
}

int KataGoManager::get_exit_status() {
    DWORD exitCode = 0;
    if (hProcess != NULL) {
        // Check if the process is still active
        if (GetExitCodeProcess(hProcess, &exitCode)) {
            if (exitCode == STILL_ACTIVE) {
                return -1; // Standard value for running process
            }
            return (int)exitCode; // Return the actual exit code
        }
    }
    return 0; // Default error/return value if handle is invalid
}