import os
import tree_sitter
from tree_sitter import Language

# 加载Tree-sitter的Python语言解析器
Language.build_library(
    # 替换为你的 Tree-sitter Python 语言解析器的路径
    'path/to/tree-sitter-python.so',
    [
        # 替换为你的 Tree-sitter Python 语言的源码目录路径
        'path/to/tree-sitter-python'
    ]
)
PYTHON_LANGUAGE = Language('path/to/tree-sitter-python.so', 'python')

# 读取 Python 代码文件
with open('path/to/your/python/file.py', 'r') as file:
    python_code = file.read()

# 创建 Tree-sitter 解析器
parser = tree_sitter.Parser()
parser.set_language(PYTHON_LANGUAGE)

# 解析 Python 代码
tree = parser.parse(bytes(python_code, 'utf8'))

# 遍历解析树，生成 Python 代码
def generate_python_code(node, indent=0):
    result = ''
    for child in node.children:
        result += ' ' * indent
        if child.type == 'function_definition':
            result += 'def ' + child.child_by_field_name('name').text
            result += '():\n'
            result += generate_python_code(child, indent + 4)
        elif child.type == 'if_statement':
            result += 'if ' + child.child_by_field_name('condition').text
            result += ':\n'
            result += generate_python_code(child, indent + 4)
        elif child.type == 'return_statement':
            result += ' ' * (indent + 4) + 'return ' + child.child_by_field_name('expression').text + '\n'
        else:
            result += child.text + '\n'
        result += generate_python_code(child, indent)
    return result

# 生成 Python 代码
generated_code = generate_python_code(tree.root_node)
print(generated_code)