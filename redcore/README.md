## 目录

在代码仓库的`src/redcore`文件夹下，可以找到如下几个脚本：
```
auto_build_win.py       // 编译脚本，其中包括git操作、gclient、gn、ninja操作。
resource_replace_win.py     // 资源替换脚本，主要针对于项目中有redcore_ntp资源文件夹更新的时候使用
sign_win.py     // （仅适用于windows）签名打包脚本，基于nsis以及GlobalSign对安装包进行的签名。
build_utils.py  // 一些工具类
```

## 使用

- auto_build_win.py中需要的参数

`--product-name `: 当前执行脚本时对应的产品名称，和 [browser-customize](https://github.com/1redcore/browser-customize.git) 库中的每个项目文件夹严格对应。例如：`--product-name=dongguanyinhang`。如果需要编译内部测试版本，传入`49_dev`或者`70_dev`即可。

`--working-dir`: 工作目录，传入一个可用的代码库目录。例如：`--working-dir=E:\redcore\src`

`--branch-name`: 编译需要的git branch的名字。例如：`--branch-name=49_dev`

`--clean-build`: python中的bool类型，是否需要做cleanbuild，默认为`False`，如果需要cleanbuild，则在执行时传入参数`--clean-build=True`。

`--push-version`: python中的bool类型，默认为`True`，是否需要在编译成功后，向github仓库中推送VERSION文件的改动。

- `auto_build_win.py 和 resource_replace_win.py`中只需要俩个参数，分别为`--product-name`和`--working-dir`，使用方法和`auto_build_win.py`中参数一样。

> 注意：执行auto_build_win.py脚本，默认会将resource_replace_win.py和sign_win.py脚本一起执行，代码如果没有问题的话，最后会在out/Release/redcore_out中生成对应的安装包、更新包以及打包好的pdb文件。

> 注意：在执行sign_win.py脚本的时候会弹出输入GlobalSign的密码输入框，输入对应密码以后可以继续执行脚本。由于GlobalSign签名的时候需要外接设备，但是远程桌面的方式会阻断外接设备的读取，所以每次打包需要使用Teamviewer远程打包电脑。

> 注意：如果签名的时候发生错误中断了整个打包流程，不需要重新继续执行auto_build_win.py脚本，可以直接执行sign_win.py脚本并传入对应的参数即可。