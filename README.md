# ZoomMeetingSDK Packaging Error Fix - Complete Documentation

## Project Information
- **Project Name**: Zoom_Sample_Project
- **Unreal Engine Version**: 5.3.2
- **Plugin**: ZoomMeetingSDK v6.1.5
- **Platform**: Windows (64-bit)
- **Issue Date**: January 2026

---

## Problem Summary

### The Error
When attempting to package the Unreal Engine project, the build failed with the following compilation error:

```
C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
```

**Location**: `Plugins/ZoomMeetingSDK/Source/Library/jwt/include/jwt-cpp/jwt.h` (Line 877)

### Root Cause
The JWT-CPP library (used by ZoomMeetingSDK for authentication) contains C++ exception handling code (`try`/`catch` blocks). However, the plugin's build configuration (`JwtCpp.Build.cs`) was not enabling C++ exception handling, which caused the MSVC compiler to reject the code.

Unreal Engine projects compile with exceptions disabled by default for performance reasons, but certain third-party libraries (like JWT-CPP) require exceptions to be explicitly enabled.

### Additional Warnings (Non-Fatal)
- **C4191**: Type cast warnings in OpenSSL headers (`core_dispatch.h`)
- **C4996**: Deprecation warnings for OpenSSL functions

These warnings did not prevent packaging and were not addressed in this fix.

---

## The Solution

### What Was Modified
**File Modified**: `Plugins/ZoomMeetingSDK/Source/Library/jwt/JwtCpp.Build.cs`

**Line Added**: `bEnableExceptions = true;` (Line 55)

### Before (Original Code)
```csharp
public class JwtCpp : ModuleRules
{
    public JwtCpp(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        CppStandard = CppStandardVersion.Cpp17;
        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
        
        // ... dependencies configuration ...
        
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
            );
    }
}
```

### After (Fixed Code)
```csharp
public class JwtCpp : ModuleRules
{
    public JwtCpp(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        CppStandard = CppStandardVersion.Cpp17;
        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
        
        // ... dependencies configuration ...
        
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
            );
        bEnableExceptions = true;  // <-- THIS LINE WAS ADDED
    }
}
```

---

## Step-by-Step Resolution Guide

### Step 1: Locate the Build Configuration File
1. Navigate to: `Plugins/ZoomMeetingSDK/Source/Library/jwt/`
2. Open `JwtCpp.Build.cs` in any text editor (Visual Studio, VS Code, Notepad++, etc.)

### Step 2: Add Exception Handling Flag
1. Scroll to the bottom of the constructor method 
2. Find the closing brace `}` of the `DynamicallyLoadedModuleNames.AddRange()` block
3. After the closing parenthesis and semicolon `);`, add a new line:
   ```csharp
   bEnableExceptions = true;
   ```
4. Save the file

### Step 3: Clean Build Artifacts
**Clean Project Build Files:**
1. Close Unreal Editor (if open)
2. Navigate to your project root: `Zoom_Sample_Project/`
3. Delete the following folders:
   - `Intermediate/`
   - `Binaries/`

**Clean Plugin Build Files (Recommended):**
4. Navigate to: `Zoom_Sample_Project/Plugins/ZoomMeetingSDK/`
5. Delete the following folders:
   - `Intermediate/`
   - `Binaries/`

### Step 4: Reopen and Package
1. Open `Zoom_Sample_Project.uproject` in Unreal Editor 5.3.2
2. Wait for the project to fully load and compile
3. Go to: **File → Package Project → Windows → Windows (64-bit)**
4. Select your output directory
5. Click **Package** and wait for the build to complete

### Step 5: Verify Success
✅ **Expected Results:**
- Build completes without C4530 error
- All 100+ Zoom SDK DLLs are copied to the packaged build
- C++ compilation shows 100/100 actions completed
- Packaged executable (.exe) is created in the output directory

---

## Technical Explanation

### What `bEnableExceptions = true` Does
- Tells Unreal Build Tool (UBT) to add the `/EHsc` compiler flag to MSVC
- `/EHsc` enables standard C++ exception handling with proper stack unwinding
- Allows `try`, `catch`, and `throw` statements to function correctly
- Required for any third-party library that uses C++ exceptions

### Why This Was Necessary
1. **JWT-CPP Library**: Uses exceptions for error handling (e.g., invalid tokens, parsing errors)
2. **Unreal Default**: Disables exceptions by default for performance
3. **Compiler Enforcement**: MSVC treats exception code without `/EHsc` as an error (not just a warning)

### Module Hierarchy
```
ZoomMeetingSDK Plugin
├── ZoomMeetingSDK (main module)
│   └── ZoomMeetingSDK.Build.cs
├── JwtCpp (JWT library module) ← FIX APPLIED HERE
│   └── JwtCpp.Build.cs
└── OpenSSL (crypto library)
    └── [Various OpenSSL files]
```

---

## Packaging Log Summary

### Before Fix
```
[98/100] Compile [x64] PKCE_ZM_Auth_Gen.cpp
ERROR: C4530: C++ exception handler used, but unwind semantics are not enabled
Packaging FAILED
```

### After Fix
```
[98/100] Compile [x64] PKCE_ZM_Auth_Gen.cpp
[99/100] Link [x64] UnrealEditor-ZoomMeetingSDK.dll
[100/100] WriteMetadata
Packaging SUCCEEDED
```

---

## Files Involved in This Fix

| File Path | Role | Modified? |
|-----------|------|----------|
| `Plugins/ZoomMeetingSDK/Source/Library/jwt/JwtCpp.Build.cs` | Build configuration for JWT-CPP module | ✅ **YES** |
| `Plugins/ZoomMeetingSDK/Source/Library/jwt/include/jwt-cpp/jwt.h` | JWT library header (contains exception code) | ❌ No |
| `Plugins/ZoomMeetingSDK/Source/ZoomMeetingSDK/Private/PKCE_ZM_Auth_Gen.cpp` | Source file that triggered the error | ❌ No |
| `Plugins/ZoomMeetingSDK/Source/ZoomMeetingSDK/ZoomMeetingSDK.Build.cs` | Main plugin build config | ❌ No |

---

## Additional Notes

### Performance Considerations
- Enabling exceptions **only** affects the JwtCpp module
- The main game code and other modules still compile without exceptions
- JWT operations are infrequent (authentication/token generation), so performance impact is negligible

### Alternative Solutions (Not Used)
1. **Modify jwt.h**: Remove exception handling from the library (not recommended - breaks library functionality)
2. **Add `/EHsc` globally**: Enable exceptions for entire project (overkill and reduces performance)
3. **Replace JWT library**: Use a different authentication method (major refactor)

### Future Zoom SDK Updates
If you update the ZoomMeetingSDK plugin in the future:
1. Check if `JwtCpp.Build.cs` still exists
2. Verify `bEnableExceptions = true;` is still present
3. If the line is missing, re-apply this fix

---

## Troubleshooting

### If Packaging Still Fails

**Issue**: Same C4530 error after applying fix
- **Solution**: Make sure you saved `JwtCpp.Build.cs` after editing
- **Solution**: Delete `Intermediate/` and `Binaries/` folders again
- **Solution**: Restart Unreal Editor and try packaging again

**Issue**: Different compilation errors appear
- **Solution**: Check the new error message and line number
- **Solution**: May need to enable exceptions in other modules if they also use exception-based libraries

**Issue**: Linker errors (LNK2019, LNK2001)
- **Solution**: These are unrelated to exception handling
- **Solution**: Check that all `.lib` files are correctly linked in `ZoomMeetingSDK.Build.cs`

**Issue**: OpenSSL warnings (C4191, C4996) become errors
- **Solution**: These should be warnings only; check your project's warning-as-error settings
- **Solution**: If necessary, add `bEnableUndefinedIdentifierWarnings = false;` to suppress

---

## Contact 

- **Linkedin**: https://www.linkedin.com/in/ornobmk/
- **Discord**: x_86or


---

## Version History

| Date | Version | Change |
|------|---------|--------|
| January 2026 | 1.0 | Initial fix applied and documented |

---

## Summary

**Problem**: ZoomMeetingSDK packaging failed due to C++ exception handling not being enabled for the JWT-CPP library.

**Solution**: Added `bEnableExceptions = true;` to `JwtCpp.Build.cs` (line 55).

**Result**: Project packages successfully on Windows (64-bit) with all Zoom SDK functionality intact.

**Impact**: Minimal - only affects the JWT module, no performance degradation in gameplay code.

---

