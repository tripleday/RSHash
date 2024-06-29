from lxml import etree
import re

parser = etree.XMLParser(recover=True)
# tree = etree.parse('dblp.xml.50MB', parser)
# tree = etree.parse('dblp.xml.200MB', parser)
tree = etree.parse('dblp.xml.all', parser)

# 解析 XML 文件，获取根元素
# tree = ET.parse('dblp.xml.50MB')
root = tree.getroot()

# 创建一个空的字符串列表
strings = []

# 遍历根元素的所有子元素
for child in root:
    # 遍历子元素的所有子元素
    for grandchild in child:
        # 获取子元素的文本内容，并替换 HTML 实体
        text = grandchild.text
        # 按空格划分文本内容为字符串列表
        if not text:
            continue
        words = text.split()
        # 筛选出长度小于等于 10 的字符串，并添加到字符串列表中
        strings += [word for word in words if len(word) < 10 and word.isascii()]

strings = list(set(strings))
# 创建一个新的文件
# with open('xml.50MB', 'w') as f:
# with open('xml.200MB', 'w') as f:
with open('dblpxml', 'w') as f:
    # 遍历字符串列表
    for string in strings:
        # 将每个字符串写入一行，并换行
        f.write(string + '\n')