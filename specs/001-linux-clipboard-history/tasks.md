# Tasks: Linux Clipboard History Manager

**Input**: Design documents from `/home/astinaam/projects/clipboard-history/specs/001-linux-clipboard-history/`
**Prerequisites**: plan.md (required), research.md, data-model.md, contracts/

## Execution Flow (main)
```
1. Load plan.md from feature directory
   → Found: Linux Clipboard History Manager implementation plan
   → Extracted: C++17/20 + Qt6, ClipboardManager/ClipboardWindow/TrayIcon libraries
2. Load optional design documents:
   → data-model.md: ClipboardItem, ClipboardHistory, Configuration entities
   → contracts/: clipboard-manager.md, clipboard-window.md, tray-icon.md
   → research.md: Qt6 architecture, global hotkey, persistence decisions
3. Generate tasks by category:
   → Setup: CMake project, Qt6 dependencies, C++ tooling
   → Tests: 3 contract tests, 5 integration tests (from quickstart scenarios)
   → Core: 3 model classes, 3 library implementations
   → Integration: global hotkey, system tray, clipboard monitoring
   → Polish: unit tests, performance validation, documentation
4. Apply task rules:
   → Different files = mark [P] for parallel execution
   → Library implementations = sequential (shared dependencies)
   → Tests before implementation (TDD mandatory)
5. Number tasks sequentially (T001-T028)
6. Generate dependency graph
7. Create parallel execution examples
8. Validate task completeness:
   ✓ All contracts have tests
   ✓ All entities have models
   ✓ All user scenarios covered
9. Return: SUCCESS (tasks ready for execution)
```

## Format: `[ID] [P?] Description`
- **[P]**: Can run in parallel (different files, no dependencies)
- Include exact file paths in descriptions

## Path Conventions
- **Single project**: `src/`, `tests/` at repository root
- Paths assume single desktop application structure per plan.md

## Phase 3.1: Setup
- [x] T001 Create CMake project structure with src/ and tests/ directories
- [x] T002 Initialize C++20 project with Qt6 dependencies (Widgets, Core, GUI, Test)
- [x] T003 [P] Configure clang-format and CMake linting tools

## Phase 3.2: Tests First (TDD) ⚠️ MUST COMPLETE BEFORE 3.3
**CRITICAL: These tests MUST be written and MUST FAIL before ANY implementation**
- [x] T004 [P] Contract test ClipboardManager API in tests/contract/test_clipboard_manager.cpp
- [x] T005 [P] Contract test ClipboardWindow API in tests/contract/test_clipboard_window.cpp
- [x] T006 [P] Contract test TrayIcon API in tests/contract/test_tray_icon.cpp
- [x] T007 [P] Integration test clipboard monitoring workflow in tests/integration/test_clipboard_monitoring.cpp
- [x] T008 [P] Integration test Meta+V hotkey popup in tests/integration/test_hotkey_interaction.cpp
- [x] T009 [P] Integration test item pinning workflow in tests/integration/test_pinning_workflow.cpp
- [x] T010 [P] Integration test system tray interaction in tests/integration/test_tray_integration.cpp
- [x] T011 [P] Integration test history persistence in tests/integration/test_persistence_integration.cpp

## Phase 3.3: Core Implementation (ONLY after tests are failing)
- [x] T012 [P] ClipboardItem model class in src/models/clipboard_item.cpp
- [x] T013 [P] ClipboardHistory model class in src/models/clipboard_history.cpp
- [x] T014 [P] Configuration model class in src/models/configuration.cpp
- [x] T015 ClipboardManager library implementation in src/services/clipboard_manager.cpp
- [x] T016 ClipboardWindow library implementation in src/ui/clipboard_window.cpp
- [x] T017 TrayIcon library implementation in src/ui/tray_icon.cpp
- [x] T018 Main application entry point in src/main.cpp
- [x] T019 CLI argument parsing in src/cli/argument_parser.cpp
- [x] T020 Global hotkey registration (X11/Wayland) in src/lib/global_hotkey.cpp

## Phase 3.4: Integration
- [ ] T021 Connect ClipboardManager to QClipboard monitoring
- [ ] T022 Wire ClipboardWindow to ClipboardManager signals
- [ ] T023 Integrate TrayIcon with system tray and menu actions
- [ ] T024 JSON persistence for configuration and history
- [ ] T025 Cross-platform global hotkey registration

## Phase 3.5: Polish
- [ ] T026 [P] Unit tests for ClipboardItem validation in tests/unit/test_clipboard_item.cpp
- [ ] T027 [P] Performance tests (<50ms response, <10MB memory) in tests/performance/test_performance.cpp
- [ ] T028 [P] Update README.md with build and usage instructions

## Dependencies
- Setup (T001-T003) before all other phases
- Tests (T004-T011) before implementation (T012-T025)
- Models (T012-T014) before services (T015-T017)
- T015 (ClipboardManager) blocks T021 (clipboard monitoring)
- T016 (ClipboardWindow) blocks T022 (UI integration)
- T017 (TrayIcon) blocks T023 (system tray)
- T020 (global hotkey) blocks T025 (hotkey integration)
- Implementation before polish (T026-T028)

## Parallel Example
```bash
# Launch T004-T006 together (contract tests):
Task: "Contract test ClipboardManager API in tests/contract/test_clipboard_manager.cpp"
Task: "Contract test ClipboardWindow API in tests/contract/test_clipboard_window.cpp"  
Task: "Contract test TrayIcon API in tests/contract/test_tray_icon.cpp"

# Launch T007-T011 together (integration tests):
Task: "Integration test clipboard monitoring in tests/integration/test_clipboard_monitoring.cpp"
Task: "Integration test Meta+V hotkey in tests/integration/test_hotkey_interaction.cpp"
Task: "Integration test item pinning in tests/integration/test_pinning_workflow.cpp"
Task: "Integration test system tray in tests/integration/test_tray_integration.cpp"
Task: "Integration test persistence in tests/integration/test_persistence_integration.cpp"

# Launch T012-T014 together (model classes):
✅ Task: "ClipboardItem model class in src/models/clipboard_item.cpp"
✅ Task: "ClipboardHistory model class in src/models/clipboard_history.cpp"  
✅ Task: "Configuration model class in src/models/configuration.cpp"

# Launch T015-T017 together (service classes):
✅ Task: "ClipboardManager service class in src/services/clipboard_manager.cpp"
✅ Task: "ClipboardWindow UI class in src/ui/clipboard_window.cpp"
Task: "TrayIcon UI class in src/ui/tray_icon.cpp"
```

## Notes
- [P] tasks = different files, no dependencies
- TDD mandatory: Verify tests fail before implementing
- Commit after each task completion
- Libraries follow Qt6 signal/slot patterns
- Performance targets: <50ms clipboard response, <200ms popup display

## Task Generation Rules
*Applied during main() execution*

1. **From Contracts**:
   - clipboard-manager.md → T004 contract test + T015 implementation
   - clipboard-window.md → T005 contract test + T016 implementation
   - tray-icon.md → T006 contract test + T017 implementation
   
2. **From Data Model**:
   - ClipboardItem → T012 model creation task [P]
   - ClipboardHistory → T013 model creation task [P]
   - Configuration → T014 model creation task [P]
   
3. **From User Stories** (quickstart scenarios):
   - Clipboard monitoring → T007 integration test [P]
   - Global hotkey → T008 integration test [P]
   - Item pinning → T009 integration test [P]
   - System tray → T010 integration test [P]
   - Persistence → T011 integration test [P]

4. **Ordering**:
   - Setup → Tests → Models → Services → Integration → Polish
   - Qt signal/slot dependencies prevent some parallelization

## Validation Checklist
*GATE: Checked by main() before returning*

- [x] All contracts have corresponding tests (T004-T006)
- [x] All entities have model tasks (T012-T014)
- [x] All tests come before implementation (T004-T011 before T012+)
- [x] Parallel tasks truly independent (different files/libraries)
- [x] Each task specifies exact file path (src/, tests/ paths included)
- [x] No task modifies same file as another [P] task
