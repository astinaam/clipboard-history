I want to build a **lightweight Linux clipboard history app** (like Windows 11 clipboard manager) with the following requirements:

## Core Features
- Runs on **Linux (X11 + Wayland if possible)**.
- Tracks clipboard text (start with text only).
- Stores clipboard history (configurable limit, e.g. last 20–50 items).
- Supports **pinning items** so they remain at the top of the history.
- **Meta+V global hotkey** opens the clipboard history popup window.
- A **system tray icon** allows opening the popup manually.
- Popup shows clipboard items in a simple vertical list.
  - Most recent at the top.
  - Each item can be clicked to:
    - Paste it directly into the currently focused input/editor window
- Minimal modern UI (GTK or Qt, whichever is easiest for Copilot).
- Very few dependencies, small memory footprint.

## Tech Guidance
- Use **Qt6 Widgets or QML** for the UI.
- Use **QClipboard** for clipboard integration.
- Use **QSystemTrayIcon** for the tray icon.
- Implement **global hotkey** (Meta+V) using:
  - `QxtGlobalShortcut` (if available), or
  - X11 XGrabKey / Wayland alternatives.
- Store configuration/history in **JSON** (`~/.config/clipboard-manager/history.json`).
- Keep architecture modular:
  - `ClipboardManager` — handles clipboard monitoring, history, pinning.
  - `ClipboardWindow` — popup UI showing history list.
  - `TrayIcon` — system tray integration.
- Code must be **lightweight, fast, cross-platform and readable**. Avoid unnecessary features or bloat.

## Stretch Goals (optional for later)
- Handle images in the clipboard.
- Search box inside popup to filter history.
- Support dark/light themes automatically.

## Stretch Goals
- Support images in the clipboard (optional later).
- Search/filter inside popup.
- Dark/light theme support.

## Development Guidance
1. First generate a **minimal working prototype**:
   - Capture clipboard changes
   - Save them to a list
   - Show popup with `Meta+V` (or a fallback hotkey if needed)
   - Allow selecting an item and copying it back
2. Then add:
   - Pinned items
   - Tray icon
   - Paste-to-focused-window support
3. Keep code modular and documented for future extension.
4. No unnecessary bloat — **only clipboard history, pinning, hotkey, tray**.
5. Add **paste into focused input/editor** functionality.
