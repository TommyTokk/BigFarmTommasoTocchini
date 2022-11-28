dict = {
    1: ["cane", "ciao", "cielo"],
    2: ["domodosola", "duomo"]
}

newName = "cielo"
key = 1
if (key in dict):
    if(newName not in dict[key]):
        dict[key].append(newName)
else:
    dict[key] = list()
    dict[key].append(newName)

print(dict.keys())