

rows = []

with open("build/miniMIDI.elf.map") as f:
    i = 0
    for l in f:
        l = l[:-1]
        if (l[15:20] == " 0x10" or "0x100842e0" in l):
            try:
                s = int(l[26:37], 16)
                if s > 512:
                    rows.append((s, i))
            except:
                pass
        i += 1

rows2 = []
n = 0
buf = ["" , "" , ""]
with open("build/miniMIDI.elf.map") as f:
    i = 0
    for l in f:
        buf.remove(buf[0])
        buf.append(l)
        if i == rows[n][1] + 1:
            rows2.append((rows[n][0], "".join(buf)[:-1]))
            n += 1
            if n == len(rows):
                break
        i += 1

rows2.sort()
for r in rows2:
    print(r[0], r[1])
    print("------")