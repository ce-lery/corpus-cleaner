import pandas as pd
from datasets import load_dataset
import csv

# # create dataset for training tokenizer
# dataset = load_dataset("wikipedia/data/20230601/ja/", data_files= "train-*-of-0015.parquet",split="train")
# # Export the text column of the dataset to wiki.txt
# dataset.to_csv("wiki.txt", columns=["text"], sep="\t", index=False, header=False, quoting=csv.QUOTE_NONE, escapechar=" ")

# create dataset for training tokenizer
dataset = load_dataset("wikipedia-utils/corpus-jawiki-20230403/", data_files= "train-*-of-00008.parquet",split="train")
# Export the text column of the dataset to wiki.txt
dataset.to_csv("wiki.txt", columns=["text"], sep="\t", index=False, header=False, quoting=csv.QUOTE_ALL, escapechar=" ",na_rep="")
# Remove head and tail's ".
# open input file
with open("wiki.txt", 'r', encoding='utf-8') as infile:
    reader = csv.reader(infile)
    
    # open output file
    with open("wiki_mrph.txt", 'w', newline='', encoding='utf-8') as outfile:
        writer = csv.writer(outfile)
        
        # 各行を処理する
        for row in reader:
            cleaned_row = [entry.strip('"') for entry in row]
            writer.writerow(cleaned_row)