# Using OpenCppCoverage

I tried [OpenCppCoverage](https://opencppcoverage.codeplex.com/) (https://github.com/Unity-Technologies/OpenCppCoverage) 0.9.3 on TuYuePDF.

I installed 64-bit version from the installer on Win 10. I ran `OpenCppCoverage.exe --sources sumatra -- .\dbg64\TuYuePDF.exe`, opened a file, closed the program.

`--sources` limits reporting to only files whose paths match a given pattern. Using sumatra for pattern matches all our source files, assuming the sources were checked out to `tuyuepdf` directory (because `sumatra` matches `tuyuepdf` in file path) and skips the sources for C/C++ libraries.

It generated `CoverageReport-${date}` directory with HTML report. There's top-level index.html with links to html file for each module (TuYuePDF.exe in our case).

They recommend running on debug code.

At the end it reports generating reports for system dlls, but also says can't get symbols, so it doesn't actually generate anything.

The html report isn't great. Another option would be to use `--export_type=binary` option which generates `TuYuePDF.cov` file and write html generator myself. I assume the format of `.cov` file is simple.