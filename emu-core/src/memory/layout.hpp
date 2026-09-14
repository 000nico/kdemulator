#pragma once

// memory
#define STACK_BASE              0x0000000700000000ULL
#define STACK_SIZE              0x00500000ULL // 5 mb

#define HEAP_BASE               0x0000000800000000ULL
#define HEAP_SIZE               0x01000000ULL // 16 mb

#define NON_PAGED_POOL_BASE     0xFFFFFA8000000000
#define NON_PAGED_POOL_SIZE     0x01000000ULL
#define PAGED_POOL_BASE         0xFFFFC00000000000
#define PAGED_POOL_SIZE         0x01000000ULL

// structures
#define NTOSKRNL_BASE           0xFFFFF80000000000
#define KUSER_SHARED_DATA_BASE  0xFFFFF78000000000
#define CODE_BASE               0x0000000140000000ULL // where .sys gets mapped
#define STRUCT_BASE             0x0000000030000000ULL // for DRIVER_OBJECT, DEVICE_OBJECT, IRP, etc.

// hooks
#define HOOK_TRAP_BASE         0x0000000020000000
#define HOOK_TRAP_SIZE         0x1000