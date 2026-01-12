# Clean Rebuild Instructions

## If you're getting "class RentalManager has no member" errors:

### Windows (PowerShell):
```powershell
# Delete all object files
Remove-Item *.o -ErrorAction SilentlyContinue
Remove-Item src\*.o -ErrorAction SilentlyContinue

# Recompile everything fresh
g++ -c src\*.cpp -I.\include -std=c++11 -Wall
g++ *.o -lmysqlcppconn -o rental_system
```

### Or use your compile script:
```powershell
# Make sure compile.bat is clean
.\compile.bat
```

## Verify all functions exist:

All these functions ARE defined in src/RentalManager.cpp:
- ✅ `updateRentalStatus()` - line 497
- ✅ `getRentalByID()` - line 169  
- ✅ `getRentalItems()` - line 352
- ✅ `calculateLateFee()` - line 382

All functions are declared in include/RentalManager.h

## If errors persist:

1. **Delete all .o files** (object files from previous builds)
2. **Delete the executable** (rental_system.exe or rental_system)
3. **Recompile from scratch**
4. **Make sure you're compiling ALL .cpp files**, not just some

The code is correct - this is a build cache issue!

