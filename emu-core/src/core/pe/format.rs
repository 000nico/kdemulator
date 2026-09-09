use std::mem;

pub struct ImageDosHeader {
    pub e_magic: u16,       // Magic Number MZ ($5A4D)
    pub e_cblp: u16,        // Bytes on last page of file
    pub e_cp: u16,          // Pages in file
    pub e_crlc: u16,        // Relocations
    pub e_cparhdr: u16,     // Size of header in paragraphs
    pub e_minalloc: u16,    // Minimum extra paragraphs needed
    pub e_maxalloc: u16,    // Maximum extra paragraphs needed
    pub e_ss: u16,          // Initial (relative) SS value
    pub e_sp: u16,          // Initial SP value
    pub e_csum: u16,        // Checksum
    pub e_ip: u16,          // Initial IP value
    pub e_cs: u16,          // Initial (relative) CS value
    pub e_lfarlc: u16,      // File address of relocation table
    pub e_ovno: u16,        // Overlay number
    pub e_res: [u16; 4],    // Reserved words
    pub e_oemid: u16,       // OEM identifier
    pub e_oeminfo: u16,     // OEM information
    pub e_res2: [u16; 10],  // Reserved words
    pub e_lfanew: u32,      // File address of new exe header
}

pub struct ImageFileHeader {
    pub signature: u32,             // ($00004550)
    pub machine: u16,
    pub number_of_sections: u16,
    pub time_date_stamp: u32,
    pub pointer_to_symbol_table: u32,
    pub number_of_symbols: u32,
    pub size_of_optional_header: u16,
    pub characteristics: u16,
}

pub struct ImageOptionalHeader32 {
    // Standard fields
    pub magic: u16,
    pub major_linker_version: u8,
    pub minor_linker_version: u8,
    pub size_of_code: u32,
    pub size_of_initialized_data: u32,
    pub size_of_uninitialized_data: u32,
    pub address_of_entry_point: u32,
    pub base_of_code: u32,
    pub base_of_data: u32,

    // NT additional fields
    pub image_base: u32,
    pub section_alignment: u32,
    pub file_alignment: u32,
    pub major_operating_system_version: u16,
    pub minor_operating_system_version: u16,
    pub major_image_version: u16,
    pub minor_image_version: u16,
    pub major_subsystem_version: u16,
    pub minor_subsystem_version: u16,
    pub reserved1: u32,
    pub size_of_image: u32,
    pub size_of_headers: u32,
    pub check_sum: u32,
    pub subsystem: u16,
    pub dll_characteristics: u16,
    pub size_of_stack_reserve: u32,
    pub size_of_stack_commit: u32,
    pub size_of_heap_reserve: u32,
    pub size_of_heap_commit: u32,
    pub loader_flags: u32,
    pub number_of_rva_and_sizes: u32,

    // Data Directories
    pub export_directory_va: u32,
    pub export_directory_size: u32,
    pub import_directory_va: u32,
    pub import_directory_size: u32,
    pub resource_directory_va: u32,
    pub resource_directory_size: u32,
    pub exception_directory_va: u32,
    pub exception_directory_size: u32,
    pub security_directory_va: u32,
    pub security_directory_size: u32,
    pub base_relocation_table_va: u32,
    pub base_relocation_table_size: u32,
    pub debug_directory_va: u32,
    pub debug_directory_size: u32,
    pub architecture_specific_data_va: u32,
    pub architecture_specific_data_size: u32,
    pub rva_of_gp_va: u32,
    pub rva_of_gp_size: u32,
    pub tls_directory_va: u32,
    pub tls_directory_size: u32,
    pub load_configuration_directory_va: u32,
    pub load_configuration_directory_size: u32,
    pub bound_import_directory_va: u32,
    pub bound_import_directory_size: u32,
    pub import_address_table_va: u32,
    pub import_address_table_size: u32,
    pub delay_load_import_descriptors_va: u32,
    pub delay_load_import_descriptors_size: u32,
    pub com_runtime_descriptor_va: u32,
    pub com_runtime_descriptor_size: u32,
    pub reserved_zero_1: u32,
    pub reserved_zero_2: u32,
}

pub struct ImageSectionHeader {
    pub name: [u8; 8],
    pub physical_address_or_virtual_size: u32,
    pub virtual_address: u32,
    pub size_of_raw_data: u32,
    pub pointer_to_raw_data: u32,
    pub pointer_to_relocations: u32,
    pub pointer_to_line_numbers: u32,
    pub number_of_relocations: u16,
    pub number_of_line_numbers: u16,
    pub characteristics: u32,
}

// bot import and export directory are not in a sequence in the headers.
// u get them with optional_header.export_directory.va and optional_header.import_directory_va
pub struct ImageExportDirectory {
    pub characteristics: u32,
    pub time_date_stamp: u32,
    pub major_version: u16,
    pub minor_version: u16,
    pub name: u32,
    pub base: u32,
    pub number_of_functions: u32,
    pub number_of_names: u32,
    pub address_of_functions: u32,
    pub address_of_names: u32,
    pub address_of_name_ordinals: u32,
}

pub struct ImageImportDescriptor {
    pub original_first_thunk: u32,
    pub time_date_stamp: u32,
    pub forwarder_chain: u32,
    pub name: u32,
    pub first_thunk: u32,
}

pub struct PeFileStructure {
    pub dos_header: ImageDosHeader,
    pub nt_signature: u32,
    pub file_header: ImageFileHeader,
    pub optional_header: ImageOptionalHeader32,
}