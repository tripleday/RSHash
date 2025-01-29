# coding: utf-8
import csv
import os

# 读取CSV文件
input_files = ['0to999999_BlockTransaction.csv',\
    '1000000to1999999_BlockTransaction.csv']

# 创建一个字典来存储不同文件的数据
output_data = {}
all_values = set()
value_sets = {}

for input_file in input_files:
    with open(input_file, mode='r') as file:
        csv_reader = csv.DictReader(file)
        for row in csv_reader:
            block_number = int(row['blockNumber'])
            blocks_per_p = 50_000  # max_txs 4671 (1e18) number of all values: 9140
            # blocks_per_p = 10_000  # max_txs 64238
            # blocks_per_p = 5_000  # max_txs 38821
            # blocks_per_p = 4_000  # max_txs 34859
            # blocks_per_p = 2_000  # max_txs 19007
            file_index = block_number // blocks_per_p
            file_name = f'value_{blocks_per_p}/p_{file_index}.csv'
            
            if file_name not in output_data:
                output_data[file_name] = []
            
            # 确保该文件名的时间戳集合已初始化
            if file_name not in value_sets:
                value_sets[file_name] = set()
            
            # 检查并添加value
            if (int(int(row['value'])//1e12)) not in value_sets[file_name]:
                value_sets[file_name].add(int(int(row['value'])//1e12))
                output_data[file_name].append({
                    'value': int(int(row['value'])//1e12),
                    'blockNumber': row['blockNumber']
                })
            
            # 提取所有的value并去重
            all_values.add(int(int(row['value'])//1e12))

max_txs = 0
# 写入不同的CSV文件
for file_name, data in output_data.items():
    with open(file_name, mode='w', newline='') as file:
        writer = csv.DictWriter(file, fieldnames=['value', 'blockNumber'])
        # writer.writeheader()
        writer.writerows(data)
    if len(data) > max_txs:
        max_txs = len(data)
print("max_txs: "+str(max_txs))

# 写入所有的value到一个单独的CSV文件 
with open('all_values.csv', mode='w', newline='') as file: 
    writer = csv.DictWriter(file, fieldnames=['value']) 
    # writer.writeheader() 
    for value in all_values:
        writer.writerow({'value': value})
print("number of all values: "+str(len(all_values)))
print("min value: "+str(min(all_values)))
print("max value: "+str(max(all_values)))

print("Completed")

# total number  2619334
# mim value  0
# max value  11901464239480
