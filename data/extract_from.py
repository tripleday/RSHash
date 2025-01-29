# coding: utf-8
import csv
import os

# 读取CSV文件
input_files = ['0to999999_BlockTransaction.csv',\
    '1000000to1999999_BlockTransaction.csv']

# 创建一个字典来存储不同文件的数据
output_data = {}
all_froms = set()
from_sets = {}

for input_file in input_files:
    with open(input_file, mode='r') as file:
        csv_reader = csv.DictReader(file)
        for row in csv_reader:
            block_number = int(row['blockNumber'])
            # blocks_per_p = 50_000  # max_txs 54776
            # blocks_per_p = 20_000  # max_txs 35046
            # blocks_per_p = 10_000  # max_txs 22393
            # blocks_per_p = 5_000  # max_txs 18075
            # blocks_per_p = 4_000  # max_txs 15865
            # blocks_per_p = 2_000  # max_txs 13855
            # blocks_per_p = 1_000  # max_txs 12418
            # blocks_per_p = 800  # max_txs 12175
            blocks_per_p = 500  # max_txs 11538
            file_index = block_number // blocks_per_p
            file_name = f'from_{blocks_per_p}/p_{file_index}.csv'
            
            if file_name not in output_data:
                output_data[file_name] = []
            
            # 确保该文件名的地址集合已初始化
            if file_name not in from_sets:
                from_sets[file_name] = set()
            
            # 检查并添加from地址
            if row['from'] not in from_sets[file_name]:
                from_sets[file_name].add(row['from'])
                output_data[file_name].append({
                    'from': row['from'],
                    'blockNumber': row['blockNumber']
                })
            # 提取所有的from并去重
            all_froms.add(row['from'])
            if row['from']=="None":
                print(row['transactionHash'])

max_txs = 0
# 写入不同的CSV文件
for file_name, data in output_data.items():
    with open(file_name, mode='w', newline='') as file:
        writer = csv.DictWriter(file, fieldnames=['from', 'blockNumber'])
        # writer.writeheader()
        writer.writerows(data)
    if len(data) > max_txs:
        max_txs = len(data)
print("max_txs: "+str(max_txs))

# 写入所有的from到一个单独的CSV文件 
with open('all_froms.csv', mode='w', newline='') as file: 
    writer = csv.DictWriter(file, fieldnames=['from']) 
    # writer.writeheader() 
    for f in all_froms:
        writer.writerow({'from': f})
print("number of all froms: "+str(len(all_froms)))

print("Completed")

# total number  308641