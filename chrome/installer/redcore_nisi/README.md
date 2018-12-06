## nsis打包注意事项

1.需要复制`mini_installer.exe`到`src/chrome/installer/redcore_nisi/nsis_src`目录下，然后将`mini_installer.exe`改名为`redcore_install.exe`。

2.修改`src/chrome/installer/redcore_nisi`下的`redcore_config.nsh`文件中的版本号，和当前编译的`mini_installer.exe`版本号相同。

3.该脚本默认安装目录为系统文件夹下，如果需要安装再用户的`UserData`下,则需要将`redcore_config.nsh`文件中的`SYSTEM_INSTALL_LEVEL`参数，修改成`FALSE`。

4.打包时将`nsis_main_redcore.nsi`文件拖动到 NSIS 打包工具中，就可以获得新版版的安装包。