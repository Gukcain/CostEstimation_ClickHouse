// Embed a binary file into an executable.

// The variable BINARY_FILE_NAME is the actual name of the file to include
// The variable SYMBOL_NAME is the "normalized" name of the symbol, with
// symbols like `-`, `.`, and `/` replaced with `_`. This is to match how
// objcopy rewrites symbol names, and matches the expectation in
// `base/common/getResource.cpp`

    .data
    .global _binary_America_Indiana_Winamac_start
_binary_America_Indiana_Winamac_start:
    .incbin "America/Indiana/Winamac"
    .global _binary_America_Indiana_Winamac_end
_binary_America_Indiana_Winamac_end:
    .global _binary_America_Indiana_Winamac_size
_binary_America_Indiana_Winamac_size:
    .quad _binary_America_Indiana_Winamac_end - _binary_America_Indiana_Winamac_start
