# Introduction #

If you are a Windows user you most probably will like to have a Terminal emulator installed in your system.
Probably the best Open Source option is [Tera Term](http://ttssh2.sourceforge.jp/index.html.en).

A the time this page was created the most up to date version is [TeraTerm 4.79](http://en.sourceforge.jp/projects/ttssh2/downloads/59442/teraterm-4.79.exe/).


# Istalling VCOM Drivers #

In order to connect your terminal to YARD-ICE through the USB port, some systems may require installing a driver provided by ST: [STM32 VCOM Drivers](http://yard-ice.googlecode.com/files/stm32_vcp.zip).

The YAR-ICE implements a standard USB-CDC-ACM device. Starting from Windows XP, a generic driver is provided for this type of devices. But in some systems it's necessary to tell windows that you need to load the appropriate driver. The ST VCOM driver do not really install any binary, it just provide a hint for Windows on which driver to use.


