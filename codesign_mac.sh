#ninja -C out/Release/ build_app_dmg
codesign -s "Developer ID Application: Allmobilize Inc. (FD9594D6YV)" out/Release/Redcore.app
