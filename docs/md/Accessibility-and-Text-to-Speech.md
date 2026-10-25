# Accessibility and Text-to-Speech

**This page is outdated. Accessibility support is not complete and therefore disabled.**

TuYuePDF prerelease supports experimental UIA accessibility API, which allows for example screen readers to read out loud the selected text in document.

## Accessibility in TuYuePDF plugin

Accessibility features are considered experimental and accessibility is not enabled at the moment in the TuYuePDF plugin.

## Usage

### Microsoft Narrator

- Start Microsoft Narrator
- Start TuYuePDF
- Open document in TuYuePDF
- Select text. Microsoft Narrator will now read out the selected text.

Known issue: Sometimes Narrator does not read the selection. This seems to be a focus related problem and selecting another window and then back the TuYuePDF window may fix the problem.

## Supported configurations

### Supported file types

PDF, XPS, DjVu

### Supported Clients

- Microsoft Narrator

### Unsupported Clients

- [NVDA](https://community.nvda-project.org/)

# Technical Documentation

This section is documentation for TuYuePDF and other developers.

## UIAutomation element structure when a document is loaded

```
Window
 |-> FragmentRoot
     Name: "Canvas"
     ControlType: UIA_CustomControlTypeId
      |
      |-> Fragment
          Name: [filename]
          ControlType: UIA_DocumentControlTypeId 
          NativeWindowHandle: 0
          Patterns: ITextProvider
            |
            | -> Fragment
            |    Name: "Page 1"
            |    Patterns: IValueProvider
            -
            -
            |
            | -> Fragment
                 Name: "Page n"
                 Patterns: IValueProvider
```

## UIAutomation element structure when no document is loaded

```
Window
 |-> FragmentRoot
     Name: "Canvas"
     ControlType: UIA_CustomControlTypeId
      |
      |-> Fragment
          Name: "Start Page"
```