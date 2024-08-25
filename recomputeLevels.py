#! /usr/bin/python3

# automatically recomputes and stores the answer to every level.
# designed for linux.

from pathlib import Path
import subprocess as SP

class Colorizer:
    def __init__(this,colorCode,quotes=False):
        this.begin = "\033[" + str(colorCode) + "m"
        this.quotes = quotes
    
    # Adds color, then resets afterward.
    def __mul__(this,other):
        if this.quotes:
            return this.begin + "\"" + str(other) + "\"\033[0m"
        return this.begin + str(other) + "\033[0m"

# Colors
red    = Colorizer(31)
green  = Colorizer(32)
quote  = Colorizer(33,True)

# returns 'False' on error, 'True' otherwise.
def updateVals(sourceFolder,destFolder):
    source = Path(sourceFolder)
    dest = Path(destFolder)
    if not (source.exists() and source.is_dir()):
        print("Source folder not found.")
        return False
    if not (dest.exists() and dest.is_dir()):
        print("Destination folder not found.")
        return False
    
    for f in source.glob('*.txt'):
        if not f.is_file(): continue # skip non-files.
        print(quote*f + ": ",end="",flush=True)
        res = SP.run(("./kamiSolver","-c0","-borders",str(f)),text=True,capture_output=True)
        if res.returncode != 0:
            print(red*"Failed to solve with exit code " + quote*str(res.returncode) + red*":")
            print(res.stdout)
            print(red*str(res.stderr))
            return False
        print(green*"Solved" + " -> ",end="",flush=True)
        resStr = str(res.stdout)
        with (dest / f.name).open("w") as f:
            f.write(resStr)
        print("Stored to "  + quote*(dest / f.name))
    print(green*"Done.")
    return True

updateVals("3ds_levels","3ds_solutions")

