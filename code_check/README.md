# codecheck

codecheck 模块能在 git 提交前为代码执行一些浅层、快速的检查。

# 使用

执行 

```
python <路径>/setup_hooks.py
```

来配置 pre-commit hook 、激活检查。

# 检查规则

当文件 myfile.txt 满足以下2条要求时，检查 mycheck 会对它开启。

(1) myfile.txt 的名为 "chk_mycheck" 的 git attribute 被置位。；

(2) mycheck 对应的 applies_to 函数接受该文件的类别。文件类别有 3 种： 

  - lfs-file: LFS管理的文件
  
  - text: 文本文件

  - binary: 其他二进制文件

绝大多数检查仅接受文本文件，所以一般只需要修改 .gitattribute 文件即可。

# 增加检查

1. 新建文件 rule_检查名.py 和 Rule 子类，在 register_rules 中进行注册。

1. 重写 Rule.name 和 Rule.check 函数。默认只检查 text 类型的文件，如果需要检查 lfs-file 或 binary 类型的文件，则还需要重写 Rule.applies_to 函数。

1. 更改 .gitattributes 文件，控制哪些文件需要本检查。例如 *.py 文件需要该检查，则增加一行

    ```
    *.py chk_检查名
    ```
    检查名即 Rule.name 的返回值。

    如果对同一个后缀名的检查比较多，还可使用 .gitattributes 的宏定义来简化，例如，用

    ```
    [attr]宏名称 chk_检查1 chk_检查2 ...
    ```

    来定义宏，使用时只需要用

    ```
    *.py 宏名称
    ```

    即可为 *.py 添加多个检查。

