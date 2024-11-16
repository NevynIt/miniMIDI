import os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

e_types = ['e', 'ae', 'u', 'au', 'm', 'am','l', 'al']
ee_types = ['ee' ,'aee','eu','aeu', 'emu', 'aemu', 'em', 'aem', 'eml', 'aeml', 'el', 'ael']

types = e_types + ee_types

e_decl = []
ee_decl = []

def writefile(filename):
    with open(filename, 'w') as f:
        f.write(''.join(e_decl))
        f.write('\n')
        f.write('#ifdef FPM_EXTEXT\n')
        f.write(''.join(ee_decl))
        f.write('#endif\n')

def e(s):
    e_decl.append(s)

def ee(s):
    ee_decl.append(s)

# for r in types:
#     if r in ee_types:
#         if r == 'ee':
#             ee(f'using {r}_type = extext;\n')
#         elif r == 'aee':
#             ee(f'using {r}_type = uextext;\n')
#         elif 'a' in r:
#             ee(f'using {r}_type = uext;\n')
#         else:
#             ee(f'using {r}_type = ext;\n')
#     else:
#         if r == 'e':
#             e(f'using {r}_type = ext;\n')
#         elif r == 'ae':
#             e(f'using {r}_type = uext;\n')
#         elif 'a' in r:
#             e(f'using {r}_type = ubase;\n')
#         else:
#             e(f'using {r}_type = base;\n')

# e('\n')
# ee('\n')
# for r in types:
#     if r in ee_types:
#         ee(f'__FPM_LIMITS({r})\n')
#     else:
#         e(f'__FPM_LIMITS({r})\n')

# e('\n')
# ee('\n')
from itertools import combinations_with_replacement

e_decl = []
ee_decl = []
for r in types:
    for a, b in combinations_with_replacement(types, 2):
        if a in ee_types or b in ee_types or r in ee_types:
            ee(f'__FPM_MUL(ee, {r}, {a}, {b})\n')
        else:
            e(f'__FPM_MUL(e, {r}, {a}, {b})\n')
writefile('fpm_mul.inc')

e_decl = []
ee_decl = []
for r in types:
    for a, b in combinations_with_replacement(types, 2):
        if a in ee_types or b in ee_types or r in ee_types:
            ee(f'__FPM_DIV(ee, {r}, {a}, {b})\n')
        else:
            e(f'__FPM_DIV(e, {r}, {a}, {b})\n')
writefile('fpm_div.inc')

e_decl = []
ee_decl = []
for r in types:
    for a in types:
        if 'ee' in a or 'ee' in r:
            w = 'ee'
        elif 'e' in a or 'e' in r:
            w = 'e'
        else:
            w = r
        if 'a' in r and w != r:
            w = 'a' + w
        if r in ee_types or a in ee_types:
            ee(f'__FPM_CONV({r}, {w}, {a})\n')
        else:
            e(f'__FPM_CONV({r}, {w}, {a})\n')
writefile('fpm_conv.inc')

e_decl = []
ee_decl = []
