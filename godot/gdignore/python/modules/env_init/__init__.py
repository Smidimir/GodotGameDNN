import os
import sys

print("--- PATH BEFORE ---")
print('\n'.join(sys.path))
print("--- PATH BEFORE ---")

# sys.path.append(os.getcwd() + "\\gdignore\\python\\env")
# sys.path = [ os.getcwd() + "\\gdignore\\python\\env" ]
# sys.path.append(os.getcwd() + "\\gdignore\\python\\env\\Lib")
# sys.path.append(os.getcwd() + "\\gdignore\\python\\env\\DLLs")
# sys.path.append(os.getcwd() + "\\gdignore\\python\\env\\Lib\\site-packages")
# sys.path.append(os.getcwd() + "\\gdignore\\python_game")
# sys.path.append(".")
sys.path = [
    '',
    os.getcwd() + '\\gdignore\\python\\env\\python37.zip',
    os.getcwd() + '\\gdignore\\python\\env\\DLLs',
    os.getcwd() + '\\gdignore\\python\\env\\lib',
    os.getcwd() + '\\gdignore\\python\\env',
    os.getcwd() + '\\gdignore\\python\\env\\lib\\site-packages',
    os.getcwd() + '\\gdignore\\python_game'
]

print("--- PATH AFTER ---")
print('\n'.join(sys.path))
print("--- PATH AFTER ---")
