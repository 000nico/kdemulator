#include <vector>
#pragma pack(push, 1)

// DOS MZ Header 
struct IMAGE_DOS_HEADER {
    unsigned short e_magic;    // +00: Magic Number MZ ($5A4D)
    unsigned short e_cblp;     // +02: Bytes on last page of file
    unsigned short e_cp;       // +04: Pages in file
    unsigned short e_crlc;     // +06: Relocations
    unsigned short e_cparhdr;  // +08: Size of header in paragraphs
    unsigned short e_minalloc; // +0A: Minimum extra paragraphs needed
    unsigned short e_maxalloc; // +0C: Maximum extra paragraphs needed
    unsigned short e_ss;       // +0E: Initial (relative) SS value
    unsigned short e_sp;       // +10: Initial SP value
    unsigned short e_csum;     // +12: Checksum
    unsigned short e_ip;       // +14: Initial IP value
    unsigned short e_cs;       // +16: Initial (relative) CS value
    unsigned short e_lfarlc;   // +18: File address of relocation table
    unsigned short e_ovno;     // +1A: Overlay number
    unsigned short e_res[4];   // +1C: Reserved words
    unsigned short e_oemid;    // +24: OEM identifier
    unsigned short e_oeminfo;  // +26: OEM information
    unsigned short e_res2[10]; // +28: Reserved words
    unsigned long  e_lfanew;   // +3C: File address of new exe header
};

// PE Header / File Header 
struct IMAGE_FILE_HEADER {
    unsigned long  Signature;             // +00: Signature ($00004550)
    unsigned short Machine;               // +04 (0x8664 for x64)
    unsigned short NumberOfSections;      // +06
    unsigned long  TimeDateStamp;         // +08
    unsigned long  PointerToSymbolTable;  // +0C
    unsigned long  NumberOfSymbols;       // +10
    unsigned short SizeOfOptionalHeader;  // +14
    unsigned short Characteristics;       // +16
};

// Optional Header x64 
struct IMAGE_OPTIONAL_HEADER {
    // Standard fields
    unsigned short       Magic;                       // +18 (0x20B for x64)
    unsigned char        MajorLinkerVersion;          // +1A
    unsigned char        MinorLinkerVersion;          // +1B
    unsigned long        SizeOfCode;                  // +1C
    unsigned long        SizeOfInitializedData;       // +20
    unsigned long        SizeOfUninitializedData;     // +24
    unsigned long        AddressOfEntryPoint;         // +28
    unsigned long        BaseOfCode;                  // +2C

    // NT additional fields (x64 con campos de 8 bytes)
    unsigned long long   ImageBase;                   // +30
    unsigned long        SectionAlignment;            // +38
    unsigned long        FileAlignment;               // +3C
    unsigned short       MajorOperatingSystemVersion; // +40
    unsigned short       MinorOperatingSystemVersion; // +42
    unsigned short       MajorImageVersion;           // +44
    unsigned short       MinorImageVersion;           // +46
    unsigned short       MajorSubsystemVersion;       // +48
    unsigned short       MinorSubsystemVersion;       // +4A
    unsigned long        Reserved1;                   // +4C
    unsigned long        SizeOfImage;                 // +50
    unsigned long        SizeOfHeaders;               // +54
    unsigned long        CheckSum;                    // +58
    unsigned short       Subsystem;                   // +5C
    unsigned short       DllCharacteristics;          // +5E
    unsigned long long   SizeOfStackReserve;          // +60
    unsigned long long   SizeOfStackCommit;           // +68
    unsigned long long   SizeOfHeapReserve;           // +70
    unsigned long long   SizeOfHeapCommit;            // +78
    unsigned long        LoaderFlags;                 // +80
    unsigned long        NumberOfRvaAndSizes;         // +84

    // Data Directories
    unsigned long  ExportDirectoryVA;           // +88
    unsigned long  ExportDirectorySize;         // +8C
    unsigned long  ImportDirectoryVA;           // +90
    unsigned long  ImportDirectorySize;         // +94
    unsigned long  ResourceDirectoryVA;         // +98
    unsigned long  ResourceDirectorySize;       // +9C
    unsigned long  ExceptionDirectoryVA;        // +A0
    unsigned long  ExceptionDirectorySize;      // +A4
    unsigned long  SecurityDirectoryVA;         // +A8
    unsigned long  SecurityDirectorySize;       // +AC
    unsigned long  BaseRelocationTableVA;       // +B0
    unsigned long  BaseRelocationTableSize;     // +B4
    unsigned long  DebugDirectoryVA;            // +B8
    unsigned long  DebugDirectorySize;          // +BC
    unsigned long  ArchitectureSpecificDataVA;  // +C0
    unsigned long  ArchitectureSpecificDataSize;// +C4
    unsigned long  RVAofGPVA;                   // +C8
    unsigned long  RVAofGPSize;                 // +CC
    unsigned long  TLSDirectoryVA;              // +D0
    unsigned long  TLSDirectorySize;            // +D4
    unsigned long  LoadConfigurationDirectoryVA;// +D8
    unsigned long  LoadConfigurationDirectorySize;// +DC
    unsigned long  BoundImportDirectoryVA;      // +E0
    unsigned long  BoundImportDirectorySize;    // +E4
    unsigned long  ImportAddressTableVA;        // +E8
    unsigned long  ImportAddressTableSize;      // +EC
    unsigned long  DelayLoadImportDescriptorsVA;// +F0
    unsigned long  DelayLoadImportDescriptorsSize;// +F4
    unsigned long  COMRuntimedescriptorVA;      // +F8
    unsigned long  COMRuntimedescriptorSize;    // +FC
    unsigned long  Reserved_0_1;                // +100
    unsigned long  Reserved_0_2;                // +104
};

// Section Header 
struct IMAGE_SECTION_HEADER {
    unsigned char  Name[8];                     // +00
    unsigned long  PhysicalAddress_VirtualSize; // +08
    unsigned long  VirtualAddress;              // +0C
    unsigned long  SizeOfRawData;               // +10
    unsigned long  PointerToRawData;            // +14
    unsigned long  PointerToRelocations;        // +18
    unsigned long  PointerToLineNumbers;        // +1C
    unsigned short NumberOfRelocations;         // +20
    unsigned short NumberOfLineNumbers;         // +22
    unsigned long  Characteristics;             // +24
};

// Export Directory 
struct IMAGE_EXPORT_DIRECTORY {
    unsigned long  Characteristics;             // +00
    unsigned long  TimeDateStamp;               // +04
    unsigned short MajorVersion;                // +08
    unsigned short MinorVersion;                // +0A
    unsigned long  Name;                        // +0C
    unsigned long  Base;                        // +10
    unsigned long  NumberOfFunctions;           // +14
    unsigned long  NumberOfNames;               // +18
    unsigned long  AddressOfFunctions;          // +1C
    unsigned long  AddressOfNames;              // +20
    unsigned long  AddressOfNameOrdinals;       // +24
};

// Import Directory / Import Descriptor
struct IMAGE_IMPORT_DESCRIPTOR {
    union {
        unsigned long Characteristics;
        unsigned long OriginalFirstThunk;       // +00
    };
    unsigned long  TimeDateStamp;               // +04
    unsigned long  ForwarderChain;              // +08
    unsigned long  Name;                        // +0C
    unsigned long  FirstThunk;                  // +10
};

struct PE {
    IMAGE_DOS_HEADER image_dos_header;
    IMAGE_FILE_HEADER image_file_header;
    IMAGE_OPTIONAL_HEADER image_optional_header;
    std::vector<IMAGE_SECTION_HEADER> sections;
    std::vector<uint8_t> raw_data;
};

#pragma pack(pop)