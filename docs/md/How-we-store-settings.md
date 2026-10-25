# How we store settings

## Where we store settings

Persisted data is stored in `TuYuePDF-settings.txt` file.

In portable version the file is stored in the same directory as TuYuePDF executable. In non-portable version, it's in `%LOCALAPPDATA%\TuYuePDF` directory.

Starting with version 1.6 we also persist thumbnails for "Frequently read" list. They are stored in subdirectory `tuyuepdfcache` as `.png` files.

See [https://www.sumatrapdfreader.org/settings/settings](https://www.sumatrapdfreader.org/settings/settings) for information about all the settings.
