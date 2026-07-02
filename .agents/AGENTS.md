# AI Agent Guidelines for app_badge_control_flutter

This document defines specific rules and constraints that all AI coding assistants must strictly adhere to when interacting with this repository.

## Git, Commit & Push Guidelines

1. **Explicit Permission Required for Commit and Push**:
   - **Do NOT execute `git commit` or `git push` without the user's explicit, clear request.** Even if you have finished a task, wait for the user to instruct you to commit or push.
2. **Do Not Commit per Minor Fix**:
   - In order to prevent expensive CI environment runs and save billing costs, **do NOT create git commits incrementally** for every small bug fix, lint correction, or minor refactoring.
   - Consolidate all related changes and modifications. Perform git commits **only** when a feature, bug fix, or validation phase is fully completed, stable, and locally verified.
3. **Follow Staging Rules**:
   - Only stage files explicitly using `git add <file_path>`. Never use wildcards or `git add .` to avoid committing unstaged local changes made by the user.

## Code Quality & Validation Guidelines

1. **Mandatory Testing & Coverage**:
   - Any new feature, bug fix, or modification **must be accompanied by corresponding unit/integration test cases**. 
   - Ensure that adequate code coverage is verified and maintained.
2. **Zero Flutter Analyze Errors**:
   - Prior to ending your turn or asking for validation, ensure that running `flutter analyze` reports **zero errors and zero warnings**.
3. **Zero Lint Errors Across All Languages**:
   - In addition to Dart, ensure there are no lint or syntax errors in other configuration or source files, including YAML, C++, Swift, Kotlin, CMake, Gradle, and Markdown.
