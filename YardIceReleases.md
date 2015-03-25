# Introduction #

This page contains periodic firmware releases binaries.

# YARD-ICE Application #

The application should be loaded at address 0x08000000 of the flash memory.
Hardware revisions 0.1 and 0.2 share the same IO so the same firmware applies.

  * [Version 0.2.22](https://yard-ice.googlecode.com/svn/trunk/src/release/yard-ice-0.2.22.bin)


# FPGA Program #

The FPGA program should be loaded at address 0x08060000 of the flash memory.
There is a difference in the pin assignments between [revision 0](https://code.google.com/p/yard-ice/source/detail?r=0).1 and 0.2 of the boards. Each board revision have it's own binary (RBF) program.

## Hardware 0.1 ##
  * [Altera  Release 0.1 (RBF)](https://yard-ice.googlecode.com/files/yard_ice-0.1.rbf)

## Hardware 0.2 ##
  * [Altera Release 0.2 (RBF)](https://yard-ice.googlecode.com/svn/trunk/src/fpga/release/yard_ice-0.2.rbf)


# Combined Application+FPGA #

These files are single .FDU packages which combine both the YARD-ICE Application and FPGA program.

## Hardware 0.1 ##
  * [YARD-ICE-0.1 Version 0.2.22 (FDU)](https://yard-ice.googlecode.com/svn/branches/rev0.1/src/release/yard-ice-r0.1-v0.2.22.dfu)

## Hardware 0.2 ##
  * [YARD-ICE-0.2 Version 0.2.22 (FDU)](https://yard-ice.googlecode.com/svn/branches/rev0.2/src/release/yard-ice-r0.2-v0.2.22.dfu)

## DFU File Upgrade - How-To ##

  * Disconnect the power and USB from the YARD-ICE
  * Insert the P2 jumper in YARD-ICE board
  * Connect the USB cable
  * Run the `STMicroelectronics->DfuSe->DfuSe Demonstration`
  * The **`DfuSe Demo`** application should show `STM Device in DFU Mode` under the `Available DFU Devices`
  * Under "Upgrade or Veriy Action" press the `[Choose...]` button
  * Select the **yard-yce.dfu** firmware file. See the next section on how to create the DFU file.
  * Press the `[Upgrade]` button
  * After writing the firmware, power cycle the system without jumper P2.

![https://yard-ice.googlecode.com/svn/trunk/doc/png/DfuSe-Demo-Yard-ICE-Upgrade.png](https://yard-ice.googlecode.com/svn/trunk/doc/png/DfuSe-Demo-Yard-ICE-Upgrade.png)