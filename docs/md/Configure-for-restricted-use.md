# Configure for restricted use

TuYuePDF can be configured for restricted use.

A restricted mode is useful if you want to use TuYuePDF as a bundled viewer for your program's documentation or in kiosk mode

In restricted mode some actions that are not appropriate in such context are disabled:

- opening new files
- launching URLs from within PDF document
- text and image selection
- printing
- changing default settings
- saving to disk
- automatic and manual update checks
- a history of recently opened files
- TeX preview support
- registering as a default PDF viewer
- opening with Adobe Acrobat
- e-mailing PDF

To restrict TuYuePDF put file `tuyuepdfrestrict.ini` in the same directory where `TuYuePDF.exe` is.

Here's a [full documentation of available options](https://github.com/sumatrapdfreader/sumatrapdf/blob/master/docs/sumatrapdfrestrict.ini).