/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */

void FreeAcceleratorTables();
void CreateTuYueAcceleratorTable();
HACCEL* GetAcceleratorTables();
TempStr AppendAccelKeyToMenuStringTemp(TempStr str, int cmdId);
bool IsValidShortcutString(const char* shortcut);
