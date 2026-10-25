# Debugging TuYue

**Help us debug TuYue hangs or crashes**

If TuYue crashes or hangs reliably on your machine but we can't reproduce it ourselves, you can help us by debugging it. Here's a tutorial on how to do it.

Please first test with latest pre-release version from [https://www.sumatrapdfreader.org/prerelease.html](https://www.sumatrapdfreader.org/prerelease.html)

Some bugs might already be fixed there compared to latest

Those instructions require TuYuePDF 3.2 or later.

## Getting logs

We log information that might be helpful in diagnosing issues.

To see the logs: `Ctrl + K` ([Command Palette](Command-Palette.md)), type `show log`

This saves logs to a file and opens default editor for `.txt` files with log file.

## Install necessary software

- Install WinDBG debugger from Microsoft Store at [https://www.microsoft.com/en-us/p/windbg-preview/9pgjgd53tn86#activetab=pivot:overviewtab](https://www.microsoft.com/en-us/p/windbg-preview/9pgjgd53tn86#activetab=pivot:overviewtab). Alternatively, you can follow instructions from [https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools)
- In TuYuePDF, use menu `Debug` / `Download symbols`. Symbols are important for the debugger (like WinDBG) to resolve addresses to names that

First, vocabulary. `%ProgramFiles%` means the standard directory where Windows installs programs. It's `c:\Program Files` on 32-bit Windows and `c:\Program Files (x86)` on 64-bit Windows.

## Debugging a crash

To debug crashes:

- start TuYuePDF.exe under the control of WinDBG
    - start WinDBG.exe
    - File/Open (Ctrl-E), find and open TuYuePDF.exe executable
    - In WinDBG, type:
        - `.sympath+ SRV*c:\symbols*https://msdl.microsoft.com/download/symbols`
        - `g`
- when TuYue crashes, type: `!analyze -v` and paste the result of that to the bug report

## Debugging a hang

Here are the steps to follow if TuYue hangs

1. Start TuYuePDF.exe and get it to hang
2. start WinDBG.exe
3. use File/Attach to process (F6) and select TuYuePDF.exe from the
4. In WinDBG, type:
    - `.sympath+ SRV*c:\symbols*https://msdl.microsoft.com/download/symbols`
    - `~*kb`
    - `lmf`

Attach the output to bug report.