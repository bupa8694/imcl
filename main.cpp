// Copyright [2024] <Macx Buddhi Chaturanga>
#include "imcl.h"
#include <iostream>
#include <windows.h> // Required for SEH

int main(int argc, char *argv[]) {

  std::cout << "Hello, World!" << std::endl;
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

    return 0;
  }
}