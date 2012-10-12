#!/bin/sed -f

# Handle registers
/[ \t]*[A-Za-z_0-9]\+.*register.*[ ]\+([A-Z_][A-Za-z_0-9]*)/N;s/[ \t]*\([A-Za-z_0-9]\+.*\)[ ]\+(\([A-Z_][A-Za-z_0-9]*\)).*\n[ \t]*Address offset: \(0x[0-9a-fA-F]\+\).*/\n\/* \1 *\/\n#define \U\2\E \3\n/

# Comment reserved multiple bits 
s/[ \t]*Bit[s]* \([1-3]\{,1\}[0-9]\):\([1-3]\{,1\}[0-9]\)[ ]*[\n]*\(Reserved.*\)/\n\/* [\1..\2] \3 *\/\n/

# Comment reserved single bits 
s/[ \t]*Bit[s]* \([1-3]\{,1\}[0-9]\)[ ]*[\n]*\(Reserved.*\)/\n\/* Bit \1 \2 *\/\n/

# Handle multiple bits field
s/[ \t]*Bit[s]* \([1-3]\{,1\}[0-9]\):\([1-3]\{,1\}[0-9]\)[ ]\+\([A-Z_][A-Za-z_0-9]*\)\[\([1-3]\{,1\}[0-9]\):\([1-3]\{,1\}[0-9]\)]:[ ]\+\(.*\)/\n\/* Bits [\1..\2] - \6 *\/\n#define \U\3\E_MSK (((1 << (\4 + 1)) - 1) << \2)\n#define \U\3\E_SET(VAL) (((VAL) << \2) \& \U\3\E_MSK)\n#define \U\3\E_GET(REG) (((REG) \& \U\3\E_MSK) >> \2)\n\/* /

# Handle multiple bits field
s/[ \t]*Bit[s]* \([1-3]\{,1\}[0-9]\):\([1-3]\{,1\}[0-9]\)[ ]\+\([A-Z_][A-Za-z_0-9]*\)\:[ ]\+\(.*\)/\n\/* Bits [\1..\2] - \4 *\/\n#define \U\3\E ((\1 - \2) << \2)\n\/* /

# Handle single bit field
s/[ \t]*Bit[s]* \([1-3]\{,1\}[0-9]\)[ ]\+\([A-Z_][A-Za-z_0-9]*\):[ ]\+\(.*\)/\n\/* Bit \1 - \3 *\/\n#define \U\2\E (1 << \1)\n\/* /

s/.*Reset value:.*//

# Handle descriptions */
#s/\(#define .*\n\)\(\([^\/][^*]\)*\n\)\+\(\/\*\) /\1 ==== \2 ==== \3/
#s/\(#define .*\n\)\(.*\n\)\+\(\/\*\)/#if \1 ==== \2 ==== \3/
