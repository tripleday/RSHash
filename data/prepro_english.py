# 导入 re 模块
import re

# 创建一个空的字符串列表
strings = []

# 打开要读取的文件，指定编码和错误处理方式
with open('english.200MB', 'r', encoding='utf-8', errors='ignore') as f:
    # 读取文件的每一行
    # lines = f.readlines()

    # 遍历每一行
    for line in f:
        # 使用正则表达式匹配以空格和标点符号划分的字符串
        words = re.findall(r'\w+|[^\w\s]', line)
        # 筛选出长度小于等于 10 的字符串，并添加到字符串列表中
        strings += [word for word in words if len(word) < 10 and word.isascii()]

strings = list(set(strings))
# 创建一个新的文件
with open('english', 'w') as f:
    # 遍历字符串列表
    for string in strings:
        # 将每个字符串写入一行，并换行
        f.write(string + '\n')