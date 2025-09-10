# Implementation Plan: Linux Clipboard History Manager

**Branch**: `001-linux-clipboard-history` | **Date**: September 11, 2025 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/home/astinaam/projects/clipboard-history/specs/001-linux-clipboard-history/spec.md`

## Execution Flow (/plan command scope)
```
1. Load feature spec from Input path
   → If not found: ERROR "No feature spec at {path}"
2. Fill Technical Context (scan for NEEDS CLARIFICATION)
   → Detect Project Type from context (web=frontend+backend, mobile=app+api)
   → Set Structure Decision based on project type
3. Evaluate Constitution Check section below
   → If violations exist: Document in Complexity Tracking
   → If no justification possible: ERROR "Simplify approach first"
   → Update Progress Tracking: Initial Constitution Check
4. Execute Phase 0 → research.md
   → If NEEDS CLARIFICATION remain: ERROR "Resolve unknowns"
5. Execute Phase 1 → contracts, data-model.md, quickstart.md, agent-specific template file (e.g., `CLAUDE.md` for Claude Code, `.github/copilot-instructions.md` for GitHub Copilot, or `GEMINI.md` for Gemini CLI).
6. Re-evaluate Constitution Check section
   → If new violations: Refactor design, return to Phase 1
   → Update Progress Tracking: Post-Design Constitution Check
7. Plan Phase 2 → Describe task generation approach (DO NOT create tasks.md)
8. STOP - Ready for /tasks command
```

**IMPORTANT**: The /plan command STOPS at step 7. Phases 2-4 are executed by other commands:
- Phase 2: /tasks command creates tasks.md
- Phase 3-4: Implementation execution (manual or via tools)

## Summary
Lightweight Linux clipboard history manager with global hotkey (Meta+V) access, system tray integration, and item pinning capability. Uses Qt6 Widgets for cross-platform UI, QClipboard for monitoring, and JSON for persistence. Modular architecture with ClipboardManager, ClipboardWindow, and TrayIcon components.

## Technical Context
**Language/Version**: C++ 17/20 with Qt6 (Qt 6.5+)
**Primary Dependencies**: Qt6 Widgets, Qt6 Core, Qt6 GUI, QxtGlobalShortcut or X11/Wayland hotkey alternatives
**Storage**: JSON files in ~/.config/clipboard-manager/history.json
**Testing**: Qt Test framework, integration tests with actual clipboard
**Target Platform**: Linux (X11 + Wayland support)
**Project Type**: single (desktop application)
**Performance Goals**: <50ms clipboard response, <10MB memory footprint, instant popup display
**Constraints**: <200ms popup open time, cross-platform compatibility, minimal dependencies
**Scale/Scope**: 20-50 clipboard items, single user, text-only initially

## Constitution Check
*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Simplicity**:
- Projects: 1 (clipboard-manager executable)
- Using framework directly? Yes (Qt6 Widgets direct usage, no wrapper classes)
- Single data model? Yes (ClipboardItem with text, timestamp, pinned status)
- Avoiding patterns? Yes (direct Qt patterns, no Repository/UoW complexity)

**Architecture**:
- EVERY feature as library? Yes (ClipboardManager, ClipboardWindow, TrayIcon as libraries)
- Libraries listed: ClipboardManager (monitoring/storage), ClipboardWindow (UI), TrayIcon (system integration)
- CLI per library: --help, --version, --config-path, --history-limit
- Library docs: llms.txt format planned? Yes

**Testing (NON-NEGOTIABLE)**:
- RED-GREEN-Refactor cycle enforced? Yes (tests written first, must fail, then implement)
- Git commits show tests before implementation? Yes (strict TDD order)
- Order: Contract→Integration→E2E→Unit strictly followed? Yes
- Real dependencies used? Yes (actual clipboard, file system, Qt widgets)
- Integration tests for: new libraries, contract changes, shared schemas? Yes
- FORBIDDEN: Implementation before test, skipping RED phase? Enforced

**Observability**:
- Structured logging included? Yes (Qt logging categories for debug/monitoring)
- Frontend logs → backend? N/A (single desktop app)
- Error context sufficient? Yes (clipboard errors, file I/O errors, hotkey conflicts)

**Versioning**:
- Version number assigned? 1.0.0
- BUILD increments on every change? Yes (MAJOR.MINOR.BUILD)
- Breaking changes handled? Yes (config migration, data format versioning)

## Project Structure

### Documentation (this feature)
```
specs/[###-feature]/
├── plan.md              # This file (/plan command output)
├── research.md          # Phase 0 output (/plan command)
├── data-model.md        # Phase 1 output (/plan command)
├── quickstart.md        # Phase 1 output (/plan command)
├── contracts/           # Phase 1 output (/plan command)
└── tasks.md             # Phase 2 output (/tasks command - NOT created by /plan)
```

### Source Code (repository root)
```
# Option 1: Single project (DEFAULT)
src/
├── models/
├── services/
├── cli/
└── lib/

tests/
├── contract/
├── integration/
└── unit/

# Option 2: Web application (when "frontend" + "backend" detected)
backend/
├── src/
│   ├── models/
│   ├── services/
│   └── api/
└── tests/

frontend/
├── src/
│   ├── components/
│   ├── pages/
│   └── services/
└── tests/

# Option 3: Mobile + API (when "iOS/Android" detected)
api/
└── [same as backend above]

ios/ or android/
└── [platform-specific structure]
```

**Structure Decision**: Option 1 (Single project) - Desktop application with modular library components

## Phase 0: Outline & Research
1. **Extract unknowns from Technical Context** above:
   - For each NEEDS CLARIFICATION → research task
   - For each dependency → best practices task
   - For each integration → patterns task

2. **Generate and dispatch research agents**:
   ```
   For each unknown in Technical Context:
     Task: "Research {unknown} for {feature context}"
   For each technology choice:
     Task: "Find best practices for {tech} in {domain}"
   ```

3. **Consolidate findings** in `research.md` using format:
   - Decision: [what was chosen]
   - Rationale: [why chosen]
   - Alternatives considered: [what else evaluated]

**Output**: research.md with all NEEDS CLARIFICATION resolved

## Phase 1: Design & Contracts
*Prerequisites: research.md complete*

1. **Extract entities from feature spec** → `data-model.md`:
   - Entity name, fields, relationships
   - Validation rules from requirements
   - State transitions if applicable

2. **Generate API contracts** from functional requirements:
   - For each user action → endpoint
   - Use standard REST/GraphQL patterns
   - Output OpenAPI/GraphQL schema to `/contracts/`

3. **Generate contract tests** from contracts:
   - One test file per endpoint
   - Assert request/response schemas
   - Tests must fail (no implementation yet)

4. **Extract test scenarios** from user stories:
   - Each story → integration test scenario
   - Quickstart test = story validation steps

5. **Update agent file incrementally** (O(1) operation):
   - Run `/scripts/update-agent-context.sh [claude|gemini|copilot]` for your AI assistant
   - If exists: Add only NEW tech from current plan
   - Preserve manual additions between markers
   - Update recent changes (keep last 3)
   - Keep under 150 lines for token efficiency
   - Output to repository root

**Output**: data-model.md, /contracts/*, failing tests, quickstart.md, agent-specific file

## Phase 2: Task Planning Approach
*This section describes what the /tasks command will do - DO NOT execute during /plan*

**Task Generation Strategy**:
- Load `/templates/tasks-template.md` as base structure
- Generate contract test tasks from `/contracts/` specifications  
- Create model implementation tasks from `data-model.md`
- Generate integration test tasks from user scenarios in `spec.md`
- Create UI implementation tasks from `quickstart.md` workflow

**Task Categories**:
1. **Contract Tests [P]**: Test each library interface (ClipboardManager, ClipboardWindow, TrayIcon)
2. **Model Creation [P]**: Implement ClipboardItem, ClipboardHistory, Configuration classes
3. **Core Logic**: ClipboardManager monitoring, history management, persistence
4. **UI Components**: ClipboardWindow popup, keyboard navigation, visual design
5. **System Integration**: TrayIcon, global hotkey, cross-platform compatibility
6. **End-to-End**: Complete user workflows from spec acceptance scenarios

**TDD Ordering Strategy**:
- Contract tests first (must fail before implementation)
- Model classes to pass basic contract tests
- Service layer implementation (ClipboardManager)
- UI layer implementation (ClipboardWindow, TrayIcon)
- Integration tests for complete user scenarios
- Performance and cross-platform validation

**Parallel Execution Markers [P]**:
- Contract tests for different libraries can run in parallel
- Model class implementations are independent
- UI component development can proceed in parallel once contracts exist

**Estimated Output**: 25-30 numbered, ordered tasks covering:
- 6 contract test tasks (2 per library)
- 8 model implementation tasks 
- 6 service layer tasks
- 8 UI implementation tasks
- 4 integration test tasks
- 3 performance/validation tasks

**IMPORTANT**: This phase is executed by the /tasks command, NOT by /plan

## Phase 3+: Future Implementation
*These phases are beyond the scope of the /plan command*

**Phase 3**: Task execution (/tasks command creates tasks.md)  
**Phase 4**: Implementation (execute tasks.md following constitutional principles)  
**Phase 5**: Validation (run tests, execute quickstart.md, performance validation)

## Complexity Tracking
*Fill ONLY if Constitution Check has violations that must be justified*

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |


## Progress Tracking
*This checklist is updated during execution flow*

**Phase Status**:
- [x] Phase 0: Research complete (/plan command)
- [x] Phase 1: Design complete (/plan command)
- [x] Phase 2: Task planning complete (/plan command - describe approach only)
- [ ] Phase 3: Tasks generated (/tasks command)
- [ ] Phase 4: Implementation complete
- [ ] Phase 5: Validation passed

**Gate Status**:
- [x] Initial Constitution Check: PASS
- [x] Post-Design Constitution Check: PASS  
- [x] All NEEDS CLARIFICATION resolved
- [x] Complexity deviations documented (none required)

---
*Based on Constitution v2.1.1 - See `/memory/constitution.md`*