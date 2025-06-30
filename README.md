# Custom Linux Kernel Drivers

This repository contains a collection of simple Linux kernel modules and character device drivers, primarily for educational purposes and targeted at the BeagleBone Black (ARM architecture). Each subfolder is a self-contained kernel module project with its own source and Makefile.

## Contents

### 001-hello-world
A minimal kernel module that prints a message when loaded and unloaded. Useful as a first test of kernel module development and cross-compilation.

- **Source:** `main.c`
- **Build:**
  ```sh
  make
  ```
- **Description:**
  - Prints a greeting to the kernel log on insertion and a farewell on removal.
  - Demonstrates the basics of module_init/module_exit.

### 002-pseudo-char-driver
A simple pseudo character device driver that implements open, read, write, lseek, and release operations.

- **Source:** `main.c`
- **Build:**
  ```sh
  make
  ```
- **Description:**
  - Allocates a 512-byte buffer as device memory.
  - Registers a single character device with the kernel.
  - Supports basic file operations from userspace.
- **Usage:**
  - After inserting the module, a device node (e.g., `/dev/pcd_devices`) will be created.
  - You can read/write to the device using standard tools:
    ```sh
    echo "HELLO THERE" | sudo tee /dev/pcd_devices
    cat /dev/pcd_devices
    ```

### 003-pseudo-char-driver-multi-device
A more advanced pseudo character driver that supports multiple devices, each with its own buffer, permissions, and serial number.

- **Source:** `pcd_n.c`
- **Build:**
  ```sh
  make
  ```
- **Description:**
  - Creates 4 character devices (e.g., `/dev/pcdev-1`, `/dev/pcdev-2`, ...).
  - Each device has different buffer sizes and permissions (read-only, write-only, read-write).
  - Demonstrates handling multiple devices and device-specific data in a single driver.

## Building the Drivers

All Makefiles are set up for cross-compiling for ARM (BeagleBone Black). You may need to adjust the `KERN_DIR` variable in the Makefiles to point to your kernel source tree.

- To build for ARM:
  ```sh
  make
  ```
- To build for the host (your current Linux machine):
  ```sh
  make host
  ```
- To clean build artifacts:
  ```sh
  make clean
  ```

## Prerequisites
- Linux kernel headers/source for your target platform
- ARM cross-compiler (e.g., `arm-linux-gnueabihf-gcc`)
- Sufficient privileges to insert/remove kernel modules (usually `sudo`)

## License
All code is licensed under the GPL. See individual source files for author information.

---

*Happy hacking with Linux device drivers!* 