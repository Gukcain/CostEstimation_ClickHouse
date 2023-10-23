// Embed a binary file into an executable.

// The variable BINARY_FILE_NAME is the actual name of the file to include
// The variable SYMBOL_NAME is the "normalized" name of the symbol, with
// symbols like `-`, `.`, and `/` replaced with `_`. This is to match how
// objcopy rewrites symbol names, and matches the expectation in
// `base/common/getResource.cpp`

    .data
    .global _binary_Africa_Lome_start
_binary_Africa_Lome_start:
    .incbin "Africa/Lome"
    .global _binary_Africa_Lome_end
_binary_Africa_Lome_end:
    .global _binary_Africa_Lome_size
_binary_Africa_Lome_size:
    .quad _binary_Africa_Lome_end - _binary_Africa_Lome_start
