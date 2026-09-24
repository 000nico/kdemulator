#pragma once

// memory
#define STACK_BASE              0x0000000700000000ULL
#define STACK_SIZE              0x00500000ULL           // 5 mb

#define HEAP_BASE               0x0000000800000000ULL
#define HEAP_SIZE               0x01000000ULL           // 16 mb

#define NON_PAGED_POOL_BASE     0xFFFFFA8000000000ULL
#define NON_PAGED_POOL_SIZE     0x01000000ULL
#define PAGED_POOL_BASE         0xFFFFC00000000000ULL
#define PAGED_POOL_SIZE         0x01000000ULL

// images
#define NTOSKRNL_BASE           0xFFFFF80000000000ULL
#define CODE_BASE               0x0000000140000000ULL   // where .sys gets mapped

// structures
//
// STRUCT_BASE layout:
//   +0x000  DRIVER_OBJECT         (0x150)
//   +0x150  DRIVER_EXTENSION      (0x028)
//   +0x178  DriverName buffer     (0x080)
//   +0x1F8  HardwareDatabase buf  (0x100)
//   +0x2F8  default dispatch stub (0x001)
//
// DEVICE_BASE layout:
//   +0x000  DEVICE_OBJECT         (0x120)
//   +0x120  DeviceExtension       (0x1E0)
//
#define STRUCT_BASE             0x0000000030000000ULL
#define DEVICE_BASE             0x0000000030001000ULL

// KUSER_SHARED_DATA
//   kernel VA:   0xFFFFF78000000000  (mapped by the kernel, read/write)
//   usermode VA: 0x7FFE0000          (same physical page, read-only)
//
#define KUSER_BASE              0xFFFFF78000000000ULL

// hooks
#define HOOK_TRAP_BASE          0x0000000020000000ULL
#define HOOK_TRAP_SIZE          0x1000

// device object layout offsets
#define OFF_DEVOBJ  0x000
#define OFF_DEVEXT  0x120