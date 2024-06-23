import json
from datasets import load_dataset
import glob
import os

input_folder_path = "./ja_meta_utf8/"
output_folder_path = "./ja_meta_utf8_cleaned/"

# 出力フォルダ作成
os.makedirs(output_folder_path,exist_ok=True)
os.makedirs(output_folder_path+"/intermediate/",exist_ok=True)

# jsonlファイル一覧取得
jsonl_list = glob.glob(input_folder_path+'/*.jsonl')
count=0
print("処理ファイル総数：",len(jsonl_list))

# 変換に失敗したファイル一覧
failed_list=[]

print("処理開始")
# 全jsonlファイル処理
for input_file_path in jsonl_list:
    count+=1
    print("####################################################")
    print("修正ファイル番号：", count)

    filename = os.path.basename(input_file_path)
    intermediate_file_path = output_folder_path+"/intermediate/"+filename
    output_file_path = output_folder_path+filename

    print("処理対象ファイル名：",filename)

    error_count = 0
    line_number = 0

    # JSONファイルの読み込み
    print("\nPhase1： 'とTabの変換")
    with open(input_file_path, "rb") as file, open(intermediate_file_path,"w",encoding="utf-8") as output:
        while True:
            line_number += 1
            # time.sleep(0.001)  # この行はコメントアウトされているので、削除するか復活させる必要があります

            try:
                bline = file.readline()
                line = bline.decode('utf-8', 'replace')
                if not line:
                    break  # ファイルの終わりの場合はループを抜ける
                
                if("\'" in line):
                    line = line.replace("\\'","'")
                
                if("\	" in line):
                    line = line.replace("\	","\\t")
                output.write(line)

            except Exception as e:
                error_count += 1
                # print(f"{line_number}行目にて例外発生({e})")
                continue

    # 読み込めなかった行数の出力
    print(f"{error_count} 行の読み込みに失敗しました。")

    error_count = 0
    line_number = 0

    print("\nPhase2： 'jsonl形式読み込みチェック")
    with open(intermediate_file_path,"r",encoding="utf-8") as file, open(output_file_path,"w",encoding="utf-8") as output:
        while True:
            line_number += 1

            try:
                line = file.readline()
                if not line:
                    break  # ファイルの終わりの場合はループを抜ける

                data = json.loads(line)
                output.write(line)

            except UnicodeDecodeError:
                error_count += 1
                # print(f"{line_number}行目にて例外発生(ファイル読み込みエラー)")
                continue

            except json.JSONDecodeError:
                error_count += 1
                # print(f"{line_number}行目にて例外発生(json load)")
                # print(line)
                continue

            except Exception as e:
                error_count += 1
                # print(f"{line_number}行目にて例外発生({e})")
                continue

    # 読み込めなかった行数の出力
    print(f"{error_count} 行の読み込みに失敗しました。これらを削除しました。")
    # 中間生成フォルダの削除
    os.remove(intermediate_file_path)

    # 最終確認
    print("\nPhase3： load_datasetによるjsonl読み込み（最終確認）")
    try:
        dataset = load_dataset("json", data_files= str(output_file_path))
        print("jsonl読み込み成功。クリーニング完了。")
    except Exception as e:
        print("jsonl読み込み失敗。異常終了。")
        failed_list.append(filename)

# results出力
print("####################################################")
print("全ファイル処理完了")
print("処理ファイル総数：",len(jsonl_list))
print("処理成功ファイル総数：",len(jsonl_list)-len(failed_list))
print("処理失敗ファイル総数：",len(failed_list))
print("処理失敗ファイル一覧：")
for failed_file in failed_list:
    print(failed_file)



