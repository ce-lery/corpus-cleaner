import json
import time

# PATH = "temp.jsonl"
FILENAME = "oscar2109_ja_test"
EXTENTION = ".jsonl"

# line_count = 0
# with open(PATH, "r") as file:
#     for line in file:
#         line_count += 1
#         print(line)
#         print(line_count)

error_count = 0
line_number = 0
# JSONファイルの読み込み
with open(FILENAME+EXTENTION, "r", encoding="utf-8") as file, open(FILENAME+"_cleaned"+EXTENTION,"w",encoding="utf-8") as output:
    while True:
        line_number += 1
        # time.sleep(0.001)  # この行はコメントアウトされているので、削除するか復活させる必要があります

        try:
            line = file.readline()
            if not line:
                break  # ファイルの終わりの場合はループを抜ける

            data = json.loads(line)
            # 行が空の場合(ファイルの終わり)の行は不要です
            output.write(line)
        except UnicodeDecodeError:
            error_count += 1
            print(f"{line_number}行目にて例外発生(UnicodeDecodeError)")
            continue

        except json.JSONDecodeError:
            error_count += 1
            print(f"{line_number}行目にて例外発生(json load)")
            if("\'" in line):
                line = line.replace("\\'","'")
            output.write(line)
            continue

        except Exception as e:
            error_count += 1
            print(f"{line_number}行目にて例外発生({e})")
            continue

    # try:
    #     data = json.loads(line)
    # except:
    #     error_count+=1
    #     print(line_number,"にて例外発生(json parse error)")
    #     continue
        
# 読み込めなかった行数の出力
print(f"{error_count} 行の読み込みに失敗しました。")

# error_count = 0
# line_number = 0
# # JSONファイルの読み込み
# with open(FILENAME+"_cleaned"+EXTENTION,"r",encoding="utf-8") as file:
#     while True:
#         line_number += 1
#         # time.sleep(0.001)  # この行はコメントアウトされているので、削除するか復活させる必要があります

#         try:
#             line = file.readline()
#             if not line:
#                 break  # ファイルの終わりの場合はループを抜ける

#             data = json.loads(line)
#             # 行が空の場合(ファイルの終わり)の行は不要です
#             # output.write(line)
#         except UnicodeDecodeError:
#             error_count += 1
#             print(f"{line_number}行目にて例外発生(ファイル読み込みエラー)")
#             continue

#         except json.JSONDecodeError:
#             error_count += 1
#             print(f"{line_number}行目にて例外発生(json load)")
#             print(line)
#             continue

#         except Exception as e:
#             error_count += 1
#             print(f"{line_number}行目にて例外発生({e})")
#             continue
    
# # 読み込めなかった行数の出力
# print(f"{error_count} 行の読み込みに失敗しました。")