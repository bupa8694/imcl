// Copyright [2024] <Macx Buddhi Chaturanga>
#include "imcl.h"
#include <iostream>
#ifdef __linux__
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <fstream>
#include <string>
#endif

// Function prototypes for security checks
bool checkForDebugger();
bool checkForHooks();
bool checkTimeConsistency();
bool checkVirtualMachine();

int main(int argc, char *argv[]) {
  std::cout << "Hello, World!" << std::endl;
  // Security checks
  if (checkForDebugger()) {
    // Silently exit or take evasive action
    std::cout << "Debugger detected" << std::endl;
    exit(0);
  }
  
  if (checkForHooks()) {
    // Silently exit or take evasive action
    std::cout << "Hooks detected" << std::endl;
    exit(0);
  }
  
  if (checkTimeConsistency()) {
    // Silently exit or take evasive action
    std::cout << "Time consistency check failed" << std::endl;
    exit(0);
  }
  
  if (checkVirtualMachine()) {
    // Silently exit or take evasive action
    std::cout << "Virtual machine detected" << std::endl;
    exit(0);
  }

  IMCL_STATUS status = imcl_init();
  switch (status) {
  case IMCL_FAILURE:
    std::cout << "Failed to initialize IMCL" << std::endl;
    return 1; // Return error code
  case IMCL_SUCCESS:
    std::cout << "IMCL initialized successfully" << std::endl;
    break;
  default:
    std::cout << "Unknown status returned" << std::endl;
    return 1;
  }
  
  return 0;
}

// Anti-debugging techniques
bool checkForDebugger() {
#ifdef __linux__
  // Method 1: Use ptrace to detect debugger
  // If we can't ptrace the process, it means another debugger is attached
  if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
    return true;
  }
  // Detach after check
  ptrace(PTRACE_DETACH, 0, 1, 0);
  
  // Method 2: Check for tracerpid in /proc/self/status
  std::ifstream status("/proc/self/status");
  std::string line;
  while (std::getline(status, line)) {
    if (line.substr(0, 10) == "TracerPid:") {
      int pid = std::stoi(line.substr(10));
      if (pid != 0) {
        return true;
      }
      break;
    }
  }
  
  // Method 3: Timing check (debuggers slow down execution)
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  
  // Some operation that should be very fast
  for (volatile int i = 0; i < 1000; i++) {}
  
  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  
  if (elapsed > 0.001) { // Threshold may need adjustment
    return true;
  }
#elif defined(_WIN32) || defined(_WIN64)
  // Windows-specific anti-debugging
  if (IsDebuggerPresent()) {
    return true;
  }
  
  // Check for remote debugger
  BOOL debuggerPresent = FALSE;
  CheckRemoteDebuggerPresent(GetCurrentProcess(), &debuggerPresent);
  if (debuggerPresent) {
    return true;
  }
  
  // Timing check for Windows
  LARGE_INTEGER start, end, freq;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&start);
  
  // Some operation that should be very fast
  int counter = 0;
  for (int i = 0; i < 1000; i++) { counter++; } // Removed volatile to fix warning
  
  QueryPerformanceCounter(&end);
  double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
  
  if (elapsed > 0.001) { // Threshold may need adjustment
    return true;
  }
#endif
  
  return false;
}

// Anti-hooking techniques
bool checkForHooks() {
#ifdef __linux__
  // Check for LD_PRELOAD environment variable
  char* preload = getenv("LD_PRELOAD");
  if (preload != NULL && strlen(preload) > 0) {
    return true;
  }
  
  // Check for suspicious loaded libraries
  std::ifstream maps("/proc/self/maps");
  std::string line;
  while (std::getline(maps, line)) {
    if (line.find("inject") != std::string::npos || 
        line.find("hook") != std::string::npos) {
      return true;
    }
  }
#elif defined(_WIN32) || defined(_WIN64)
  // Check for suspicious loaded modules in Windows
  HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
  if (hModuleSnap != INVALID_HANDLE_VALUE) {
    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);
    
    if (Module32First(hModuleSnap, &me32)) {
      do {
        std::string moduleName = me32.szModule;
        if (moduleName.find("inject") != std::string::npos ||
            moduleName.find("hook") != std::string::npos) {
          CloseHandle(hModuleSnap);
          return true;
        }
      } while (Module32Next(hModuleSnap, &me32));
    }
    CloseHandle(hModuleSnap);
  }
#endif
  
  return false;
}

// Anti-time tampering
bool checkTimeConsistency() {
#ifdef __linux__
  // Store initial time
  static time_t lastTime = 0;
  static struct timespec lastMonotonic = {0, 0};
  
  if (lastTime == 0) {
    lastTime = time(NULL);
    clock_gettime(CLOCK_MONOTONIC, &lastMonotonic);
    return false;
  }
  
  // Current time
  time_t currentTime = time(NULL);
  struct timespec currentMonotonic;
  clock_gettime(CLOCK_MONOTONIC, &currentMonotonic);
  
  // Check for time going backward (tampering)
  if (currentTime < lastTime) {
    return true;
  }
  
  // Calculate time differences
  double monotonicDiff = (currentMonotonic.tv_sec - lastMonotonic.tv_sec) + 
                         (currentMonotonic.tv_nsec - lastMonotonic.tv_nsec) / 1000000000.0;
  double timeDiff = difftime(currentTime, lastTime);
  
  // If system time changed significantly but monotonic time didn't
  if (timeDiff > 60 && monotonicDiff < 60) {
    return true;
  }
  
  // Update values
  lastTime = currentTime;
  lastMonotonic = currentMonotonic;
#elif defined(_WIN32) || defined(_WIN64)
  // Windows time consistency check
  static ULARGE_INTEGER lastTime = {0, 0};
  static LARGE_INTEGER lastQPC = {0};
  static LARGE_INTEGER qpcFreq = {0};
  
  if (lastTime.QuadPart == 0) {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    lastTime.LowPart = ft.dwLowDateTime;
    lastTime.HighPart = ft.dwHighDateTime;
    
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&lastQPC);
    return false;
  }
  
  // Get current times
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  ULARGE_INTEGER currentTime;
  currentTime.LowPart = ft.dwLowDateTime;
  currentTime.HighPart = ft.dwHighDateTime;
  
  LARGE_INTEGER currentQPC;
  QueryPerformanceCounter(&currentQPC);
  
  // Check for time going backward
  if (currentTime.QuadPart < lastTime.QuadPart) {
    return true;
  }
  
  // Calculate time differences
  double qpcDiff = (double)(currentQPC.QuadPart - lastQPC.QuadPart) / qpcFreq.QuadPart;
  double systemTimeDiff = (currentTime.QuadPart - lastTime.QuadPart) / 10000000.0; // Convert 100ns to seconds
  
  // If system time changed significantly but QPC didn't
  if (systemTimeDiff > 60 && qpcDiff < 60) {
    return true;
  }
  
  // Update values
  lastTime = currentTime;
  lastQPC = currentQPC;
#endif
  
  return false;
}

// Virtual machine detection
bool checkVirtualMachine() {
#ifdef __linux__
  // Method 1: Check for VM-specific files or directories
  const char* vmIndicators[] = {
    "/sys/devices/virtual/dmi/id/product_name",
    "/sys/devices/virtual/dmi/id/sys_vendor",
    "/proc/scsi/scsi"
  };
  
  for (const char* path : vmIndicators) {
    std::ifstream file(path);
    if (file.is_open()) {
      std::string content;
      std::getline(file, content);
      file.close();
      
      if (content.find("VMware") != std::string::npos ||
          content.find("VirtualBox") != std::string::npos ||
          content.find("QEMU") != std::string::npos ||
          content.find("KVM") != std::string::npos) {
        return true;
      }
    }
  }
  
  // Method 2: Check for VM-specific processes
  DIR* dir = opendir("/proc");
  if (dir != NULL) {
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
      // Check if the entry is a directory and the name is a number (process ID)
      if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
        char cmdlinePath[256];
        snprintf(cmdlinePath, sizeof(cmdlinePath), "/proc/%s/cmdline", entry->d_name);
        
        std::ifstream cmdline(cmdlinePath);
        if (cmdline.is_open()) {
          std::string cmd;
          std::getline(cmdline, cmd);
          cmdline.close();
          
          if (cmd.find("vmtoolsd") != std::string::npos ||
              cmd.find("VBoxService") != std::string::npos) {
            closedir(dir);
            return true;
          }
        }
      }
    }
    closedir(dir);
  }
#elif defined(_WIN32) || defined(_WIN64)
  // Windows VM detection
  
  // Check for VM-specific services and drivers
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot != INVALID_HANDLE_VALUE) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(hSnapshot, &pe32)) {
      do {
        std::string processName = pe32.szExeFile;
        if (processName.find("vmtoolsd") != std::string::npos ||
            processName.find("VBoxService") != std::string::npos ||
            processName.find("vmware") != std::string::npos) {
          CloseHandle(hSnapshot);
          return true;
        }
      } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
  }

  // Check system firmware tables for VM indicators
 char buffer[1024];
    DWORD size = GetSystemFirmwareTable('RSMB', 0, buffer, sizeof(buffer)); // Fixed multi-character constant
    if (size > 0) {
      std::string firmware(buffer, buffer + size);
      std::cout << firmware << std::endl;
      if (firmware.find("VMware") != std::string::npos ||
          firmware.find("VirtualBox") != std::string::npos) {
        return true;
      }
    }
  std::cout<<"No VM detected"<<std::endl;
  // Check registry for VM indicators
  HKEY hKey;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
    char subKeyName[256];
    DWORD nameSize = 255;
    DWORD index = 0;
    
    while (RegEnumKeyEx(hKey, index, subKeyName, &nameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
      std::string keyName = subKeyName;
      if (keyName.find("vmware") != std::string::npos ||
          keyName.find("vbox") != std::string::npos) {
        RegCloseKey(hKey);
        return true;
      }
      index++;
      nameSize = 255; // Reset nameSize for next iteration
    }
    RegCloseKey(hKey);
  }
#endif
  
  return false;
}