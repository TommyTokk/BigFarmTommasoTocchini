Dict = {
    1: ["cane", "ciao", "cielo"],
    2: ["domodosola", "duomo"]
}

Dict = dict(sorted(Dict.items()))
print(Dict)
print("true" if 1 in Dict else "false")