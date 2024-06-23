import os
import json

# archiveディレクトリのパス
archive_dir = "results/archive"

# マージ先のファイルパス
file_list = ["cc100_train.jsonl","wiki_train.jsonl"]

for file in range(file_list):
# 出力用の空のリストを用意
    merged_file = "cc100_train_merged.json"
    merged_data = []

    # 0~7までのフォルダ番号をループ
    for folder_num in range(8):
    folder_path = os.path.join(archive_dir, f"{folder_num:01d}", "output/cleaned/")
    json_file = os.path.join(folder_path, file)
    
    # ファイルが存在する場合のみ処理を行う
    if os.path.isfile(json_file):
        with open(json_file, "r", encoding="utf-8") as f:
            for line in f:
                data = json.loads(line)
                merged_data.append(data)

    # マージしたデータを1つのJSONLファイルに書き出し
    with open(merged_file, "w", encoding="utf-8") as f:
    for data in merged_data:
        json_dump = json.dumps(data, ensure_ascii=False)
        f.write(json_dump + "\n")

    print(f"マージ完了: {merged_file}")