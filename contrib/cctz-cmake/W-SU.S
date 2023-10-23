// Embed a binary file into an executable.

// The variable BINARY_FILE_NAME is the actual name of the file to include
// The variable SYMBOL_NAME is the "normalized" name of the symbol, with
// symbols like `-`, `.`, and `/` replaced with `_`. This is to match how
// objcopy rewrites symbol names, and matches the expectation in
// `base/common/getResource.cpp`

    .data
    .global _binary_W_SU_start
_binary_W_SU_start:
    .incbin "W-SU"
    .global _binary_W_SU_end
_binary_W_SU_end:
    .global _binary_W_SU_size
_binary_W_SU_size:
    .quad _binary_W_SU_end - _binary_W_SU_start
