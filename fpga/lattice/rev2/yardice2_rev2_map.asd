[ActiveSupport MAP]
Device = LCMXO2-4000HE;
Package = TQFP144;
Performance = 5;
LUTS_avail = 4320;
LUTS_used = 1029;
FF_avail = 4435;
FF_used = 768;
INPUT_LVCMOS25 = 9;
OUTPUT_LVCMOS25 = 12;
OUTPUT_SSTL25D_I = 1;
OUTPUT_SSTL25_I = 18;
BIDI_LVCMOS25 = 16;
BIDI_SSTL25_I = 18;
IO_avail = 115;
IO_used = 75;
EBR_avail = 10;
EBR_used = 8;
; Begin EBR Section
Instance_Name = aux_mem/mem0;
Type = DP8KC;
Width_A = 9;
Width_B = 7;
Depth_A = 256;
Depth_B = 256;
REGMODE_A = NOREG;
REGMODE_B = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
WRITEMODE_A = READBEFOREWRITE;
WRITEMODE_B = READBEFOREWRITE;
GSR = DISABLED;
Instance_Name = desc_mem/n26440;
Type = PDPW8KC;
Width = 16;
Depth_R = 256;
Depth_W = 256;
REGMODE = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = desc_mem/n26430;
Type = PDPW8KC;
Width = 16;
Depth_R = 256;
Depth_W = 256;
REGMODE = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = ptr_mem/mem0;
Type = PDPW8KC;
Width = 16;
Depth_R = 256;
Depth_W = 256;
REGMODE = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = rx_mem/mem1;
Type = PDPW8KC;
Width = 16;
Depth_R = 512;
Depth_W = 512;
REGMODE = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = rx_mem/mem0;
Type = PDPW8KC;
Width = 16;
Depth_R = 512;
Depth_W = 512;
REGMODE = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = tx_mem/mem0;
Type = PDPW8KC;
Width = 16;
Depth_R = 512;
Depth_W = 512;
REGMODE = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = tx_mem/mem1;
Type = PDPW8KC;
Width = 16;
Depth_R = 512;
Depth_W = 512;
REGMODE = NOREG;
RESETMODE = SYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
; End EBR Section
