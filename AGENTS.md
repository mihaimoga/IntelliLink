# AGENTS.md

Welcome to the **IntelliLink** repository. This document provides technical context, architecture details, build instructions, coding standards, and operational guidelines for AI coding agents and human developers working in this solution.

---

## 1. Project Overview

**IntelliLink** is a lightweight, high-performance Windows desktop hyperlink manager written in C++ using Win32 API, MFC (Microsoft Foundation Classes), and modern C++ STL. It features an MFC Ribbon interface, WebView2 browser integration, system tray integration, XML data persistence, and automated updates via `genUp4win`.

- **Primary Language**: C++ (C++17 / C++20 / C++23 compatible)
- **Frameworks**: Win32 API, MFC (Static Link), Microsoft Edge WebView2
- **License**: GNU General Public License v3.0 (GPL-3.0)
- **Target OS**: Windows 10 / 11 (WindowsTargetPlatformVersion `10.0`)
- **Supported Platforms**: `Win32` (x86) and `x64`
- **Supported Configurations**: `Debug` and `Release`

---

## 2. Repository & Solution Structure

```text
.
├── IntelliLink.sln                 # Visual Studio solution file
├── IntelliLink.vcxproj             # Main MFC application project file
├── IntelliLink.vcxproj.filters     # Visual Studio project filters
├── packages.config                 # NuGet package definitions (WebView2, WIL)
│
├── IntelliLink.h / .cpp            # Application entry point and CWinAppEx lifecycle
├── MainFrame.h / .cpp              # Main application frame (Ribbon UI, tray notifications)
├── ChildView.h / .cpp              # Container view for child controls
├── LinkView.h / .cpp               # Hyperlink view management
├── LinkList.h / .cpp               # Hyperlink data models and list management
├── LinkPropertiesDlg.h / .cpp      # Add / Edit hyperlink dialog
├── WebBrowserDlg.h / .cpp          # Embedded web browser dialog
├── EdgeWebBrowser.h / .cpp         # Microsoft Edge WebView2 control wrapper
├── CheckForUpdatesDlg.h / .cpp     # Update checker dialog (genUp4win integration)
│
├── MFCListView.h / .cpp            # Custom MFC list view implementation
├── SortListCtrl.h / .cpp           # Header-sorted list control
├── HLinkCtrl.h / .cpp              # Hyperlink static control (PJ Naughter)
├── NTray.h / .cpp                  # Shell notify icon / system tray (PJ Naughter)
├── sinstance.h / .cpp              # Single instance checker (PJ Naughter)
├── VersionInfo.h / .cpp            # Version resource reader (PJ Naughter)
├── tinyxml2.h / .cpp               # XML parser for link serialization (Lee Thomason)
│
├── res/                            # Application icons, bitmaps, ribbons (.mfcribbon-ms)
├── hlp/                            # HTML Help assets and topics (.htm, .hhc, .hhk, .hhp)
│
├── genUp4win/                      # Submodule: Generic updater library
│   ├── genUp4win.vcxproj           # Visual Studio project for updater DLL
│   ├── CMakeLists.txt              # CMake build definition
│   ├── genUp4win.h / .cpp          # Updater library implementation
│   ├── SHA256.h / .cpp             # Checksum validation
│   └── AppSettings.h               # XML/Registry application settings
│
├── CONTRIBUTING.md                 # Contribution rules and coding style guidelines
├── CODE_OF_CONDUCT.md              # Contributor Covenant Code of Conduct
├── README.md                       # Project overview and user documentation
└── AGENTS.md                       # AI agent instructions and repository context
```

---

## 3. Technology Stack & Dependencies

### Third-Party & Bundled Libraries
- **genUp4win**: Embedded updater DLL supporting HTTPS download and XML configuration.
- **TinyXML2**: Lightweight DOM-based XML parser for link storage.
- **PJ Naughter MFC Components**:
  - `CHLinkCtrl` (Static hyperlink control)
  - `CInstanceChecker` (Single application instance prevention)
  - `CTrayNotifyIcon` (System tray integration)
  - `CVersionInfo` (Extracting version info from binary resources)

### NuGet Packages (`packages.config`)
- `Microsoft.Web.WebView2` (Native WebView2 SDK)
- `Microsoft.Windows.ImplementationLibrary` (WIL)

---

## 4. Build and Environment Setup

### Prerequisites
- **Visual Studio 2022 / 2026** with:
  - Desktop development with C++
  - C++ MFC for latest v143/v145 build tools (x86 & x64)
  - Windows 10/11 SDK
- **MSBuild** or **Visual Studio IDE**
- **CMake** (3.15+) for optional CMake-based building of `genUp4win`

### Building the Solution

#### Via MSBuild:
```powershell
# Restore NuGet packages
nuget restore IntelliLink.sln

# Build Release x64
msbuild IntelliLink.sln /p:Configuration=Release /p:Platform=x64

# Build Debug x64
msbuild IntelliLink.sln /p:Configuration=Debug /p:Platform=x64

# Build Release Win32
msbuild IntelliLink.sln /p:Configuration=Release /p:Platform=Win32
```

#### Building genUp4win Submodule with CMake:
```bash
cd genUp4win
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

---

## 5. Key Architecture & Workflows

1. **Application Lifecycle (`CIntelliLinkApp`)**:
   - Initializes COM and MFC Ribbon state.
   - Enforces single instance check using `CInstanceChecker`.
   - Loads custom user configurations and saved links from XML storage.

2. **Main User Interface (`CMainFrame`)**:
   - Manages ribbon categories, quick access toolbar, and status bar.
   - Hosts `CChildView` / `CLinkView` displaying the sorted hyperlinks.
   - Interacts with `CTrayNotifyIcon` for minimize-to-tray and tray notifications.

3. **Link Management (`CLinkList`, `CLinkPropertiesDlg`)**:
   - Stores links with metadata (URL, description, state).
   - Serializes and deserializes link items to/from XML via `TinyXML2`.

4. **Web Browser View (`CEdgeWebBrowser`, `CWebBrowserDlg`)**:
   - Wraps Microsoft Edge WebView2 for rendering in-app web views.

5. **Auto-Update Workflow (`CCheckForUpdatesDlg`, `genUp4win`)**:
   - Background worker thread checks remote XML update manifest.
   - Validates SHA256 hashes of update packages.
   - Launches installer upon user confirmation.

---

## 6. Coding Standards & Conventions

Agents must strictly adhere to the project's coding standards as specified in `CONTRIBUTING.md`:

### Formatting & Indentation
- **Indentation**: Use **Tabs** (tab width = 4 spaces). Do not convert tabs to spaces.
- **Braces (Allman Style)**: Opening and closing braces must be on their own lines:
  ```cpp
  void MyClass::method1()
  {
	  if (aCondition)
	  {
		  // Action
	  }
  }
  ```
  *Exception*: Single-line method definitions in header files (`.h`) may use compact braces.
- **Spacing**:
  - Always leave one space before and after binary and ternary operators (`a == 10 && b == 42`).
  - No space between function name and opening parenthesis (`foo(arg1, arg2)`).
  - Exactly one space between keywords and opening parenthesis (`if (condition)`, `while (loop)`).
  - Exactly one space after semicolons in `for` loops (`for (int i = 0; i != 10; ++i)`).

### Naming Conventions
- **Classes / Structs**: `PascalCase` (e.g., `CIntelliLinkApp`, `CLinkView`).
- **Methods & Function Parameters**: `camelCase` (e.g., `refreshLinks()`, `myVeryLongParameter`).
- **Member Variables**: Preceded by an underscore `_` (e.g., `_linkCount`, `_pPrivateAttribute`).
- **Descriptive Names**: Avoid single-character or cryptic names; use descriptive identifiers.

### C++ Best Practices
- **Standard**: Modern C++ (C++17/C++20/C++23) constructs where appropriate.
- **Casts**: Always use C++ style casts (`static_cast`, `reinterpret_cast`) instead of C-style casts `(type)val`.
- **Booleans & Strings**:
  - Use `!string.empty()` rather than `string != ""`.
  - Use logical operators `!`, `&&`, `||` (do not use alternative tokens `not`, `and`, `or`).
- **Pointers & Memory**:
  - Prefer references over pointers.
  - Avoid raw `new`/`delete`; prefer automatic stack variables or `std::unique_ptr`.
  - Avoid `std::shared_ptr` unless shared ownership is strictly required.
- **Loops & Iteration**: Prefer pre-increment (`++i`) over post-increment (`i++`).
- **Headers**: Never put `using namespace` directives inside header (`.h`) files.
- **Constants**: Prefer `enum` or `enum class` over raw magic numbers.

---

## 7. Testing & Verification

- Verify clean compilation using MSBuild (`run_build` tool) across both `Win32` and `x64` configurations when applicable.
- Confirm MFC resource headers (`Resource.h`, `IntelliLink.rc`) remain consistent when modifying UI elements or string tables.
- Ensure no memory leaks or unhandled exceptions are introduced in Win32 / MFC message handlers.

---

## 8. Agent Operating Rules

1. **Respect Coding Style**: Match existing style in the file being edited. Use tabs for indentation.
2. **Atomic & Minimal Edits**: Make focused modifications that directly address the user's request without unnecessary refactoring.
3. **Keep Resources in Sync**: When modifying UI IDs, update `Resource.h` and `IntelliLink.rc` in harmony.
4. **Preserve Compatibility**: Keep compatibility with MSVC compiler toolsets (`v143`/`v145`) and Unicode character set configurations.
