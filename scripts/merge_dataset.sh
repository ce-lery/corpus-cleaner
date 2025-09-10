#!/bin/bash

# Result file name
file_name="wiki_train.jsonl"
merged_file=$file_name

# Temporary file name
tmp_file="/tmp/$file_name"

# clear temporary file
> "$tmp_file"


for folder_num in {0..7}
do
   folder_path="results/archive/${folder_num:0:1}/output/cleaned"
   json_file="$folder_path/$file_name"

   if [ -f "$json_file" ]; then
       cat "$json_file" >> "$tmp_file"
   fi
done

# Create Result file
mv "$tmp_file" "$merged_file"

echo "Merge completed: $merged_file"