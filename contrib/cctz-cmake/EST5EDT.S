// Embed a binary file into an executable.

// The variable BINARY_FILE_NAME is the actual name of the file to include
// The variable SYMBOL_NAME is the "normalized" name of the symbol, with
// symbols like `-`, `.`, and `/` replaced with `_`. This is to match how
// objcopy rewrites symbol names, and matches the expectation in
// `base/common/getResource.cpp`

    .data
    .global _binary_EST5EDT_start
_binary_EST5EDT_start:
    .incbin "EST5EDT"
    .global _binary_EST5EDT_end
_binary_EST5EDT_end:
    .global _binary_EST5EDT_size
_binary_EST5EDT_size:
    .quad _binary_EST5EDT_end - _binary_EST5EDT_start
