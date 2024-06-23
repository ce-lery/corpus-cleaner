import json
import time

# PATH = "temp.jsonl"
FILENAME = "test"
EXTENTION = ".jsonl"

error_count = 0
line_number = 0
# JSONファイルの読み込み
with open(FILENAME+EXTENTION, "r", encoding="utf-8") as file, open(FILENAME+"_cleaned"+EXTENTION,"w",encoding="utf-8") as output:
    while True:
        line_number += 1
        line = file.readline()
        print(line)
        if not line:
            break  # ファイルの終わりの場合はループを抜ける

        if("\'" in line):
            line = line.replace("\\'","'")
            print(line)
            continue
        if("\	" in line):
            line = line.replace("\	","	")
            print(line)
            continue
        output.write(line)
        # time.sleep(0.001)  # この行はコメントアウトされているので、削除するか復活させる必要があります
